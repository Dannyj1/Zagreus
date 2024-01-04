/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "tuner.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>

#include "../senjo/UCIAdapter.h"
#include "bitboard.h"
#include "evaluate.h"
#include "features.h"
#include "movegen.h"
#include "pst.h"
#include "search.h"

namespace Zagreus {
Bitboard tunerBoard{};

int stopCounter = 0;
int iteration = 0;
double K = 0.0;

int batchSize = 512;
double learningRate = 0.25;
double delta = 1.0;
double optimizerEpsilon = 1e-6;
double epsilonDecay = 0.98;
double beta1 = 0.9;
double beta2 = 0.999;
// 0 = random seed
long seed = 0;
int epsilonWarmupIterations = 0;
int patience = 20;

std::vector<std::vector<TunePosition>> createBatches(std::vector<TunePosition>& positions) {
    // Create random batches of batchSize positions
    std::vector<std::vector<TunePosition>> batches;
    batches.reserve(positions.size() / batchSize);

    for (int i = 0; i < positions.size(); i += batchSize) {
        std::vector<TunePosition> batch;
        for (int j = i; j < i + batchSize && j < positions.size(); j++) {
            batch.push_back(positions[j]);
        }
        batches.push_back(batch);
    }

    return batches;
}

double sigmoid(double x) { return 1.0 / (1.0 + pow(10.0, -K * x / 400.0)); }

double evaluationLoss(std::vector<TunePosition>& positions, int amountOfPositions,
                      std::chrono::time_point<std::chrono::steady_clock>& maxEndTime,
                      ZagreusEngine& engine) {
    double totalLoss = 0.0;

    for (TunePosition& pos : positions) {
        tunerBoard.setFromFenTuner(pos.fen);
        int evalScore = Evaluation(tunerBoard).evaluate();;

        double loss = std::pow(pos.result - sigmoid(evalScore), 2);
        totalLoss += loss;
    }

    return (1.0 / amountOfPositions) * totalLoss;
}

double findOptimalK(std::vector<TunePosition>& positions,
                    std::chrono::time_point<std::chrono::steady_clock>& maxEndTime,
                    ZagreusEngine& engine) {
    std::vector<int> evalScores(positions.size());

    for (TunePosition& pos : positions) {
        tunerBoard.setFromFenTuner(pos.fen);
        Move rootMove{};
        SearchContext context{};
        context.endTime = maxEndTime;
        senjo::SearchStats stats{};

        /*if (tunerBoard.getMovingColor() == WHITE) {
            int qScore = qScore = qsearch<WHITE, PV>(tunerBoard, MAX_NEGATIVE, MAX_POSITIVE, 0,
                                                           rootMove, context, stats);
        } else {
            int qScore = qScore = qsearch<BLACK, PV>(tunerBoard, MAX_NEGATIVE, MAX_POSITIVE, 0,
                                                           rootMove, context, stats);
        }*/

        int evalScore = Evaluation(tunerBoard).evaluate();

        pos.score = evalScore;
    }

    // Find optimal K
    double bestK = 0.0;
    double bestLoss = 9999999.0;
    double oldK = K;

    for (double k = 0.0; k <= 2.0; k += 0.001) {
        K = k;
        double totalLoss = 0.0;

        for (TunePosition& pos : positions) {
            double loss = pos.result - sigmoid((double)pos.score);
            totalLoss += loss * loss;
        }

        double loss = totalLoss / static_cast<double>(positions.size());

        if (loss < bestLoss) {
            bestLoss = loss;
            bestK = k;
        }
    }

    K = oldK;
    return bestK;
}

std::vector<TunePosition> loadPositions(
    char* filePath, std::chrono::time_point<std::chrono::steady_clock>& maxEndTime,
    ZagreusEngine& engine, std::mt19937_64 gen) {
    std::cout << "Loading positions..." << std::endl;
    std::vector<TunePosition> positions;
    std::vector<std::string> lines;
    std::ifstream fin(filePath);
    int win = 0;
    int loss = 0;
    int draw = 0;

    std::string line;
    while (std::getline(fin, line)) {
        lines.emplace_back(line);
    }

    positions.reserve(lines.size());

    for (std::string& posLine : lines) {
        if (posLine.empty() || posLine == " ") {
            continue;
        }

        double result;
        std::string resultStr = posLine.substr(posLine.find(" c9 ") + 4, posLine.find(" c9 ") + 4);
        std::string fen = posLine.substr(0, posLine.find(" c9 "));

        Move rootMove{};
        SearchContext context{};
        senjo::SearchStats stats{};
        context.endTime = maxEndTime;
        int qScore;

        if (tunerBoard.getMovingColor() == WHITE) {
            qScore = qsearch<WHITE, PV>(tunerBoard, MAX_NEGATIVE, MAX_POSITIVE, 0, context, stats);
        } else {
            qScore = qsearch<BLACK, PV>(tunerBoard, MAX_NEGATIVE, MAX_POSITIVE, 0, context, stats);
        }

        if (!tunerBoard.setFromFen(fen) || tunerBoard.isDraw() || tunerBoard.isWinner<WHITE>() ||
            tunerBoard.isWinner<BLACK>() || tunerBoard.isKingInCheck<WHITE>() ||
            tunerBoard.isKingInCheck<BLACK>() || popcnt(tunerBoard.getColorBoard<WHITE>()) <= 4 ||
            popcnt(tunerBoard.getColorBoard<BLACK>()) <= 4 ||
            tunerBoard.getAmountOfMinorOrMajorPieces() < 4 ||
            tunerBoard.getAmountOfMinorOrMajorPieces<WHITE>() <= 2 ||
            tunerBoard.getAmountOfMinorOrMajorPieces<BLACK>() <= 2 || qScore <= -(
                MATE_SCORE - MAX_PLY) ||
            qScore >= (MATE_SCORE - MAX_PLY)) {
            continue;
        }

        // Remove " and ; from result
        resultStr.erase(std::remove(resultStr.begin(), resultStr.end(), '"'), resultStr.end());
        resultStr.erase(std::remove(resultStr.begin(), resultStr.end(), ';'), resultStr.end());

        if (resultStr == "1" || resultStr == "1-0") {
            result = 1.0;
            win++;
        } else if (resultStr == "0" || resultStr == "0-1") {
            result = 0.0;
            loss++;
        } else {
            result = 0.5;
            draw++;
        }

        int evalScore = Evaluation(tunerBoard).evaluate();
        positions.emplace_back(TunePosition{fen, result, evalScore});
    }

    // Reduce the biggest two classes to the size of the smallest class
    int smallestClassSize = std::min(win, std::min(loss, draw));
    std::vector<TunePosition> newPositions;
    int newWin = 0;
    int newLoss = 0;
    int newDraw = 0;

    // Shuffle positions
    std::shuffle(positions.begin(), positions.end(), gen);

    for (TunePosition& pos : positions) {
        if (pos.result == 1.0 && newWin < smallestClassSize) {
            newPositions.emplace_back(pos);
            newWin++;
        } else if (pos.result == 0.0 && newLoss < smallestClassSize) {
            newPositions.emplace_back(pos);
            newLoss++;
        } else if (pos.result == 0.5 && newDraw < smallestClassSize) {
            newPositions.emplace_back(pos);
            newDraw++;
        }

        if (newWin >= smallestClassSize && newLoss >= smallestClassSize &&
            newDraw >= smallestClassSize) {
            break;
        }
    }

    // Write all newPositions to a file by their fen strings
    std::ofstream fout("cleaned_positions.epd");
    for (TunePosition& pos : newPositions) {
        std::string result;

        if (pos.result == 1.0) {
            result = "1-0";
        } else if (pos.result == 0.0) {
            result = "0-1";
        } else {
            result = "1/2-1/2";
        }

        fout << pos.fen << " c9 \"" << result << "\";" << std::endl;
    }

    fout.close();

    std::cout << "Loaded " << newPositions.size() << " positions." << std::endl;
    std::cout << "Win: " << newWin << ", Loss: " << newLoss << ", Draw: " << newDraw << std::endl;
    return positions;
}

void exportNewEvalValues(std::vector<double>& bestParams) {
    std::ofstream fout("tuned_params.txt");

    // Declare pieceNames
    std::string pieceNames[6] = {"Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};

    fout << "int evalValues[" << getEvalFeatureSize() << "] = { ";
    for (int i = 0; i < getEvalFeatureSize(); i++) {
        fout << static_cast<int>(bestParams[i]);

        if (i != bestParams.size() - 1) {
            fout << ", ";
        }
    }
    fout << " };" << std::endl << std::endl;

    // Write the 6 piece square tables to a file (2 tables per piece, a midgame and endgame table
    // declared like this:  int midgamePawnTable[64] and int endgamePawnTable[64] The 6 midgame tables
    // are declared first, then the 6 endgame tables
    int pstSize = getMidgameValues().size();
    for (int i = 0; i < 6; i++) {
        fout << "int midgame" << pieceNames[i] << "Table[64] = { ";
        for (int j = 0; j < 64; j++) {
            fout << static_cast<int>(bestParams[getEvalFeatureSize() + i * 64 + j]);

            if (j != 63) {
                fout << ", ";
            }
        }
        fout << " };" << std::endl;

        fout << "int endgame" << pieceNames[i] << "Table[64] = { ";
        for (int j = 0; j < 64; j++) {
            fout << static_cast<int>(bestParams[getEvalFeatureSize() + pstSize + i * 64 + j]);

            if (j != 63) {
                fout << ", ";
            }
        }
        fout << " };" << std::endl << std::endl;
    }
}

void startTuning(char* filePath) {
    std::random_device rd;
    std::mt19937_64 gen; // NOLINT(*-msc51-cpp)
    ExponentialMovingAverage smoothedValidationLoss(10);

    if (seed == 0) {
        gen = std::mt19937_64(rd());
    } else {
        gen = std::mt19937_64(seed);
    }

    ZagreusEngine engine;
    senjo::UCIAdapter adapter(engine);
    std::chrono::time_point<std::chrono::steady_clock> maxEndTime =
        std::chrono::time_point<std::chrono::steady_clock>::max();
    std::vector<TunePosition> positions = loadPositions(filePath, maxEndTime, engine, gen);

    engine.setTuning(false);

    std::vector<double> bestParameters = getBaseEvalValues();
    updateEvalValues(bestParameters);
    exportNewEvalValues(bestParameters);

    std::cout << "Finding the optimal K value..." << std::endl;
    K = findOptimalK(positions, maxEndTime, engine);
    std::cout << "Optimal K value: " << K << std::endl;

    std::shuffle(positions.begin(), positions.end(), gen);

    std::vector<TunePosition> validationPositions(positions.begin() + positions.size() * 0.9,
                                                  positions.end());
    positions.erase(positions.begin() + positions.size() * 0.9, positions.end());

    std::cout << "Starting tuning..." << std::endl;
    std::vector<double> m(bestParameters.size(), 0.0);
    std::vector<double> v(bestParameters.size(), 0.0);

    std::cout << "Calculating the initial loss..." << std::endl;
    double bestLoss =
        evaluationLoss(validationPositions, validationPositions.size(), maxEndTime, engine);
    double bestAverageLoss = bestLoss;

    std::cout << "Initial loss: " << bestLoss << std::endl;
    std::cout << "Finding the best parameters. This may take a while..." << std::endl;
    std::vector<std::vector<TunePosition>> batches = createBatches(positions);

    while (stopCounter <= patience) {
        iteration++;

        if (batches.empty()) {
            std::shuffle(positions.begin(), positions.end(), gen);
            batches = createBatches(positions);
        }

        std::vector<TunePosition> batch = batches.back();
        batches.pop_back();
        std::vector<double> gradients(bestParameters.size(), 0.0);

        for (TunePosition& pos : batch) {
            std::vector<TunePosition> position{pos};
            double loss = evaluationLoss(position, 1, maxEndTime, engine);

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                double oldParam = bestParameters[paramIndex];
                bestParameters[paramIndex] = oldParam + delta;
                updateEvalValues(bestParameters);
                double lossPlus = evaluationLoss(position, 1, maxEndTime, engine);

                bestParameters[paramIndex] = oldParam - delta;
                updateEvalValues(bestParameters);
                double lossMinus = evaluationLoss(position, 1, maxEndTime, engine);

                gradients[paramIndex] += (lossPlus - lossMinus) / (2 * delta);

                // reset
                bestParameters[paramIndex] = oldParam;
            }
        }

        for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
            gradients[paramIndex] /= static_cast<double>(batch.size());
            m[paramIndex] = beta1 * m[paramIndex] + (1.0 - beta1) * gradients[paramIndex];
            v[paramIndex] = beta2 * v[paramIndex] + (1.0 - beta2) * std::pow(
                                gradients[paramIndex], 2.0);
            double m_hat = m[paramIndex] / (1.0 - pow(beta1, iteration));
            double v_hat = v[paramIndex] / (1.0 - pow(beta2, iteration));
            bestParameters[paramIndex] -= learningRate * (m_hat / (sqrt(v_hat) + optimizerEpsilon));
        }

        updateEvalValues(bestParameters);
        double validationLoss =
            evaluationLoss(validationPositions, validationPositions.size(), maxEndTime, engine);
        smoothedValidationLoss.add(validationLoss);
        double averageValidationLoss = smoothedValidationLoss.getMA();

        if (iteration > epsilonWarmupIterations) {
            // Decay epsilon
            delta *= epsilonDecay;
            delta = std::max(delta, 1.0);
        }

        if (validationLoss < bestLoss) {
            bestLoss = validationLoss;
            exportNewEvalValues(bestParameters);
        }

        if (averageValidationLoss < bestAverageLoss) {
            bestAverageLoss = averageValidationLoss;
            stopCounter = 0;
        } else {
            stopCounter++;
        }

        std::cout << "Iteration: " << iteration << ", Val Loss: " << validationLoss
            << ", Best Loss: " << bestLoss << ", Avg Val Loss: " << averageValidationLoss
            << ", Lr: " << learningRate << ", Epsilon: " << delta << std::endl;
    }

    std::cout << "Best loss: " << bestLoss << std::endl;
}
} // namespace Zagreus
