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

#include <iostream>
#include <chrono>

#include "search.h"
#include "timemanager.h"
#include "movegen.h"
#include "../senjo/Output.h"
#include "movepicker.h"
#include "pst.h"
#include "tt.h"
#include "features.h"
#include "evaluate.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    // Half the biggest pawn value
//    int initialAspirationWindow = std::max(getEvalValue(MIDGAME_PAWN_MATERIAL), getEvalValue(ENDGAME_PAWN_MATERIAL)) * 0.5;

    Move SearchManager::getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board) {
        std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
        TimeContext context{};
        context.startTime = startTime;
        std::chrono::time_point<std::chrono::steady_clock> endTime = getEndTime(context, params, engine, board.getMovingColor());
        searchStats = {};
        isSearching = true;
        int bestScore = -1000000;
        Move bestMove = {};
        int iterationScore = -1000000;
        int alpha = -1000000;
        int beta = 1000000;
//        int alphaWindow = initialAspirationWindow;
//        int betaWindow = initialAspirationWindow;
        Move iterationMove = {};
        int depth = 0;

        TranspositionTable::getTT()->ageHistoryTable();

        Line iterationPvLine = {};
        while (!engine.stopRequested() && (std::chrono::steady_clock::now() - startTime < (endTime - startTime) * 0.5 || depth == 0)) {
            depth += 1;

            if (params.depth > 0 && depth > params.depth) {
                return bestMove;
            }

            searchStats.depth = depth;
            searchStats.seldepth = 0;

            senjo::Output(senjo::Output::InfoPrefix) << "Searching depth " << depth << "...";
            board.setPreviousPvLine(iterationPvLine);

            MoveList* moveList = moveListPool->getMoveList();
            if (board.getMovingColor() == WHITE) {
                generateMoves<WHITE>(board, moveList);
            } else {
                generateMoves<BLACK>(board, moveList);
            }

            if (depth == 0) {
                context.rootMoveCount = moveList->size;
            }

            endTime = getEndTime(context, params, engine, board.getMovingColor());
            auto moves = MovePicker(moveList);

            while (moves.hasNext()) {
                Move move = moves.getNextMove();
                assert(move.from != move.to);
                assert(move.from >= 0 && move.from < 64);
                assert(move.to >= 0 && move.to < 64);

                board.makeMove(move);

                if (board.getMovingColor() == WHITE) {
                    if (board.isKingInCheck<BLACK>()) {
                        board.unmakeMove(move);
                        continue;
                    }
                } else {
                    if (board.isKingInCheck<WHITE>()) {
                        board.unmakeMove(move);
                        continue;
                    }
                }

                Line pvLine = {};
                Move previousMove = {};
                int score;

                if (board.getMovingColor() == WHITE) {
                    score = search<WHITE>(board, depth, alpha, beta, move, previousMove, endTime, pvLine, engine, true, true);
                } else {
                    score = search<BLACK>(board, depth, alpha, beta, move, previousMove, endTime, pvLine, engine, true, true);
                }

                score *= -1;
                board.unmakeMove(move);

                if (iterationScore == -1000000 || (score > iterationScore && std::chrono::steady_clock::now() < endTime)) {
                    assert(move.piece != PieceType::EMPTY);
                    iterationScore = score;
                    iterationMove = move;

                    iterationPvLine.moves[0] = move;
                    memcpy(iterationPvLine.moves + 1, pvLine.moves, pvLine.moveCount * sizeof(Move));
                    iterationPvLine.moveCount = pvLine.moveCount + 1;

                    searchStats.score = iterationScore;
                }

                searchStats.pv = "";
                for (int i = 0; i < iterationPvLine.moveCount; i++) {
                    Move move = iterationPvLine.moves[i];

                    searchStats.pv += getNotation(move.from) + getNotation(move.to);

                    if (i != iterationPvLine.moveCount - 1) {
                        searchStats.pv += " ";
                    }
                }

                searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - startTime).count();
                senjo::Output(senjo::Output::NoPrefix) << searchStats;
            }

            if (depth == 1 || bestScore == -1000000 || std::chrono::steady_clock::now() < endTime) {
                assert(iterationMove.piece != PieceType::EMPTY);
                // If bestScore is positive and iterationScore is 0 or negative or vice versa, set suddenScoreSwing to true
                if (depth > 1 && ((bestScore > 0 && iterationScore <= 0) || (bestScore < 0 && iterationScore >= 0))) {
                    context.suddenScoreSwing = true;
                }

                // If the iterationScore suddenly dropped by 150 or more from bestScore, set suddenScoreDrop to true
                if (depth > 1 && bestScore - iterationScore >= 150) {
                    context.suddenScoreDrop = true;
                }

                bestScore = iterationScore;

                // If bestMove changes, increment context.pvChanges
                if (depth > 1 && (bestMove.from != iterationMove.from || bestMove.to != iterationMove.to)) {
                    context.pvChanges += 1;
                }

                bestMove = iterationMove;
                searchStats.score = bestScore;

                /*if (depth >= 3) {
                    if (bestScore <= alpha) {
                        alpha += alphaWindow;
                        alphaWindow *= 4;
                        alpha -= alphaWindow;
                        depth -= 1;
                        continue;
                    }

                    if (bestScore >= beta) {
                        beta -= betaWindow;
                        betaWindow *= 4;
                        beta += betaWindow;
                        depth -= 1;
                        continue;
                    }

                    alpha = bestScore - alphaWindow;
                    beta = bestScore + betaWindow;
                }*/
            }

            iterationScore = -1000000;
            iterationMove = {};
            moveListPool->releaseMoveList(moveList);
        }

        searchStats.pv = "";
        for (int i = 0; i < iterationPvLine.moveCount; i++) {
            Move move = iterationPvLine.moves[i];

            searchStats.pv += getNotation(move.from) + getNotation(move.to);

            if (i != iterationPvLine.moveCount - 1) {
                searchStats.pv += " ";
            }
        }

        searchStats.score = bestScore;
        searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count();
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
        return bestMove;
    }

    template<PieceColor color>
    int SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                              Move &previousMove,
                              std::chrono::time_point<std::chrono::steady_clock> &endTime, Line &pvLine, ZagreusEngine &engine, bool isPv, bool canNull) {
        if (board.getPly() >= MAX_PLY || engine.stopRequested() || std::chrono::steady_clock::now() > endTime) {
            return beta;
        }

        searchStats.nodes += 1;

        bool depthExtended = false;
        bool isOwnKingInCheck = board.isKingInCheck<color>();

        if (isOwnKingInCheck) {
            depth += 1;
            depthExtended = true;
        }

        if (depth == 0 || board.isWinner<WHITE>() || board.isWinner<BLACK>() ||
            board.isDraw()) {
            pvLine.moveCount = 0;
            return quiesce<color>(board, alpha, beta, rootMove, previousMove, endTime, engine, isPv);
        }

        if (!isPv) {
            int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha, beta);

            if (ttScore != INT32_MIN) {
                return ttScore;
            }
        }

        Line line{};

        if (!depthExtended && !isPv && canNull && depth >= 3 && board.hasMinorOrMajorPieces()) {
            board.makeNullMove();
            int R = depth > 6 ? 3 : 2;
            Move nullMove = {};
            int score;

            if (color == WHITE) {
                score = search<BLACK>(board, depth - R - 1, -beta, -beta + 1, rootMove, nullMove, endTime, line,
                                      engine, false, false);
            } else {
                score = search<WHITE>(board, depth - R - 1, -beta, -beta + 1, rootMove, nullMove, endTime, line,
                                      engine, false, false);
            }

            score *= -1;
            board.unmakeNullMove();

            if (score >= beta) {
                return beta;
            }
        }

        MoveList* moveList = moveListPool->getMoveList();
        NodeType nodeType = FAIL_LOW_NODE;

        generateMoves<color>(board, moveList);
        auto moves = MovePicker(moveList);
        uint32_t bestMoveCode = 0;

        while (moves.hasNext()) {
            if (std::chrono::steady_clock::now() > endTime) {
                return beta;
            }

            Move move = moves.getNextMove();

            board.makeMove(move);

            if (board.isKingInCheck<color>()) {
                board.unmakeMove(move);
                continue;
            }

            __builtin_prefetch(TranspositionTable::getTT()->getEntry(board.getZobristHash()), 0, 3);

            int depthReduction = 0;
            bool isOpponentKingInCheck;

            if (color == WHITE) {
                isOpponentKingInCheck = board.isKingInCheck<BLACK>();
            } else {
                isOpponentKingInCheck = board.isKingInCheck<WHITE>();
            }

            // Late move reduction
            if (!depthExtended && !isPv) {
                if (depth >= 3 && moves.movesSearched() > 4 && move.captureScore != -1 &&
                    move.promotionPiece == EMPTY && !isOwnKingInCheck && !isOpponentKingInCheck) {
                    // Scale the reduction value between 1 and (depth - 1), depending on how many moves have been searched.
                    // It should reach (depth - 1) when 60% of the moves have been searched.
                    int R = 1 + (int) ((depth - 1) * (1 - moves.movesSearched() / (0.6 * moves.size())));
                    depthReduction += R;
                }
            }

            int score;

            if (isPv) {
                if (color == WHITE) {
                    score = search<BLACK>(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                                          engine, true, false);
                } else {
                    score = search<WHITE>(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                                          engine, true, false);
                }

                score *= -1;
            } else {
                if (color == WHITE) {
                    score = search<BLACK>(board, depth - 1 - depthReduction, -alpha - 1, -alpha, rootMove,
                                          previousMove, endTime, line, engine, false, canNull);
                } else {
                    score = search<WHITE>(board, depth - 1 - depthReduction, -alpha - 1, -alpha, rootMove,
                                          previousMove, endTime, line, engine, false, canNull);
                }

                score *= -1;

                if (score > alpha && score < beta) {
                    if (color == WHITE) {
                        score = search<BLACK>(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                                              engine, true, false);
                    } else {
                        score = search<WHITE>(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                                              engine, true, false);
                    }

                    score *= -1;
                }
            }

            board.unmakeMove(move);

            if (score > alpha) {
                bestMoveCode = encodeMove(&move);

                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(&move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(&move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta, FAIL_HIGH_NODE, bestMoveCode, endTime);
                    moveListPool->releaseMoveList(moveList);
                    return score;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
                nodeType = PV_NODE;
                isPv = false;
                TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, FAIL_LOW_NODE, bestMoveCode, endTime);
            }
        }

        TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, nodeType, bestMoveCode, endTime);
        moveListPool->releaseMoveList(moveList);
        return alpha;
    }

    int delta = std::max(getEvalValue(ENDGAME_QUEEN_MATERIAL), getEvalValue(MIDGAME_QUEEN_MATERIAL));
    int minPawnValue = std::min(getEvalValue(ENDGAME_PAWN_MATERIAL), getEvalValue(MIDGAME_PAWN_MATERIAL));
    template<PieceColor color>
    int SearchManager::quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                               Move &previousMove,
                               std::chrono::time_point<std::chrono::steady_clock> &endTime, ZagreusEngine &engine, bool isPv, int depth) {
        if (board.getPly() >= MAX_PLY) {
            return Evaluation(board).evaluate();
        }

        if (engine.stopRequested() || std::chrono::steady_clock::now() > endTime) {
            return beta;
        }

        searchStats.qnodes += 1;

        int standPat = Evaluation(board).evaluate();

        if (standPat >= beta) {
            return beta;
        }

        if (previousMove.promotionPiece != EMPTY) {
            delta += getPieceWeight(previousMove.promotionPiece) - minPawnValue;
        }

        if (standPat < alpha - delta) {
            return alpha;
        }

        if (alpha < standPat) {
            alpha = standPat;
        }

        MoveList* moveList = moveListPool->getMoveList();
        generateQuiescenceMoves<color>(board, moveList);

        auto moves = MovePicker(moveList);
        while (moves.hasNext()) {
            if (std::chrono::steady_clock::now() > endTime) {
                return beta;
            }

            Move move = moves.getNextMove();
            assert(move.from != move.to);

            if (move.captureScore <= NO_CAPTURE_SCORE) {
                continue;
            }

            board.makeMove(move);

            if (board.isKingInCheck<color>()) {
                board.unmakeMove(move);
                continue;
            }

            __builtin_prefetch(TranspositionTable::getTT()->getEntry(board.getZobristHash()), 0, 3);

            int score;

            if (color == WHITE) {
                score = quiesce<BLACK>(board, -beta, -alpha, rootMove, move, endTime, engine, depth - 1);
            } else {
                score = quiesce<WHITE>(board, -beta, -alpha, rootMove, move, endTime, engine, depth - 1);
            }

            score *= -1;
            board.unmakeMove(move);

            if (score >= beta) {
                moveListPool->releaseMoveList(moveList);
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }

        moveListPool->releaseMoveList(moveList);
//        TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, NodeType::PV_NODE);
        return alpha;
    }

    bool SearchManager::isCurrentlySearching() {
        return isSearching;
    }

    senjo::SearchStats SearchManager::getSearchStats() {
        return searchStats;
    }
}

#pragma clang diagnostic pop