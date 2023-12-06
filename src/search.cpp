/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

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

#include "evaluate.h"
#include "movegen.h"
#include "movelist_pool.h"
#include "movepicker.h"
#include "timemanager.h"


namespace Zagreus {
MoveListPool* moveListPool = MoveListPool::getInstance();

template <PieceColor color>
Move getBestMove(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                 senjo::SearchStats& searchStats) {
    auto startTime = std::chrono::steady_clock::now();
    // TODO: replace timecontext with searchcontext
    TimeContext timeContext{};
    SearchContext searchContext{};
    timeContext.startTime = startTime;
    int bestScore = -1000000;
    Move bestMove = {};
    int alpha = -1000000;
    int beta = 1000000;
    int depth = 0;
    constexpr PieceColor oppositeColor = color == WHITE ? BLACK : WHITE;

    while (!engine.stopRequested()) {
        auto currentTime = std::chrono::steady_clock::now();
        // Update the endtime using new data
        searchContext.endTime = getEndTime(timeContext, params, engine, board.getMovingColor());

        if (currentTime > searchContext.endTime) {
            break;
        }

        depth += 1;

        if (depth + board.getPly() >= MAX_PLY) {
            break;
        }

        // If the go command has a max depth argument, terminate when reaching the desired depth.
        if (params.depth > 0 && depth > params.depth) {
            return bestMove;
        }

        MoveList* moves = moveListPool->getMoveList();
        generateMoves<color>(board, moves);

        auto movePicker = MovePicker(moves);

        while (movePicker.hasNext()) {
            Move move = movePicker.getNextMove();

            board.makeMove(move);

            if (board.isKingInCheck<color>()) {
                board.unmakeMove(move);
                continue;
            }

            int score = -search<oppositeColor, PV>(board, alpha, beta, depth, searchContext);

            if (score > bestScore) {
                bestMove = move;
                bestScore = score;
            }

            board.unmakeMove(move);
        }

        moveListPool->releaseMoveList(moves);
    }

    return bestMove;
}

template <PieceColor color, NodeType nodeType>
int search(Bitboard& board, int alpha, int beta, int depth, SearchContext& context) {
    auto currentTime = std::chrono::steady_clock::now();

    if (currentTime > context.endTime) {
        // Immediately evaluate when time is up
        return Evaluation(board).evaluate();
    }

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (depth == 0) {
        // return qsearch<color, NodeType>();
        // No qsearch so we have a "base case" to test against
        return Evaluation(board).evaluate();
    }

    constexpr bool isPvNode = nodeType == PV;
    constexpr PieceColor oppositeColor = color == WHITE ? BLACK : WHITE;
    bool doPvSearch = true;
    MoveList* moves = moveListPool->getMoveList();

    generateMoves<color>(moves);
    auto movePicker = MovePicker(moves);
    int legalMoveCount = 0;

    while (movePicker.hasNext()) {
        Move move = movePicker.getNextMove();
        int score;

        board.makeMove(move);

        if (board.isKingInCheck<color>()) {
            board.unmakeMove(move);
            continue;
        }

        legalMoveCount += 1;

        if (isPvNode && doPvSearch) {
            score = -search<PV, oppositeColor>(-beta, -alpha, depth - 1, context);
        } else {
            score = -search<NO_PV, oppositeColor>(-alpha - 1, -alpha, depth - 1);

            if (score > alpha) {
                score = -search<PV, oppositeColor>(-beta, -alpha, depth - 1);
            }
        }

        board.unmakeMove(move);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
            doPvSearch = false;
        }
    }

    moveListPool->releaseMoveList(moves);
    return alpha;
}

template <PieceColor color, NodeType nodeType>
int qsearch(Bitboard& board, int alpha, int beta, int depth, SearchContext& context) {
}
} // namespace Zagreus

#pragma clang diagnostic pop
