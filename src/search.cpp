/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    Move SearchManager::getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board) {
        searchStats = {};
        isSearching = true;
        int bestScore = -1000000;
        Move bestMove = {};
        int iterationScore = -1000000;
        int alpha = -1000000;
        int beta = 1000000;
        int alphaWindow = 50;
        int betaWindow = 50;
        Move iterationMove = {};
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime = getEndTime(params, board, engine, board.getMovingColor());
        int depth = 0;

        TranspositionTable::getTT()->ageHistoryTable();

        Line iterationPvLine = {};
        while (!engine.stopRequested() && std::chrono::high_resolution_clock::now() - startTime < (endTime - startTime) * 0.7) {
            depth += 1;

            if (params.depth > 0 && depth > params.depth) {
                return bestMove;
            }

            searchStats.depth = depth;
            searchStats.seldepth = 0;

            senjo::Output(senjo::Output::InfoPrefix) << "Searching depth " << depth << "...";
            board.setPreviousPvLine(iterationPvLine);

            MoveList* moveList = moveListPool->getMoveList();
            if (board.getMovingColor() == PieceColor::WHITE) {
                generateMoves<PieceColor::WHITE>(board, moveList);
            } else {
                generateMoves<PieceColor::BLACK>(board, moveList);
            }

            MovePicker moves = MovePicker(moveList);

            if (moves.size() == 1) {
                moveListPool->releaseMoveList(moveList);
                return bestMove;
            }

            while (moves.hasNext()) {
                Move move = moves.getNextMove();
                assert(move.from != move.to);
                assert(move.from >= 0 && move.from < 64);
                assert(move.to >= 0 && move.to < 64);

                board.makeMove(move);

                if (board.getMovingColor() == PieceColor::WHITE) {
                    if (board.isKingInCheck<PieceColor::BLACK>()) {
                        board.unmakeMove(move);
                        continue;
                    }
                } else {
                    if (board.isKingInCheck<PieceColor::WHITE>()) {
                        board.unmakeMove(move);
                        continue;
                    }
                }

                __builtin_prefetch(TranspositionTable::getTT()->getEntry(board.getZobristHash()), 0, 3);

                Line pvLine = {};
                Move previousMove = {};

                int score = search(board, depth, alpha, beta, move, previousMove, endTime, pvLine, engine, true, true);
                score *= -1;

                board.unmakeMove(move);

                if (iterationScore == -1000000 || (score > iterationScore && std::chrono::high_resolution_clock::now() < endTime)) {
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
                        std::chrono::high_resolution_clock::now() - startTime).count();
                senjo::Output(senjo::Output::NoPrefix) << searchStats;
            }

            if (depth == 1 || bestScore == -1000000 || std::chrono::high_resolution_clock::now() < endTime) {
                assert(iterationMove.piece != PieceType::EMPTY);
                bestScore = iterationScore;
                bestMove = iterationMove;
                searchStats.score = bestScore;

                if (depth >= 2) {
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
                }
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
                std::chrono::high_resolution_clock::now() - startTime).count();
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
        return bestMove;
    }

    // TODO: use template
    int SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                              Move &previousMove,
                              std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine, ZagreusEngine &engine, bool isPv, bool canNull) {
        searchStats.nodes += 1;

        if (searchStats.nodes % 2048 == 0 &&
            (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return beta;
        }

        bool isOwnKingInCheck = false;
        bool depthExtended = false;

        if (board.getMovingColor() == PieceColor::WHITE) {
            isOwnKingInCheck = board.isKingInCheck<PieceColor::WHITE>();
        } else {
            isOwnKingInCheck = board.isKingInCheck<PieceColor::BLACK>();
        }

        if (isOwnKingInCheck) {
            depth += 1;
            depthExtended = true;
        }

        if (depth == 0 || board.isWinner<PieceColor::WHITE>() || board.isWinner<PieceColor::BLACK>() ||
            board.isDraw()) {
            pvLine.moveCount = 0;
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha, beta);

        if (!isPv && ttScore != INT32_MIN) {
            return ttScore;
        }

/*        int amountOfPieces = 0;

        if (board.getMovingColor() == PieceColor::WHITE) {
            amountOfPieces = popcnt(board.getColorBoard<PieceColor::WHITE>());
        } else {
            amountOfPieces = popcnt(board.getColorBoard<PieceColor::BLACK>());
        }*/

        Line line{};

        if (!depthExtended && !isPv && canNull && depth >= 3 && board.hasMinorOrMajorPieces() && evaluate(board, endTime, engine) >= beta) {
            board.makeNullMove();
            int R = depth > 6 ? 3 : 2;
            Move nullMove = {};
            int score = search(board, depth - R - 1, -beta, -beta + 1, rootMove, nullMove, endTime, line,
                               engine, false, false);
            score *= -1;
            board.unmakeNullMove();

            if (score >= beta) {
                return beta;
            }
        }

        Move bestMove = {};
        int bestScore = -1000000;
        MoveList* moveList = moveListPool->getMoveList();
        NodeType nodeType = NodeType::FAIL_LOW_NODE;

        if (board.getMovingColor() == PieceColor::WHITE) {
            generateMoves<PieceColor::WHITE>(board, moveList);
        } else {
            generateMoves<PieceColor::BLACK>(board, moveList);
        }

        MovePicker moves = MovePicker(moveList);
        bool searchedFirstLegalMove = false;

        while (isPv && !searchedFirstLegalMove && moves.hasNext()) {
            Move move = moves.getNextMove();

            board.makeMove(move);

            if (board.getMovingColor() == PieceColor::WHITE) {
                if (board.isKingInCheck<PieceColor::BLACK>()) {
                    board.unmakeMove(move);
                    continue;
                }
            } else {
                if (board.isKingInCheck<PieceColor::WHITE>()) {
                    board.unmakeMove(move);
                    continue;
                }
            }

            __builtin_prefetch(TranspositionTable::getTT()->getEntry(board.getZobristHash()), 0, 3);

            int score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                               engine, true, false);
            score *= -1;

            if (score > bestScore) {
                bestMove = move;
            }

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(&move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(&move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta,
                                                             NodeType::FAIL_HIGH_NODE,
                                                             encodeMove(&bestMove));
                    moveListPool->releaseMoveList(moveList);
                    return score;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
                nodeType = NodeType::PV_NODE;
            }

            searchedFirstLegalMove = true;
        }

        if (isPv && !searchedFirstLegalMove) {
            moveListPool->releaseMoveList(moveList);
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        while (moves.hasNext()) {
            if (searchStats.nodes % 2048 == 0 &&
                (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
                moveListPool->releaseMoveList(moveList);
                return beta;
            }

            Move move = moves.getNextMove();

            board.makeMove(move);

            if (board.getMovingColor() == PieceColor::WHITE) {
                if (board.isKingInCheck<PieceColor::BLACK>()) {
                    board.unmakeMove(move);
                    continue;
                }
            } else {
                if (board.isKingInCheck<PieceColor::WHITE>()) {
                    board.unmakeMove(move);
                    continue;
                }
            }

            __builtin_prefetch(TranspositionTable::getTT()->getEntry(board.getZobristHash()), 0, 3);

            int depthReduction = 0;
            bool isOpponentKingInCheck;

            if (board.getMovingColor() == PieceColor::WHITE) {
                isOpponentKingInCheck = board.isKingInCheck<PieceColor::WHITE>();
            } else {
                isOpponentKingInCheck = board.isKingInCheck<PieceColor::BLACK>();
            }

            // Late move reduction
            if (!depthExtended && !isPv) {
                if (depth >= 3 && moves.movesSearched() > 4 && move.captureScore != -1 &&
                    move.promotionPiece == PieceType::EMPTY && !isOwnKingInCheck && !isOpponentKingInCheck) {
                    // Scale the reduction value between 1 and (depth - 1), depending on how many moves have been searched.
                    // It should reach (depth - 1) when 60% of the moves have been searched.
                    int R = 1 + (int) ((depth - 1) * (1 - moves.movesSearched() / (0.6 * moves.size())));
                    depthReduction += R;
                }
            }

            int score;
            score = search(board, depth - 1 - depthReduction, -alpha - 1, -alpha, rootMove,
                           previousMove, endTime, line, engine, false, canNull);
            score *= -1;

            if (score > alpha && score < beta) {
                score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                               engine, true, false);
                score *= -1;
            }

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(&move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(&move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta,
                                                             NodeType::FAIL_HIGH_NODE, encodeMove(&bestMove));
                    moveListPool->releaseMoveList(moveList);
                    return score;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
                nodeType = NodeType::PV_NODE;
            }
        }

        TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, nodeType, encodeMove(&bestMove));
        moveListPool->releaseMoveList(moveList);
        return alpha;
    }

    int SearchManager::quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                               Move &previousMove,
                               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine) {
        searchStats.qnodes += 1;

        if (searchStats.qnodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return beta;
        }

        int standPat = evaluate(board, endTime, engine);

        if (standPat >= beta) {
            return beta;
        }

        int delta = 1000;

        if (previousMove.promotionPiece != PieceType::EMPTY) {
            delta += getPieceWeight(previousMove.promotionPiece) - 100;
        }

        if (standPat < alpha - delta) {
            return alpha;
        }

        if (alpha < standPat) {
            alpha = standPat;
        }

        MoveList* moveList = moveListPool->getMoveList();

        if (board.getMovingColor() == PieceColor::WHITE) {
            generateQuiescenceMoves<PieceColor::WHITE>(board, moveList);
        } else {
            generateQuiescenceMoves<PieceColor::BLACK>(board, moveList);
        }

        MovePicker moves = MovePicker(moveList);
        while (moves.hasNext()) {
            Move move = moves.getNextMove();
            assert(move.from != move.to);

            if (searchStats.qnodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
                moveListPool->releaseMoveList(moveList);
                return beta;
            }

            if (move.captureScore <= NO_CAPTURE_SCORE) {
                continue;
            }

            board.makeMove(move);

            if (board.getMovingColor() == PieceColor::WHITE) {
                if (board.isKingInCheck<PieceColor::BLACK>()) {
                    board.unmakeMove(move);
                    continue;
                }
            } else {
                if (board.isKingInCheck<PieceColor::WHITE>()) {
                    board.unmakeMove(move);
                    continue;
                }
            }

            __builtin_prefetch(TranspositionTable::getTT()->getEntry(board.getZobristHash()), 0, 3);

            int score = quiesce(board, -beta, -alpha, rootMove, move, endTime, engine);
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
        return alpha;
    }

    bool SearchManager::isCurrentlySearching() {
        return isSearching;
    }

    senjo::SearchStats SearchManager::getSearchStats() {
        return searchStats;
    }

    int SearchManager::evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine) {
        if ((searchStats.nodes + searchStats.qnodes) % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return 0;
        }

        int modifier = board.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board.isWinner<PieceColor::WHITE>()) {
            return (MATE_SCORE - board.getPly()) * modifier;
        } else if (board.isWinner<PieceColor::BLACK>()) {
            return (-MATE_SCORE + board.getPly()) * modifier;
        }

        if (board.isDraw()) {
            // Thanks to Stockfish for the "3-fold blindness avoidance" idea
            return 0 - 1 + ((int) searchStats.nodes & 0x2);
        }

        initEvalContext(board);

        getWhiteMaterialScore(board);
        getBlackMaterialScore(board);

        getWhitePositionalScore(board);
        getBlackPositionalScore(board);

        getWhiteMobilityScore(board);
        getBlackMobilityScore(board);

        getWhiteKingScore(board);
        getBlackKingScore(board);

        getWhiteConnectivityScore(board);
        getBlackConnectivityScore(board);

        getWhiteRookScore(board);
        getBlackRookScore(board);

        getWhiteBishopScore(board);
        getBlackBishopScore(board);

        getPawnScore<PieceColor::WHITE>(board);
        getPawnScore<PieceColor::BLACK>(board);

        getOutpostScore<PieceColor::WHITE>(board);
        getOutpostScore<PieceColor::BLACK>(board);

        int whiteScore = ((evalContext.whiteMidgameScore * (256 - evalContext.phase)) + (evalContext.whiteEndgameScore * evalContext.phase)) / 256;
        int blackScore = ((evalContext.blackMidgameScore * (256 - evalContext.phase)) + (evalContext.blackEndgameScore * evalContext.phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    void SearchManager::getBlackMaterialScore(Bitboard &board) {
        int blackPawnCount = board.getMaterialCount<BLACK_PAWN>();
        int blackKnightCount = board.getMaterialCount<BLACK_KNIGHT>();
        int blackBishopCount = board.getMaterialCount<BLACK_BISHOP>();
        int blackRookCount = board.getMaterialCount<BLACK_ROOK>();
        int blackQueenCount = board.getMaterialCount<BLACK_QUEEN>();

        evalContext.blackMidgameScore += blackPawnCount * getEvalValue(MIDGAME_PAWN_MATERIAL);
        evalContext.blackEndgameScore += blackPawnCount * getEvalValue(ENDGAME_PAWN_MATERIAL);

        evalContext.blackMidgameScore += blackKnightCount * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
        evalContext.blackEndgameScore += blackKnightCount * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

        evalContext.blackMidgameScore += blackBishopCount * getEvalValue(MIDGAME_BISHOP_MATERIAL);
        evalContext.blackEndgameScore += blackBishopCount * getEvalValue(ENDGAME_BISHOP_MATERIAL);

        evalContext.blackMidgameScore += blackRookCount * getEvalValue(MIDGAME_ROOK_MATERIAL);
        evalContext.blackEndgameScore += blackRookCount * getEvalValue(ENDGAME_ROOK_MATERIAL);

        evalContext.blackMidgameScore += blackQueenCount * getEvalValue(MIDGAME_QUEEN_MATERIAL);
        evalContext.blackEndgameScore += blackQueenCount * getEvalValue(ENDGAME_QUEEN_MATERIAL);
    }

    void SearchManager::getWhiteMaterialScore(Bitboard &board) {
        int whitePawnCount = board.getMaterialCount<WHITE_PAWN>();
        int whiteKnightCount = board.getMaterialCount<WHITE_KNIGHT>();
        int whiteBishopCount = board.getMaterialCount<WHITE_BISHOP>();
        int whiteRookCount = board.getMaterialCount<WHITE_ROOK>();
        int whiteQueenCount = board.getMaterialCount<WHITE_QUEEN>();

        evalContext.whiteMidgameScore += whitePawnCount * getEvalValue(MIDGAME_PAWN_MATERIAL);
        evalContext.whiteEndgameScore += whitePawnCount * getEvalValue(ENDGAME_PAWN_MATERIAL);

        evalContext.whiteMidgameScore += whiteKnightCount * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
        evalContext.whiteEndgameScore += whiteKnightCount * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

        evalContext.whiteMidgameScore += whiteBishopCount * getEvalValue(MIDGAME_BISHOP_MATERIAL);
        evalContext.whiteEndgameScore += whiteBishopCount * getEvalValue(ENDGAME_BISHOP_MATERIAL);

        evalContext.whiteMidgameScore += whiteRookCount * getEvalValue(MIDGAME_ROOK_MATERIAL);
        evalContext.whiteEndgameScore += whiteRookCount * getEvalValue(ENDGAME_ROOK_MATERIAL);

        evalContext.whiteMidgameScore += whiteQueenCount * getEvalValue(MIDGAME_QUEEN_MATERIAL);
        evalContext.whiteEndgameScore += whiteQueenCount * getEvalValue(ENDGAME_QUEEN_MATERIAL);
    }

    void SearchManager::getWhitePositionalScore(Bitboard &bitboard) {
        evalContext.whiteMidgameScore += bitboard.getWhiteMidgamePst();
        evalContext.whiteEndgameScore += bitboard.getWhiteEndgamePst();
    }

    void SearchManager::getBlackPositionalScore(Bitboard &bitboard) {
        evalContext.blackMidgameScore += bitboard.getBlackMidgamePst();
        evalContext.blackEndgameScore += bitboard.getBlackEndgamePst();
    }

    void SearchManager::getWhiteMobilityScore(Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getColorBoard<PieceColor::WHITE>();
        uint64_t ownPiecesBBLoop = ownPiecesBB & ~evalContext.blackPawnAttacks;

        // Slight bonus for squares defended by own pawn
        evalContext.whiteMidgameScore += popcnt(evalContext.whiteCombinedAttacks & evalContext.whitePawnAttacks) * getEvalValue(MIDGAME_SQUARE_DEFENDED_BY_PAWN);
        evalContext.whiteEndgameScore += popcnt(evalContext.whiteCombinedAttacks & evalContext.whitePawnAttacks) * getEvalValue(ENDGAME_SQUARE_DEFENDED_BY_PAWN);

        while (ownPiecesBBLoop) {
            int index = bitscanForward(ownPiecesBBLoop);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = evalContext.attacksFrom[index];
            int mobility = popcnt(attacks & ~ownPiecesBB);

            switch (pieceOnSquare) {
                case WHITE_KNIGHT:
                    evalContext.whiteMidgameScore += mobility * getEvalValue(MIDGAME_KNIGHT_MOBILITY);
                    evalContext.whiteEndgameScore += mobility * getEvalValue(ENDGAME_KNIGHT_MOBILITY);
                    break;
                case WHITE_BISHOP:
                    evalContext.whiteMidgameScore += mobility * getEvalValue(MIDGAME_BISHOP_MOBILITY);
                    evalContext.whiteEndgameScore += mobility * getEvalValue(ENDGAME_BISHOP_MOBILITY);
                    break;
                case WHITE_ROOK:
                    evalContext.whiteMidgameScore += mobility * getEvalValue(MIDGAME_ROOK_MOBILITY);
                    evalContext.whiteEndgameScore += mobility * getEvalValue(ENDGAME_ROOK_MOBILITY);
                    break;
                case WHITE_QUEEN:
                    evalContext.whiteMidgameScore += mobility * getEvalValue(MIDGAME_QUEEN_MOBILITY);
                    evalContext.whiteEndgameScore += mobility * getEvalValue(ENDGAME_QUEEN_MOBILITY);
                    break;
            }

            ownPiecesBBLoop &= ~(1ULL << index);
        }
    }

    void SearchManager::getBlackMobilityScore(Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getColorBoard<PieceColor::BLACK>();
        uint64_t ownPiecesBBLoop = ownPiecesBB & ~evalContext.whitePawnAttacks;

        // Slight bonus for squares defended by own pawn
        evalContext.blackMidgameScore += popcnt(evalContext.blackCombinedAttacks & evalContext.blackPawnAttacks) * getEvalValue(MIDGAME_SQUARE_DEFENDED_BY_PAWN);
        evalContext.blackEndgameScore += popcnt(evalContext.blackCombinedAttacks & evalContext.blackPawnAttacks) * getEvalValue(ENDGAME_SQUARE_DEFENDED_BY_PAWN);

        while (ownPiecesBBLoop) {
            int index = bitscanForward(ownPiecesBBLoop);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = evalContext.attacksFrom[index];
            uint64_t mobility = popcnt(attacks & ~ownPiecesBB);

            switch (pieceOnSquare) {
                case BLACK_KNIGHT:
                    evalContext.blackMidgameScore += mobility * getEvalValue(MIDGAME_KNIGHT_MOBILITY);
                    evalContext.blackEndgameScore += mobility * getEvalValue(ENDGAME_KNIGHT_MOBILITY);
                    break;
                case BLACK_BISHOP:
                    evalContext.blackMidgameScore += mobility * getEvalValue(MIDGAME_BISHOP_MOBILITY);
                    evalContext.blackEndgameScore += mobility * getEvalValue(ENDGAME_BISHOP_MOBILITY);
                    break;
                case BLACK_ROOK:
                    evalContext.blackMidgameScore += mobility * getEvalValue(MIDGAME_ROOK_MOBILITY);
                    evalContext.blackEndgameScore += mobility * getEvalValue(ENDGAME_ROOK_MOBILITY);
                    break;
                case BLACK_QUEEN:
                    evalContext.blackMidgameScore += mobility * getEvalValue(MIDGAME_QUEEN_MOBILITY);
                    evalContext.blackEndgameScore += mobility * getEvalValue(ENDGAME_QUEEN_MOBILITY);
                    break;
            }

            ownPiecesBBLoop &= ~(1ULL << index);
        }
    }

    uint64_t whiteQueenCastlingAttackPattern = 0x70;
    uint64_t whiteKingCastlingAttackPattern = 0x6;
    void SearchManager::getWhiteKingScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::WHITE_KING>();
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = evalContext.attacksFrom[kingLocation];
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        int pawnShieldCount = popcnt(pawnShield);

        evalContext.whiteMidgameScore += std::min(getEvalValue(MIDGAME_PAWN_SHIELD) * 3, (int) (pawnShieldCount * getEvalValue(MIDGAME_PAWN_SHIELD)));
        evalContext.whiteEndgameScore += std::min(getEvalValue(ENDGAME_PAWN_SHIELD) * 3, (int) (pawnShieldCount * getEvalValue(ENDGAME_PAWN_SHIELD)));

        if (bitboard.isSemiOpenFileLenient<PieceColor::WHITE>(kingLocation)) {
            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KING_ON_OPEN_FILE);
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KING_ON_OPEN_FILE);
        }

        if (kingLocation % 8 != 0) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::WHITE>(kingLocation - 1)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        if (kingLocation % 8 != 7) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::WHITE>(kingLocation + 1)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        uint8_t castlingRights = bitboard.getCastlingRights();

        if (!(castlingRights & CastlingRights::WHITE_QUEENSIDE) && !(castlingRights & CastlingRights::WHITE_KINGSIDE)) {
            if (!(bitboard.getCastlingRights() & CastlingRights::HAS_WHITE_CASTLED)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_NO_CASTLING_RIGHTS);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_NO_CASTLING_RIGHTS);
            }
        } else {
            if ((castlingRights & CastlingRights::WHITE_QUEENSIDE) && (evalContext.blackCombinedAttacks & whiteQueenCastlingAttackPattern)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_QUEENSIDE_CASTLING_PREVENTED);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_QUEENSIDE_CASTLING_PREVENTED);
            }

            if ((castlingRights & CastlingRights::WHITE_KINGSIDE) && (evalContext.blackCombinedAttacks & whiteKingCastlingAttackPattern)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KINGSIDE_CASTLING_PREVENTED);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KINGSIDE_CASTLING_PREVENTED);
            }
        }

        uint64_t blackColorBoard = bitboard.getColorBoard<PieceColor::BLACK>();

        while (kingAttacks) {
            int8_t attackIndex = bitscanForward(kingAttacks);
            uint64_t attacksTo = evalContext.attacksTo[attackIndex] & blackColorBoard;

            while (attacksTo) {
                int8_t index = bitscanForward(attacksTo);
                PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

                switch (pieceOnSquare) {
                    case PieceType::WHITE_BISHOP:
                        evalContext.whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_ATTACK_NEAR_KING);
                        evalContext.whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_ATTACK_NEAR_KING);
                        break;
                    case PieceType::WHITE_ROOK:
                        evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ATTACK_NEAR_KING);
                        evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ATTACK_NEAR_KING);
                        break;
                    case PieceType::WHITE_QUEEN:
                        evalContext.whiteMidgameScore += getEvalValue(MIDGAME_QUEEN_ATTACK_NEAR_KING);
                        evalContext.whiteEndgameScore += getEvalValue(ENDGAME_QUEEN_ATTACK_NEAR_KING);
                        break;
                }

                attacksTo &= ~(1ULL << index);
            }

            kingAttacks &= ~(1ULL << attackIndex);
        }
    }

    uint64_t blackQueenCastlingAttackPattern = 0x7000000000000000;
    uint64_t blackKingCastlingAttackPattern = 0x600000000000000;
    void SearchManager::getBlackKingScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::BLACK_KING>();
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = evalContext.attacksFrom[kingLocation];
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= soutOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        uint64_t pawnShieldCount = popcnt(pawnShield);

        evalContext.blackMidgameScore += std::min(getEvalValue(MIDGAME_PAWN_SHIELD) * 3, (int) (pawnShieldCount * getEvalValue(MIDGAME_PAWN_SHIELD)));
        evalContext.blackEndgameScore += std::min(getEvalValue(ENDGAME_PAWN_SHIELD) * 3, (int) (pawnShieldCount * getEvalValue(ENDGAME_PAWN_SHIELD)));
        if (bitboard.isSemiOpenFileLenient<PieceColor::BLACK>(kingLocation)) {
            evalContext.blackMidgameScore += getEvalValue(MIDGAME_KING_ON_OPEN_FILE);
            evalContext.blackEndgameScore += getEvalValue(ENDGAME_KING_ON_OPEN_FILE);
        }

        if (kingLocation % 8 != 0) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::BLACK>(kingLocation - 1)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        if (kingLocation % 8 != 7) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::BLACK>(kingLocation + 1)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        uint8_t castlingRights = bitboard.getCastlingRights();
        if (!(castlingRights & CastlingRights::BLACK_QUEENSIDE) && !(castlingRights & CastlingRights::BLACK_KINGSIDE)) {
            if (!(bitboard.getCastlingRights() & CastlingRights::HAS_BLACK_CASTLED)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_NO_CASTLING_RIGHTS);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_NO_CASTLING_RIGHTS);
            }
        } else {
            if ((castlingRights & CastlingRights::BLACK_QUEENSIDE) && (evalContext.whiteCombinedAttacks & blackQueenCastlingAttackPattern)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_QUEENSIDE_CASTLING_PREVENTED);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_QUEENSIDE_CASTLING_PREVENTED);
            }

            if ((castlingRights & CastlingRights::BLACK_KINGSIDE) && (evalContext.whiteCombinedAttacks & blackKingCastlingAttackPattern)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_KINGSIDE_CASTLING_PREVENTED);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_KINGSIDE_CASTLING_PREVENTED);
            }
        }

        while (kingAttacks) {
            int8_t attackIndex = bitscanForward(kingAttacks);
            uint64_t attacksTo = evalContext.attacksTo[attackIndex] & bitboard.getColorBoard<PieceColor::WHITE>();

            while (attacksTo) {
                int8_t index = bitscanForward(attacksTo);

                PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

                switch (pieceOnSquare) {
                    case PieceType::BLACK_BISHOP:
                        evalContext.blackMidgameScore += getEvalValue(MIDGAME_BISHOP_ATTACK_NEAR_KING);
                        evalContext.blackEndgameScore += getEvalValue(ENDGAME_BISHOP_ATTACK_NEAR_KING);
                        break;
                    case PieceType::BLACK_ROOK:
                        evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ATTACK_NEAR_KING);
                        evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ATTACK_NEAR_KING);
                        break;
                    case PieceType::BLACK_QUEEN:
                        evalContext.blackMidgameScore += getEvalValue(MIDGAME_QUEEN_ATTACK_NEAR_KING);
                        evalContext.blackEndgameScore += getEvalValue(ENDGAME_QUEEN_ATTACK_NEAR_KING);
                        break;
                }

                attacksTo &= ~(1ULL << index);
            }

            kingAttacks &= ~(1ULL << attackIndex);
        }
    }

    void SearchManager::getWhiteConnectivityScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::WHITE_KING>();
        uint64_t whitePieces = bitboard.getColorBoard<PieceColor::WHITE>() & ~kingBB;
        uint64_t protectedPieces = whitePieces & evalContext.whiteCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);

            switch (pieceType) {
                case WHITE_PAWN:
                case BLACK_PAWN:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PAWN_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PAWN_CONNECTIVITY);
                    break;
                case WHITE_KNIGHT:
                case BLACK_KNIGHT:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KNIGHT_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KNIGHT_CONNECTIVITY);
                    break;
                case WHITE_BISHOP:
                case BLACK_BISHOP:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_CONNECTIVITY);
                    break;
                case WHITE_ROOK:
                case BLACK_ROOK:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_CONNECTIVITY);
                    break;
                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_QUEEN_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_QUEEN_CONNECTIVITY);
                    break;
            }

            protectedPieces &= ~(1ULL << index);
        }
    }

    void SearchManager::getBlackConnectivityScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::BLACK_KING>();
        uint64_t blackPieces = bitboard.getColorBoard<PieceColor::BLACK>() & ~kingBB;
        uint64_t protectedPieces = blackPieces & evalContext.blackCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);

            switch (pieceType) {
                case WHITE_PAWN:
                case BLACK_PAWN:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_PAWN_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_PAWN_CONNECTIVITY);
                    break;
                case WHITE_KNIGHT:
                case BLACK_KNIGHT:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_KNIGHT_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_KNIGHT_CONNECTIVITY);
                    break;
                case WHITE_BISHOP:
                case BLACK_BISHOP:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_BISHOP_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_BISHOP_CONNECTIVITY);
                    break;
                case WHITE_ROOK:
                case BLACK_ROOK:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_CONNECTIVITY);
                    break;
                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_QUEEN_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_QUEEN_CONNECTIVITY);
                    break;
            }

            protectedPieces &= ~(1ULL << index);
        }
    }

    void SearchManager::getWhiteRookScore(Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard<PieceType::WHITE_ROOK>();
        int rookAmount = popcnt(rookBB);

        while (rookBB) {
            int8_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_OPEN_FILE);
            } else if (bitboard.isSemiOpenFile<PieceColor::WHITE>(index)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_SEMI_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_SEMI_OPEN_FILE);
            }

            if (index >= Square::A7)  {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_7TH_RANK);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_7TH_RANK);
            }

            uint64_t enemyQueenOnFile = bitboard.getPieceBoard<PieceType::BLACK_QUEEN>() & bitboard.getFile(index);

            if (enemyQueenOnFile) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_QUEEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_QUEEN_FILE);
            }

            rookBB &= ~(1ULL << index);
        }

        uint64_t pawnCount = popcnt(bitboard.getPieceBoard<PieceType::WHITE_PAWN>());
        evalContext.whiteMidgameScore += ((8 - pawnCount) * getEvalValue(MIDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
        evalContext.whiteEndgameScore += ((8 - pawnCount) * getEvalValue(ENDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
    }

    void SearchManager::getBlackRookScore(Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard<PieceType::BLACK_ROOK>();
        int rookAmount = popcnt(rookBB);

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_OPEN_FILE);
            } else if (bitboard.isSemiOpenFile<PieceColor::BLACK>(index)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_SEMI_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_SEMI_OPEN_FILE);
            }

            if (index <= Square::H2)  {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_7TH_RANK);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_7TH_RANK);
            }

            uint64_t enemyQueenOnFile = bitboard.getPieceBoard<PieceType::WHITE_QUEEN>() & bitboard.getFile(index);

            if (enemyQueenOnFile) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_QUEEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_QUEEN_FILE);
            }

            rookBB &= ~(1ULL << index);
        }

        uint64_t pawnCount = popcnt(bitboard.getPieceBoard<PieceType::BLACK_PAWN>());
        evalContext.blackMidgameScore += ((8 - pawnCount) * getEvalValue(MIDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
        evalContext.blackEndgameScore += ((8 - pawnCount) * getEvalValue(ENDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
    }

    void SearchManager::getWhiteBishopScore(Bitboard &bitboard) {
        uint64_t bishopBB = bitboard.getPieceBoard<PieceType::WHITE_BISHOP>();
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();

        if (bishopAmount == 1) {
            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_NO_BISHOP_PAIR);
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_NO_BISHOP_PAIR);
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G2 || index == Square::B2) {
                uint64_t fianchettoPattern = nortOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = noWeOne(1ULL << index) | noEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_FIANCHETTO);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_FIANCHETTO);
                }
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void SearchManager::getBlackBishopScore(Bitboard &bitboard) {
        uint64_t bishopBB = bitboard.getPieceBoard<PieceType::BLACK_BISHOP>();
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();

        if (bishopAmount == 1) {
            evalContext.blackMidgameScore += getEvalValue(MIDGAME_NO_BISHOP_PAIR);
            evalContext.blackEndgameScore += getEvalValue(ENDGAME_NO_BISHOP_PAIR);
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G7 || index == Square::B7) {
                uint64_t fianchettoPattern = soutOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = soWeOne(1ULL << index) | soEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_BISHOP_FIANCHETTO);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_BISHOP_FIANCHETTO);
                }
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    template<PieceColor color>
    void SearchManager::getPawnScore(Bitboard &bitboard) {
        for (int i = 0; i < 8; i++) {
            uint64_t pawnsOnFile = bitboard.getPawnsOnSameFile<color>(i);
            int amountOfPawns = popcnt(pawnsOnFile);

            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PAWN_ON_SAME_FILE) * (amountOfPawns - 1);
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PAWN_ON_SAME_FILE) * (amountOfPawns - 1);

            if (bitboard.isPassedPawn<color>(i)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN);
            } else if (bitboard.isIsolatedPawn<color>(i)) {
                if (bitboard.isSemiOpenFile<color>(i)) {
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_SEMI_OPEN_PAWN);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_SEMI_OPEN_PAWN);
                } else {
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_PAWN);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_PAWN);
                }
            } else if (bitboard.isSemiOpenFile<color>(i)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PAWN_SEMI_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PAWN_SEMI_OPEN_FILE);
            }
        }
    }

    template<PieceColor color>
    void SearchManager::getOutpostScore(Bitboard &bitboard) {
        uint64_t rankMask = RANK_4 | RANK_5 | RANK_6;
        uint64_t ownPawnAttacks;
        uint64_t opponentPawnAttacks;
        uint64_t opponentAttackSpans;

        if (color == PieceColor::WHITE) {
            ownPawnAttacks = evalContext.whitePawnAttacks;
            opponentPawnAttacks = evalContext.blackPawnAttacks;
            uint64_t opponentPawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();
            opponentAttackSpans = westOne(blackFrontSpans(opponentPawnBB)) | eastOne(blackFrontSpans(opponentPawnBB));
        } else {
            ownPawnAttacks = evalContext.blackPawnAttacks;
            opponentPawnAttacks = evalContext.whitePawnAttacks;
            uint64_t opponentPawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();
            opponentAttackSpans = westOne(whiteFrontSpans(opponentPawnBB)) | eastOne(whiteFrontSpans(opponentPawnBB));
        }

        uint64_t outpostSquares = (ownPawnAttacks & rankMask) & ~opponentPawnAttacks & ~opponentAttackSpans;
        uint64_t knightBB;
        uint64_t bishopBB;

        if (color == PieceColor::WHITE) {
            knightBB = bitboard.getPieceBoard<PieceType::WHITE_KNIGHT>();
            bishopBB = bitboard.getPieceBoard<PieceType::WHITE_BISHOP>();
        } else {
            knightBB = bitboard.getPieceBoard<PieceType::BLACK_KNIGHT>();
            bishopBB = bitboard.getPieceBoard<PieceType::BLACK_BISHOP>();
        }

        int knightsOnOutpost = popcnt(knightBB & outpostSquares);
        int bishopsOnOutpost = popcnt(bishopBB & outpostSquares);

        if (color == PieceColor::WHITE) {
            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KNIGHT_OUTPOST) * knightsOnOutpost;
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KNIGHT_OUTPOST) * knightsOnOutpost;
            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_OUTPOST) * bishopsOnOutpost;
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_OUTPOST) * bishopsOnOutpost;
        } else {
            evalContext.blackMidgameScore += getEvalValue(MIDGAME_KNIGHT_OUTPOST) * knightsOnOutpost;
            evalContext.blackEndgameScore += getEvalValue(ENDGAME_KNIGHT_OUTPOST) * knightsOnOutpost;
            evalContext.blackMidgameScore += getEvalValue(MIDGAME_BISHOP_OUTPOST) * bishopsOnOutpost;
            evalContext.blackEndgameScore += getEvalValue(ENDGAME_BISHOP_OUTPOST) * bishopsOnOutpost;
        }
    }

    int knightPhase = 1;
    int bishopPhase = 1;
    int rookPhase = 2;
    int queenPhase = 4;
    int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;
    int getGamePhase(Bitboard &bitboard) {
        int phase = totalPhase;

        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_KNIGHT>() | bitboard.getPieceBoard<PieceType::BLACK_KNIGHT>()) * knightPhase;
        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_BISHOP>() | bitboard.getPieceBoard<PieceType::BLACK_BISHOP>()) * bishopPhase;
        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_ROOK>() | bitboard.getPieceBoard<PieceType::BLACK_ROOK>()) * rookPhase;
        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_QUEEN>() | bitboard.getPieceBoard<PieceType::BLACK_QUEEN>()) * queenPhase;

        return (phase * 256 + (totalPhase / 2)) / totalPhase;
    }

    void SearchManager::initEvalContext(Bitboard &bitboard) {
        uint64_t whitePawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();
        uint64_t whiteKnightBB = bitboard.getPieceBoard<PieceType::WHITE_KNIGHT>();
        uint64_t whiteBishopBB = bitboard.getPieceBoard<PieceType::WHITE_BISHOP>();
        uint64_t whiteRookBB = bitboard.getPieceBoard<PieceType::WHITE_ROOK>();
        uint64_t whiteQueenBB = bitboard.getPieceBoard<PieceType::WHITE_QUEEN>();

        uint64_t blackPawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();
        uint64_t blackKnightBB = bitboard.getPieceBoard<PieceType::BLACK_KNIGHT>();
        uint64_t blackBishopBB = bitboard.getPieceBoard<PieceType::BLACK_BISHOP>();
        uint64_t blackRookBB = bitboard.getPieceBoard<PieceType::BLACK_ROOK>();
        uint64_t blackQueenBB = bitboard.getPieceBoard<PieceType::BLACK_QUEEN>();

        uint64_t whitePawnAttacks = calculatePawnAttacks<PieceColor::WHITE>(whitePawnBB);
        uint64_t whiteKnightAttacks = 0;
        uint64_t whiteBishopAttacks = 0;
        uint64_t whiteRookAttacks = 0;
        uint64_t whiteQueenAttacks = 0;

        uint64_t blackPawnAttacks = calculatePawnAttacks<PieceColor::BLACK>(blackPawnBB);
        uint64_t blackKnightAttacks = 0;
        uint64_t blackBishopAttacks = 0;
        uint64_t blackRookAttacks = 0;
        uint64_t blackQueenAttacks = 0;

        for (int i = 0; i < 64; i++) {
            evalContext.attacksFrom[i] = 0;
            evalContext.attacksTo[i] = 0;
        }

        while (whiteKnightBB) {
            uint64_t index = bitscanForward(whiteKnightBB);

            uint64_t attacks = bitboard.getKnightAttacks(index);
            whiteKnightAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            whiteKnightBB &= ~(1ULL << index);
        }

        while (blackKnightBB) {
            uint64_t index = bitscanForward(blackKnightBB);

            uint64_t attacks = bitboard.getKnightAttacks(index);
            blackKnightAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            blackKnightBB &= ~(1ULL << index);
        }

        while (whiteBishopBB) {
            uint64_t index = bitscanForward(whiteBishopBB);

            uint64_t attacks = bitboard.getBishopAttacks(index);
            whiteBishopAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            whiteBishopBB &= ~(1ULL << index);
        }

        while (whiteRookBB) {
            uint64_t index = bitscanForward(whiteRookBB);

            uint64_t attacks = bitboard.getRookAttacks(index);
            whiteRookAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            whiteRookBB &= ~(1ULL << index);
        }

        while (whiteQueenBB) {
            uint64_t index = bitscanForward(whiteQueenBB);

            uint64_t attacks = bitboard.getQueenAttacks(index);
            whiteQueenAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            whiteQueenBB &= ~(1ULL << index);
        }

        while (blackBishopBB) {
            uint64_t index = bitscanForward(blackBishopBB);

            uint64_t attacks = bitboard.getBishopAttacks(index);
            blackBishopAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            blackBishopBB &= ~(1ULL << index);
        }

        while (blackRookBB) {
            uint64_t index = bitscanForward(blackRookBB);

            uint64_t attacks = bitboard.getRookAttacks(index);
            blackRookAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            blackRookBB &= ~(1ULL << index);
        }

        while (blackQueenBB) {
            uint64_t index = bitscanForward(blackQueenBB);

            uint64_t attacks = bitboard.getQueenAttacks(index);
            blackQueenAttacks |= attacks;
            evalContext.attacksFrom[index] = attacks;

            while (attacks) {
                uint64_t attackIndex = bitscanForward(attacks);
                evalContext.attacksTo[attackIndex] |= (1ULL << index);
                attacks &= ~(1ULL << attackIndex);
            }

            blackQueenBB &= ~(1ULL << index);
        }

        uint64_t whiteCombinedAttacks = whiteKnightAttacks | whiteBishopAttacks | whiteRookAttacks | whiteQueenAttacks | whitePawnAttacks;
        uint64_t blackCombinedAttacks = blackKnightAttacks | blackBishopAttacks | blackRookAttacks | blackQueenAttacks | blackPawnAttacks;

        evalContext.phase = getGamePhase(bitboard);
        evalContext.whiteMidgameScore = 0;
        evalContext.whiteEndgameScore = 0;
        evalContext.blackMidgameScore = 0;
        evalContext.blackEndgameScore = 0;
        evalContext.whitePawnAttacks = whitePawnAttacks;
        evalContext.whiteCombinedAttacks = whiteCombinedAttacks;
        evalContext.blackPawnAttacks = blackPawnAttacks;
        evalContext.blackCombinedAttacks = blackCombinedAttacks;
    }
}

#pragma clang diagnostic pop