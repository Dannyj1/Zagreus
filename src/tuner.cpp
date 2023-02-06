/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>

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
    float K = 0.0;

    int batchSize = 512;
    float learningRate = 0.4f;
    float epsilon = 6.0f;
    float optimizerEpsilon = 1e-6f;
    float epsilonDecay = 0.97f;
    float beta1 = 0.9f;
    float beta2 = 0.999f;
    int epsilonWarmupIterations = 0;

    float sigmoid(float x) {
        return 1.0f / (1.0f + pow(10.0f, -K * x / 400.0f));
    }

    float evaluationLoss(std::vector<TunePosition> &positions, int amountOfPositions, std::chrono::time_point<std::chrono::high_resolution_clock> &maxEndTime, ZagreusEngine &engine) {
        float totalLoss = 0.0f;

        for (TunePosition &pos : positions) {
            tunerBoard.setFromFenTuner(pos.fen);
//            Move rootMove{};
//            int qScore = searchManager.quiesce(tunerBoard, -9999999, 9999999, rootMove, rootMove, maxEndTime, engine);
            int evalScore = searchManager.evaluate(tunerBoard, maxEndTime, engine);
            float loss = pos.result - sigmoid((float) evalScore);
            totalLoss += loss * loss;
        }

        return totalLoss / (float) amountOfPositions;
    }

    float findOptimalK(std::vector<TunePosition> &positions, std::chrono::time_point<std::chrono::high_resolution_clock> &maxEndTime, ZagreusEngine &engine) {
        std::vector<int> evalScores(positions.size());

        for (TunePosition &pos : positions) {
            tunerBoard.setFromFenTuner(pos.fen);
//            Move rootMove{};
//            int qScore = searchManager.quiesce(tunerBoard, -9999999, 9999999, rootMove, rootMove, maxEndTime, engine);
            int evalScore = searchManager.evaluate(tunerBoard, maxEndTime, engine);
            pos.score = evalScore;
        }

        // Find optimal K
        float bestK = 0.0f;
        float bestLoss = 9999999.0f;
        float oldK = K;

        for (float k = 0.0f; k <= 2.0f; k += 0.001f) {
            K = k;

            float totalLoss = 0.0f;
            for (TunePosition &pos : positions) {
                float loss = pos.result - sigmoid((float) pos.score);
                totalLoss += loss * loss;
            }

            float loss = totalLoss / (float) positions.size();

            if (loss < bestLoss) {
                bestLoss = loss;
                bestK = k;
            }
        }

        K = oldK;
        return bestK;
    }

    std::vector<TunePosition> loadPositions(char* filePath) {
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

            float result;
            std::string resultStr = posLine.substr(posLine.find(" c9 ") + 4, posLine.find(" c9 ") + 4);
            std::string fen = posLine.substr(0, posLine.find(" c9 "));

            // Remove " and ; from result
            resultStr.erase(std::remove(resultStr.begin(), resultStr.end(), '"'), resultStr.end());
            resultStr.erase(std::remove(resultStr.begin(), resultStr.end(), ';'), resultStr.end());

            if (resultStr == "1" || resultStr == "1-0") {
                result = 1.0f;
            } else if (resultStr == "0" || resultStr == "0-1") {
                result = 0.0f;
            } else {
                result = 0.5f;
            }

            if (!tunerBoard.setFromFen(fen) || !tunerBoard.hasMinorOrMajorPieces() || tunerBoard.isDraw()
            || tunerBoard.isWinner<PieceColor::WHITE>() || tunerBoard.isWinner<PieceColor::BLACK>()
            || tunerBoard.isKingInCheck<PieceColor::WHITE>() || tunerBoard.isKingInCheck<PieceColor::BLACK>()) {
                continue;
            }

            positions.emplace_back(TunePosition{fen, result});
        }

        return positions;
    }

    void exportNewEvalValues(std::vector<float> &bestParams) {
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
        std::vector<TunePosition> trainingSet = loadPositions(filePath);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        engine.setTuning(false);

        std::vector<float> bestParameters = getBaseEvalValues();
        updateEvalValues(bestParameters);
        exportNewEvalValues(bestParameters);

        std::cout << "Finding the optimal K value..." << std::endl;
        K = findOptimalK(trainingSet, maxEndTime, engine);
        std::cout << "Optimal K value: " << K << std::endl;

        std::cout << "Starting tuning..." << std::endl;
        std::vector<float> m(bestParameters.size(), 0.0f);
        std::vector<float> v(bestParameters.size(), 0.0f);

        std::cout << "Splitting the trainingSet into a training set and a validation set..." << std::endl;
        // 10% validation set, 90% training set
        int validationSetSize = trainingSet.size() / 10;

        std::vector<TunePosition> validationSet(validationSetSize);

        std::shuffle(trainingSet.begin(), trainingSet.end(), gen);
        std::copy(trainingSet.begin(), trainingSet.begin() + validationSetSize, validationSet.begin());
        trainingSet.erase(trainingSet.begin(), trainingSet.begin() + validationSetSize);

        std::cout << "Calculating the initial loss..." << std::endl;
        float bestLoss = evaluationLoss(validationSet, validationSetSize, maxEndTime, engine);

        std::cout << "Initial loss: " << bestLoss << std::endl;
        std::cout << "Finding the best parameters. This may take a while..." << std::endl;

        while (stopCounter <= 20) {
            iteration++;
            std::vector<TunePosition> batch(batchSize);
            std::vector<float> gradients(bestParameters.size(), 0.0f);

            for (int i = 0; i < batchSize; i++) {
                batch.emplace_back(trainingSet[dis(gen) % trainingSet.size()]);
            }

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                float oldParam = bestParameters[paramIndex];
                bestParameters[paramIndex] += epsilon;
                updateEvalValues(bestParameters);
                float lossPlus = evaluationLoss(batch, batchSize, maxEndTime, engine);

                bestParameters[paramIndex] -= 2 * epsilon;
                updateEvalValues(bestParameters);
                float lossMinus = evaluationLoss(batch, batchSize, maxEndTime, engine);

                gradients[paramIndex] = (lossPlus - lossMinus) / (2 * epsilon);
                // reset
                bestParameters[paramIndex] = oldParam;
            }

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                m[paramIndex] = beta1 * m[paramIndex] + (1.0f - beta1) * gradients[paramIndex];
                v[paramIndex] = beta2 * v[paramIndex] + (1.0f - beta2) * std::pow(gradients[paramIndex], 2.0f);
                float m_hat = m[paramIndex] / (1.0f - pow(beta1, (float) iteration));
                float v_hat = v[paramIndex] / (1.0f - pow(beta2, (float) iteration));
                bestParameters[paramIndex] -= learningRate * (m_hat / (sqrt(v_hat) + optimizerEpsilon));
            }

            updateEvalValues(bestParameters);
            float newLoss = evaluationLoss(validationSet, validationSetSize, maxEndTime, engine);

            if (iteration > epsilonWarmupIterations) {
                // Decay epsilon
                epsilon *= epsilonDecay;
                epsilon = std::max(epsilon, 1.0f);
            }

            if (newLoss < bestLoss) {
                bestLoss = newLoss;
                stopCounter = 0;
                exportNewEvalValues(bestParameters);
            } else {
                stopCounter++;
            }

            float trainingLoss = evaluationLoss(trainingSet, trainingSet.size(), maxEndTime, engine);
            std::cout << "Iteration: " << iteration << ", Training Loss: " << trainingLoss << ", Val Loss: " << newLoss << ", Best Val Loss: " << bestLoss << ", Lr: " << learningRate << ", Epsilon: " << epsilon << std::endl;
        }

        std::cout << "Best loss: " << bestLoss << std::endl;
    }
}
