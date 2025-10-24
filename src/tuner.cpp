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
#include "eval_features.h"
#include "pst.h"
#include "types.h"
#include "uci.h"

namespace Zagreus {
const double learningRate = 0.1;
const double earlyStoppingMinDelta = 1e-4;
const int maxIterations = 200;
const int batchSize = 256;
const int earlyStoppingPatience = 5;
const int earlyStoppingWarmup = 0;
const int saveEvery = 5;
int seed = 42;

const int delta = 1;
const double beta1 = 0.9;
const double beta2 = 0.999;
const double epsilon = 1e-8;

double K = 0.0;

std::vector<double> weights{};
std::vector<double> initialWeights{};

int materialWeightStart = 0;
int pstWeightStart;
int mobilityWeightStart;
int doubledPawnWeightStart;
int totalWeights;

void initializeWeights() {
    const int numMaterialWeights = GAME_PHASES * PIECE_TYPES;
    pstWeightStart = numMaterialWeights;

    const int numPstWeights = GAME_PHASES * PIECE_TYPES * SQUARES;
    mobilityWeightStart = pstWeightStart + numPstWeights;

    const int mobilityWeights = GAME_PHASES * PIECE_TYPES;
    doubledPawnWeightStart = mobilityWeightStart + mobilityWeights;

    const int doubledPawnWeights = GAME_PHASES;
    totalWeights = doubledPawnWeightStart + doubledPawnWeights;

    weights.resize(totalWeights);
    initialWeights.resize(totalWeights);
    std::ranges::fill(weights, 0.0);
}

struct TunedEvaluationParameters {
    int evalMaterialValues[GAME_PHASES][PIECE_TYPES];
    int midgamePstTable[PIECES][SQUARES];
    int endgamePstTable[PIECES][SQUARES];
    int evalMobility[GAME_PHASES][PIECE_TYPES];
    int evalDoubledPawnPenalty[GAME_PHASES];
};

TunedEvaluationParameters getTunedParameters(const std::vector<double>& newWeights) {
    TunedEvaluationParameters params{};
    auto weightsCopy = newWeights;

    for (int i = materialWeightStart; i < pstWeightStart; ++i) {
        if (weightsCopy[i] < 0) {
            weightsCopy[i] = 0;
        }
    }

    for (int i = mobilityWeightStart; i < doubledPawnWeightStart; ++i) {
        if (weightsCopy[i] < 0) {
            weightsCopy[i] = 0;
        }
    }

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            params.evalMaterialValues[phase][piece] = static_cast<int>(std::round(
                baseMaterialValues[phase][piece] + weightsCopy[materialWeightStart + (phase * PIECE_TYPES) + piece]));
        }
    }

    for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        for (Square square = A1; square <= H8; square++) {
            const PieceType pieceType = getPieceType(piece);
            const PieceColor color = getPieceColor(piece);
            const int mgIndex = pstWeightStart + (MIDGAME * PIECE_TYPES * SQUARES) + (pieceType * SQUARES) +
                                (color == WHITE ? square ^ 56 : square);
            const int egIndex = pstWeightStart + (ENDGAME * PIECE_TYPES * SQUARES) + (pieceType * SQUARES) +
                                (color == WHITE ? square ^ 56 : square);

            const int baseMgPst =
                color == WHITE ? getBaseMidgameTable(pieceType)[square ^ 56] : getBaseMidgameTable(pieceType)[square];
            const int baseEgPst =
                color == WHITE ? getBaseEndgameTable(pieceType)[square ^ 56] : getBaseEndgameTable(pieceType)[square];

            params.midgamePstTable[piece][square] = params.evalMaterialValues[MIDGAME][pieceType] +
                                                    static_cast<int>(std::round(baseMgPst + weightsCopy[mgIndex]));
            params.endgamePstTable[piece][square] = params.evalMaterialValues[ENDGAME][pieceType] +
                                                    static_cast<int>(std::round(baseEgPst + weightsCopy[egIndex]));
        }
    }

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const int index = mobilityWeightStart + (phase * PIECE_TYPES) + piece;
            params.evalMobility[phase][piece] =
                static_cast<int>(std::round(baseMobility[phase][piece] + weightsCopy[index]));
        }
    }

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        const int index = doubledPawnWeightStart + phase;
        params.evalDoubledPawnPenalty[phase] =
            static_cast<int>(std::round(baseDoubledPawnPenalty[phase] + weightsCopy[index]));
    }

    return params;
}

