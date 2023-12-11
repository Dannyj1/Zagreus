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

#include "../senjo/Output.h"
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
    // TODO: replace timecontext with searchcontext?
    TimeContext timeContext{};
    timeContext.startTime = startTime;
    SearchContext searchContext{};
    Move bestMove = {};
    int depth = 0;
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    Line pvLine{};

    while (!engine.stopRequested()) {
        int alpha = -1000000;
        int beta = 1000000;
        int bestScore = -1000000;

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

        searchStats.depth = depth;
        searchStats.seldepth = 0;

        // If the go command has a max depth argument, terminate when reaching the desired depth.
        if (params.depth > 0 && depth > params.depth) {
            return bestMove;
        }

        MoveList* moves = moveListPool->getMoveList();
        generateMoves<color>(board, moves);

        auto movePicker = MovePicker(moves);

        Line nodeLine{};
        while (movePicker.hasNext()) {
            Move move = movePicker.getNextMove();

            board.makeMove(move);

            if (board.isKingInCheck<color>()) {
                board.unmakeMove(move);
                continue;
            }

            searchContext.startPly = board.getPly();
            int score = -search<OPPOSITE_COLOR, PV>(board, alpha, beta, depth, searchContext,
                                                    searchStats, nodeLine);

            if (score > bestScore) {
                bestMove = move;
                bestScore = score;
                searchStats.score = bestScore;
                pvLine.moves[0] = bestMove;
                std::memcpy(pvLine.moves + 1, nodeLine.moves, nodeLine.moveCount * sizeof(Move));
                pvLine.moveCount = nodeLine.moveCount + 1;
            }

            board.unmakeMove(move);
        }

        board.setPvLine(pvLine);
        moveListPool->releaseMoveList(moves);
        searchStats.score = bestScore;
        printPv(searchStats, startTime, pvLine);
    }

    return bestMove;
}

template Move getBestMove<WHITE>(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                                 senjo::SearchStats& searchStats);
template Move getBestMove<BLACK>(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                                 senjo::SearchStats& searchStats);

template <PieceColor color, NodeType nodeType>
int search(Bitboard& board, int alpha, int beta, int depth, SearchContext& context,
           senjo::SearchStats& searchStats, Line& pvLine) {
    auto currentTime = std::chrono::steady_clock::now();

    if (currentTime > context.endTime) {
        // Immediately evaluate when time is up
        return Evaluation(board).evaluate();
    }

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (depth <= 0) {
        pvLine.moveCount = 0;
        // return qsearch<color, NodeType>();
        // No qsearch so we have a "base case" to test against
        return Evaluation(board).evaluate();
    }

    searchStats.nodes += 1;

    constexpr bool IS_PV_NODE = nodeType == PV;
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    bool doPvSearch = true;
    MoveList* moves = moveListPool->getMoveList();

    generateMoves<color>(board, moves);

    auto movePicker = MovePicker(moves);
    int legalMoveCount = 0;
    Line nodeLine{};

    while (movePicker.hasNext()) {
        Move move = movePicker.getNextMove();
        int score;

        board.makeMove(move);

        if (board.isKingInCheck<color>()) {
            board.unmakeMove(move);
            continue;
        }

        legalMoveCount += 1;

        if (IS_PV_NODE && doPvSearch) {
            score = -search<OPPOSITE_COLOR, PV>(board, -beta, -alpha, depth - 1, context,
                                                searchStats, nodeLine);
        } else {
            score = -search<OPPOSITE_COLOR, NO_PV>(board, -alpha - 1, -alpha, depth - 1, context,
                                                   searchStats, nodeLine);

            if (score > alpha) {
                score = -search<OPPOSITE_COLOR, PV>(board, -beta, -alpha, depth - 1, context,
                                                    searchStats, nodeLine);
            }
        }

        board.unmakeMove(move);

        if (score >= beta) {
            moveListPool->releaseMoveList(moves);
            return beta;
        }

        if (score > alpha) {
            alpha = score;
            doPvSearch = false;

            pvLine.moves[0] = move;
            std::memcpy(pvLine.moves + 1, nodeLine.moves, nodeLine.moveCount * sizeof(Move));
            pvLine.moveCount = nodeLine.moveCount + 1;
        }
    }

    moveListPool->releaseMoveList(moves);

    if (!legalMoveCount) {
        if (board.isKingInCheck<color>()) {
            return -MATE_SCORE + board.getPly();
        } else {
            return DRAW_SCORE;
        }
    }

    return alpha;
}

template int search<WHITE, PV>(Bitboard& board, int alpha, int beta, int depth,
                               SearchContext& context, senjo::SearchStats& searchStats, Line& line);
template int search<WHITE, NO_PV>(Bitboard& board, int alpha, int beta, int depth,
                                  SearchContext& context, senjo::SearchStats& searchStats,
                                  Line& line);
template int search<BLACK, PV>(Bitboard& board, int alpha, int beta, int depth,
                               SearchContext& context, senjo::SearchStats& searchStats, Line& line);
template int search<BLACK, NO_PV>(Bitboard& board, int alpha, int beta, int depth,
                                  SearchContext& context, senjo::SearchStats& searchStats,
                                  Line& line);

template <PieceColor color, NodeType nodeType>
int qsearch(Bitboard& board, int alpha, int beta, int depth, SearchContext& context,
            senjo::SearchStats& searchStats) {
    searchStats.qnodes += 1;
}

void printPv(senjo::SearchStats& searchStats, std::chrono::steady_clock::time_point& startTime,
             Line& pvLine) {
    searchStats.pv = "";
    for (int i = 0; i < pvLine.moveCount; i++) {
        Move move = pvLine.moves[i];

        searchStats.pv += getNotation(move.from) + getNotation(move.to);

        if (move.promotionPiece != EMPTY) {
            if (move.promotionPiece == WHITE_QUEEN || move.promotionPiece == BLACK_QUEEN) {
                searchStats.pv += "q";
            } else if (move.promotionPiece == WHITE_ROOK || move.promotionPiece == BLACK_ROOK) {
                searchStats.pv += "r";
            } else if (move.promotionPiece == WHITE_BISHOP || move.promotionPiece == BLACK_BISHOP) {
                searchStats.pv += "b";
            } else if (move.promotionPiece == WHITE_KNIGHT || move.promotionPiece == BLACK_KNIGHT) {
                searchStats.pv += "n";
            }
        }

        if (i != pvLine.moveCount - 1) {
            searchStats.pv += " ";
        }
    }

    searchStats.msecs =
        duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime)
        .count();
    senjo::Output(senjo::Output::NoPrefix) << searchStats;
}
} // namespace Zagreus
