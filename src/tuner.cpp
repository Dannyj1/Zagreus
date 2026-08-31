/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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

#ifdef ZAGREUS_TUNER
#include "tuner.h"

#include <omp.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

#include "board.h"
#include "constants.h"
#include "eval.h"
#include "eval_base_values.h"
#include "types.h"
#include "uci.h"

namespace Zagreus {
const double learningRate = 1.0;
const double earlyStoppingMinDelta = 1e-6;
const int maxIterations = 200;
const int batchSize = 8192;
const int earlyStoppingPatience = 15;
const int earlyStoppingWarmup = 0;
const int saveFrequency = 5;
int seed = 42;

const double epsilon = 1e-8;
const double sigmoidGradientScale = 2.0 * std::log(10.0) / 400.0;

double K = 0.0;

std::vector<double> weights{};
std::vector<double> baseWeights{};

int pstWeightStart = 0;
int mobilityWeightStart;
int doubledPawnWeightStart;
int totalWeights;

void initializeWeights() {
    const int numPstWeights = GAME_PHASES * PIECE_TYPES * SQUARES;
    mobilityWeightStart = numPstWeights;

    const int numMobilityWeights = GAME_PHASES * PIECE_TYPES;
    doubledPawnWeightStart = mobilityWeightStart + numMobilityWeights;

    const int numDoubledPawnWeights = GAME_PHASES;
    totalWeights = doubledPawnWeightStart + numDoubledPawnWeights;

    weights.resize(totalWeights);
    baseWeights.resize(totalWeights);
    std::ranges::fill(weights, 0.0);

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const auto pieceType = static_cast<PieceType>(piece);
            const int* baseTable = phase == MIDGAME ? getBaseMidgameTable(pieceType) : getBaseEndgameTable(pieceType);

            for (int square = 0; square < SQUARES; ++square) {
                const int index = pstWeightStart + (phase * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;
                baseWeights[index] = baseMaterialValues[phase][piece] + baseTable[square];
            }
        }
    }

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const int index = mobilityWeightStart + (phase * PIECE_TYPES) + piece;
            baseWeights[index] = baseMobility[phase][piece];
        }
    }

    baseWeights[doubledPawnWeightStart + MIDGAME] = baseDoubledPawnPenalty[MIDGAME];
    baseWeights[doubledPawnWeightStart + ENDGAME] = baseDoubledPawnPenalty[ENDGAME];
}

std::vector<TraceCoefficient> createCoefficients(const EvalTrace& trace) {
    std::vector<TraceCoefficient> coefficients;

    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        for (int canonicalSquare = 0; canonicalSquare < SQUARES; ++canonicalSquare) {
            const int whiteCount = trace.pst[WHITE][piece][canonicalSquare ^ 56];
            const int blackCount = trace.pst[BLACK][piece][canonicalSquare];

            if (whiteCount == blackCount) {
                continue;
            }

            const int midgameIndex =
                pstWeightStart + (MIDGAME * PIECE_TYPES * SQUARES) + (piece * SQUARES) + canonicalSquare;
            const int endgameIndex =
                pstWeightStart + (ENDGAME * PIECE_TYPES * SQUARES) + (piece * SQUARES) + canonicalSquare;

            coefficients.push_back(
                {midgameIndex, endgameIndex, static_cast<int16_t>(whiteCount), static_cast<int16_t>(blackCount)});
        }
    }

    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int whiteCount = trace.mobility[WHITE][piece];
        const int blackCount = trace.mobility[BLACK][piece];

        if (whiteCount == blackCount) {
            continue;
        }

        const int midgameIndex = mobilityWeightStart + (MIDGAME * PIECE_TYPES) + piece;
        const int endgameIndex = mobilityWeightStart + (ENDGAME * PIECE_TYPES) + piece;

        coefficients.push_back(
            {midgameIndex, endgameIndex, static_cast<int16_t>(whiteCount), static_cast<int16_t>(blackCount)});
    }

    const int whiteDoubledPawns = trace.doubledPawns[WHITE];
    const int blackDoubledPawns = trace.doubledPawns[BLACK];

    if (whiteDoubledPawns != blackDoubledPawns) {
        coefficients.push_back({doubledPawnWeightStart + MIDGAME, doubledPawnWeightStart + ENDGAME,
                                static_cast<int16_t>(whiteDoubledPawns), static_cast<int16_t>(blackDoubledPawns)});
    }

    return coefficients;
}