void updateEvaluationParameters(const std::vector<double>& newWeights) {
    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            evalMaterialValues[phase][piece] = static_cast<int>(std::round(
                baseMaterialValues[phase][piece] + newWeights[materialWeightStart + (phase * PIECE_TYPES) + piece]));
        }
    }

    for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        for (Square square = A1; square <= H8; square++) {
            const PieceType pieceType = getPieceType(piece);
            const PieceColor color = getPieceColor(piece);
            const int mgIndex = pstWeightStart + (MIDGAME * PIECE_TYPES * SQUARES) + (pieceType * SQUARES) +
                                (color == WHITE ? square ^ 56 : square);
            const int egIndex = pstWeightStart + (ENDGAME * PIECE_TYPES * SQUARES) + (pieceType * SQUARES) +
                                (color == WHITE ? square ^ 56 : square);

            const int baseMgPst =
                color == WHITE ? getBaseMidgameTable(pieceType)[square ^ 56] : getBaseMidgameTable(pieceType)[square];
            const int baseEgPst =
                color == WHITE ? getBaseEndgameTable(pieceType)[square ^ 56] : getBaseEndgameTable(pieceType)[square];

            midgamePstTable[piece][square] =
                evalMaterialValues[MIDGAME][pieceType] + static_cast<int>(std::round(baseMgPst + newWeights[mgIndex]));
            endgamePstTable[piece][square] =
                evalMaterialValues[ENDGAME][pieceType] + static_cast<int>(std::round(baseEgPst + newWeights[egIndex]));
        }
    }

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const int index = mobilityWeightStart + (phase * PIECE_TYPES) + piece;
            evalMobility[phase][piece] = static_cast<int>(std::round(baseMobility[phase][piece] + newWeights[index]));
        }
    }

    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        const int index = doubledPawnWeightStart + phase;
        evalDoubledPawnPenalty[phase] = static_cast<int>(std::round(baseDoubledPawnPenalty[phase] + newWeights[index]));
    }
}

double sigmoid(const double x) { return 1.0 / (1.0 + std::pow(10.0, -K * x / 400.0)); }

int evaluateFromTrace(const EvalTrace& trace, const PieceColor sideToMove, const TunedEvaluationParameters& params) {
    int whiteMidgameScore = 0;
    int whiteEndgameScore = 0;
    int blackMidgameScore = 0;
    int blackEndgameScore = 0;

    // PST + material
    for (int pieceTypeInt = 0; pieceTypeInt < PIECE_TYPES; ++pieceTypeInt) {
        const PieceType pieceType = static_cast<PieceType>(pieceTypeInt);
        for (int square = 0; square < SQUARES; ++square) {
            if (trace.pst[WHITE][pieceType][square] > 0) {
                const Piece piece = getPieceFromType(pieceType, WHITE);
                whiteMidgameScore += params.midgamePstTable[piece][square];
                whiteEndgameScore += params.endgamePstTable[piece][square];
            }
            if (trace.pst[BLACK][pieceType][square] > 0) {
                const Piece piece = getPieceFromType(pieceType, BLACK);
                blackMidgameScore += params.midgamePstTable[piece][square];
                blackEndgameScore += params.endgamePstTable[piece][square];
            }
        }
    }

    // Mobility
    for (int pieceTypeInt = 0; pieceTypeInt < PIECE_TYPES; ++pieceTypeInt) {
        const PieceType pieceType = static_cast<PieceType>(pieceTypeInt);
        whiteMidgameScore += trace.mobility[WHITE][pieceType] * params.evalMobility[MIDGAME][pieceType];
        whiteEndgameScore += trace.mobility[WHITE][pieceType] * params.evalMobility[ENDGAME][pieceType];
        blackMidgameScore += trace.mobility[BLACK][pieceType] * params.evalMobility[MIDGAME][pieceType];
        blackEndgameScore += trace.mobility[BLACK][pieceType] * params.evalMobility[ENDGAME][pieceType];
    }

    // Doubled pawns
    whiteMidgameScore += trace.doubledPawns[WHITE] * params.evalDoubledPawnPenalty[MIDGAME];
    whiteEndgameScore += trace.doubledPawns[WHITE] * params.evalDoubledPawnPenalty[ENDGAME];
    blackMidgameScore += trace.doubledPawns[BLACK] * params.evalDoubledPawnPenalty[MIDGAME];
    blackEndgameScore += trace.doubledPawns[BLACK] * params.evalDoubledPawnPenalty[ENDGAME];

    const int phase = trace.phase;
    const int whiteScore = ((whiteMidgameScore * (256 - phase)) + (whiteEndgameScore * phase)) / 256;
    const int blackScore = ((blackMidgameScore * (256 - phase)) + (blackEndgameScore * phase)) / 256;

    const int modifier = sideToMove == WHITE ? 1 : -1;

    return (whiteScore - blackScore) * modifier;
}

