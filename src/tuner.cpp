#ifdef ZAGREUS_TUNER
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <ranges>

#include "board.h"
#include "eval.h"
#include "uci.h"
#include "tuner.h"
#include "pst.h"
#include "constants.h"
#include "types.h"
#include "eval_base_values.h"
#include "eval_features.h"

namespace Zagreus {
const int epochs = 10000;
const int batchSize = 16384;
const double learningRate = 0.1;
const int earlyStoppingPatience = 20;
const int saveEvery = 50;
const double maxGradientNorm = 1.0;
int seed = 42;

double K = 0.0;

std::vector<double> weights{};
std::vector<double> gradients{};

int materialWeightStart = 0;
int pstWeightStart;
int mobilityWeightStart;
int totalWeights;

void initializeWeights() {
    const int numMaterialWeights = GAME_PHASES * PIECE_TYPES;
    pstWeightStart = numMaterialWeights;

    const int numPstWeights = GAME_PHASES * PIECE_TYPES * SQUARES;
    mobilityWeightStart = pstWeightStart + numPstWeights;

    const int numMobilityWeights = GAME_PHASES * PIECE_TYPES;
    totalWeights = mobilityWeightStart + numMobilityWeights;

    weights.resize(totalWeights);
    gradients.resize(totalWeights);

    // Initialize material weights
    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            weights[materialWeightStart + (phase * PIECE_TYPES) + piece] = baseMaterialValues[phase][piece];
        }
    }

    // Initialize PST weights
    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            for (int square = 0; square < SQUARES; ++square) {
                const int index = pstWeightStart + (phase * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;

                if (phase == MIDGAME) {
                    weights[index] = getBaseMidgameTable(static_cast<PieceType>(piece))[square];
                } else {
                    weights[index] = getBaseEndgameTable(static_cast<PieceType>(piece))[square];
                }
            }
        }
    }

    // Initialize mobility weights
    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const int index = mobilityWeightStart + (phase * PIECE_TYPES) + piece;
            weights[index] = baseMobility[phase][piece];
        }
    }
}

void updateEvaluationParameters() {
    // Update material weights
    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            evalMaterialValues[phase][piece] = static_cast<int>(std::round(weights[materialWeightStart + (phase * PIECE_TYPES) + piece]));
        }
    }

    // Update PST weights
    for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        for (Square square = A1; square <= H8; square++) {
            const PieceType pieceType = getPieceType(piece);
            const PieceColor color = getPieceColor(piece);
            const int mgIndex = pstWeightStart + (MIDGAME * PIECE_TYPES * SQUARES) + (pieceType * SQUARES) + (color == WHITE ? square ^ 56 : square);
            const int egIndex = pstWeightStart + (ENDGAME * PIECE_TYPES * SQUARES) + (pieceType * SQUARES) + (color == WHITE ? square ^ 56 : square);

            midgamePstTable[piece][square] = evalMaterialValues[MIDGAME][pieceType] + static_cast<int>(std::round(weights[mgIndex]));
            endgamePstTable[piece][square] = evalMaterialValues[ENDGAME][pieceType] + static_cast<int>(std::round(weights[egIndex]));
        }
    }

    // Update mobility weights
    for (int phase = 0; phase < GAME_PHASES; ++phase) {
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const int index = mobilityWeightStart + (phase * PIECE_TYPES) + piece;
            evalMobility[phase][piece] = static_cast<int>(std::round(weights[index]));
        }
    }
}

double sigmoid(const double x) {
    return 1.0 / (1.0 + std::exp(-x * K));
}

double calculateError(const std::vector<TunePosition>& positions) {
    const double epsilon = 1e-10;
    double totalError = 0.0;
    Board board;

    for (const auto& pos : positions) {
        if (!board.setFromFEN(pos.fen)) continue;

        Evaluation eval{board};
        int evalScore = eval.evaluate();
        if (board.getSideToMove() == BLACK) {
            evalScore *= -1;
        }

        const double prediction = sigmoid(evalScore);
        totalError += pos.result * -std::log(prediction + epsilon) + (1.0 - pos.result) * -std::log(1.0 - prediction + epsilon);
    }

    return (1.0 / static_cast<double>(positions.size())) * totalError;
}

