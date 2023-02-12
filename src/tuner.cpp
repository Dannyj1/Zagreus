/*
 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <map>

#include "tuner.h"
#include "features.h"
#include "bitboard.h"
#include "search.h"
#include "../senjo/UCIAdapter.h"
#include "pst.h"
#include "movegen.h"

namespace Zagreus {

    Bitboard tunerBoard{};

    int stopCounter = 0;
    int iteration = 0;
    double K = 0.0;

    int batchSize = 128;
    double learningRate = 0.4;
    double epsilon = 6.0;
    double optimizerEpsilon = 1e-6;
    double epsilonDecay = 0.98;
    double beta1 = 0.9;
    double beta2 = 0.999;
    int epsilonWarmupIterations = 0;

    std::vector<std::vector<TunePosition>> createBatches(std::vector<TunePosition> v, std::mt19937_64 gen) {
        // Create random batches of batchSize positions
        std::vector<std::vector<TunePosition>> batches;

        std::shuffle(v.begin(), v.end(), gen);
        for (int i = 0; i < v.size(); i += batchSize) {
            std::vector<TunePosition> batch;
            for (int j = i; j < i + batchSize && j < v.size(); j++) {
                batch.push_back(v[j]);
            }
            batches.push_back(batch);
        }

        return batches;
    }

    double sigmoid(double x) {
        return 1.0 / (1.0 + pow(10.0, -K * x / 400.0));
    }

    double evaluationLoss(std::vector<TunePosition> &positions, int amountOfPositions, std::chrono::time_point<std::chrono::high_resolution_clock> &maxEndTime, ZagreusEngine &engine) {
        double totalLoss = 0.0;

        for (TunePosition &pos : positions) {
            tunerBoard.setFromFenTuner(pos.fen);
            Move rootMove{};
//            int evalScore = searchManager.evaluate(tunerBoard, maxEndTime, engine);
            int qScore = searchManager.quiesce(tunerBoard, -9999999, 9999999, rootMove, rootMove, maxEndTime, engine);
            double loss = pos.result - sigmoid((double) qScore);
            totalLoss += loss * loss;
        }

        return 1.0 / (2.0 * (double) amountOfPositions) * totalLoss;
    }

    double findOptimalK(std::vector<TunePosition> &positions, std::chrono::time_point<std::chrono::high_resolution_clock> &maxEndTime, ZagreusEngine &engine) {
        std::vector<int> evalScores(positions.size());

        for (TunePosition &pos : positions) {
            tunerBoard.setFromFenTuner(pos.fen);
            Move rootMove{};
            int qScore = searchManager.quiesce(tunerBoard, -9999999, 9999999, rootMove, rootMove, maxEndTime, engine);
//            int evalScore = searchManager.evaluate(tunerBoard, maxEndTime, engine);
            pos.score = qScore;
        }

        // Find optimal K
        double bestK = 0.0;
        double bestLoss = 9999999.0;
        double oldK = K;

        for (double k = 0.0; k <= 2.0; k += 0.001) {
            K = k;
            double totalLoss = 0.0;

            for (TunePosition &pos : positions) {
                double loss = pos.result - sigmoid((double) pos.score);
                totalLoss += loss * loss;
            }

            double loss = totalLoss / (double) positions.size();

            if (loss < bestLoss) {
                bestLoss = loss;
                bestK = k;
            }
        }

        K = oldK;
        return bestK;
    }

    std::vector<TunePosition> loadPositions(char* filePath, std::chrono::time_point<std::chrono::high_resolution_clock> &maxEndTime, ZagreusEngine &engine) {
        std::cout << "Loading positions..." << std::endl;
        std::vector<TunePosition> positions;
        std::vector<std::string> lines;
        std::ifstream fin(filePath);

        std::string line;
        while (std::getline(fin, line)) {
            lines.emplace_back(line);
        }

        for (std::string &posLine : lines) {
            if (posLine.empty() || posLine == " ") {
                continue;
            }

            double result;
            std::string resultStr = posLine.substr(posLine.find(" c9 ") + 4, posLine.find(" c9 ") + 4);
            std::string fen = posLine.substr(0, posLine.find(" c9 "));

            // Remove " and ; from result
            resultStr.erase(std::remove(resultStr.begin(), resultStr.end(), '"'), resultStr.end());
            resultStr.erase(std::remove(resultStr.begin(), resultStr.end(), ';'), resultStr.end());

            if (resultStr == "1" || resultStr == "1-0") {
                result = 1.0;
            } else if (resultStr == "0" || resultStr == "0-1") {
                result = 0.0;
            } else {
                result = 0.5;
            }

            if (!tunerBoard.setFromFen(fen) || !tunerBoard.hasMinorOrMajorPieces<PieceColor::WHITE>()
                    || !tunerBoard.hasMinorOrMajorPieces<PieceColor::BLACK>() || tunerBoard.isDraw()
                    || tunerBoard.isWinner<PieceColor::WHITE>() || tunerBoard.isWinner<PieceColor::BLACK>()
                    || tunerBoard.isKingInCheck<PieceColor::WHITE>() || tunerBoard.isKingInCheck<PieceColor::BLACK>()
                    || popcnt(tunerBoard.getColorBoard<PieceColor::WHITE>()) <= 4 || popcnt(tunerBoard.getColorBoard<PieceColor::BLACK>()) <= 4) {
                continue;
            }

            positions.emplace_back(TunePosition{fen, result, searchManager.evaluate(tunerBoard, maxEndTime, engine)});
        }

        return positions;
    }

    void exportNewEvalValues(std::vector<double> &bestParams) {
        std::ofstream fout("tuned_params.txt");

        // Declare pieceNames
        std::string pieceNames[6] = {"Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};

        fout << "int evalValues[" << getEvalFeatureSize() << "] = { ";
        for (int i = 0; i < getEvalFeatureSize(); i++) {
            fout << (int) bestParams[i];

            if (i != bestParams.size() - 1) {
                fout << ", ";
            }
        }
        fout << " };" << std::endl << std::endl;

        // Write the 6 piece square tables to a file (2 tables per piece, a midgame and endgame table declared like this:  int midgamePawnTable[64] and int endgamePawnTable[64]
        // The 6 midgame tables are declared first, then the 6 endgame tables
        int pstSize = getMidgameValues().size();
        for (int i = 0; i < 6; i++) {
            fout << "int midgame" << pieceNames[i] << "Table[64] = { ";
            for (int j = 0; j < 64; j++) {
                fout << (int) (bestParams[getEvalFeatureSize() + i * 64 + j]);

                if (j != 63) {
                    fout << ", ";
                }
            }
            fout << " };" << std::endl;

            fout << "int endgame" << pieceNames[i] << "Table[64] = { ";
            for (int j = 0; j < 64; j++) {
                fout << (int) (bestParams[getEvalFeatureSize() + pstSize + i * 64 + j]);

                if (j != 63) {
                    fout << ", ";
                }
            }
            fout << " };" << std::endl << std::endl;
        }
    }

    void startTuning(char* filePath) {
        ZagreusEngine engine;
        senjo::UCIAdapter adapter(engine);
        std::chrono::time_point<std::chrono::high_resolution_clock> maxEndTime = std::chrono::time_point<std::chrono::high_resolution_clock>::max();
        std::vector<TunePosition> positions = loadPositions(filePath, maxEndTime, engine);
        std::vector<TunePosition> validationPositions(positions.begin() + positions.size() * 0.9, positions.end());
        positions.erase(positions.begin() + positions.size() * 0.9, positions.end());

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        engine.setTuning(false);

        std::vector<double> bestParameters = getBaseEvalValues();
        updateEvalValues(bestParameters);
        exportNewEvalValues(bestParameters);

        std::cout << "Finding the optimal K value..." << std::endl;
        K = findOptimalK(positions, maxEndTime, engine);
        std::cout << "Optimal K value: " << K << std::endl;

        std::cout << "Starting tuning..." << std::endl;
        std::vector<double> m(bestParameters.size(), 0.0);
        std::vector<double> v(bestParameters.size(), 0.0);

        std::shuffle(positions.begin(), positions.end(), gen);

        std::cout << "Calculating the initial loss..." << std::endl;
        double bestLoss = evaluationLoss(validationPositions, validationPositions.size(), maxEndTime, engine);

        std::cout << "Initial loss: " << bestLoss << std::endl;
        std::cout << "Finding the best parameters. This may take a while..." << std::endl;
        std::vector<std::vector<TunePosition>> batches = createBatches(positions, gen);

        while (stopCounter <= 30) {
            iteration++;

            if (batches.empty()) {
                batches = createBatches(positions, gen);
            }

            std::vector<TunePosition> batch = batches.back();
            batches.pop_back();
            std::vector<double> gradients(bestParameters.size(), 0.0);

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                double oldParam = bestParameters[paramIndex];
                bestParameters[paramIndex] += epsilon;
                updateEvalValues(bestParameters);
                double lossPlus = evaluationLoss(batch, batchSize, maxEndTime, engine);

                bestParameters[paramIndex] -= 2 * epsilon;
                updateEvalValues(bestParameters);
                double lossMinus = evaluationLoss(batch, batchSize, maxEndTime, engine);

                gradients[paramIndex] = (lossPlus - lossMinus) / (2 * epsilon);
                // reset
                bestParameters[paramIndex] = oldParam;
            }

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                m[paramIndex] = beta1 * m[paramIndex] + (1.0 - beta1) * gradients[paramIndex];
                v[paramIndex] = beta2 * v[paramIndex] + (1.0 - beta2) * std::pow(gradients[paramIndex], 2.0);
                double m_hat = m[paramIndex] / (1.0 - pow(beta1, (double) iteration));
                double v_hat = v[paramIndex] / (1.0 - pow(beta2, (double) iteration));
                bestParameters[paramIndex] -= learningRate * (m_hat / (sqrt(v_hat) + optimizerEpsilon));
            }

            updateEvalValues(bestParameters);
            double newLoss = evaluationLoss(validationPositions, validationPositions.size(), maxEndTime, engine);

            if (iteration > epsilonWarmupIterations) {
                // Decay epsilon
                epsilon *= epsilonDecay;
                epsilon = std::max(epsilon, 1.0);
            }

            if (newLoss < bestLoss) {
                bestLoss = newLoss;
                stopCounter = 0;
                exportNewEvalValues(bestParameters);
            } else {
                stopCounter++;
            }

            std::cout << "Iteration: " << iteration << ", Val Loss: " << newLoss << ", Best Loss: " << bestLoss << ", Lr: " << learningRate << ", Epsilon: " << epsilon << std::endl;
        }

        std::cout << "Best loss: " << bestLoss << std::endl;
    }
}
