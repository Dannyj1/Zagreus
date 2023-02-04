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

    float sigmoid(float x) {
        return 1.0f / (1.0f + pow(10.0f, -K * x / 400.0f));
    }

    float evaluationError(std::vector<TunePosition> &positions, int amountOfPositions, std::chrono::time_point<std::chrono::high_resolution_clock> &maxEndTime, ZagreusEngine &engine) {
        float totalError = 0.0f;

        for (TunePosition &pos : positions) {
            tunerBoard.setFromFenTuner(pos.fen);
            Move rootMove{};
            int qScore = searchManager.quiesce(tunerBoard, -9999999, 9999999, rootMove, rootMove, maxEndTime, engine);
//            int evalScore = searchManager.evaluate(tunerBoard, maxEndTime, engine);
            float error = pos.result - sigmoid((float) qScore);
            totalError += error * error;
        }

        return (1.0f / (float) amountOfPositions) * totalError;
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

    // Initial error: 0.31197
    void startTuning(char* filePath) {
        ZagreusEngine engine;
        senjo::UCIAdapter adapter(engine);
        std::chrono::time_point<std::chrono::high_resolution_clock> maxEndTime = std::chrono::time_point<std::chrono::high_resolution_clock>::max();

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        engine.setTuning(true);

        std::cout << "Starting tuning..." << std::endl;
        std::vector<TunePosition> positions = loadPositions(filePath);
        std::vector<float> bestParameters = getBaseEvalValues();
        updateEvalValues(bestParameters);

        std::cout << "Calculating the initial error..." << std::endl;
        int amountOfPositions = positions.size();
        float bestError = evaluationError(positions, amountOfPositions, maxEndTime, engine);

        std::cout << "Initial error: " << bestError << std::endl;
        std::cout << "Finding the best parameters. This may take a while..." << std::endl;

        exportNewEvalValues(bestParameters);

        int stopCounter = 0;
        int iterator = 0;
        int batchSize = 64;
        float learningRate = 1.0f;

        while (stopCounter < 20) {
            iterator++;
            std::vector<TunePosition> batch(batchSize);
            std::vector<float> gradients(bestParameters.size(), 0.0f);

            for (int i = 0; i < batchSize; i++) {
                batch.emplace_back(positions[dis(gen) % amountOfPositions]);
            }

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                int oldParam = bestParameters[paramIndex];

                bestParameters[paramIndex] += 1;
                updateEvalValues(bestParameters);
                float errorPlus = evaluationError(batch, batchSize, maxEndTime, engine);

                bestParameters[paramIndex] -= 2;
                updateEvalValues(bestParameters);
                float errorMinus = evaluationError(batch, batchSize, maxEndTime, engine);

                bestParameters[paramIndex] = oldParam;
                float gradient = (errorPlus - errorMinus) / 2.0f;
                gradients[paramIndex] = gradient;
            }

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                bestParameters[paramIndex] += gradients[paramIndex] * learningRate;
            }

            updateEvalValues(bestParameters);
            float newError = evaluationError(positions, amountOfPositions, maxEndTime, engine);

            // Decay learning rate
            learningRate *= 0.999f;

            std::cout << "Iteration: " << iterator << ", Error: " << newError << std::endl;

            if (newError < bestError) {
                bestError = newError;
                stopCounter = 0;
                exportNewEvalValues(bestParameters);
            } else {
                stopCounter++;
            }
        }

        std::cout << "Best error: " << bestError << std::endl;
    }
}