double sigmoid(const double x) { return 1.0 / (1.0 + std::pow(10.0, -K * x / 400.0)); }

double evaluateFromCoefficients(const TunePosition& position) {
    double midgameScore = 0.0;
    double endgameScore = 0.0;

    for (const auto& coefficient : position.coefficients) {
        const int count = coefficient.whiteCount - coefficient.blackCount;

        midgameScore += (baseWeights[coefficient.midgameIndex] + weights[coefficient.midgameIndex]) * count;
        endgameScore += (baseWeights[coefficient.endgameIndex] + weights[coefficient.endgameIndex]) * count;
    }

    return ((midgameScore * (256 - position.phase)) + (endgameScore * position.phase)) / 256.0;
}

double calculateError(const std::vector<TunePosition>& positions) {
    double totalError = 0.0;

#pragma omp parallel for reduction(+ : totalError) default(none) shared(positions)
    for (const auto& position : positions) {
        const double evalScore = evaluateFromCoefficients(position);
        const double prediction = sigmoid(evalScore);
        const double error = position.result - prediction;
        totalError += error * error;
    }

    return totalError / static_cast<double>(positions.size());
}

std::vector<double> calculateGradients(const std::vector<TunePosition>& positions) {
    std::vector<double> gradients(weights.size(), 0.0);

#pragma omp parallel default(none) shared(positions, gradients)
    {
        std::vector<double> localGradients(gradients.size(), 0.0);

#pragma omp for nowait
        for (int i = 0; i < static_cast<int>(positions.size()); ++i) {
            const TunePosition& position = positions[i];
            const double evalScore = evaluateFromCoefficients(position);
            const double prediction = sigmoid(evalScore);
            const double errorGradient =
                -sigmoidGradientScale * K * (position.result - prediction) * prediction * (1.0 - prediction);

            const double midgameFactor = (256 - position.phase) / 256.0;
            const double endgameFactor = position.phase / 256.0;

            for (const auto& coefficient : position.coefficients) {
                const int count = coefficient.whiteCount - coefficient.blackCount;

                localGradients[coefficient.midgameIndex] += errorGradient * midgameFactor * count;
                localGradients[coefficient.endgameIndex] += errorGradient * endgameFactor * count;
            }
        }

#pragma omp critical
        {
            for (size_t i = 0; i < gradients.size(); ++i) {
                gradients[i] += localGradients[i];
            }
        }
    }

    const double N = static_cast<double>(positions.size());
    for (double& gradient : gradients) {
        gradient /= N;
    }

    return gradients;
}

std::vector<std::vector<TunePosition>> createBatches(const std::vector<TunePosition>& positions) {
    std::vector<std::vector<TunePosition>> batches;
    batches.reserve((positions.size() + batchSize - 1) / batchSize);

    for (size_t i = 0; i < positions.size(); i += batchSize) {
        std::vector<TunePosition> batch;
        batch.reserve(std::min((size_t)batchSize, positions.size() - i));
        for (size_t j = i; j < std::min(i + batchSize, positions.size()); j++) {
            batch.emplace_back(positions[j]);
        }
        batches.emplace_back(std::move(batch));
    }

    return batches;
}

