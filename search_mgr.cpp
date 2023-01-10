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
    SearchResult SearchManager::getBestMove(Bitboard &board, PieceColor color) {
        searchStats = {};
        isSearching = true;
        SearchResult bestResult;
        SearchResult iterationResult;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime = timeManager.getEndTime(board, color);
        std::vector<Move> moves = generateLegalMoves(board, color);
        int depth = 0;

        while (std::chrono::high_resolution_clock::now() - startTime < (endTime - startTime) * 0.7) {
            depth += 1;
            searchStats.depth = depth;
            searchStats.seldepth = 0;

            senjo::Output(senjo::Output::InfoPrefix) << "Searching depth " << depth << "...";

            for (Move &move : moves) {
                assert(move.fromSquare != move.toSquare);
                assert(move.fromSquare >= 0 && move.fromSquare < 64);
                assert(move.toSquare >= 0 && move.toSquare < 64);

                if (depth > 1 && std::chrono::high_resolution_clock::now() > endTime) {
                    break;
                }

                board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

                if (board.isKingInCheck(color)) {
                    board.unmakeMove();
                    continue;
                }

                Move previousMove = {board.getPreviousMoveFrom(), board.getPreviousMoveTo()};
                SearchResult result = search(board, depth, -99999999, 99999999, move, previousMove, endTime);
                result.score *= -1;
                board.unmakeMove();

                if (result.score > iterationResult.score) {
                    assert(result.move.pieceType != PieceType::EMPTY);
                    iterationResult = result;
                    searchStats.score = iterationResult.score;
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

        searchStats.score = bestResult.score;
        searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count();
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
        return bestResult;
    }

    SearchResult SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                                       Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() > endTime
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) || board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime);
        }

        searchStats.nodes += 1;

        bool searchPv = true;
        std::vector<Move> moves = generateLegalMoves(board, board.getMovingColor());

        for (int i = 0; i < moves.size(); i++) {
            Move &move = moves[i];
            assert(move.fromSquare != move.toSquare);

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            SearchResult result;
            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()))) {
                board.unmakeMove();
                continue;
            }

            TTEntry* entry = tt.getPosition(board.getZobristHash());

            if (entry->zobristHash == board.getZobristHash() && entry->depth >= depth) {
                result = {rootMove, entry->score};
            } else {
                bool ownKingInCheck = board.isKingInCheck(board.getMovingColor());
                bool opponentKingInCheck = board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()));
                int depthExtension = 0;

                if (ownKingInCheck || opponentKingInCheck) {
                    depthExtension++;
                } else if (!depthExtension && move.promotionPiece == PieceType::EMPTY) {
                    if (depth >= 3 && !board.isPawnEndgame() && !depthExtension) {
                        board.makeNullMove();
                        int score = zwSearch(board, depth - 3, -alpha, rootMove, rootMove, endTime).score * -1;
                        board.unmakeMove();

                        if (score >= beta) {
                            board.unmakeMove();
                            return {rootMove, beta};
                        }
                    }

                    if (depth >= 3 && move.captureScore < 0 && i > 5) {
                        if (searchPv) {
                            result = search(board, depth / 2, -beta, -alpha, rootMove, move, endTime);
                        } else {
                            result = zwSearch(board, depth / 4, -alpha, rootMove, rootMove,
                                              endTime);
                        }

                        result.score *= -1;

                        if (result.score <= alpha) {
                            board.unmakeMove();
                            continue;
                        }
                    }
                }

                if (searchPv) {
                    result = search(board, depth - 1 + depthExtension, -beta, -alpha, rootMove, move, endTime);
                    result.score *= -1;
                    tt.addPosition(board.getZobristHash(), depth, result.score, searchPv);
                } else {
                    result = zwSearch(board, depth - 1 + depthExtension, -alpha, rootMove, move, endTime);
                    result.score *= -1;

                    if (result.score > alpha) {
                        result = search(board, depth - 1 + depthExtension, -beta, -alpha, rootMove, move, endTime);
                        result.score *= -1;
                        tt.addPosition(board.getZobristHash(), depth, result.score, searchPv);
                    }
                }
            }

            board.unmakeMove();

            if (result.score >= beta) {
                tt.killerMoves[1][board.getPly()] = tt.killerMoves[0][depth];
                tt.killerMoves[0][board.getPly()] = encodeMove(move);
                tt.counterMoves[previousMove.fromSquare][previousMove.toSquare] = encodeMove(previousMove);

                return {rootMove, beta};
            }

            if (result.score > alpha) {
                if (searchPv) {
                    tt.addPosition(board.getZobristHash(), depth, result.score, searchPv);
                }

                assert(move.fromSquare >= 0);
                assert(move.toSquare >= 0);
                tt.historyMoves[move.pieceType][move.toSquare] += depth * depth;
                alpha = result.score;
                searchPv = false;
            }
        }

        return {rootMove, alpha};
    }

    SearchResult
    SearchManager::zwSearch(Bitboard &board, int depth, int beta, Move &rootMove,
                            Move &previousMove,
                            std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() > endTime
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) || board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            return quiesce(board, beta - 1, beta, rootMove, previousMove, endTime);
        }

        searchStats.nodes += 1;

        std::vector<Move> moves = generateLegalMoves(board, board.getMovingColor());

        for (int i = 0; i < moves.size(); i++) {
            Move &move = moves[i];
            assert(move.fromSquare != move.toSquare);

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()))) {
                board.unmakeMove();
                continue;
            }

            int depthExtension = 0;
            bool ownKingInCheck = board.isKingInCheck(board.getMovingColor());
            bool opponentKingInCheck = board.isKingInCheck(Bitboard::getOppositeColor(board.getMovingColor()));
            SearchResult result;

            if (ownKingInCheck || opponentKingInCheck) {
                depthExtension++;
            } else if (depth >= 3 && !depthExtension && move.captureScore < 0 && i > 5) {
                result = zwSearch(board, depth / 4, 1 - beta, rootMove, rootMove, endTime);
                result.score *= -1;

                if (result.score <= beta - 1) {
                    board.unmakeMove();
                    continue;
                }
            }


            result = zwSearch(board, depth - 1 + depthExtension, 1 - beta, rootMove, move, endTime);
            result.score *= -1;

            board.unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }
        }

        return {rootMove, beta - 1};
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

        std::vector<Move> moves = generateQuiescenceMoves(board, board.getMovingColor());
        for (Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
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

    int SearchManager::evaluate(Bitboard &board,
                                std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
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
            return 0;
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

        getWhiteDevelopmentScore(evalContext, board);
        getBlackDevelopmentScore(evalContext, board);

        getPositionalScore(evalContext, board, PieceColor::WHITE);
        getPositionalScore(evalContext, board, PieceColor::BLACK);

        return (evalContext.whiteScore - evalContext.blackScore) * modifier;
    }

    void SearchManager::getBlackMaterialScore(EvalContext &evalContext, Bitboard &board) const {
        evalContext.blackScore += popcnt(board.getPieceBoard(BLACK_PAWN)) * board.getPieceWeight(BLACK_PAWN);
        evalContext.blackScore += popcnt(board.getPieceBoard(BLACK_KNIGHT)) * board.getPieceWeight(BLACK_KNIGHT);
        evalContext.blackScore += popcnt(board.getPieceBoard(BLACK_BISHOP)) * board.getPieceWeight(BLACK_BISHOP);
        evalContext.blackScore += popcnt(board.getPieceBoard(BLACK_ROOK)) * board.getPieceWeight(BLACK_ROOK);
        evalContext.blackScore += popcnt(board.getPieceBoard(BLACK_QUEEN)) * board.getPieceWeight(BLACK_QUEEN);
    }

    void SearchManager::getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board) const {
        evalContext.whiteScore += popcnt(board.getPieceBoard(WHITE_PAWN)) * board.getPieceWeight(WHITE_PAWN);
        evalContext.whiteScore += popcnt(board.getPieceBoard(WHITE_KNIGHT)) * board.getPieceWeight(WHITE_KNIGHT);
        evalContext.whiteScore += popcnt(board.getPieceBoard(WHITE_BISHOP)) * board.getPieceWeight(WHITE_BISHOP);
        evalContext.whiteScore += popcnt(board.getPieceBoard(WHITE_ROOK)) * board.getPieceWeight(WHITE_ROOK);
        evalContext.whiteScore += popcnt(board.getPieceBoard(WHITE_QUEEN)) * board.getPieceWeight(WHITE_QUEEN);
    }

    void SearchManager::getWhiteConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t whitePieces = bitboard.getWhiteBoard() & ~kingBB;
        uint64_t protectedPieces = whitePieces & evalContext.whiteCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);
            int weight = bitboard.getPieceWeight(pieceType);

            evalContext.whiteScore += 11 - (weight / 100);
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

            evalContext.blackScore += 11 - (weight / 100);
            protectedPieces &= ~(1ULL << index);
        }
    }

    void SearchManager::getWhiteKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.whiteScore += std::min(60, (int) (popcnt(pawnShield) * 20));

        if (bitboard.getPly() < 40 && kingLocation <= 15) {
            evalContext.whiteScore += 10;
        }

        if (bitboard.getPly() < 40 && bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::WHITE)) {
            evalContext.whiteScore -= 50;
        }

        uint8_t castlingRights = bitboard.getCastlingRights();

        if (!(castlingRights & CastlingRights::WHITE_QUEENSIDE) && !(castlingRights & CastlingRights::WHITE_KINGSIDE) && !bitboard.isHasWhiteCastled()) {
            evalContext.whiteScore -= 40;
        }

        evalContext.whiteScore -= popcnt(evalContext.blackPawnAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_PAWN) / 100);
        evalContext.whiteScore -= popcnt(evalContext.blackKnightAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_KNIGHT) / 100);
        evalContext.whiteScore -= popcnt(evalContext.blackBishopAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_BISHOP) / 100);
        evalContext.whiteScore -= popcnt(evalContext.blackRookAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_ROOK) / 100);
        evalContext.whiteScore -= popcnt(evalContext.blackQueenAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::BLACK_QUEEN) / 100);
    }

    // TODO: add proper game phase stuff
    void SearchManager::getBlackKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::BLACK_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= soutOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.blackScore += std::min(60, (int) (popcnt(pawnShield) * 20));

        if (bitboard.getPly() < 40 && kingLocation <= 15) {
            evalContext.blackScore += 10;
        }

        if (bitboard.getPly() < 40 && bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::BLACK)) {
            evalContext.blackScore -= 50;
        }

        uint8_t castlingRights = bitboard.getCastlingRights();
        if (!(castlingRights & CastlingRights::BLACK_QUEENSIDE) && !(castlingRights & CastlingRights::BLACK_KINGSIDE) && !bitboard.isHasBlackCastled()) {
            evalContext.blackScore -= 40;
        }

        evalContext.blackScore -= popcnt(evalContext.whitePawnAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_PAWN) / 100);
        evalContext.blackScore -= popcnt(evalContext.whiteKnightAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_KNIGHT) / 100);
        evalContext.blackScore -= popcnt(evalContext.whiteBishopAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_BISHOP) / 100);
        evalContext.blackScore -= popcnt(evalContext.whiteRookAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_ROOK) / 100);
        evalContext.blackScore -= popcnt(evalContext.whiteQueenAttacks & kingAttacks) * (bitboard.getPieceWeight(PieceType::WHITE_QUEEN) / 100);
    }

    void SearchManager::resetStats() {
        searchStats = {};
    }

    void SearchManager::getWhiteBishopScore(EvalContext &evalContext, Bitboard &bitboard) {
        int score = 0;
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::WHITE_BISHOP);
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);

        if (bishopAmount == 1) {
            score -= 25;
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G2 || index == Square::B2) {
                uint64_t fianchettoPattern = nortOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = noWeOne(1ULL << index) | noEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    score += 10;
                }
            }

            bishopBB &= ~(1ULL << index);
        }

        evalContext.whiteScore += score;
    }

    void SearchManager::getBlackBishopScore(EvalContext &evalContext, Bitboard &bitboard) {
        int score = 0;
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::BLACK_BISHOP);
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);

        if (bishopAmount == 1) {
            score -= 25;
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G7 || index == Square::B7) {
                uint64_t fianchettoPattern = soutOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = soWeOne(1ULL << index) | soEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    score += 10;
                }
            }

            bishopBB &= ~(1ULL << index);
        }

        evalContext.blackScore += score;
    }

    uint64_t whiteMinorPiecesStartBB = 0x66ULL;
    uint64_t whiteRookStartBB = 0x81ULL;
    uint64_t whiteQueenStartBB = 0x10ULL;

    void SearchManager::getWhiteDevelopmentScore(EvalContext &evalContext, Bitboard &bitboard) {
        if (bitboard.getFullmoveClock() > 12) {
            return;
        }

        uint64_t minorPiecesOnStart =
                (bitboard.getPieceBoard(PieceType::WHITE_KNIGHT) | bitboard.getPieceBoard(PieceType::WHITE_BISHOP)) &
                whiteMinorPiecesStartBB;
        uint64_t minorPiecesOnStartAmount = popcnt(minorPiecesOnStart);
        uint64_t rooksOnStart = bitboard.getPieceBoard(PieceType::WHITE_ROOK) & whiteRookStartBB;
        uint64_t rooksOnStartAmount = popcnt(rooksOnStart);
        uint64_t queenOnStart = bitboard.getPieceBoard(PieceType::WHITE_QUEEN) & whiteQueenStartBB;
        uint64_t queenOnStartAmount = popcnt(queenOnStart);

        evalContext.whiteScore += (4 - minorPiecesOnStartAmount) * 30;

        if (minorPiecesOnStartAmount == 0) {
            evalContext.whiteScore += (2 - rooksOnStartAmount) * 30;
            evalContext.whiteScore += (1 - queenOnStartAmount) * 30;
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

        evalContext.blackScore += (4 - minorPiecesOnStartAmount) * 30;

        if (minorPiecesOnStartAmount == 0) {
            evalContext.blackScore += (2 - rooksOnStartAmount) * 30;
            evalContext.blackScore += (1 - queenOnStartAmount) * 30;
        }
    }

    void SearchManager::getWhiteRookScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard(PieceType::WHITE_ROOK);
        int score = 0;

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                score += 20;
            } else if (bitboard.isSemiOpenFile(index, PieceColor::WHITE)) {
                score += 5;
            }

            if (index >= Square::H7) {
                score += 20;
            }

            rookBB &= ~(1ULL << index);
        }

        evalContext.whiteScore += score;
    }

    void SearchManager::getBlackRookScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard(PieceType::BLACK_ROOK);
        int score = 0;

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                score += 20;
            } else if (bitboard.isSemiOpenFile(index, PieceColor::BLACK)) {
                score += 5;
            }

            if (index <= Square::A2) {
                score += 20;
            }

            rookBB &= ~(1ULL << index);
        }

        evalContext.blackScore += score;
    }

    void SearchManager::getPositionalScore(EvalContext &evalContext, Bitboard &bitboard, PieceColor color) {
        uint64_t colorBB = bitboard.getBoardByColor(color);
        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            if (color == PieceColor::WHITE) {
                evalContext.whiteScore += getPstValue(pieceOnSquare, index);
            } else {
                evalContext.blackScore += getPstValue(pieceOnSquare, index);
            }

            colorBB = _blsr_u64(colorBB);
        }
    }

    uint64_t centerPattern = 0x0000001818000000;
    uint64_t extendedCenterPattern = 0x00003C3C3C3C0000 & ~centerPattern;
    void SearchManager::getWhiteMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getWhiteBoard();

        // Slight bonus for squares defended by own pawn
        evalContext.whiteScore += popcnt((evalContext.whiteCombinedAttacks & ~ownPiecesBB) & evalContext.whitePawnAttacks);

        // Bonus for center control
        evalContext.whiteScore += popcnt((evalContext.whiteCombinedAttacks & ~ownPiecesBB) & centerPattern) * 10;
        evalContext.whiteScore += popcnt((evalContext.whiteCombinedAttacks & ~ownPiecesBB) & extendedCenterPattern) * 4;

        if (bitboard.getFullmoveClock() > 12) {
            evalContext.whiteScore += popcnt(evalContext.whiteKnightAttacks & ~ownPiecesBB) * 3;
            evalContext.whiteScore += popcnt(evalContext.whiteBishopAttacks & ~ownPiecesBB) * 3;
            evalContext.whiteScore += popcnt(evalContext.whiteRookAttacks & ~ownPiecesBB) * 7;
            evalContext.whiteScore += popcnt(evalContext.whiteQueenAttacks & ~ownPiecesBB) * 10;
        } else {
            evalContext.whiteScore += popcnt(evalContext.whiteKnightAttacks & ~ownPiecesBB) * 8;
            evalContext.whiteScore += popcnt(evalContext.whiteBishopAttacks & ~ownPiecesBB) * 8;
            evalContext.whiteScore += popcnt(evalContext.whiteRookAttacks & ~ownPiecesBB) * 2;
            evalContext.whiteScore += popcnt(evalContext.whiteQueenAttacks & ~ownPiecesBB) * 4;
        }

        evalContext.whiteScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackPawnAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_PAWN) / 100));
        evalContext.whiteScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackKnightAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_KNIGHT) / 100));
        evalContext.whiteScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackBishopAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_BISHOP) / 100));
        evalContext.whiteScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackRookAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_ROOK) / 100));
        evalContext.whiteScore -= popcnt(evalContext.whiteCombinedAttacks & evalContext.blackQueenAttacks) * (11 - (bitboard.getPieceWeight(PieceType::BLACK_QUEEN) / 100));
    }

    // TODO: new mobility causes very low NPS??
    void SearchManager::getBlackMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getBlackBoard() | evalContext.whitePawnAttacks;

        // Slight bonus for squares defended by own pawn
        evalContext.blackScore += popcnt((evalContext.blackCombinedAttacks & ~ownPiecesBB) & evalContext.blackPawnAttacks);

        // Bonus for center control
        evalContext.blackScore += popcnt((evalContext.blackCombinedAttacks & ~ownPiecesBB) & centerPattern) * 10;
        evalContext.blackScore += popcnt((evalContext.blackCombinedAttacks & ~ownPiecesBB) & extendedCenterPattern) * 4;

        if (bitboard.getFullmoveClock() > 12) {
            evalContext.blackScore += popcnt(evalContext.blackKnightAttacks & ~ownPiecesBB) * 3;
            evalContext.blackScore += popcnt(evalContext.blackBishopAttacks & ~ownPiecesBB) * 3;
            evalContext.blackScore += popcnt(evalContext.blackRookAttacks & ~ownPiecesBB) * 7;
            evalContext.blackScore += popcnt(evalContext.blackQueenAttacks & ~ownPiecesBB) * 10;
        } else {
            evalContext.blackScore += popcnt(evalContext.blackKnightAttacks & ~ownPiecesBB) * 8;
            evalContext.blackScore += popcnt(evalContext.blackBishopAttacks & ~ownPiecesBB) * 8;
            evalContext.blackScore += popcnt(evalContext.blackRookAttacks & ~ownPiecesBB) * 2;
            evalContext.blackScore += popcnt(evalContext.blackQueenAttacks & ~ownPiecesBB) * 4;
        }

        evalContext.blackScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whitePawnAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_PAWN) / 100));
        evalContext.blackScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteKnightAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_KNIGHT) / 100));
        evalContext.blackScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteBishopAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_BISHOP) / 100));
        evalContext.blackScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteRookAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_ROOK) / 100));
        evalContext.blackScore -= popcnt(evalContext.blackCombinedAttacks & evalContext.whiteQueenAttacks) * (11 - (bitboard.getPieceWeight(PieceType::WHITE_QUEEN) / 100));
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

        if (color == PieceColor::WHITE) {
            evalContext.whiteScore += score;
        } else {
            evalContext.blackScore += score;
        }
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

            if (!bitboard.isPinned(index, PieceColor::WHITE)) {
                whiteBishopAttacks |= bitboard.getBishopAttacks(index, occupied);
            }

            whiteBishopBB &= ~(1ULL << index);
        }

        while (whiteRookBB) {
            uint64_t index = bitscanForward(whiteRookBB);

            if (!bitboard.isPinned(index, PieceColor::WHITE)) {
                whiteRookAttacks |= bitboard.getRookAttacks(index, occupied);
            }

            whiteRookBB &= ~(1ULL << index);
        }

        while (whiteQueenBB) {
            uint64_t index = bitscanForward(whiteQueenBB);

            if (!bitboard.isPinned(index, PieceColor::WHITE)) {
                whiteQueenAttacks |= bitboard.getQueenAttacks(index, occupied);
            }

            whiteQueenBB &= ~(1ULL << index);
        }

        while (blackBishopBB) {
            uint64_t index = bitscanForward(blackBishopBB);

            if (!bitboard.isPinned(index, PieceColor::BLACK)) {
                blackBishopAttacks |= bitboard.getBishopAttacks(index, occupied);
            }

            blackBishopBB &= ~(1ULL << index);
        }

        while (blackRookBB) {
            uint64_t index = bitscanForward(blackRookBB);

            if (!bitboard.isPinned(index, PieceColor::BLACK)) {
                blackRookAttacks |= bitboard.getRookAttacks(index, occupied);
            }

            blackRookBB &= ~(1ULL << index);
        }

        while (blackQueenBB) {
            uint64_t index = bitscanForward(blackQueenBB);

            if (!bitboard.isPinned(index, PieceColor::BLACK)) {
                blackQueenAttacks |= bitboard.getQueenAttacks(index, occupied);
            }

            blackQueenBB &= ~(1ULL << index);
        }
        
        uint64_t whiteCombinedAttacks = whiteKnightAttacks | whiteBishopAttacks | whiteRookAttacks | whiteQueenAttacks | whitePawnAttacks;
        uint64_t blackCombinedAttacks = blackKnightAttacks | blackBishopAttacks | blackRookAttacks | blackQueenAttacks | blackPawnAttacks;
        
        return EvalContext {
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
}

#pragma clang diagnostic pop