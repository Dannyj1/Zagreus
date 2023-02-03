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

#include "tuner.h"
#include "features.h"
#include "bitboard.h"
#include "search.h"
#include "../senjo/UCIAdapter.h"

#include <cmath>
#include <fstream>
#include <iostream>

#include "features.h"

namespace Zagreus {

    float sigmoid(float x) {
        return 1.0f / (1.0f + pow(10.0f, -K * x / 400.0f));
    }

    float evaluationError(std::vector<TunePosition> &positions) {
        ZagreusEngine engine;
        senjo::UCIAdapter adapter(engine);
        float totalError = 0.0f;
        std::chrono::time_point<std::chrono::high_resolution_clock> maxEndTime = std::chrono::time_point<std::chrono::high_resolution_clock>::max();

        for (TunePosition &pos : positions) {
            Bitboard bb{};
            bb.setFromFen(pos.fen);

            int evalScore = searchManager.evaluate(bb, maxEndTime, engine);
            float error = pos.result - sigmoid((float) evalScore);
            totalError += pow(error, 2.0f);
        }

        return (1.0f / (float) positions.size()) * totalError;
    }

    std::vector<TunePosition> loadPositions(char* filePath) {
        std::vector<TunePosition> positions;
        std::vector<std::string> lines;
        std::ifstream fin(filePath);

        std::string line;
        while (std::getline(fin, line)) {
            lines.emplace_back(line);
        }

        for (std::string &posLine : lines) {
            float result;
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

            Bitboard bb{};

            if (!bb.setFromFen(fen) || !bb.hasMinorOrMajorPieces()) {
                continue;
            }

            positions.emplace_back(TunePosition{fen, result});
        }

        return positions;
    }

    void exportNewEvalValues(std::vector<int> vector1);

    void startTuning(char* filePath) {
        std::vector<TunePosition> positions = loadPositions(filePath);
        std::vector<int> bestParameters = getBaseEvalValues();
        float bestError = evaluationError(positions);
        bool hasImproved = true;

        std::cout << "Initial error: " << bestError << std::endl;

        while (hasImproved) {
            hasImproved = false;

            for (int paramIndex = 0; paramIndex < bestParameters.size(); paramIndex++) {
                std::vector<int> newParameters = bestParameters;
                newParameters[paramIndex] += 1;
                updateEvalValues(newParameters);
                float newError = evaluationError(positions);

                if (newError < bestError) {
                    bestError = newError;
                    bestParameters = newParameters;
                    hasImproved = true;
                    std::cout << "Improved error: " << bestError << std::endl;
                } else {
                    newParameters[paramIndex] -= 2;
                    updateEvalValues(newParameters);
                    newError = evaluationError(positions);

                    if (newError < bestError) {
                        bestError = newError;
                        bestParameters = newParameters;
                        hasImproved = true;
                        std::cout << "Improved error: " << bestError << std::endl;
                    }
                }
            }
        }

        std::cout << "Best error: " << bestError << std::endl;
        exportNewEvalValues(bestParameters);
    }

    void exportNewEvalValues(std::vector<int> bestParams) {
        std::ofstream fout("tuned_params.txt");

        fout << "int evalValues[" << bestParams.size() << "] = { ";
        for (int i = 0; i < bestParams.size(); i++) {
            fout << bestParams[i];

            if (i != bestParams.size() - 1) {
                fout << ", ";
            }
        }
        fout << " };" << std::endl;
    }
}
