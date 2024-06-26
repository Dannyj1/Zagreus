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

#include <cmath>

#include "../senjo/Output.h"
#include "evaluate.h"
#include "features.h"
#include "movegen.h"
#include "movelist_pool.h"
#include "movepicker.h"
#include "timemanager.h"
#include "tt.h"

namespace Zagreus {
TranspositionTable* tt = TranspositionTable::getTT();
static int lmrReductions[MAX_PLY][MAX_MOVES]{};

void initializeSearch() {
    for (int depth = 0; depth < MAX_PLY; depth++) {
        for (int movesPlayed = 0; movesPlayed < MAX_MOVES; movesPlayed++) {
            // Formula from ethereal: https://github.com/AndyGrant/Ethereal/blob/a7a7a8ed69cbbb4e9a3b02fc5d3d0d9facfa1526/src/search.c#L155C13-L155C21
            // Will probably tune the constants using SPSA at some point
            lmrReductions[depth][movesPlayed] = static_cast<int>(0.78 + std::log(depth) * log(movesPlayed) / 2.47);
        }
    }
}

template <PieceColor color>
Move getBestMove(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                 senjo::SearchStats& searchStats) {
    MoveListPool* moveListPool = MoveListPool::getInstance();
    auto startTime = std::chrono::steady_clock::now();
    SearchContext searchContext{};
    searchContext.startTime = startTime;
    int depth = 0;
    int bestScore = MAX_NEGATIVE;
    Line bestPvLine{};
    Line pvLine{};
    pvLine.startPly = board.getPly();

    tt->ageHistoryTable();

    while (!engine.stopRequested()) {
        // Update the endtime using new data
        searchContext.endTime = getEndTime(searchContext, params, engine, board.getMovingColor());

        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime > searchContext.endTime) {
            engine.stopSearching();
            break;
        }

        depth += 1;
        searchStats.depth = depth;
        searchStats.seldepth = 0;

        if (board.getPly() + depth >= MAX_PLY + 1) {
            break;
        }

        // If the go command has a max depth argument, terminate when reaching the desired depth.
        if (params.depth > 0 && depth > params.depth) {
            return pvLine.moves[0];
        }

        int score = search<color, ROOT>(board, MAX_NEGATIVE, MAX_POSITIVE, depth, searchContext,
                                        searchStats, pvLine);

        currentTime = std::chrono::steady_clock::now();
        if (currentTime > searchContext.endTime) {
            engine.stopSearching();
            break;
        }

        Move bestMove = pvLine.moves[0];
        Move previousBestMove = board.getPvLine().moves[0];

        // If bestScore is positive and iterationScore is 0 or negative or vice versa, set suddenScoreSwing to true
        if (depth > 1 && ((bestScore > 0 && score < 0) || (bestScore < 0 && score > 0))) {
            searchContext.suddenScoreSwing = true;
        }

        // If the iterationScore suddenly dropped by 150 or more from bestScore, set suddenScoreDrop to true
        if (depth > 1 && score - bestScore <= -150) {
            searchContext.suddenScoreDrop = true;
        }

        // If bestMove changes, increment context.pvChanges
        if (depth > 1 && (bestMove.from != previousBestMove.from || bestMove.to != previousBestMove.
                          to)) {
            searchContext.pvChanges += 1;
        }

        if (score > bestScore) {
            bestScore = score;
        }

        bestPvLine = pvLine;
        board.setPvLine(bestPvLine);
        searchStats.score = score;
        printPv(searchStats, startTime, bestPvLine);
    }

    engine.stopSearching();
    Move bestMove = bestPvLine.moves[0];
    MoveList* legalMoves = moveListPool->getMoveList();
    generateMoves<color, NORMAL>(board, legalMoves);

    // Check if bestMove is a legal move (sometimes in endgames that drag on for long time, the PV is empty)
    for (int i = 0; i < legalMoves->size; i++) {
        if (legalMoves->moves[i].from == bestMove.from && legalMoves->moves[i].to == bestMove.to) {
            moveListPool->releaseMoveList(legalMoves);
            return bestMove;
        }
    }

