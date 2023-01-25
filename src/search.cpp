/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <chrono>
#include <immintrin.h>

#include "search.h"
#include "timemanager.h"
#include "movegen.h"
#include "../senjo/Output.h"
#include "movepicker.h"
#include "pst.h"
#include "tt.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    Move SearchManager::getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board) {
        searchStats = {};
        isSearching = true;
        int bestScore = -1000000;
        Move bestMove = {};
        int iterationScore = -1000000;
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
            MoveList moveList;

            if (board.getMovingColor() == PieceColor::WHITE) {
                moveList = generateMoves<PieceColor::WHITE>(board);
            } else {
                moveList = generateMoves<PieceColor::BLACK>(board);
            }

            MovePicker moves = MovePicker(moveList);

            if (moves.size() == 1) {
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

                Line pvLine = {};
                Move previousMove = {};

                int score = search(board, depth, -9999999, 9999999, move, previousMove, endTime, pvLine, engine, true);
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
            }

            iterationScore = -1000000;
            iterationMove = {};
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

    int SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                                       Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine, ZagreusEngine &engine, bool isPv) {
        searchStats.nodes += 1;

        if (searchStats.nodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return beta;
        }

        if (depth == 0 || board.isWinner<PieceColor::WHITE>() || board.isWinner<PieceColor::BLACK>() || board.isDraw()) {
            pvLine.moveCount = 0;
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        Line line{};
        MoveList moveList;
        NodeType nodeType = NodeType::FAIL_LOW_NODE;

        if (board.getMovingColor() == PieceColor::WHITE) {
            moveList = generateMoves<PieceColor::WHITE>(board);
        } else {
            moveList = generateMoves<PieceColor::BLACK>(board);
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

            int score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line, engine, true);
            score *= -1;

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta, NodeType::FAIL_HIGH_NODE);
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
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha, beta);

        if (!isPv && ttScore != INT32_MIN) {
            return ttScore;
        }

        while (moves.hasNext()) {
            if (searchStats.nodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
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

            int score;
            score = search(board, depth - 1, -alpha - 1, -alpha, rootMove, previousMove, endTime, line, engine, false);
            score *= -1;

            if (score > alpha && score < beta) {
                score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line, engine, false);
                score *= -1;
            }

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta, NodeType::FAIL_HIGH_NODE);
                    return beta;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
                nodeType = NodeType::PV_NODE;
            }
        }

        TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, nodeType);
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

        MoveList moveList;

        if (board.getMovingColor() == PieceColor::WHITE) {
            moveList = generateQuiescenceMoves<PieceColor::WHITE>(board);
        } else {
            moveList = generateQuiescenceMoves<PieceColor::BLACK>(board);
        }

        MovePicker moves = MovePicker(moveList);
        while (moves.hasNext()) {
            Move move = moves.getNextMove();
            assert(move.from != move.to);

            if (searchStats.qnodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
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

            int score = quiesce(board, -beta, -alpha, rootMove, move, endTime, engine);
            score *= -1;
            board.unmakeMove(move);

            if (score >= beta) {
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }

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
            return (15000 - board.getPly()) * modifier;
        } else if (board.isWinner<PieceColor::BLACK>()) {
            return (-15000 + board.getPly()) * modifier;
        }

        if (board.isDraw()) {
            // Thanks to Stockfish for the "3-fold blindness avoidance" idea
            return 0 - 1 + ((int) searchStats.nodes & 0x2);
        }

        EvalContext evalContext = createEvalContext(board);

        getWhiteMaterialScore(evalContext, board);
        getBlackMaterialScore(evalContext, board);

        getWhitePositionalScore(evalContext, board);
        getBlackPositionalScore(evalContext, board);

        getWhiteMobilityScore(evalContext, board);
        getBlackMobilityScore(evalContext, board);

        getWhiteKingScore(evalContext, board);
        getBlackKingScore(evalContext, board);

        getWhiteConnectivityScore(evalContext, board);
        getBlackConnectivityScore(evalContext, board);

        int whiteScore = ((evalContext.whiteMidgameScore * (256 - evalContext.phase)) + (evalContext.whiteEndgameScore * evalContext.phase)) / 256;
        int blackScore = ((evalContext.blackMidgameScore * (256 - evalContext.phase)) + (evalContext.blackEndgameScore * evalContext.phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    void getBlackMaterialScore(EvalContext &evalContext, Bitboard &board) {
        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_PAWN>()) * getPieceWeight(BLACK_PAWN);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_PAWN>()) * getPieceWeight(BLACK_PAWN);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_KNIGHT>()) * getPieceWeight(BLACK_KNIGHT);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_KNIGHT>()) * getPieceWeight(BLACK_KNIGHT);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_BISHOP>()) * getPieceWeight(BLACK_BISHOP);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_BISHOP>()) * getPieceWeight(BLACK_BISHOP);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_ROOK>()) * getPieceWeight(BLACK_ROOK);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_ROOK>()) * getPieceWeight(BLACK_ROOK);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_QUEEN>()) * getPieceWeight(BLACK_QUEEN);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_QUEEN>()) * getPieceWeight(BLACK_QUEEN);
    }

    void getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board) {
        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_PAWN>()) * getPieceWeight(WHITE_PAWN);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_PAWN>()) * getPieceWeight(WHITE_PAWN);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_KNIGHT>()) * getPieceWeight(WHITE_KNIGHT);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_KNIGHT>()) * getPieceWeight(WHITE_KNIGHT);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_BISHOP>()) * getPieceWeight(WHITE_BISHOP);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_BISHOP>()) * getPieceWeight(WHITE_BISHOP);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_ROOK>()) * getPieceWeight(WHITE_ROOK);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_ROOK>()) * getPieceWeight(WHITE_ROOK);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_QUEEN>()) * getPieceWeight(WHITE_QUEEN);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_QUEEN>()) * getPieceWeight(WHITE_QUEEN);
    }

    void getWhitePositionalScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t colorBB = bitboard.getColorBoard<PieceColor::WHITE>();

        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            evalContext.whiteMidgameScore += getMidgamePstValue(pieceOnSquare, index);
            evalContext.whiteEndgameScore += getEndgamePstValue(pieceOnSquare, index);

            colorBB = _blsr_u64(colorBB);
        }
    }

    void getBlackPositionalScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t colorBB = bitboard.getColorBoard<PieceColor::BLACK>();

        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            evalContext.blackMidgameScore += getMidgamePstValue(pieceOnSquare, index);
            evalContext.blackEndgameScore += getEndgamePstValue(pieceOnSquare, index);

            colorBB = _blsr_u64(colorBB);
        }
    }

    uint64_t centerPattern = 0x0000001818000000;
    uint64_t extendedCenterPattern = 0x00003C3C3C3C0000 & ~centerPattern;
    void getWhiteMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getColorBoard<PieceColor::WHITE>();
        uint64_t ownPiecesBBLoop = bitboard.getColorBoard<PieceColor::WHITE>()  & ~(evalContext.blackPawnAttacks);

        // Slight bonus for squares defended by own pawn
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & evalContext.whitePawnAttacks);
        evalContext.whiteEndgameScore += popcnt((evalContext.whiteCombinedAttacks) & evalContext.whitePawnAttacks);

        // Bonus for center control
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & centerPattern) * 10;
        evalContext.whiteEndgameScore += 0;
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & extendedCenterPattern) * 4;
        evalContext.whiteEndgameScore += 0;

        while (ownPiecesBBLoop) {
            int index = bitscanForward(ownPiecesBBLoop);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = evalContext.attacksFrom[index];

            switch (pieceOnSquare) {
                case WHITE_KNIGHT:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * 7;
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * 2;
                    break;
                case WHITE_BISHOP:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * 8;
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * 3;
                    break;
                case WHITE_ROOK:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * 2;
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * 6;
                    break;
                case WHITE_QUEEN:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * 4;
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * 8;
                    break;
            }

            ownPiecesBBLoop &= ~(1ULL << index);
        }
    }

    void getBlackMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getColorBoard<PieceColor::BLACK>();
        uint64_t ownPiecesBBLoop = bitboard.getColorBoard<PieceColor::BLACK>()  & ~(evalContext.whitePawnAttacks);

        // Slight bonus for squares defended by own pawn
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & evalContext.blackPawnAttacks);
        evalContext.blackEndgameScore += popcnt((evalContext.blackCombinedAttacks) & evalContext.blackPawnAttacks);

        // Bonus for center control
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & centerPattern) * 10;
        evalContext.blackEndgameScore += 0;
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & extendedCenterPattern) * 4;
        evalContext.blackEndgameScore += 0;

        while (ownPiecesBBLoop) {
            int index = bitscanForward(ownPiecesBBLoop);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = evalContext.attacksFrom[index];

            switch (pieceOnSquare) {
                case BLACK_KNIGHT:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * 7;
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * 2;
                    break;
                case BLACK_BISHOP:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * 8;
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * 3;
                    break;
                case BLACK_ROOK:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * 2;
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * 6;
                    break;
                case BLACK_QUEEN:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * 4;
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * 8;
                    break;
            }

            ownPiecesBBLoop &= ~(1ULL << index);
        }
    }

    uint64_t whiteQueenCastlingAttackPattern = 0x70;
    uint64_t whiteKingCastlingAttackPattern = 0x6;
    void getWhiteKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::WHITE_KING>();
        uint64_t kingLocation = bitscanForward(kingBB);