void computeGradients(const std::vector<TunePosition>& positions, Board& board) {
    std::ranges::fill(gradients, 0.0);
    const double N = static_cast<double>(positions.size());
    
    for (const auto& pos : positions) {
        if (!board.setFromFEN(pos.fen)) continue;

        Evaluation eval{board};
        int evalScore = eval.evaluate();
        const bool isBlack = board.getSideToMove() == BLACK;
        if (isBlack) {
            evalScore *= -1;
        }

        const double error = pos.result - sigmoid(evalScore);
        const int phase = eval.calculatePhase();
        const double mgPhaseScale = static_cast<double>(256 - phase) / 256.0;
        const double egPhaseScale = static_cast<double>(phase) / 256.0;

        // Material gradients
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const double diff = eval.trace.material[WHITE][piece] - eval.trace.material[BLACK][piece];

            if (diff != 0.0) {
                const int mgIndex = materialWeightStart + (MIDGAME * PIECE_TYPES) + piece;
                const int egIndex = materialWeightStart + (ENDGAME * PIECE_TYPES) + piece;
                gradients[mgIndex] += -error * (diff * mgPhaseScale) / N;
                gradients[egIndex] += -error * (diff * egPhaseScale) / N;
            }
        }

        // PST gradients
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            for (int square = 0; square < SQUARES; ++square) {
                const double diff = eval.trace.pst[WHITE][piece][square] - eval.trace.pst[BLACK][piece][square];
                if (diff != 0.0) {
                    const int mgIndex = pstWeightStart + (MIDGAME * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;
                    const int egIndex = pstWeightStart + (ENDGAME * PIECE_TYPES * SQUARES) + (piece * SQUARES) + square;
                    gradients[mgIndex] += -error * (diff * mgPhaseScale) / N;
                    gradients[egIndex] += -error * (diff * egPhaseScale) / N;
                }
            }
        }

        // Mobility gradients
        for (int piece = 0; piece < PIECE_TYPES; ++piece) {
            const double diff = eval.trace.mobility[WHITE][piece] - eval.trace.mobility[BLACK][piece];
            if (diff != 0.0) {
                const int mgIndex = mobilityWeightStart + (MIDGAME * PIECE_TYPES) + piece;
                const int egIndex = mobilityWeightStart + (ENDGAME * PIECE_TYPES) + piece;
                gradients[mgIndex] += -error * (diff * mgPhaseScale) / N;
                gradients[egIndex] += -error * (diff * egPhaseScale) / N;
            }
        }
    }
}

std::vector<std::vector<TunePosition>> createBatches(const std::vector<TunePosition>& positions) {
    std::vector<std::vector<TunePosition>> batches;
    batches.reserve((positions.size() + batchSize - 1) / batchSize);

    for (size_t i = 0; i < positions.size(); i += batchSize) {
        std::vector<TunePosition> batch;
        for (size_t j = i; j < std::min(i + batchSize, positions.size()); j++) {
            batch.push_back(positions[j]);
        }
        batches.push_back(batch);
    }

    return batches;
}

