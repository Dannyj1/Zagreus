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
#include "features.h"
#include "movegen.h"
#include "movelist_pool.h"
#include "movepicker.h"
#include "timemanager.h"
#include "tt.h"

namespace Zagreus {
MoveListPool* moveListPool = MoveListPool::getInstance();
TranspositionTable* tt = TranspositionTable::getTT();

template <PieceColor color>
Move getBestMove(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                 senjo::SearchStats& searchStats) {
    auto startTime = std::chrono::steady_clock::now();
    SearchContext searchContext{};
    searchContext.startTime = startTime;
    int depth = 0;
    int bestScore = MAX_NEGATIVE;
    Line pvLine{};

    tt->ageHistoryTable();

    while (!engine.stopRequested()) {
        auto currentTime = std::chrono::steady_clock::now();
        // Update the endtime using new data
        searchContext.endTime = getEndTime(searchContext, params, engine, board.getMovingColor());

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
            return pvLine.moves[0];
        }

        Move emptyMove{};
        int score = search<color, ROOT>(board, MAX_NEGATIVE, MAX_POSITIVE, depth, emptyMove,
                                        searchContext, searchStats, pvLine);
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

        board.setPvLine(pvLine);
        searchStats.score = score;
        printPv(searchStats, startTime, pvLine);
    }

    return pvLine.moves[0];
}

template Move getBestMove<WHITE>(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                                 senjo::SearchStats& searchStats);
template Move getBestMove<BLACK>(senjo::GoParams params, ZagreusEngine& engine, Bitboard& board,
                                 senjo::SearchStats& searchStats);

template <PieceColor color, NodeType nodeType>
int search(Bitboard& board, int alpha, int beta, int16_t depth, Move& previousMove,
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
        // Immediately evaluate when time is up
        return beta;
    }

    searchStats.nodes += 1;

    if (depth <= 0) {
        pvLine.moveCount = 0;
        return qsearch<color, nodeType>(board, alpha, beta, depth, previousMove, context,
                                        searchStats);
    }

    if (!IS_PV_NODE) {
        int ttScore = tt->getScore(board.getZobristHash(), depth, alpha,
                                   beta);

        if (ttScore != INT32_MIN) {
            return ttScore;
        }
    }

    bool isPreviousMoveNull = previousMove.from == NO_SQUARE && previousMove.to == NO_SQUARE;

    if (!IS_PV_NODE && depth >= 3 && !isPreviousMoveNull && board.getAmountOfMinorOrMajorPieces<
            color>() > 0) {
        bool ownKingInCheck = board.isKingInCheck<color>();

        if (!ownKingInCheck
            && Evaluation(board).evaluate() >= beta) {
            int r = 3 + (depth >= 6) + (depth >= 12);

            Move nullMove{NO_SQUARE, NO_SQUARE};
            Line nullLine{};
            SearchContext nullContext{};
            nullContext.startTime = context.startTime;
            nullContext.endTime = context.endTime;
            board.makeNullMove();
            int nullScore = -search<OPPOSITE_COLOR, NO_PV>(board, -beta, -beta + 1, depth - r,
                                                           nullMove, nullContext, searchStats,
                                                           nullLine);
            board.unmakeNullMove();
            int mateScores = MATE_SCORE - MAX_PLY;

            if (nullScore >= beta && nullScore < mateScores) {
                return nullScore;
            }
        }
    }

    bool doPvSearch = true;
    MoveList* moves = moveListPool->getMoveList();

    generateMoves<color, NORMAL>(board, moves);

    auto movePicker = MovePicker(moves);
    int legalMoveCount = 0;
    Line nodeLine{};
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
        previousMove = move;

        int score;
        if (IS_PV_NODE && doPvSearch) {
            score = -search<OPPOSITE_COLOR, PV>(board, -beta, -alpha, depth - 1, previousMove,
                                                context,
                                                searchStats, nodeLine);
        } else {
            score = -search<OPPOSITE_COLOR, NO_PV>(board, -alpha - 1, -alpha, depth - 1,
                                                   previousMove, context,
                                                   searchStats, nodeLine);

            if (score > alpha && score < beta) {
                score = -search<OPPOSITE_COLOR, PV>(board, -beta, -alpha, depth - 1, previousMove,
                                                    context,
                                                    searchStats, nodeLine);
            }
        }

        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;

                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE && move.promotionPiece == EMPTY) {
                        tt->killerMoves[2][board.getPly()] = tt->killerMoves[1][board.getPly()];
                        tt->killerMoves[1][board.getPly()] = tt->killerMoves[0][board.getPly()];
                        tt->killerMoves[0][board.getPly()] = encodeMove(&move);
                        tt->historyMoves[move.piece][move.to] += depth * depth;
                        tt->counterMoves[move.piece][move.to] = encodeMove(&move);
                    }

                    moveListPool->releaseMoveList(moves);
                    uint32_t bestMoveCode = encodeMove(&bestMove);
                    tt->addPosition(board.getZobristHash(), depth, score, FAIL_HIGH_NODE,
                                    bestMoveCode);
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
        if (board.isKingInCheck<color>()) {
            alpha = -MATE_SCORE + board.getPly();
        } else {
            alpha = DRAW_SCORE;
        }
    }

    TTNodeType ttNodeType = FAIL_LOW_NODE;

    if (IS_PV_NODE && (bestMove.from != NO_SQUARE && bestMove.to != NO_SQUARE)) {
        ttNodeType = EXACT_NODE;
    }

    uint32_t bestMoveCode = encodeMove(&bestMove);
    tt->addPosition(board.getZobristHash(), depth, alpha, ttNodeType, bestMoveCode);
    return alpha;
}