//        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.whiteMidgameScore += std::min(60, (int) (popcnt(pawnShield) * 20));
        evalContext.whiteEndgameScore += 0;

       /* if (bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::WHITE)) {
            evalContext.whiteMidgameScore -= 50;
            evalContext.whiteEndgameScore -= 0;
        }

        uint8_t castlingRights = bitboard.getCastlingRights();

        if (!(castlingRights & CastlingRights::WHITE_QUEENSIDE) && !(castlingRights & CastlingRights::WHITE_KINGSIDE)) {
            if (!bitboard.isHasWhiteCastled()) {
                evalContext.whiteMidgameScore -= 40;
                evalContext.whiteEndgameScore -= 0;
            }
        } else {
            if ((castlingRights & CastlingRights::WHITE_QUEENSIDE) && (evalContext.blackCombinedAttacks & whiteQueenCastlingAttackPattern)) {
                evalContext.whiteMidgameScore -= 15;
                evalContext.whiteEndgameScore -= 0;
            }

            if ((castlingRights & CastlingRights::WHITE_KINGSIDE) && (evalContext.blackCombinedAttacks & whiteKingCastlingAttackPattern)) {
                evalContext.whiteMidgameScore -= 20;
                evalContext.whiteEndgameScore -= 0;
            }
        }*/

