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

#include "search.h"

#include <__format/format_functions.h>

#include <limits>

#include "uci.h"
#include "board.h"
#include "constants.h"
#include "eval.h"
#include "move.h"
#include "move_gen.h"
#include "move_picker.h"
#include "timeman.h"
#include "types.h"

namespace Zagreus {
// TODO: Support more search variables (max nodes, etc.)
template <PieceColor color>
Move search(Engine& engine, Board& board, int whiteTime, int blackTime) {
    constexpr PieceColor opponentColor = !color;
    int searchTime;

    if (color == WHITE) {
        searchTime = calculateSearchTime(whiteTime, board.getPly());
    } else {
        searchTime = calculateSearchTime(blackTime, board.getPly());
    }

    const auto endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(searchTime);

    int depth = 1;
    int currentPly = board.getPly();
    MoveList moves = MoveList{};
    generateMoves<color, ALL>(board, moves);
    MovePicker movePicker{moves};
    Move bestMove = NO_MOVE;

    while (std::chrono::steady_clock::now() < endTime && (currentPly + depth) < MAX_PLY) {
        if (std::chrono::steady_clock::now() + std::chrono::milliseconds(searchTime / 10) > endTime) {
            break;
        }

        movePicker.reset();
        Move move;
        int bestScore = std::numeric_limits<int>::min();
        Move bestIterationMove{};

        while (movePicker.next(move)) {
            board.makeMove(movePicker.getCurrentMove());

            if (!board.isPositionLegal<color>()) {
                board.unmakeMove();
                continue;
            }

            const int score = -pvSearch<opponentColor, ROOT>(board, INITIAL_ALPHA, INITIAL_BETA, depth, endTime);

            board.unmakeMove();

            if (std::chrono::steady_clock::now() > endTime) {
                break;
            }

            if (score > bestScore) {
                bestScore = score;
                bestIterationMove = move;
            }
        }

        if (std::chrono::steady_clock::now() > endTime) {
            break;
        }

        bestMove = bestIterationMove;
        // TODO: Implement PV and nodes per second
        engine.sendInfoMessage(std::format("depth {} score cp {}", depth, bestScore));
        depth += 1;
    }

    if (bestMove == NO_MOVE) {
        bestMove = moves.moves[0];
    }

    return bestMove;
}

template Move search<WHITE>(Engine& engine, Board& board, int whiteTime, int blackTime);
template Move search<BLACK>(Engine& engine, Board& board, int whiteTime, int blackTime);

template <PieceColor color, NodeType nodeType>
int pvSearch(Board& board, int alpha, int beta, int depth,
             const std::chrono::time_point<std::chrono::steady_clock>& endTime) {
    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (std::chrono::steady_clock::now() > endTime) {
        return beta;
    }

    if (depth == 0) {
        return qSearch<color>(board, alpha, beta, endTime);
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
            score = -pvSearch<opponentColor, nodeType>(board, -beta, -alpha, depth - 1, endTime);
            firstMove = false;
        } else {
            score = -pvSearch<opponentColor, REGULAR>(board, -alpha - 1, -alpha, depth - 1, endTime);

            if (score > alpha && isPV) {
                score = -pvSearch<opponentColor, PV>(board, -beta, -alpha, depth - 1, endTime);
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

template <PieceColor color>
int qSearch(Board& board, int alpha, int beta, const std::chrono::time_point<std::chrono::steady_clock>& endTime) {
    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (std::chrono::steady_clock::now() > endTime) {
        return beta;
    }

    int bestScore = Evaluation(board).evaluate();

    if (bestScore >= beta) {
        return bestScore;
    }

    if (bestScore > alpha) {
        alpha = bestScore;
    }

    Move move;
    MoveList moves = MoveList{};
    generateMoves<color, QSEARCH>(board, moves);
    MovePicker movePicker{moves};

    while (movePicker.next(move)) {
        board.makeMove(move);

        if (!board.isPositionLegal<color>()) {
            board.unmakeMove();
            continue;
        }

        const int score = -qSearch<!color>(board, -beta, -alpha, endTime);

        board.unmakeMove();

        if (score >= beta) {
            return score;
        }

        if (score > bestScore) {
            bestScore = score;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return bestScore;
}
} // namespace Zagreus