void exportTunedValues(const std::string& outputPath, int finalEpoch, double trainingError, double validationError, double testError) {
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
        const int value = static_cast<int>(std::round(weights[materialWeightStart + (MIDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "}, // Midgame\n";
    
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(std::round(weights[materialWeightStart + (ENDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "} // Endgame\n";
    fout << "};\n\n";

    fout << "// Mobility values\n";
    fout << "int evalMobility[GAME_PHASES][PIECE_TYPES]{\n";
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(std::round(weights[mobilityWeightStart + (MIDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "}, // Midgame\n";
    
    fout << "    {";
    for (int piece = 0; piece < PIECE_TYPES; ++piece) {
        const int value = static_cast<int>(std::round(weights[mobilityWeightStart + (ENDGAME * PIECE_TYPES) + piece]));
        fout << value;
        if (piece < PIECE_TYPES - 1) fout << ", ";
    }
    fout << "} // Endgame\n";
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
                const int value = static_cast<int>(std::round(weights[index]));
                fout << value;
                if (square < 63) fout << ", ";
            }
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
                const int value = static_cast<int>(std::round(weights[index]));
                fout << value;
                if (square < 63) fout << ", ";
            }
            fout << "\n";
        }
        fout << "};\n\n";
    }

    fout.close();
    std::cout << "Tuned values exported to: " << outputPath << std::endl;
}

void gradientDescent(std::vector<TunePosition>& trainingSet, const std::vector<TunePosition>& validationSet,
                     const std::vector<TunePosition>& testSet, std::mt19937_64& gen, Board& board) {
    initializeWeights();
    
    double bestValidationError = std::numeric_limits<double>::infinity();
    std::vector<double> bestWeights = weights;
    int epochsWithoutImprovement = 0;
    int finalEpoch = 0;
    double finalTrainingError = 0.0;
    double finalValidationError = 0.0;

    for (int epoch = 0; epoch < epochs; epoch++) {
        std::ranges::shuffle(trainingSet, gen);
        std::vector<std::vector<TunePosition>> batches = createBatches(trainingSet);

        double epochError = 0.0;
        int numBatches = 0;

        for (const auto& batch : batches) {
            computeGradients(batch, board);

            double gradNormSq = 0.0;
            for (double g : gradients) {
                gradNormSq += g * g;
            }
            double gradNorm = std::sqrt(gradNormSq);
            if (gradNorm > maxGradientNorm) {
                double scale = maxGradientNorm / gradNorm;
                for (auto &g : gradients) {
                    g *= scale;
                }
            }

            for (size_t j = 0; j < weights.size(); ++j) {
                weights[j] -= learningRate * gradients[j];
            }

            updateEvaluationParameters();

            epochError += calculateError(batch);
            numBatches++;
        }

        epochError /= numBatches;
        finalTrainingError = epochError;

        const double validationError = calculateError(validationSet);
        finalValidationError = validationError;
        std::cout << "Epoch " << epoch + 1 << "/" << epochs 
                 << " - Training Error: " << epochError 
                 << " - Validation Error: " << validationError << std::endl;

        if ((epoch + 1) % saveEvery == 0) {
            exportTunedValues("tuned_values.h", epoch + 1, epochError, validationError, calculateError(testSet));
        }

        if (validationError < bestValidationError) {
            bestValidationError = validationError;
            bestWeights = weights;
            epochsWithoutImprovement = 0;
        } else {
            epochsWithoutImprovement++;
            if (epochsWithoutImprovement >= earlyStoppingPatience) {
                std::cout << "Early stopping triggered." << std::endl;
                finalEpoch = epoch + 1;
                break;
            }
        }

        finalEpoch = epoch + 1;
    }

    weights = bestWeights;
    updateEvaluationParameters();

    const double testError = calculateError(testSet);
    std::cout << "Final test error: " << testError << std::endl;

    exportTunedValues("tuned_values.h", finalEpoch, finalTrainingError, finalValidationError, testError);
}

std::vector<TunePosition> loadPositions(const std::string& filePath, std::mt19937_64& gen, Board& board) {
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
        size_t c9_pos = posLine.find(" c9 ");
        if (c9_pos == std::string::npos) continue;

        std::string resultStr = posLine.substr(c9_pos + 4);
        std::string fen = posLine.substr(0, c9_pos);

        if (!board.setFromFEN(fen) || board.isDraw() ||
            board.isKingInCheck<WHITE>() || board.isKingInCheck<BLACK>()) {
            continue;
        }

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

        Evaluation eval{board};
        int evalScore = eval.evaluate();

        if (board.getSideToMove() == BLACK) {
            evalScore *= -1;
        }

        TunePosition tunePos{fen, result, evalScore};
        positions.emplace_back(tunePos);
    }

    // Reduce the biggest two classes to the size of the smallest class
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

        if (newWin >= smallestClassSize && newLoss >= smallestClassSize &&
            newDraw >= smallestClassSize) {
            break;
        }
    }

    // Write all newPositions to a file by their fen strings
    std::ofstream fout("cleaned_positions.epd");
    for (TunePosition& pos : newPositions) {
        std::string resultStr;

        if (pos.result == 1.0) {
            resultStr = "1-0";
        } else if (pos.result == 0.0) {
            resultStr = "0-1";
        } else {
            resultStr = "1/2-1/2";
        }

        fout << pos.fen << " c9 \"" << resultStr << "\";" << std::endl;
    }

    fout.close();

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

    Board board{};
    std::vector<TunePosition> trainingSet = loadPositions(filePath, gen, board);
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

    gradientDescent(trainingSet, validationSet, testSet, gen, board);
}
}
#endif