/*        evalContext.whiteMidgameScore -= popcnt(evalContext.blackPawnAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_PAWN) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackPawnAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_PAWN) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackKnightAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_KNIGHT) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackKnightAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_KNIGHT) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackBishopAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_BISHOP) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackBishopAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_BISHOP) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackRookAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_ROOK) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackRookAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_ROOK) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackQueenAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_QUEEN) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackQueenAttacks & kingAttacks) * (getPieceWeight(PieceType::BLACK_QUEEN) / 100);*/
    }

    uint64_t blackQueenCastlingAttackPattern = 0x7000000000000000;
    uint64_t blackKingCastlingAttackPattern = 0x600000000000000;
    void getBlackKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::BLACK_KING>();
        uint64_t kingLocation = bitscanForward(kingBB);
//        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= soutOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.blackMidgameScore += std::min(60, (int) (popcnt(pawnShield) * 20));
        evalContext.blackEndgameScore += 0;

       /* if (bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::BLACK)) {
            evalContext.blackMidgameScore -= 50;
            evalContext.blackEndgameScore -= 0;
        }

        uint8_t castlingRights = bitboard.getCastlingRights();
        if (!(castlingRights & CastlingRights::BLACK_QUEENSIDE) && !(castlingRights & CastlingRights::BLACK_KINGSIDE)) {
            if (!bitboard.isHasBlackCastled()) {
                evalContext.blackMidgameScore -= 40;
                evalContext.blackEndgameScore -= 0;
            }
        } else {
            if ((castlingRights & CastlingRights::BLACK_QUEENSIDE) && (evalContext.whiteCombinedAttacks & blackQueenCastlingAttackPattern)) {
                evalContext.blackMidgameScore -= 15;
                evalContext.blackEndgameScore -= 0;
            }

            if ((castlingRights & CastlingRights::BLACK_KINGSIDE) && (evalContext.whiteCombinedAttacks & blackKingCastlingAttackPattern)) {
                evalContext.blackMidgameScore -= 20;
                evalContext.blackEndgameScore -= 0;
            }
        }*/

