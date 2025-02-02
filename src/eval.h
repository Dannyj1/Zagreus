
/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

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

#pragma once

#include <cstdint>
#include "board.h"
#include "constants.h"

#ifdef ZAGREUS_TUNER
    #include <vector>
#endif

namespace Zagreus {

#ifdef ZAGREUS_TUNER
struct FeatureValue {
    int featureIndex;
    int value;
};
#endif

struct EvalData {
    uint64_t attacksFrom[SQUARES];
    uint64_t attacksByColor[COLORS];
    uint64_t attacksByPiece[PIECES];
};

class Evaluation {
private:
    const Board& board;
    EvalData evalData{};
    int whiteMidgameScore{};
    int whiteEndgameScore{};
    int blackMidgameScore{};
    int blackEndgameScore{};

#ifdef ZAGREUS_TUNER
    std::vector<FeatureValue> featureValues{};
#endif

    /**
     * \brief Evaluates the material on the board.
     *
     * This function evaluates the material on the board for both sides (white and black)
     * and updates the midgame and endgame scores accordingly.
     */
    [[nodiscard]] int calculatePhase() const;

    /**
     * \brief Evaluates the material on the board.
     */
    void evaluateMaterial();

    /**
     * \brief Evaluates several features related to the pieces on the board.
     */
    void evaluatePieces();

public:
    /**
     * \brief Constructs an Evaluation object with the given board.
     * \param board The current state of the chess board.
     */
    explicit Evaluation(const Board& board) : board(board) {}

    Evaluation(const Evaluation&) = delete;

    Evaluation& operator=(const Evaluation&) = delete;


    /**
     * \brief Initializes the evaluation data needed to evaluate the board position.
     */
    void initializeEvalData();

    /**
     * \brief Evaluates the current board position.
     * \return The evaluation score of the current board position.
     */
    [[nodiscard]] int evaluate();
};

/**
 * \brief Gets the value of a given piece.
 *
 * @param piece The piece to get the value of.
 * @return The value of the given piece.
 */
[[nodiscard]] int getPieceValue(Piece piece);
} // namespace Zagreus
