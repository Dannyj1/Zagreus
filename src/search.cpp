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

#include <limits>

#include "search.h"

#include "board.h"
#include "eval.h"
#include "move.h"
#include "move_gen.h"
#include "move_picker.h"

namespace Zagreus {
Move search(Board& board) {
    return Move();
}

template <PieceColor color, NodeType nodeType>
int pvSearch(Board& board, int alpha, int beta, int depth) {
    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (depth == 0) {
        return Evaluation(board).evaluate();
    }

    constexpr bool isPV = nodeType == PV || nodeType == ROOT;
    constexpr PieceColor opponentColor = !color;
    int bestScore = std::numeric_limits<int>::min();
    bool firstMove = true;
    int legalMoves = 0;

    Move move;
    MoveList moves = MoveList{};
    generateMoves<color, ALL>(board, moves);
    MovePicker movePicker{moves};

    while (movePicker.next(move)) {
        int score;
        board.makeMove(move);

        if (!board.isPositionLegal<color>()) {
            board.unmakeMove();
            continue;
        }

        legalMoves += 1;

        if (firstMove) {
            score = -pvSearch<opponentColor, nodeType>(board, -beta, -alpha, depth - 1);
            firstMove = false;
        } else {
            score = -pvSearch<opponentColor, REGULAR>(board, -alpha - 1, -alpha, depth - 1);

            if (score > alpha && isPV) {
                score = -pvSearch<opponentColor, PV>(board, -beta, -alpha, depth - 1);
            }
        }

        board.unmakeMove();

        if (score >= beta) {
            return score;
        }

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                alpha = score;
            }
        }
    }

    if (!legalMoves) {
        bool inCheck = board.isKingInCheck<color>();

        if (inCheck) {
            alpha = -MATE_SCORE + board.getPly();
        } else {
            alpha = DRAW_SCORE;
        }
    }

    return alpha;
}
} // namespace Zagreus
