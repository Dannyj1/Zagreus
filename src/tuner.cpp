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
#include "pst.h"
#include "search.h"

namespace Zagreus {
int epochs = 10;
float K = 0.0;

int batchSize = 256;
float learningRate = 0.1;
float delta = 1.0;
float optimizerEpsilon = 1e-6;
float beta1 = 0.9;
float beta2 = 0.999;
// 0 = random seed
long seed = 0;

Bitboard tunerBoard{};

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

float sigmoid(float x) { return 1.0 / (1.0 + pow(10.0, -K * x / 400.0)); }

float evaluationLoss(std::vector<TunePosition>& positions, int amountOfPositions) {
    float totalLoss = 0.0;

    for (TunePosition& pos : positions) {
        tunerBoard.setFromFenTuner(pos.fen);
        int evalScore = Evaluation(tunerBoard).evaluate();

        // All scores are from white's perspective
        if (tunerBoard.getMovingColor() == BLACK) {
            evalScore *= -1;
        }

        float loss = std::pow(pos.result - sigmoid(evalScore), 2);
        totalLoss += loss;
    }

    return (1.0 / amountOfPositions) * totalLoss;
}

float findOptimalK(std::vector<TunePosition>& positions) {
    const float phi = (1.0 + sqrt(5.0)) / 2.0;
    const float tolerance = 1e-6;

    float a = 0.0;
    float b = 2.0;

    float x1 = b - (b - a) / phi;
    float x2 = a + (b - a) / phi;

    K = x1;
    float f1 = evaluationLoss(positions, positions.size());
    K = x2;
    float f2 = evaluationLoss(positions, positions.size());

    while (std::abs(b - a) > tolerance) {
        if (f1 < f2) {
            b = x2;
            x2 = x1;
            x1 = b - (b - a) / phi;
            f2 = f1;
            K = x1;
            f1 = evaluationLoss(positions, positions.size());
        } else {
            a = x1;
            x1 = x2;
            x2 = a + (b - a) / phi;
            f1 = f2;
            K = x2;
            f2 = evaluationLoss(positions, positions.size());
        }
    }

    return (a + b) / 2.0;
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

        float result;
        std::string resultStr = posLine.substr(posLine.find(" c9 ") + 4, posLine.find(" c9 ") + 4);
        std::string fen = posLine.substr(0, posLine.find(" c9 "));

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

        // All scores are from white's perspective
        if (tunerBoard.getMovingColor() == BLACK) {
            evalScore *= -1;
        }

        TunePosition tunePos{fen, result, evalScore};
        positions.emplace_back(tunePos);
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

void exportNewEvalValues(std::vector<float>& bestParams) {
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

    if (seed == 0) {
        seed = rd();
    }

    gen = std::mt19937_64(seed);
    std::cout << "Using seed: " << seed << std::endl;

    ZagreusEngine engine;
    senjo::UCIAdapter adapter(engine);
    std::chrono::time_point<std::chrono::steady_clock> maxEndTime =
        std::chrono::time_point<std::chrono::steady_clock>::max();
    std::vector<TunePosition> positions = loadPositions(filePath, maxEndTime, engine, gen);

    engine.setTuning(false);

    std::vector<float> bestParameters = getBaseEvalValues();
    updateEvalValues(bestParameters);
    exportNewEvalValues(bestParameters);

    std::cout << "Finding the optimal K value..." << std::endl;
    K = findOptimalK(positions);
    std::cout << "Optimal K value: " << K << std::endl;

    std::shuffle(positions.begin(), positions.end(), gen);

    std::vector<TunePosition> validationPositions(positions.begin() + positions.size() * 0.9,
                                                  positions.end());
    positions.erase(positions.begin() + positions.size() * 0.9, positions.end());

    std::cout << "Starting tuning..." << std::endl;
    std::vector<float> m(bestParameters.size(), 0.0);
    std::vector<float> v(bestParameters.size(), 0.0);

    std::cout << "Calculating the initial loss..." << std::endl;
    float bestLoss =
        evaluationLoss(validationPositions, validationPositions.size());

    std::cout << "Initial loss: " << bestLoss << std::endl;
    std::cout << "Finding the best parameters. This may take a while..." << std::endl;
    std::vector<std::vector<TunePosition>> batches = createBatches(positions);
    int epoch = 1;

    while (epoch <= epochs) {
        int iteration = 0;
        std::shuffle(positions.begin(), positions.end(), gen);
        batches = createBatches(positions);
        int totalIterations = batches.size();

        while (batches.size() > 0) {
            iteration++;
            int percentDone = static_cast<int>(
                (iteration / static_cast<float>(totalIterations)) * 100);
            std::cout << "Epoch: " << epoch << ", Iteration: " << iteration << "/" <<
                totalIterations << " (" << percentDone << "%)" << std::endl;
            std::vector<TunePosition> batch = batches.back();
            batches.pop_back();
            std::vector<float> gradients(bestParameters.size(), 0.0);

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                float oldParam = bestParameters[paramIndex];

                bestParameters[paramIndex] = oldParam + delta;
                updateEvalValues(bestParameters);
                float fPlusDelta = evaluationLoss(batch, 1);

                bestParameters[paramIndex] = oldParam - delta;
                updateEvalValues(bestParameters);
                float fMinusDelta = evaluationLoss(batch, 1);

                bestParameters[paramIndex] = oldParam + 2 * delta;
                updateEvalValues(bestParameters);
                float fPlus2Delta = evaluationLoss(batch, 1);

                bestParameters[paramIndex] = oldParam - 2 * delta;
                updateEvalValues(bestParameters);
                float fMinus2Delta = evaluationLoss(batch, 1);

                gradients[paramIndex] += (
                    -fPlus2Delta + 8 * fPlusDelta - 8 * fMinusDelta + fMinus2Delta) / (12 * delta);

                // reset
                bestParameters[paramIndex] = oldParam;
            }

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                m[paramIndex] = beta1 * m[paramIndex] + (1.0 - beta1) * gradients[paramIndex];
                v[paramIndex] = beta2 * v[paramIndex] + (1.0 - beta2) * std::pow(
                                    gradients[paramIndex], 2.0);
                float mCorrected = m[paramIndex] / (1.0 - std::pow(beta1, iteration));
                float vCorrected = v[paramIndex] / (1.0 - std::pow(beta2, iteration));
                bestParameters[paramIndex] -= learningRate * mCorrected / (
                    sqrt(vCorrected) + optimizerEpsilon);
            }
        }

        updateEvalValues(bestParameters);
        float validationLoss =
            evaluationLoss(validationPositions, validationPositions.size());

        exportNewEvalValues(bestParameters);

        std::cout << "======== Epoch " << epoch << " Done ========" << std::endl;
        std::cout << "Epoch: " << epoch << ", Val Loss: " << validationLoss << std::endl;
        std::cout << "==============================" << std::endl;
        epoch++;
    }
}
} // namespace Zagreus