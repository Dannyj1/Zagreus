
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

#pragma once

#include "board.h"

namespace Zagreus {
class Evaluation {
private:
    Board board;
    int whiteMidgameScore{};
    int whiteEndgameScore{};
    int blackMidgameScore{};
    int blackEndgameScore{};

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

public:
    /**
     * \brief Constructs an Evaluation object with the given board.
     * \param board The current state of the chess board.
     */
    explicit Evaluation(const Board& board) {
        this->board = board;
    }

    /**
     * \brief Evaluates the current board position.
     * \return The evaluation score of the current board position.
     */
    [[nodiscard]] int evaluate();
};
} // namespace Zagreus