void exportTunedValues(const std::string& outputPath, int finalEpoch, double trainingError, double validationError,
                       double testError) {
    std::ofstream fout(outputPath);
    if (!fout.is_open()) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return;
    }

    fout << "/*\n";
    fout << " * Tuned evaluation parameters\n";
    fout << " * Generated by Zagreus tuner\n";
    fout << " *\n";
    fout << " * Training metrics:\n";
    fout << " * - Final epoch: " << finalEpoch << "\n";
    fout << " * - Training error: " << trainingError << "\n";
    fout << " * - Validation error: " << validationError << "\n";
    fout << " * - Test error: " << testError << "\n";
    fout << " */\n\n";

    int materialValues[GAME_PHASES][PIECE_TYPES]{};
    int pstValues[GAME_PHASES][PIECE_TYPES][SQUARES]{};

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            int combinedValues[SQUARES];
            double sum = 0.0;

            for (int square = 0; square < SQUARES; ++square) {
                const int index = pstWeightStart + (phase * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;
                combinedValues[square] = static_cast<int>(std::round(baseWeights[index] + weights[index]));
                sum += combinedValues[square];
            }

            if (piece == KING) {
                materialValues[phase][piece] = 0;

                for (int square = 0; square < SQUARES; ++square) {
                    pstValues[phase][piece][square] = combinedValues[square];
                }
            } else {
                const int offset = static_cast<int>(std::round(sum / SQUARES));
                materialValues[phase][piece] = offset;

                for (int square = 0; square < SQUARES; ++square) {
                    pstValues[phase][piece][square] = combinedValues[square] - offset;
                }
            }
        }
    }

    fout << "// Material values\n";
    fout << "int evalMaterialValues[GAME_PHASES][PIECE_TYPES]{\n";
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        fout << materialValues[MIDGAME][piece];
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "}, // Midgame\n";

    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        fout << materialValues[ENDGAME][piece];
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "} // Endgame\n";
    fout << "};\n\n";

    fout << "// Mobility values\n";
    fout << "int evalMobility[GAME_PHASES][PIECE_TYPES]{\n";
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int index = mobilityWeightStart + (MIDGAME * PIECE_TYPES) + piece;
        fout << static_cast<int>(std::round(baseWeights[index] + weights[index]));
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "}, // Midgame\n";

    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int index = mobilityWeightStart + (ENDGAME * PIECE_TYPES) + piece;
        fout << static_cast<int>(std::round(baseWeights[index] + weights[index]));
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "} // Endgame\n";
    fout << "};\n\n";

    fout << "// Pawn structure\n";
    fout << "int evalDoubledPawnPenalty[GAME_PHASES] = {";
    fout << static_cast<int>(
                std::round(baseWeights[doubledPawnWeightStart + MIDGAME] + weights[doubledPawnWeightStart + MIDGAME]))
         << ", ";
    fout << static_cast<int>(
        std::round(baseWeights[doubledPawnWeightStart + ENDGAME] + weights[doubledPawnWeightStart + ENDGAME]));
    fout << "};\n\n";

    const std::string pieceNames[] = {"pawn", "knight", "bishop", "rook", "queen", "king"};

    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        fout << "// Midgame " << pieceNames[piece] << " PST\n";
        fout << "int mg_" << pieceNames[piece] << "_table[64] = {\n";
        for (int square = 0; square < SQUARES; ++square) {
            if (square % 8 == 0) fout << "    ";
            fout << pstValues[MIDGAME][piece][square];
            if (square < SQUARES - 1) fout << ", ";
            if (square % 8 == 7) fout << "\n";
        }
        fout << "};\n\n";
    }

    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        fout << "// Endgame " << pieceNames[piece] << " PST\n";
        fout << "int eg_" << pieceNames[piece] << "_table[64] = {\n";
        for (int square = 0; square < SQUARES; ++square) {
            if (square % 8 == 0) fout << "    ";
            fout << pstValues[ENDGAME][piece][square];
            if (square < SQUARES - 1) fout << ", ";
            if (square % 8 == 7) fout << "\n";
        }
        fout << "};\n\n";
    }

    fout.close();
    std::cout << "Tuned values exported to: " << outputPath << std::endl;
}