double calculateError(const std::vector<TunePosition>& positions) {
    const double epsilon = 1e-10;
    double totalError = 0.0;

    const auto params = getTunedParameters(weights);

#pragma omp parallel for reduction(+ : totalError) default(none) shared(positions, epsilon, params)
    for (const auto& pos : positions) {
        int evalScore = evaluateFromTrace(pos.trace, pos.sideToMove, params);

        if (pos.sideToMove == BLACK) {
            evalScore *= -1;
        }

        const double prediction = sigmoid(evalScore);
        totalError +=
            pos.result * -std::log(prediction + epsilon) + (1.0 - pos.result) * -std::log(1.0 - prediction + epsilon);
    }

    return (1.0 / static_cast<double>(positions.size())) * totalError;
}

std::vector<double> calculateGradients(const std::vector<TunePosition>& positions) {
    std::vector<double> gradients(weights.size());
    std::ranges::fill(gradients, 0.0);

    const double N = static_cast<double>(positions.size());
    const double epsilon = 1e-10;

#pragma omp parallel for default(none) shared(positions, weights, delta, gradients, N, epsilon)
    for (int j = 0; j < static_cast<int>(weights.size()); ++j) {
        std::vector<double> weightsPlus = weights;
        weightsPlus[j] += delta;
        const auto paramsPlus = getTunedParameters(weightsPlus);

        double totalLossPlus = 0.0;

        for (const auto& pos : positions) {
            int evalScorePlus = evaluateFromTrace(pos.trace, pos.sideToMove, paramsPlus);

            if (pos.sideToMove == BLACK) {
                evalScorePlus *= -1;
            }

            const double plusSigmoid = sigmoid(evalScorePlus);
            totalLossPlus += pos.result * -std::log(plusSigmoid + epsilon) +
                             (1.0 - pos.result) * -std::log(1.0 - plusSigmoid + epsilon);
        }

        const double avgLossPlus = totalLossPlus / N;

        std::vector<double> weightsMinus = weights;
        weightsMinus[j] -= delta;
        const auto paramsMinus = getTunedParameters(weightsMinus);

        double totalLossMinus = 0.0;

        for (const auto& pos : positions) {
            int evalScoreMinus = evaluateFromTrace(pos.trace, pos.sideToMove, paramsMinus);

            if (pos.sideToMove == BLACK) {
                evalScoreMinus *= -1;
            }

            const double minusSigmoid = sigmoid(evalScoreMinus);
            totalLossMinus += pos.result * -std::log(minusSigmoid + epsilon) +
                              (1.0 - pos.result) * -std::log(1.0 - minusSigmoid + epsilon);
        }

        const double avgLossMinus = totalLossMinus / N;

        gradients[j] = (avgLossPlus - avgLossMinus) / (2.0 * delta);
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

    fout << "// Material values\n";
    fout << "int evalMaterialValues[GAME_PHASES][PIECE_TYPES]{\n";
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(std::round(baseMaterialValues[MIDGAME][piece] +
                                                      weights[materialWeightStart + (MIDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "}, // Midgame\n";

    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(std::round(baseMaterialValues[ENDGAME][piece] +
                                                      weights[materialWeightStart + (ENDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "} // Endgame\n";
    fout << "};\n\n";

    fout << "// Mobility values\n";
    fout << "int evalMobility[GAME_PHASES][PIECE_TYPES]{\n";
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(
            std::round(baseMobility[MIDGAME][piece] + weights[mobilityWeightStart + (MIDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "}, // Midgame\n";

    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(
            std::round(baseMobility[ENDGAME][piece] + weights[mobilityWeightStart + (ENDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "} // Endgame\n";
    fout << "};\n\n";

    fout << "// Pawn structure\n";
    fout << "int evalDoubledPawnPenalty[GAME_PHASES] = {";
    fout << static_cast<int>(std::round(baseDoubledPawnPenalty[MIDGAME] + weights[doubledPawnWeightStart + MIDGAME]))
         << ", ";
    fout << static_cast<int>(std::round(baseDoubledPawnPenalty[ENDGAME] + weights[doubledPawnWeightStart + ENDGAME]));
    fout << "};\n\n";

    const std::string pieceNames[] = {"pawn", "knight", "bishop", "rook", "queen", "king"};

    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        fout << "// Midgame " << pieceNames[piece] << " PST\n";
        fout << "int mg_" << pieceNames[piece] << "_table[64] = {\n";
        for (int rank = 7; rank >= 0; --rank) {
            fout << "    ";
            for (int file = 0; file < 8; ++file) {
                const int square = (7 - rank) * 8 + file;
                const int index = pstWeightStart + (MIDGAME * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;
                const int value = static_cast<int>(
                    std::round(getBaseMidgameTable(static_cast<PieceType>(piece))[square] + weights[index]));
                fout << value;
                if (file < 7) fout << ", ";
            }
            if (rank > 0) fout << ",";
            fout << "\n";
        }
        fout << "};\n\n";
    }

    // Endgame PST
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        fout << "// Endgame " << pieceNames[piece] << " PST\n";
        fout << "int eg_" << pieceNames[piece] << "_table[64] = {\n";
        for (int rank = 7; rank >= 0; --rank) {
            fout << "    ";
            for (int file = 0; file < 8; ++file) {
                const int square = (7 - rank) * 8 + file;
                const int index = pstWeightStart + (ENDGAME * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;
                const int value = static_cast<int>(
                    std::round(getBaseEndgameTable(static_cast<PieceType>(piece))[square] + weights[index]));
                fout << value;
                if (file < 7) fout << ", ";
            }
            if (rank > 0) fout << ",";
            fout << "\n";
        }
        fout << "};\n\n";
    }

    fout.close();
    std::cout << "Tuned values exported to: " << outputPath << std::endl;
}

void gradientDescent(std::vector<TunePosition>& trainingSet, const std::vector<TunePosition>& validationSet,
                     const std::vector<TunePosition>& testSet, std::mt19937_64& gen) {
    double bestValidationError = std::numeric_limits<double>::infinity();
    std::vector<double> bestWeights = weights;
    int epochsWithoutImprovement = 0;
    int finalEpoch = 0;
    double finalTrainingError = 0.0;
    double finalValidationError = 0.0;

    std::vector<double> m(weights.size(), 0.0);
    std::vector<double> v(weights.size(), 0.0);
    int timestep = 0;

    // Print the initial training and validation error
    updateEvaluationParameters(weights);
    double initialTrainingError = calculateError(trainingSet);
    double initialValidationError = calculateError(validationSet);
    std::cout << "Initial Training Error: " << initialTrainingError
              << " - Initial Validation Error: " << initialValidationError << std::endl;

    for (int iteration = 0; iteration < maxIterations; iteration++) {
        std::ranges::shuffle(trainingSet, gen);
        std::vector<std::vector<TunePosition>> batches = createBatches(trainingSet);

        double iterationError = 0.0;
        int numBatches = 0;

        for (const auto& batch : batches) {
            timestep += 1;
            std::vector<double> gradients = calculateGradients(batch);

            for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
                m[i] = beta1 * m[i] + (1.0 - beta1) * gradients[i];
                v[i] = beta2 * v[i] + (1.0 - beta2) * (gradients[i] * gradients[i]);
            }

            double beta1T = std::pow(beta1, timestep);
            double beta2T = std::pow(beta2, timestep);
            std::vector<double> mHat(weights.size());
            std::vector<double> vHat(weights.size());

            for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
                mHat[i] = m[i] / (1.0 - beta1T);
                vHat[i] = v[i] / (1.0 - beta2T);
            }

            for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
                weights[i] -= learningRate * mHat[i] / (std::sqrt(vHat[i]) + epsilon);
            }

            updateEvaluationParameters(weights);

            iterationError += calculateError(batch);
            numBatches++;
        }

        iterationError /= numBatches;
        finalTrainingError = iterationError;

        const double validationError = calculateError(validationSet);
        finalValidationError = validationError;
        std::cout << "Iteration " << iteration + 1 << "/" << maxIterations << " - Training Error: " << iterationError
                  << " - Validation Error: " << validationError << std::endl;

        if ((iteration + 1) % saveEvery == 0) {
            exportTunedValues("tuned_values.h", iteration + 1, iterationError, validationError,
                              calculateError(testSet));
        }

        if (iteration > earlyStoppingWarmup) {
            if (validationError < bestValidationError - earlyStoppingMinDelta) {
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
    updateEvaluationParameters(weights);

    const double testError = calculateError(testSet);
    std::cout << "Final test error: " << testError << std::endl;

    exportTunedValues("tuned_values.h", finalEpoch, finalTrainingError, finalValidationError, testError);
}

std::vector<TunePosition> loadPositions(const std::string& filePath, std::mt19937_64& gen) {
    std::cout << "Loading positions..." << std::endl;
    std::vector<std::string> lines;
    std::ifstream fin(filePath);
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
        tunePos.fen = fen;
        tunePos.result = result;
        tunePos.sideToMove = board.getSideToMove();
        tunePos.trace = eval.trace;
        parsedPositions[i] = tunePos;
        isValid[i] = true;
    }

    std::vector<TunePosition> positions;
    positions.reserve(win + loss + draw);

    for (size_t i = 0; i < lines.size(); ++i) {
        if (isValid[i]) {
            positions.push_back(parsedPositions[i]);
        }
    }

    int smallestClassSize = std::min({win, loss, draw});
    std::vector<TunePosition> newPositions;
    int newWin = 0;
    int newLoss = 0;
    int newDraw = 0;

    std::ranges::shuffle(positions, gen);

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

        if (newWin >= smallestClassSize && newLoss >= smallestClassSize && newDraw >= smallestClassSize) {
            break;
        }
    }

    std::cout << "Loaded " << newPositions.size() << " positions." << std::endl;
    std::cout << "Win: " << newWin << ", Loss: " << newLoss << ", Draw: " << newDraw << std::endl;
    return newPositions;
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
    std::cout << "Starting cross-entropy error with K=" << K << ": " << startingError << std::endl;

    while (b - a > tol) {
        if (f1 < f2) {
            // Minimum lies in [a, x2]
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = a + invphi2 * (b - a);
            f1 = averageError(x1);
            std::cout << "Testing K=" << x1 << ", error=" << f1 << std::endl;
        } else {
            // Minimum lies in [x1, b]
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
    std::cout << "Found optimal K=" << optimalK << " with cross-entropy error: " << finalError << std::endl;
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
    // Initialize evaluation parameters with base values (all weights are 0.0)
    updateEvaluationParameters(weights);

    std::vector<TunePosition> trainingSet = loadPositions(filePath, gen);
    if (trainingSet.empty()) {
        std::cout << "Error: No training positions loaded. Tuning cannot start." << std::endl;
        return;
    }

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
