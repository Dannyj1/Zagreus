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

#include "search_mgr.h"
#include "time_mgr.h"
#include "tt.h"
#include "move_gen.h"
#include "senjo/Output.h"
#include "pst.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    SearchResult SearchManager::getBestMove(ZagreusEngine &engine, Bitboard &board, PieceColor color) {
        searchStats = {};
        isSearching = true;
        SearchResult bestResult;
        SearchResult iterationResult;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime = timeManager.getEndTime(engine, board, color);
        int depth = 0;

        Line iterationPvLine = {};
        while (std::chrono::high_resolution_clock::now() - startTime < (endTime - startTime) * 0.7) {
            depth += 1;

            if (board.isBenchmarking() && depth >= 3) {
                return bestResult;
            }

            searchStats.depth = depth;
            searchStats.seldepth = 0;

            senjo::Output(senjo::Output::InfoPrefix) << "Searching depth " << depth << "...";
            board.setPreviousPvLine(iterationPvLine);
            MovePicker moves = generateLegalMoves(board, color);

            if (moves.size() == 1) {
                return {moves.getNextMove(), 0};
            }

            while (moves.hasNext()) {
                Move move = moves.getNextMove();
                assert(move.fromSquare != move.toSquare);
                assert(move.fromSquare >= 0 && move.fromSquare < 64);
                assert(move.toSquare >= 0 && move.toSquare < 64);

                if (depth > 1 && std::chrono::high_resolution_clock::now() > endTime) {
                    return bestResult;
                }

                board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

                if (board.isKingInCheck(color)) {
                    board.unmakeMove();
                    continue;
                }

                Line pvLine = {};
                Move previousMove = {board.getPreviousMoveFrom(), board.getPreviousMoveTo()};
                SearchResult result = search(board, depth, -9999999, 9999999, move, previousMove, endTime, pvLine);
                result.score *= -1;
                board.unmakeMove();

                if (result.score > iterationResult.score && std::chrono::high_resolution_clock::now() < endTime) {
                    assert(result.move.pieceType != PieceType::EMPTY);
                    iterationResult = result;

                    iterationPvLine.moves[0] = move;
                    memcpy(iterationPvLine.moves + 1, pvLine.moves, pvLine.moveCount * sizeof(Move));
                    iterationPvLine.moveCount = pvLine.moveCount + 1;

                    searchStats.score = iterationResult.score;
                }

                searchStats.pv = "";
                for (int i = 0; i < iterationPvLine.moveCount; i++) {
                    Move move = iterationPvLine.moves[i];

                    searchStats.pv += board.getNotation(move.fromSquare) + board.getNotation(move.toSquare);

                    if (i != iterationPvLine.moveCount - 1) {
                        searchStats.pv += " ";
                    }
                }

                searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - startTime).count();
                senjo::Output(senjo::Output::NoPrefix) << searchStats;
            }

            if (depth == 1 || std::chrono::high_resolution_clock::now() < endTime) {
                assert(iterationResult.move.pieceType != PieceType::EMPTY);
                bestResult = iterationResult;
                searchStats.score = bestResult.score;
            }

            iterationResult = {};
        }

        searchStats.pv = "";
        for (int i = 0; i < iterationPvLine.moveCount; i++) {
            Move move = iterationPvLine.moves[i];

            searchStats.pv += board.getNotation(move.fromSquare) + board.getNotation(move.toSquare);

            if (i != iterationPvLine.moveCount - 1) {
                searchStats.pv += " ";
            }
        }

        searchStats.score = bestResult.score;
        searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count();
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
        return bestResult;
    }

    SearchResult SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                                       Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() > endTime
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) || board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            pvLine.moveCount = 0;
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime);
        }

        searchStats.nodes += 1;

        int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha, beta);

        if (ttScore != INT32_MIN && !board.isBenchmarking()) {
            return {rootMove, ttScore};
        }

        Line line{};

        MovePicker moves = generateLegalMoves(board, board.getMovingColor());
        int moveCounter = 0;
        NodeType nodeType = NodeType::FAIL_LOW_NODE;
        bool searchedFirstLegalMove = false;

        while (!searchedFirstLegalMove && moves.hasNext()) {
            Move move = moves.getNextMove();

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()))) {
                board.unmakeMove();
                continue;
            }

            SearchResult result = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line);
            result.score *= -1;

            board.unmakeMove();

            if (result.score > alpha) {
                if (result.score >= beta) {
                    int moveCode = encodeMove(move);
                    TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][depth];
                    TranspositionTable::getTT()->killerMoves[0][board.getPly()] = moveCode;

                    if (move.captureScore == -1) {
                        TranspositionTable::getTT()->historyMoves[move.pieceType][move.toSquare] += depth * depth;
                        TranspositionTable::getTT()->counterMoves[previousMove.fromSquare][previousMove.toSquare] = moveCode;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta, NodeType::FAIL_HIGH_NODE);
                    return {rootMove, beta};
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = result.score;
                nodeType = NodeType::PV_NODE;
            }

            searchedFirstLegalMove = true;
        }

        if (!searchedFirstLegalMove) {
            return {rootMove, beta};
        }

        while (moves.hasNext()) {
            if (std::chrono::high_resolution_clock::now() > endTime) {
                return {rootMove, beta};
            }

            Move move = moves.getNextMove();

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()))) {
                board.unmakeMove();
                continue;
            }

            bool ownKingInCheck = board.isKingInCheck(board.getMovingColor());
            int depthExtension = 0;
            int depthReduction = 0;

            if (ownKingInCheck) {
                depthExtension++;
            } else if (!depthExtension && move.promotionPiece == PieceType::EMPTY) {
/*                if (depth >= 3 && !board.isPawnEndgame()) {
                    board.makeNullMove();
                    int score = search(board, depth > 6 ? depth - 4 : depth - 3, -beta, -alpha, rootMove, previousMove, endTime, line).score * -1;
                    board.unmakeMove();

                    if (score >= beta) {
                        board.unmakeMove();
                        return {rootMove, beta};
                    }
                }*/

                if (depth >= 3 && move.captureScore < 0 && moveCounter > 4) {
                    depthReduction += depth / 2;
                }
            }

            SearchResult result;
            result = search(board, depth - 1 - depthReduction, -alpha - 1, -alpha, rootMove, previousMove, endTime, line);
            result.score *= -1;

            if (result.score > alpha && result.score < beta) {
                result = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line);
                result.score *= -1;
            }

            board.unmakeMove();

            if (result.score > alpha) {
                if (result.score >= beta) {
                    int moveCode = encodeMove(move);
                    TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][depth];
                    TranspositionTable::getTT()->killerMoves[0][board.getPly()] = moveCode;

                    if (move.captureScore == -1) {
                        TranspositionTable::getTT()->historyMoves[move.pieceType][move.toSquare] += depth * depth;
                        TranspositionTable::getTT()->counterMoves[previousMove.fromSquare][previousMove.toSquare] = moveCode;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, result.score, NodeType::FAIL_HIGH_NODE);
                    return {rootMove, beta};
                }

                nodeType = NodeType::PV_NODE;
                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = result.score;
            }

            moveCounter++;
        }

        TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, nodeType);
        return {rootMove, alpha};
    }

    SearchResult SearchManager::quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                                        Move &previousMove,
                                        std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        searchStats.qnodes += 1;

        if (std::chrono::high_resolution_clock::now() > endTime) {
            return {rootMove, beta};
        }

        int standPat = evaluate(board, endTime);

        if (standPat >= beta) {
            return {rootMove, beta};
        }

        int delta = 1000;

        if (previousMove.promotionPiece != PieceType::EMPTY) {
            delta += board.getPieceWeight(previousMove.promotionPiece) - 100;
        }

        if (standPat < alpha - delta) {
            return {rootMove, alpha};
        }

        if (alpha < standPat) {
            alpha = standPat;
        }

        MovePicker moves = generateQuiescenceMoves(board, board.getMovingColor());
        while (moves.hasNext()) {
            Move move = moves.getNextMove();
            assert(move.fromSquare != move.toSquare);

            if (std::chrono::high_resolution_clock::now() > endTime) {
                return {rootMove, beta};
            }

            if (move.captureScore < 0) {
                continue;
            }

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()))) {
                board.unmakeMove();
                continue;
            }

            SearchResult result = quiesce(board, -beta, -alpha, rootMove, move, endTime);
            result.score *= -1;
            board.unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }

            if (result.score > alpha) {
                alpha = result.score;
            }
        }

        return {rootMove, alpha};
    }

    bool SearchManager::isCurrentlySearching() {
        return isSearching;
    }

    senjo::SearchStats SearchManager::getSearchStats() {
        return searchStats;
    }

    int SearchManager::evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (std::chrono::high_resolution_clock::now() > endTime) {
            return 0;
        }

        int modifier = board.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board.isWinner(board.getMovingColor())) {
            return 10000 - board.getPly();
        } else if (board.isWinner(Bitboard::getOppositeColor(board.getMovingColor()))) {
            return -10000 + board.getPly();
        }

        if (board.isDraw()) {
            // Thanks Stockfish for the "3-fold blindness avoidance"
            return 0 - 1 + ((int) searchStats.nodes & 0x2);
        }

        EvalContext evalContext = createEvalContext(board);

        getWhiteMaterialScore(evalContext, board);
        getBlackMaterialScore(evalContext, board);

        getWhiteMobilityScore(evalContext, board);
        getBlackMobilityScore(evalContext,board);

        getWhiteConnectivityScore(evalContext, board);
        getBlackConnectivityScore(evalContext, board);

        getWhiteKingScore(evalContext, board);
        getBlackKingScore(evalContext, board);

        getPawnScore(evalContext, board, PieceColor::WHITE);
        getPawnScore(evalContext, board, PieceColor::BLACK);

        getWhiteBishopScore(evalContext, board);
        getBlackBishopScore(evalContext, board);

        getWhiteRookScore(evalContext, board);
        getBlackRookScore(evalContext, board);

        getWhiteKnightScore(evalContext, board);
        getBlackKnightScore(evalContext, board);

        getWhiteDevelopmentScore(evalContext, board);
        getBlackDevelopmentScore(evalContext, board);

        getPositionalScore(evalContext, board, PieceColor::WHITE);
        getPositionalScore(evalContext, board, PieceColor::BLACK);

        int whiteScore = ((evalContext.whiteMidgameScore * (256 - evalContext.phase)) + (evalContext.whiteEndgameScore * evalContext.phase)) / 256;
        int blackScore = ((evalContext.blackMidgameScore * (256 - evalContext.phase)) + (evalContext.blackEndgameScore * evalContext.phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    void SearchManager::getBlackMaterialScore(EvalContext &evalContext, Bitboard &board) const {
        evalContext.blackMidgameScore += popcnt(board.getPieceBoard(BLACK_PAWN)) * board.getPieceWeight(BLACK_PAWN);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard(BLACK_PAWN)) * board.getPieceWeight(BLACK_PAWN);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard(BLACK_KNIGHT)) * board.getPieceWeight(BLACK_KNIGHT);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard(BLACK_KNIGHT)) * board.getPieceWeight(BLACK_KNIGHT);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard(BLACK_BISHOP)) * board.getPieceWeight(BLACK_BISHOP);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard(BLACK_BISHOP)) * board.getPieceWeight(BLACK_BISHOP);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard(BLACK_ROOK)) * board.getPieceWeight(BLACK_ROOK);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard(BLACK_ROOK)) * board.getPieceWeight(BLACK_ROOK);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard(BLACK_QUEEN)) * board.getPieceWeight(BLACK_QUEEN);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard(BLACK_QUEEN)) * board.getPieceWeight(BLACK_QUEEN);
    }

    void SearchManager::getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board) const {
        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard(WHITE_PAWN)) * board.getPieceWeight(WHITE_PAWN);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard(WHITE_PAWN)) * board.getPieceWeight(WHITE_PAWN);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard(WHITE_KNIGHT)) * board.getPieceWeight(WHITE_KNIGHT);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard(WHITE_KNIGHT)) * board.getPieceWeight(WHITE_KNIGHT);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard(WHITE_BISHOP)) * board.getPieceWeight(WHITE_BISHOP);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard(WHITE_BISHOP)) * board.getPieceWeight(WHITE_BISHOP);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard(WHITE_ROOK)) * board.getPieceWeight(WHITE_ROOK);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard(WHITE_ROOK)) * board.getPieceWeight(WHITE_ROOK);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard(WHITE_QUEEN)) * board.getPieceWeight(WHITE_QUEEN);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard(WHITE_QUEEN)) * board.getPieceWeight(WHITE_QUEEN);
    }

    void SearchManager::getWhiteConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t whitePieces = bitboard.getWhiteBoard() & ~kingBB;
        uint64_t protectedPieces = whitePieces & evalContext.whiteCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);
            int weight = bitboard.getPieceWeight(pieceType);

            evalContext.whiteMidgameScore += 11 - (weight / 100);
            evalContext.whiteEndgameScore += 11 - (weight / 100);
            protectedPieces &= ~(1ULL << index);
        }
    }

    void SearchManager::getBlackConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::BLACK_KING);
        uint64_t blackPieces = bitboard.getBlackBoard() & ~kingBB;
        uint64_t protectedPieces = blackPieces & evalContext.blackCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);
            int weight = bitboard.getPieceWeight(pieceType);

            evalContext.blackMidgameScore += 11 - (weight / 100);
            evalContext.blackEndgameScore += 11 - (weight / 100);
            protectedPieces &= ~(1ULL << index);
        }
    }

    uint64_t whiteQueenCastlingAttackPattern = 0x70;
    uint64_t whiteKingCastlingAttackPattern = 0x6;
    void SearchManager::getWhiteKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.whiteMidgameScore += std::min(60, (int) (popcnt(pawnShield) * 20));
        evalContext.whiteEndgameScore += 0;

        if (bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::WHITE)) {
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
        }

        evalContext.whiteMidgameScore -= popcnt(evalContext.blackPawnAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_PAWN) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackPawnAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_PAWN) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackKnightAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_KNIGHT) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackKnightAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_KNIGHT) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackBishopAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_BISHOP) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackBishopAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_BISHOP) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackRookAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_ROOK) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackRookAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_ROOK) / 100);
        evalContext.whiteMidgameScore -= popcnt(evalContext.blackQueenAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_QUEEN) / 100);
        evalContext.whiteEndgameScore -= popcnt(evalContext.blackQueenAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_QUEEN) / 100);
    }

    uint64_t blackQueenCastlingAttackPattern = 0x7000000000000000;
    uint64_t blackKingCastlingAttackPattern = 0x600000000000000;
    void SearchManager::getBlackKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::BLACK_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= soutOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.blackMidgameScore += std::min(60, (int) (popcnt(pawnShield) * 20));
        evalContext.blackEndgameScore += 0;

        if (bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::BLACK)) {
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
        }

        evalContext.blackMidgameScore -= popcnt(evalContext.whitePawnAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_PAWN) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whitePawnAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_PAWN) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteKnightAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_KNIGHT) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteKnightAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_KNIGHT) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteBishopAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_BISHOP) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteBishopAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_BISHOP) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteRookAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_ROOK) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteRookAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_ROOK) / 100);
        evalContext.blackMidgameScore -= popcnt(evalContext.whiteQueenAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_QUEEN) / 100);
        evalContext.blackEndgameScore -= popcnt(evalContext.whiteQueenAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_QUEEN) / 100);
    }

    void SearchManager::resetStats() {
    }

    void SearchManager::getWhiteBishopScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::WHITE_BISHOP);
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);

        if (bishopAmount == 1) {
            evalContext.whiteMidgameScore -= 25;
            evalContext.whiteEndgameScore -= 25;
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G2 || index == Square::B2) {
                uint64_t fianchettoPattern = nortOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = noWeOne(1ULL << index) | noEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    evalContext.whiteMidgameScore += 10;
                    evalContext.whiteEndgameScore += 0;
                }
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void SearchManager::getBlackBishopScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::BLACK_BISHOP);
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);

        if (bishopAmount == 1) {
            evalContext.blackMidgameScore -= 25;
            evalContext.blackEndgameScore -= 25;
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G7 || index == Square::B7) {
                uint64_t fianchettoPattern = soutOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = soWeOne(1ULL << index) | soEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    evalContext.blackMidgameScore += 10;
                    evalContext.blackEndgameScore += 0;
                }
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    uint64_t whiteMinorPiecesStartBB = 0x66ULL;
    uint64_t whiteRookStartBB = 0x81ULL;
    uint64_t whiteQueenStartBB = 0x10ULL;

    void SearchManager::getWhiteDevelopmentScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t minorPiecesOnStart =
                (bitboard.getPieceBoard(PieceType::WHITE_KNIGHT) | bitboard.getPieceBoard(PieceType::WHITE_BISHOP)) &
                whiteMinorPiecesStartBB;
        uint64_t minorPiecesOnStartAmount = popcnt(minorPiecesOnStart);
        uint64_t rooksOnStart = bitboard.getPieceBoard(PieceType::WHITE_ROOK) & whiteRookStartBB;
        uint64_t rooksOnStartAmount = popcnt(rooksOnStart);
        uint64_t queenOnStart = bitboard.getPieceBoard(PieceType::WHITE_QUEEN) & whiteQueenStartBB;
        uint64_t queenOnStartAmount = popcnt(queenOnStart);

        evalContext.whiteMidgameScore += (4 - minorPiecesOnStartAmount) * 30;
        evalContext.whiteEndgameScore += 0;

        if (minorPiecesOnStartAmount == 0) {
            evalContext.whiteMidgameScore += (2 - rooksOnStartAmount) * 30;
            evalContext.whiteEndgameScore += 0;
            evalContext.whiteMidgameScore += (1 - queenOnStartAmount) * 30;
            evalContext.whiteEndgameScore += 0;
        }
    }

    uint64_t blackMinorPiecesStartBB = 0x6600000000000000ULL;
    uint64_t blackRookStartBB = 0x8100000000000000ULL;
    uint64_t blackQueenStartBB = 0x1000000000000000ULL;

    void SearchManager::getBlackDevelopmentScore(EvalContext &evalContext, Bitboard &bitboard) {
        if (bitboard.getPly() > 12) {
            return;
        }

        uint64_t minorPiecesOnStart =
                (bitboard.getPieceBoard(PieceType::BLACK_KNIGHT) | bitboard.getPieceBoard(PieceType::BLACK_BISHOP)) &
                blackMinorPiecesStartBB;
        uint64_t minorPiecesOnStartAmount = popcnt(minorPiecesOnStart);
        uint64_t rooksOnStart = bitboard.getPieceBoard(PieceType::BLACK_ROOK) & blackRookStartBB;
        uint64_t rooksOnStartAmount = popcnt(rooksOnStart);
        uint64_t queenOnStart = bitboard.getPieceBoard(PieceType::BLACK_QUEEN) & blackQueenStartBB;
        uint64_t queenOnStartAmount = popcnt(queenOnStart);

        evalContext.blackMidgameScore += (4 - minorPiecesOnStartAmount) * 30;
        evalContext.blackEndgameScore += 0;

        if (minorPiecesOnStartAmount == 0) {
            evalContext.blackMidgameScore += (2 - rooksOnStartAmount) * 30;
            evalContext.blackEndgameScore += 0;
            evalContext.blackMidgameScore += (1 - queenOnStartAmount) * 30;
            evalContext.blackEndgameScore += 0;
        }
    }

    void SearchManager::getWhiteRookScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard(PieceType::WHITE_ROOK);
        int rookAmount = popcnt(rookBB);
        int score = 0;

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                score += 20;
            } else if (bitboard.isSemiOpenFile(index, PieceColor::WHITE)) {
                score += 5;
            }

            rookBB &= ~(1ULL << index);
        }

        score += ((8 - popcnt(bitboard.getPieceBoard(PieceType::WHITE_PAWN))) * 4) * rookAmount;

        evalContext.whiteMidgameScore += score;
        evalContext.whiteEndgameScore += score;
    }

    void SearchManager::getBlackRookScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard(PieceType::BLACK_ROOK);
        int rookAmount = popcnt(rookBB);
        int score = 0;

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                score += 20;
            } else if (bitboard.isSemiOpenFile(index, PieceColor::BLACK)) {
                score += 5;
            }

            rookBB &= ~(1ULL << index);
        }

        score += ((8 - popcnt(bitboard.getPieceBoard(PieceType::BLACK_PAWN))) * 4) * rookAmount;

        evalContext.blackMidgameScore += score;
        evalContext.blackEndgameScore += score;
    }

    void SearchManager::getPositionalScore(EvalContext &evalContext, Bitboard &bitboard, PieceColor color) {
        uint64_t colorBB = bitboard.getBoardByColor(color);
        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            if (color == PieceColor::WHITE) {
                evalContext.whiteMidgameScore += getMidgamePstValue(pieceOnSquare, index);
                evalContext.whiteEndgameScore += getEndgamePstValue(pieceOnSquare, index);
            } else {
                evalContext.blackMidgameScore += getMidgamePstValue(pieceOnSquare, index);
                evalContext.blackEndgameScore += getEndgamePstValue(pieceOnSquare, index);
            }

            colorBB = _blsr_u64(colorBB);
        }
    }

    uint64_t centerPattern = 0x0000001818000000;
    uint64_t extendedCenterPattern = 0x00003C3C3C3C0000 & ~centerPattern;
    void SearchManager::getWhiteMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getWhiteBoard();

        // Slight bonus for squares defended by own pawn
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & evalContext.whitePawnAttacks);
        evalContext.whiteEndgameScore += popcnt((evalContext.whiteCombinedAttacks) & evalContext.whitePawnAttacks);

        // Bonus for center control
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & centerPattern) * 10;
        evalContext.whiteEndgameScore += 0;
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & extendedCenterPattern) * 4;
        evalContext.whiteEndgameScore += 0;

        evalContext.whiteMidgameScore += popcnt(evalContext.whiteKnightAttacks & ~ownPiecesBB) * 7;
        evalContext.whiteMidgameScore += popcnt(evalContext.whiteBishopAttacks & ~ownPiecesBB) * 8;
        evalContext.whiteMidgameScore += popcnt(evalContext.whiteRookAttacks & ~ownPiecesBB) * 2;
        evalContext.whiteMidgameScore += popcnt(evalContext.whiteQueenAttacks & ~ownPiecesBB) * 4;

        evalContext.whiteEndgameScore += popcnt(evalContext.whiteKnightAttacks & ~ownPiecesBB) * 2;
        evalContext.whiteEndgameScore += popcnt(evalContext.whiteBishopAttacks & ~ownPiecesBB) * 3;
        evalContext.whiteEndgameScore += popcnt(evalContext.whiteRookAttacks & ~ownPiecesBB) * 6;
        evalContext.whiteEndgameScore += popcnt(evalContext.whiteQueenAttacks & ~ownPiecesBB) * 8;

        evalContext.whiteMidgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackPawnAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_PAWN) / 100));
        evalContext.whiteEndgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackPawnAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_PAWN) / 100));
        evalContext.whiteMidgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackKnightAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_KNIGHT) / 100));
        evalContext.whiteEndgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackKnightAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_KNIGHT) / 100));
        evalContext.whiteMidgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackBishopAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_BISHOP) / 100));
        evalContext.whiteEndgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackBishopAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_BISHOP) / 100));
        evalContext.whiteMidgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackRookAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_ROOK) / 100));
        evalContext.whiteEndgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackRookAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_ROOK) / 100));
        evalContext.whiteMidgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackQueenAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_QUEEN) / 100));
        evalContext.whiteEndgameScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackQueenAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_QUEEN) / 100));
    }

    void SearchManager::getBlackMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getBlackBoard();

        // Slight bonus for squares defended by own pawn
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & evalContext.blackPawnAttacks);
        evalContext.blackEndgameScore += popcnt((evalContext.blackCombinedAttacks) & evalContext.blackPawnAttacks);

        // Bonus for center control
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & centerPattern) * 10;
        evalContext.blackEndgameScore += 0;
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & extendedCenterPattern) * 4;
        evalContext.blackEndgameScore += 0;

        evalContext.blackMidgameScore += popcnt(evalContext.blackKnightAttacks & ~ownPiecesBB) * 7;
        evalContext.blackMidgameScore += popcnt(evalContext.blackBishopAttacks & ~ownPiecesBB) * 8;
        evalContext.blackMidgameScore += popcnt(evalContext.blackRookAttacks & ~ownPiecesBB) * 2;
        evalContext.blackMidgameScore += popcnt(evalContext.blackQueenAttacks & ~ownPiecesBB) * 4;

        evalContext.blackEndgameScore += popcnt(evalContext.blackKnightAttacks & ~ownPiecesBB) * 2;
        evalContext.blackEndgameScore += popcnt(evalContext.blackBishopAttacks & ~ownPiecesBB) * 3;
        evalContext.blackEndgameScore += popcnt(evalContext.blackRookAttacks & ~ownPiecesBB) * 6;
        evalContext.blackEndgameScore += popcnt(evalContext.blackQueenAttacks & ~ownPiecesBB) * 8;

        evalContext.blackMidgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whitePawnAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_PAWN) / 100));
        evalContext.blackEndgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whitePawnAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_PAWN) / 100));
        evalContext.blackMidgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteKnightAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_KNIGHT) / 100));
        evalContext.blackEndgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteKnightAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_KNIGHT) / 100));
        evalContext.blackMidgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteBishopAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_BISHOP) / 100));
        evalContext.blackEndgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteBishopAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_BISHOP) / 100));
        evalContext.blackMidgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteRookAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_ROOK) / 100));
        evalContext.blackEndgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteRookAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_ROOK) / 100));
        evalContext.blackMidgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteQueenAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_QUEEN) / 100));
        evalContext.blackEndgameScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteQueenAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_QUEEN) / 100));
    }

    void SearchManager::getPawnScore(EvalContext &evalContext, Bitboard &bitboard, PieceColor color) {
        int score = 0;

        for (int i = 0; i < 8; i++) {
            uint64_t pawnsOnFile = bitboard.getPawnsOnSameFile(i, color);
            bool isPassed = bitboard.isPassedPawn(i, color);
            bool isIsolated = bitboard.isIsolatedPawn(i, color);
            bool isSemiOpen = bitboard.isSemiOpenFile(i, color);
            int amountOfPawns = popcnt(pawnsOnFile);

            score -= 10 * (amountOfPawns - 1);

            if (isPassed) {
                score += 10;
            } else if (isIsolated) {
                if (isSemiOpen) {
                    score -= 20;
                } else {
                    score -= 10;
                }
            } else if (isSemiOpen) {
                score += 3;
            }
        }

        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN + color);

        while (pawnBB) {
            int index = bitscanForward(pawnBB);
            int fileNumber = index % 8;
            int promotionSquare = (color == PieceColor::WHITE) ? 56 + fileNumber : fileNumber;
            uint64_t tilesBetween = bitboard.getTilesBetween(index, promotionSquare);

            if (color == PieceColor::WHITE) {
                evalContext.whiteEndgameScore -= popcnt(tilesBetween) * 10;
            } else {
                evalContext.blackEndgameScore -= popcnt(tilesBetween) * 10;
            }

            pawnBB &= ~(1ULL << index);
        }

        if (color == PieceColor::WHITE) {
            evalContext.whiteMidgameScore += score;
            evalContext.whiteEndgameScore += score;
        } else {
            evalContext.blackMidgameScore += score;
            evalContext.blackEndgameScore += score;
        }
    }

    int knightPhase = 1;
    int bishopPhase = 1;
    int rookPhase = 2;
    int queenPhase = 4;
    int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;
    int SearchManager::getGamePhase(Bitboard &bitboard) {
        int phase = totalPhase;

        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_KNIGHT) | bitboard.getPieceBoard(PieceType::BLACK_KNIGHT)) * knightPhase;
        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_BISHOP) | bitboard.getPieceBoard(PieceType::BLACK_BISHOP)) * bishopPhase;
        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_ROOK) | bitboard.getPieceBoard(PieceType::BLACK_ROOK)) * rookPhase;
        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_QUEEN) | bitboard.getPieceBoard(PieceType::BLACK_QUEEN)) * queenPhase;

        return (phase * 256 + (totalPhase / 2)) / totalPhase;
    }

    EvalContext SearchManager::createEvalContext(Bitboard &bitboard) {
        uint64_t whitePawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);
        uint64_t whiteKnightBB = bitboard.getPieceBoard(PieceType::WHITE_KNIGHT);
        uint64_t whiteBishopBB = bitboard.getPieceBoard(PieceType::WHITE_BISHOP);
        uint64_t whiteRookBB = bitboard.getPieceBoard(PieceType::WHITE_ROOK);
        uint64_t whiteQueenBB = bitboard.getPieceBoard(PieceType::WHITE_QUEEN);

        uint64_t blackPawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);
        uint64_t blackKnightBB = bitboard.getPieceBoard(PieceType::BLACK_KNIGHT);
        uint64_t blackBishopBB = bitboard.getPieceBoard(PieceType::BLACK_BISHOP);
        uint64_t blackRookBB = bitboard.getPieceBoard(PieceType::BLACK_ROOK);
        uint64_t blackQueenBB = bitboard.getPieceBoard(PieceType::BLACK_QUEEN);

        uint64_t whitePawnAttacks = bitboard.calculatePawnAttacks(whitePawnBB, PieceColor::WHITE);
        uint64_t whiteKnightAttacks = calculateKnightAttacks(whiteKnightBB);
        uint64_t whiteBishopAttacks = 0;
        uint64_t whiteRookAttacks = 0;
        uint64_t whiteQueenAttacks = 0;

        uint64_t blackPawnAttacks = bitboard.calculatePawnAttacks(blackPawnBB, PieceColor::BLACK);
        uint64_t blackKnightAttacks = calculateKnightAttacks(blackKnightBB);
        uint64_t blackBishopAttacks = 0;
        uint64_t blackRookAttacks = 0;
        uint64_t blackQueenAttacks = 0;

        uint64_t occupied = bitboard.getOccupiedBoard();

        while (whiteBishopBB) {
            uint64_t index = bitscanForward(whiteBishopBB);

//            if (!bitboard.isPinned(index, PieceColor::WHITE)) {
                whiteBishopAttacks |= bitboard.getBishopAttacks(index, occupied);
//            }

            whiteBishopBB &= ~(1ULL << index);
        }

        while (whiteRookBB) {
            uint64_t index = bitscanForward(whiteRookBB);

//            if (!bitboard.isPinned(index, PieceColor::WHITE)) {
                whiteRookAttacks |= bitboard.getRookAttacks(index, occupied);
//            }

            whiteRookBB &= ~(1ULL << index);
        }

        while (whiteQueenBB) {
            uint64_t index = bitscanForward(whiteQueenBB);

//            if (!bitboard.isPinned(index, PieceColor::WHITE)) {
                whiteQueenAttacks |= bitboard.getQueenAttacks(index, occupied);
//            }

            whiteQueenBB &= ~(1ULL << index);
        }

        while (blackBishopBB) {
            uint64_t index = bitscanForward(blackBishopBB);

//            if (!bitboard.isPinned(index, PieceColor::BLACK)) {
                blackBishopAttacks |= bitboard.getBishopAttacks(index, occupied);
//            }

            blackBishopBB &= ~(1ULL << index);
        }

        while (blackRookBB) {
            uint64_t index = bitscanForward(blackRookBB);

//            if (!bitboard.isPinned(index, PieceColor::BLACK)) {
                blackRookAttacks |= bitboard.getRookAttacks(index, occupied);
//            }

            blackRookBB &= ~(1ULL << index);
        }

        while (blackQueenBB) {
            uint64_t index = bitscanForward(blackQueenBB);

//            if (!bitboard.isPinned(index, PieceColor::BLACK)) {
                blackQueenAttacks |= bitboard.getQueenAttacks(index, occupied);
//            }

            blackQueenBB &= ~(1ULL << index);
        }

        uint64_t whiteCombinedAttacks = whiteKnightAttacks | whiteBishopAttacks | whiteRookAttacks | whiteQueenAttacks | whitePawnAttacks;
        uint64_t blackCombinedAttacks = blackKnightAttacks | blackBishopAttacks | blackRookAttacks | blackQueenAttacks | blackPawnAttacks;
        
        return EvalContext {
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
            blackCombinedAttacks
        };
    }

    void SearchManager::getWhiteKnightScore(EvalContext &context, Bitboard &bitboard) {
        uint64_t whiteKnightBB = bitboard.getPieceBoard(PieceType::WHITE_KNIGHT);
        int knightAmount = popcnt(whiteKnightBB);

        context.whiteMidgameScore -= ((8 - popcnt(bitboard.getPieceBoard(PieceType::WHITE_PAWN))) * 4) * knightAmount;
        context.whiteEndgameScore -= ((8 - popcnt(bitboard.getPieceBoard(PieceType::WHITE_PAWN))) * 4) * knightAmount;
    }

    void SearchManager::getBlackKnightScore(EvalContext &context, Bitboard &bitboard) {
        uint64_t blackKnightBB = bitboard.getPieceBoard(PieceType::BLACK_KNIGHT);
        int knightAmount = popcnt(blackKnightBB);

        context.blackMidgameScore -= ((8 - popcnt(bitboard.getPieceBoard(PieceType::BLACK_PAWN))) * 4) * knightAmount;
        context.blackEndgameScore -= ((8 - popcnt(bitboard.getPieceBoard(PieceType::BLACK_PAWN))) * 4) * knightAmount;
    }
}

#pragma clang diagnostic pop