    return legalMoves->moves[0];
}

template Move getBestMove<WHITE>(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                                 senjo::SearchStats& searchStats);
template Move getBestMove<BLACK>(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                                 senjo::SearchStats& searchStats);

template <PieceColor color, NodeType nodeType>
int search(Bitboard& board, int alpha, int beta, int16_t depth,
           SearchContext& context,
           senjo::SearchStats& searchStats, Line& pvLine) {
    constexpr bool IS_PV_NODE = nodeType == PV || nodeType == ROOT;
    constexpr bool IS_ROOT_NODE = nodeType == ROOT;
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    auto currentTime = std::chrono::steady_clock::now();
    if (!IS_ROOT_NODE && (currentTime > context.endTime || board.getPly() >= MAX_PLY)) {
        pvLine.moveCount = 0;
        return beta;
    }

    searchStats.nodes += 1;

    bool ownKingInCheck = board.isKingInCheck<color>();
    if (ownKingInCheck) {
        int see = board.seeOpponent<OPPOSITE_COLOR>(board.getPreviousMove().to);

        if (see >= NO_CAPTURE_SCORE) {
            depth += 1;
        }
    }

    if (depth <= 0) {
        pvLine.moveCount = 0;
        return qsearch<color, nodeType>(board, alpha, beta, depth, context, searchStats);
    }

    if (!IS_PV_NODE && board.getHalfMoveClock() < 80) {
        int ttScore = tt->getScore(board.getZobristHash(), depth, alpha,
                                   beta, board.getPly());

        if (ttScore != INT32_MIN) {
            return ttScore;
        }
    }

    constexpr bool isPreviousMoveNull = nodeType == NULL_MOVE;

    // Null move pruning
    if (!IS_PV_NODE && depth >= 3 && !isPreviousMoveNull && board.
        getAmountOfMinorOrMajorPieces<
            color>() > 0) {
        if (!ownKingInCheck && Evaluation(board).evaluate() >= beta) {
            int r = 3 + (depth >= 6) + (depth >= 12);

            Line nullLine{};
            SearchContext nullContext{};
            nullContext.startTime = context.startTime;
            nullContext.endTime = context.endTime;
            board.makeNullMove();
            int nullScore = -search<OPPOSITE_COLOR, NULL_MOVE>(board, -beta, -beta + 1, depth - r,
                                                               nullContext, searchStats, nullLine);
            board.unmakeNullMove();
            int mateScores = MATE_SCORE - MAX_PLY;

            if (nullScore >= beta && nullScore < mateScores) {
                return nullScore;
            }
        }
    }

    bool doPvSearch = true;
    MoveListPool* moveListPool = MoveListPool::getInstance();
    MoveList* moves = moveListPool->getMoveList();

    if (ownKingInCheck) {
        generateMoves<color, EVASIONS>(board, moves);
    } else {
        generateMoves<color, NORMAL>(board, moves);
    }

    auto movePicker = MovePicker(moves);
    int legalMoveCount = 0;
    Line nodeLine{};
    nodeLine.startPly = board.getPly();
    int bestScore = MAX_NEGATIVE;
    Move bestMove = {NO_SQUARE, NO_SQUARE};

    while (movePicker.hasNext()) {
        Move move = movePicker.getNextMove();
        board.makeMove(move);

        if (board.isKingInCheck<color>()) {
            board.unmakeMove(move);
            continue;
        }

        legalMoveCount += 1;

        int score = 0;
        bool didLmr = false;
        bool shouldFullSearch = false;

        // Late Move Reduction (LMR, not in Root nodes)
        if (!IS_ROOT_NODE && depth >= 3 && move.captureScore == NO_CAPTURE_SCORE && move.
            promotionPiece == EMPTY && legalMoveCount > 1) {
            int R = std::max(0, lmrReductions[depth][legalMoveCount]);

            // Increase reduction for non-PV nodes
            R += !IS_PV_NODE;

            // Decrease reduction when in check
            R -= ownKingInCheck;

            // Decrease reduction for killer moves
            uint64_t moveCode = encodeMove(&move);
            if (tt->killerMoves[0][board.getPly()] == moveCode
                || tt->killerMoves[1][board.getPly()] == moveCode
                || tt->killerMoves[2][board.getPly()] == moveCode) {
                R -= 1;
            }

            // Decrease for counter moves
            if (tt->counterMoves[move.piece][move.to] == moveCode) {
                R -= 1;
            }

            // Don't drop into qsearch
            R = std::min(depth - 1, std::max(1, R));

            // Depth - 1 (R = 1) is the "default" search, so skip LMR
            if (R > 1) {
                score = -search<OPPOSITE_COLOR, NO_PV>(
                    board, -alpha - 1, -alpha, depth - R, context, searchStats, nodeLine);

                didLmr = true;

                if (score > alpha) {
                    shouldFullSearch = true;
                }
            }
        }

        if (!didLmr || shouldFullSearch) {
            if (IS_PV_NODE && doPvSearch) {
                score = -search<OPPOSITE_COLOR, PV>(board, -beta, -alpha, depth - 1,
                                                    context,
                                                    searchStats, nodeLine);
            } else {
                score = -search<OPPOSITE_COLOR, NO_PV>(board, -alpha - 1, -alpha,
                                                       depth - 1, context,
                                                       searchStats, nodeLine);

                if (score > alpha && score < beta) {
                    score = -search<OPPOSITE_COLOR, PV>(board, -beta, -alpha,
                                                        depth - 1, context,
                                                        searchStats, nodeLine);
                }
            }
        }

        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;

                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE && move.promotionPiece == EMPTY) {
                        uint64_t moveCode = encodeMove(&move);
                        tt->killerMoves[2][board.getPly()] = tt->killerMoves[1][board.getPly()];
                        tt->killerMoves[1][board.getPly()] = tt->killerMoves[0][board.getPly()];
                        tt->killerMoves[0][board.getPly()] = moveCode;
                        tt->historyMoves[move.piece][move.to] += depth * depth;

                        if (!isPreviousMoveNull) {
                            tt->counterMoves[board.getPreviousMove().piece][board.getPreviousMove().
                                to] = moveCode;
                        }
                    }

                    moveListPool->releaseMoveList(moves);
                    if (!IS_ROOT_NODE) {
                        uint32_t bestMoveCode = encodeMove(&bestMove);
                        tt->addPosition(board.getZobristHash(), depth, score, FAIL_HIGH_NODE,
                                        bestMoveCode, board.getPly(), context);
                    }
                    return score;
                }

                alpha = score;
                doPvSearch = false;

                pvLine.moves[0] = move;
                std::memcpy(pvLine.moves + 1, nodeLine.moves, nodeLine.moveCount * sizeof(Move));
                pvLine.moveCount = nodeLine.moveCount + 1;
            }
        }
    }

    moveListPool->releaseMoveList(moves);

    if (!legalMoveCount) {
        if (ownKingInCheck) {
            alpha = -MATE_SCORE + board.getPly();
        } else {
            alpha = DRAW_SCORE;
        }
    }

    TTNodeType ttNodeType = FAIL_LOW_NODE;

    if (IS_PV_NODE && (bestMove.from != NO_SQUARE && bestMove.to != NO_SQUARE)) {
        ttNodeType = EXACT_NODE;
    }

    if (!IS_ROOT_NODE) {
        uint32_t bestMoveCode = encodeMove(&bestMove);
        tt->addPosition(board.getZobristHash(), depth, alpha, ttNodeType, board.getPly(),
                        bestMoveCode, context);
    }

    return alpha;
}