void gradientDescent(std::vector<TunePosition>& trainingSet, const std::vector<TunePosition>& validationSet,
                     const std::vector<TunePosition>& testSet, std::mt19937_64& gen) {
    std::vector<double> gradientAccumulator(weights.size(), 0.0);

    const double initialTrainingError = calculateError(trainingSet);
    const double initialValidationError = calculateError(validationSet);
    std::cout << "Initial Training Error: " << initialTrainingError
              << " - Initial Validation Error: " << initialValidationError << std::endl;

    double bestTrainingError = initialTrainingError;
    double bestValidationError = initialValidationError;
    std::vector<double> bestWeights = weights;
    int epochsWithoutImprovement = 0;
    int finalEpoch = 0;

    for (int iteration = 0; iteration < maxIterations; iteration++) {
        std::ranges::shuffle(trainingSet, gen);
        std::vector<std::vector<TunePosition>> batches = createBatches(trainingSet);

        double iterationError = 0.0;
        int numBatches = 0;

        for (const auto& batch : batches) {
            std::vector<double> gradients = calculateGradients(batch);

            for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
                gradientAccumulator[i] += gradients[i] * gradients[i];
                weights[i] -= learningRate * gradients[i] / (std::sqrt(gradientAccumulator[i]) + epsilon);
            }

            iterationError += calculateError(batch);
            numBatches++;
        }

        iterationError /= numBatches;

        const double validationError = calculateError(validationSet);
        std::cout << "Iteration " << iteration + 1 << "/" << maxIterations << " - Training Error: " << iterationError
                  << " - Validation Error: " << validationError << std::endl;

        if ((iteration + 1) % saveFrequency == 0) {
            exportTunedValues("tuned_values.h", iteration + 1, iterationError, validationError,
                              calculateError(testSet));
        }

        if (iteration > earlyStoppingWarmup) {
            if (validationError < bestValidationError - earlyStoppingMinDelta) {
                bestTrainingError = iterationError;
                bestValidationError = validationError;
                bestWeights = weights;
                epochsWithoutImprovement = 0;
            } else {
                epochsWithoutImprovement++;

                if (epochsWithoutImprovement >= earlyStoppingPatience) {
                    std::cout << "Early stopping triggered." << std::endl;
                    finalEpoch = iteration + 1;
                    break;
                }
            }
        }

        finalEpoch = iteration + 1;
    }

    weights = bestWeights;

    const double testError = calculateError(testSet);
    std::cout << "Final test error: " << testError << std::endl;

    exportTunedValues("tuned_values.h", finalEpoch, bestTrainingError, bestValidationError, testError);
}

std::vector<TunePosition> loadPositions(const std::string& filePath) {
    std::cout << "Loading positions..." << std::endl;
    std::vector<std::string> lines;
    std::ifstream fin(filePath);

    if (!fin.is_open()) {
        std::cerr << "Failed to open positions file: " << filePath << std::endl;
        return {};
    }

    int win = 0;
    int loss = 0;
    int draw = 0;

    std::string line;
    while (std::getline(fin, line)) {
        lines.emplace_back(line);
    }

    std::vector<TunePosition> parsedPositions(lines.size());
    std::vector<bool> isValid(lines.size(), false);

#pragma omp parallel for reduction(+ : win, loss, draw) default(none) shared(lines, parsedPositions, isValid)
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string posLine = lines[i];
        if (posLine.empty() || posLine == " ") {
            continue;
        }

        double result;
        size_t c9_pos = posLine.find(" c9 ");
        if (c9_pos == std::string::npos) continue;

        std::string resultStr = posLine.substr(c9_pos + 4);
        std::string fen = posLine.substr(0, c9_pos);
        Board board{};

        if (!board.setFromFEN(fen) || board.isDraw() || board.isKingInCheck<WHITE>() || board.isKingInCheck<BLACK>()) {
            continue;
        }

        Evaluation eval{board};
        eval.evaluate();

        std::erase(resultStr, '"');
        std::erase(resultStr, ';');
        std::erase(resultStr, ' ');

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

        TunePosition tunePos;
        tunePos.result = result;
        tunePos.phase = eval.trace.phase;
        tunePos.coefficients = createCoefficients(eval.trace);
        parsedPositions[i] = tunePos;
        isValid[i] = true;
    }

    std::vector<TunePosition> positions;
    positions.reserve(win + loss + draw);

    for (size_t i = 0; i < lines.size(); ++i) {
        if (isValid[i]) {
            positions.push_back(std::move(parsedPositions[i]));
        }
    }

    std::cout << "Loaded " << positions.size() << " positions." << std::endl;
    std::cout << "Win: " << win << ", Loss: " << loss << ", Draw: " << draw << std::endl;
    return positions;
}

