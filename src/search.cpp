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

#include <iostream>
#include <limits>

#include "uci.h"
#include "board.h"
#include "constants.h"
#include "eval.h"
#include "move.h"
#include "move_gen.h"
#include "move_picker.h"
#include "timeman.h"
#include "tt.h"
#include "types.h"

namespace Zagreus {
static TranspositionTable* tt = TranspositionTable::getTT();

// TODO: Support more search variables (infinite, max nodes, etc.)
template <PieceColor color>
Move search(Engine& engine, Board& board, SearchParams& params, SearchStats& stats) {
    constexpr PieceColor opponentColor = !color;

    int depth = 1;
    const int currentPly = board.getPly();
    MoveList moves = MoveList{};
    generateMoves<color, ALL>(board, moves);
    MovePicker movePicker{moves};
    Move bestMove = NO_MOVE;

    int searchTime = calculateSearchTime<color>(params);
    const auto endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(searchTime);
    const auto startTime = std::chrono::steady_clock::now();

    engine.setSearchStopped(false);

    while (!engine.isSearchStopped() && (currentPly + depth) < MAX_PLY) {
        if (params.blackTime > 0 || params.whiteTime > 0) {
            // Don't start the next iteration if we are 10% away from the end time
            if (depth > 1 && std::chrono::steady_clock::now() + std::chrono::milliseconds(searchTime / 10) > endTime) {
                engine.setSearchStopped(true);
                break;
            }
        }

        if (params.depth > 0 && depth > params.depth) {
            engine.setSearchStopped(true);
            break;
        }

        movePicker.reset();
        Move move;
        int bestScore = std::numeric_limits<int>::min();
        Move bestIterationMove{};

        while (movePicker.next(move)) {
            board.makeMove(move);

            if (!board.isPositionLegal<color>()) {
                board.unmakeMove();
                continue;
            }

            const int score = -pvSearch<opponentColor, ROOT>(engine, board, INITIAL_ALPHA, INITIAL_BETA, depth, stats,
                                                             endTime);

            board.unmakeMove();

            if (score > bestScore) {
                bestScore = score;
                bestIterationMove = move;
            }
        }

        if (depth > 1 && std::chrono::steady_clock::now() > endTime) {
            engine.setSearchStopped(true);
            break;
        }

        if (engine.isSearchStopped()) {
            break;
        }

        bestMove = bestIterationMove;

        // TODO: Implement PV
        stats.timeSpentMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count();
        stats.depth = depth;
        stats.score = bestScore;

        // Make sure we don't divide by zero
        if (stats.timeSpentMs == 0) {
            stats.timeSpentMs = 1;
        }

        uint64_t totalNodesSearch = stats.nodesSearched + stats.qNodesSearched;
        uint64_t nps = static_cast<double>(totalNodesSearch) / (
                           static_cast<double>(stats.timeSpentMs) / 1000.0);
        engine.sendInfoMessage(std::format("depth {} score cp {} nodes {} time {} nps {}", stats.depth,
                                           stats.score, totalNodesSearch, stats.timeSpentMs, nps));
        depth += 1;
    }

    if (bestMove == NO_MOVE) {
        // Find the first legal move and play that
        movePicker.reset();
        Move move;

        while (movePicker.next(move)) {
            board.makeMove(move);

            if (!board.isPositionLegal<color>()) {
                board.unmakeMove();
                continue;
            }

            bestMove = move;
            board.unmakeMove();
            break;
        }
    }

    return bestMove;
}

template Move search<WHITE>(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);
template Move search<BLACK>(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);

template <PieceColor color, NodeType nodeType>
int pvSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
             const std::chrono::time_point<std::chrono::steady_clock>& endTime) {
    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if ((stats.nodesSearched + stats.qNodesSearched) % 4096 == 0 && std::chrono::steady_clock::now() > endTime) {
        engine.setSearchStopped(true);
        return beta;
    }

    if (depth == 0) {
        return qSearch<color, nodeType>(engine, board, alpha, beta, depth, stats, endTime);
    }

    stats.nodesSearched += 1;

    constexpr bool isPV = nodeType == PV || nodeType == ROOT;
    constexpr bool isRoot = nodeType == ROOT;
    constexpr PieceColor opponentColor = !color;

    if (!isPV) {
        const int16_t score = tt->probePosition(board.getZobristHash(), depth, alpha, beta, board.getPly());

        if (score != NO_TT_SCORE) {
            return score;
        }
    }

    int bestScore = std::numeric_limits<int>::min();
    bool firstMove = true;
    int legalMoves = 0;

    Move move;
    MoveList moves = MoveList{};
    generateMoves<color, ALL>(board, moves);
    MovePicker movePicker{moves};
    Move bestMove = NO_MOVE;

    while (movePicker.next(move)) {
        int score;
        board.makeMove(move);

        if (!board.isPositionLegal<color>()) {
            board.unmakeMove();
            continue;
        }

        legalMoves += 1;

        if (firstMove) {
            if (isRoot) {
                score = -pvSearch<opponentColor, PV>(engine, board, -beta, -alpha, depth - 1, stats, endTime);
            } else {
                score = -pvSearch<opponentColor, nodeType>(engine, board, -beta, -alpha, depth - 1, stats, endTime);
            }

            firstMove = false;
        } else {
            score = -pvSearch<opponentColor, REGULAR>(engine, board, -alpha - 1, -alpha, depth - 1, stats, endTime);

            if (score > alpha && isPV) {
                score = -pvSearch<opponentColor, PV>(engine, board, -beta, -alpha, depth - 1, stats, endTime);
            }
        }

        board.unmakeMove();

        if (score >= beta) {
            if (!engine.isSearchStopped()) {
                bestMove = move;
                tt->savePosition(board.getZobristHash(), depth, board.getPly(), score, bestMove, BETA);
            }

            return score;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
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

    if (!isRoot) {
        TTNodeType ttNodeType = ALPHA;

        if (isPV) {
            ttNodeType = EXACT;
        }

        if (!engine.isSearchStopped()) {
            tt->savePosition(board.getZobristHash(), depth, board.getPly(), alpha, bestMove, ttNodeType);
        }
    }

    return alpha;
}

template <PieceColor color, NodeType nodeType>
int qSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
            const std::chrono::time_point<std::chrono::steady_clock>
            & endTime) {
    constexpr bool isPV = nodeType == PV;

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (!isPV) {
        const int16_t score = tt->probePosition(board.getZobristHash(), depth, alpha, beta, board.getPly());

        if (score != NO_TT_SCORE) {
            return score;
        }
    }

    if ((stats.nodesSearched + stats.qNodesSearched) % 4096 == 0 && std::chrono::steady_clock::now() > endTime) {
        engine.setSearchStopped(true);
        return beta;
    }

    stats.qNodesSearched += 1;

    int bestScore = Evaluation(board).evaluate();

    if (bestScore >= beta) {
        if (!engine.isSearchStopped()) {
            tt->savePosition(board.getZobristHash(), depth, board.getPly(), bestScore, NO_MOVE, BETA);
        }

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

        const int score = -qSearch<!color, nodeType>(engine, board, -beta, -alpha, depth - 1, stats, endTime);

        board.unmakeMove();

        if (score >= beta) {
            if (!engine.isSearchStopped()) {
                tt->savePosition(board.getZobristHash(), depth, board.getPly(), score, NO_MOVE, BETA);
            }

            return score;
        }

        if (score > bestScore) {
            bestScore = score;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    TTNodeType ttNodeType = ALPHA;

    if (isPV) {
        ttNodeType = EXACT;
    }

    if (!engine.isSearchStopped()) {
        tt->savePosition(board.getZobristHash(), depth, board.getPly(), bestScore, NO_MOVE, ttNodeType);
    }

    return bestScore;
}
} // namespace Zagreus