template <PieceColor color, NodeType nodeType>
int qsearch(Bitboard& board, int alpha, int beta, int16_t depth,
            SearchContext& context,
            senjo::SearchStats& searchStats) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    constexpr TTNodeType IS_PV_NODE = nodeType == PV ? EXACT_NODE : FAIL_LOW_NODE;

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    auto currentTime = std::chrono::steady_clock::now();
    if (currentTime > context.endTime || board.getPly() >= MAX_PLY) {
        return beta;
    }

    if (!IS_PV_NODE && board.getHalfMoveClock() < 80) {
        int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha,
                                                            beta, board.getPly());

        if (ttScore != INT32_MIN) {
            return ttScore;
        }
    }

    searchStats.qnodes += 1;

    bool inCheck = board.isKingInCheck<color>();
    Move previousMove = board.getPreviousMove();

    if (!inCheck) {
        int standPat = Evaluation(board).evaluate();

        if (standPat >= beta) {
            tt->addPosition(board.getZobristHash(), depth, standPat, FAIL_HIGH_NODE, 0,
                            board.getPly(), context);
            return standPat;
        }

        if (board.getAmountOfMinorOrMajorPieces<color>() >= 2 && board.getAmountOfMinorOrMajorPieces
            <OPPOSITE_COLOR>() >= 2 && board.getAmountOfPawns<color>() > 0 && board.getAmountOfPawns
            <OPPOSITE_COLOR>() > 0) {
            int queenDelta = std::max(getEvalValue(ENDGAME_QUEEN_MATERIAL),
                                      getEvalValue(MIDGAME_QUEEN_MATERIAL));
            int minPawnValue = std::min(getEvalValue(ENDGAME_PAWN_MATERIAL),
                                        getEvalValue(MIDGAME_PAWN_MATERIAL));

            if (previousMove.promotionPiece != EMPTY) {
                queenDelta += getPieceWeight(previousMove.promotionPiece) - minPawnValue;
            }

            if (standPat < alpha - queenDelta) {
                return alpha;
            }
        }

        if (alpha < standPat) {
            alpha = standPat;
        }
    }

    MoveListPool* moveListPool = MoveListPool::getInstance();
    MoveList* moves = moveListPool->getMoveList();

    if (inCheck) {
        generateMoves<color, EVASIONS>(board, moves);
    } else {
        generateMoves<color, QSEARCH>(board, moves);
    }

    auto movePicker = MovePicker(moves);
    int legalMoveCount = 0;
    previousMove = {};
    int bestScore = MAX_NEGATIVE;
    Move bestMove = {NO_SQUARE, NO_SQUARE};

    while (movePicker.hasNext()) {
        Move move = movePicker.getNextMove();

        if (!inCheck && move.captureScore < NO_CAPTURE_SCORE) {
            continue;
        }

        board.makeMove(move);

        if (board.isKingInCheck<color>()) {
            board.unmakeMove(move);
            continue;
        }

        legalMoveCount += 1;

        int score = -qsearch<OPPOSITE_COLOR, nodeType>(board, -beta, -alpha, depth - 1, context,
                                                       searchStats);
        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;

                if (score >= beta) {
                    moveListPool->releaseMoveList(moves);
                    uint32_t bestMoveCode = encodeMove(&bestMove);
                    tt->addPosition(board.getZobristHash(), depth, score, FAIL_HIGH_NODE,
                                    bestMoveCode, board.getPly(), context);
                    return beta;
                }

                alpha = score;
            }
        }
    }

    moveListPool->releaseMoveList(moves);

    if (legalMoveCount == 0 && inCheck) {
        return -MATE_SCORE + board.getPly();
    }

    TTNodeType ttNodeType = FAIL_LOW_NODE;

    if (IS_PV_NODE && (bestMove.from != NO_SQUARE && bestMove.to != NO_SQUARE)) {
        ttNodeType = EXACT_NODE;
    }

    uint32_t bestMoveCode = encodeMove(&bestMove);
    tt->addPosition(board.getZobristHash(), depth, alpha, ttNodeType, bestMoveCode, board.getPly(),
                    context);
    return alpha;
}

template int qsearch<WHITE, PV>(Bitboard& board, int alpha, int beta,
                                int16_t depth, SearchContext& context,
                                senjo::SearchStats& searchStats);
template int qsearch<BLACK, PV>(Bitboard& board, int alpha, int beta,
                                int16_t depth, SearchContext& context,
                                senjo::SearchStats& searchStats);

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