double findOptimalK(const std::vector<TunePosition>& positions) {
    double a = 0.0001;
    double b = 10.0;
    const double invphi = (std::sqrt(5.0) - 1.0) / 2.0;
    const double invphi2 = (3.0 - std::sqrt(5.0)) / 2.0;
    constexpr double tol = 1e-4;

    auto averageError = [&positions](const double candidateK) -> double {
        const double oldK = K;
        K = candidateK;
        const double error = calculateError(positions);
        K = oldK;
        return error;
    };

    double x1 = a + invphi2 * (b - a);
    double x2 = a + invphi * (b - a);
    double f1 = averageError(x1);
    double f2 = averageError(x2);

    const double startingError = averageError(K);
    std::cout << "Starting error with K=" << K << ": " << startingError << std::endl;

    while (b - a > tol) {
        if (f1 < f2) {
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = a + invphi2 * (b - a);
            f1 = averageError(x1);
            std::cout << "Testing K=" << x1 << ", error=" << f1 << std::endl;
        } else {
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + invphi * (b - a);
            f2 = averageError(x2);
            std::cout << "Testing K=" << x2 << ", error=" << f2 << std::endl;
        }
    }

    const double optimalK = (a + b) / 2.0;
    const double finalError = averageError(optimalK);
    std::cout << "Found optimal K=" << optimalK << " with error: " << finalError << std::endl;
    return optimalK;
}

void startTuning(std::string filePath) {
    if (seed == 0) {
        std::random_device rd;
        seed = rd();
    }

    std::mt19937_64 gen = std::mt19937_64(seed);
    Engine engine{};
    engine.registerOptions();
    engine.doSetup();
    initializeBasePst();
    initializeWeights();

    std::vector<TunePosition> trainingSet = loadPositions(filePath);
    if (trainingSet.empty()) {
        std::cout << "Error: No training positions loaded. Tuning cannot start." << std::endl;
        return;
    }

    std::ranges::shuffle(trainingSet, gen);

    std::vector<TunePosition> validationSet;
    std::vector<TunePosition> testSet;

    const int64_t validationSetSize = trainingSet.size() * 0.1;
    const int64_t testSetSize = trainingSet.size() * 0.1;

    validationSet.assign(trainingSet.begin(), trainingSet.begin() + validationSetSize);
    testSet.assign(trainingSet.begin() + validationSetSize, trainingSet.begin() + validationSetSize + testSetSize);
    trainingSet.erase(trainingSet.begin(), trainingSet.begin() + validationSetSize + testSetSize);

    std::cout << "Training set size: " << trainingSet.size() << std::endl;
    std::cout << "Validation set size: " << validationSet.size() << std::endl;
    std::cout << "Test set size: " << testSet.size() << std::endl;

    std::cout << "Finding the optimal K value..." << std::endl;
    K = findOptimalK(trainingSet);
    std::cout << "Optimal K value: " << K << std::endl;

    gradientDescent(trainingSet, validationSet, testSet, gen);
}
}  // namespace Zagreus
#endif