/*        evalContext.blackMidgameScore -= popcnt(evalContext.whitePawnAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_PAWN) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whitePawnAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_PAWN) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteKnightAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_KNIGHT) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteKnightAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_KNIGHT) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteBishopAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_BISHOP) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteBishopAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_BISHOP) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteRookAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_ROOK) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteRookAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_ROOK) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteQueenAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_QUEEN) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteQueenAttacks & kingAttacks) * (getPieceWeight(PieceType::WHITE_QUEEN) / 100);*/
    }

    void getWhiteConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::WHITE_KING>();
        uint64_t whitePieces = bitboard.getColorBoard<PieceColor::WHITE>() & ~kingBB;
        uint64_t protectedPieces = whitePieces & evalContext.whiteCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);
            int weight = getPieceWeight(pieceType);

            evalContext.whiteMidgameScore += 11 - (weight / 100);
            evalContext.whiteEndgameScore += 11 - (weight / 100);
            protectedPieces &= ~(1ULL << index);
        }
    }

    void getBlackConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::BLACK_KING>();
        uint64_t blackPieces = bitboard.getColorBoard<PieceColor::BLACK>() & ~kingBB;
        uint64_t protectedPieces = blackPieces & evalContext.blackCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);
            int weight = getPieceWeight(pieceType);

            evalContext.blackMidgameScore += 11 - (weight / 100);
            evalContext.blackEndgameScore += 11 - (weight / 100);
            protectedPieces &= ~(1ULL << index);
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

    EvalContext createEvalContext(Bitboard &bitboard) {
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
        uint64_t attacksFrom[64] = { 0ULL };

        while (whiteKnightBB) {
            uint64_t index = bitscanForward(whiteKnightBB);

            uint64_t attacks = bitboard.getKnightAttacks(index);
            whiteKnightAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteKnightBB &= ~(1ULL << index);
        }

        while (blackKnightBB) {
            uint64_t index = bitscanForward(blackKnightBB);

            uint64_t attacks = bitboard.getKnightAttacks(index);
            blackKnightAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackKnightBB &= ~(1ULL << index);
        }

        while (whiteBishopBB) {
            uint64_t index = bitscanForward(whiteBishopBB);

            uint64_t attacks = bitboard.getBishopAttacks(index);
            whiteBishopAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteBishopBB &= ~(1ULL << index);
        }

        while (whiteRookBB) {
            uint64_t index = bitscanForward(whiteRookBB);

            uint64_t attacks = bitboard.getRookAttacks(index);
            whiteRookAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteRookBB &= ~(1ULL << index);
        }

        while (whiteQueenBB) {
            uint64_t index = bitscanForward(whiteQueenBB);

            uint64_t attacks = bitboard.getQueenAttacks(index);
            whiteQueenAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteQueenBB &= ~(1ULL << index);
        }

        while (blackBishopBB) {
            uint64_t index = bitscanForward(blackBishopBB);

            uint64_t attacks = bitboard.getBishopAttacks(index);
            blackBishopAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackBishopBB &= ~(1ULL << index);
        }

        while (blackRookBB) {
            uint64_t index = bitscanForward(blackRookBB);

            uint64_t attacks = bitboard.getRookAttacks(index);
            blackRookAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackRookBB &= ~(1ULL << index);
        }

        while (blackQueenBB) {
            uint64_t index = bitscanForward(blackQueenBB);

            uint64_t attacks = bitboard.getQueenAttacks(index);
            blackQueenAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackQueenBB &= ~(1ULL << index);
        }

        uint64_t whiteCombinedAttacks = whiteKnightAttacks | whiteBishopAttacks | whiteRookAttacks | whiteQueenAttacks | whitePawnAttacks;
        uint64_t blackCombinedAttacks = blackKnightAttacks | blackBishopAttacks | blackRookAttacks | blackQueenAttacks | blackPawnAttacks;

        auto context =  EvalContext {
                getGamePhase(bitboard),
                0,
                0,
                0,
                0,
                whitePawnAttacks,
                whiteKnightAttacks,
                whiteBishopAttacks,
                whiteRookAttacks,
                whiteQueenAttacks,
                whiteCombinedAttacks,
                blackPawnAttacks,
                blackKnightAttacks,
                blackBishopAttacks,
                blackRookAttacks,
                blackQueenAttacks,
                blackCombinedAttacks,
                {}
        };

        for (int i = 0; i < 64; i++) {
            context.attacksFrom[i] = attacksFrom[i];
        }

        return context;
    }
}

#pragma clang diagnostic pop