template <PieceColor color, NodeType nodeType>
int qsearch(Bitboard& board, int alpha, int beta, int16_t depth, Move& previousMove,
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

    if (!IS_PV_NODE) {
        int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha,
                                                            beta);

        if (ttScore != INT32_MIN) {
            return ttScore;
        }
    }

    searchStats.qnodes += 1;

    bool inCheck = board.isKingInCheck<color>();

    if (!inCheck) {
        int standPat = Evaluation(board).evaluate();

        if (standPat >= beta) {
            tt->addPosition(board.getZobristHash(), depth, standPat, FAIL_HIGH_NODE, 0);
            return standPat;
        }

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

        if (alpha < standPat) {
            alpha = standPat;
        }
    }

    MoveList* moves = moveListPool->getMoveList();

    if (inCheck) {
        // TODO: implement evasions eval. Have a validSquares argument for each generate move function which determines which squares can be moved to. This will also clean up the movegen code a bit.
        generateMoves<color, NORMAL>(board, moves);
    } else {
        generateMoves<color, QUIESCE>(board, moves);
    }

    auto movePicker = MovePicker(moves);
    int legalMoveCount = 0;
    previousMove = {};
    int bestScore = MAX_NEGATIVE;
    Move bestMove = {NO_SQUARE, NO_SQUARE};

    while (movePicker.hasNext()) {
        Move move = movePicker.getNextMove();

        if (!inCheck && move.captureScore <= NO_CAPTURE_SCORE) {
            continue;
        }

        board.makeMove(move);

        if (board.isKingInCheck<color>()) {
            board.unmakeMove(move);
            continue;
        }

        legalMoveCount += 1;

        int score = -qsearch<OPPOSITE_COLOR, nodeType>(board, -beta, -alpha, depth - 1,
                                                       previousMove, context, searchStats);
        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;

                if (score >= beta) {
                    moveListPool->releaseMoveList(moves);
                    tt->addPosition(board.getZobristHash(), depth, score, FAIL_HIGH_NODE, 0);
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

    tt->addPosition(board.getZobristHash(), depth, alpha, ttNodeType, 0);
    return alpha;
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
