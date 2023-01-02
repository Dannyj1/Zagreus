//
// Created by Danny on 26-4-2022.
//

#include <iostream>
#include <chrono>

#include "search_mgr.h"
#include "time_mgr.h"
#include "tt.h"
#include "move_gen.h"

namespace Zagreus {
    SearchResult SearchManager::getBestMove(Bitboard &board, PieceColor color) {
        isSearching = true;
        SearchResult bestResult;
        SearchResult iterationResult;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime = timeManager.getEndTime(board, color);
        MoveList moves;

        generateLegalMoves(moves, board, color);
        int depth = 0;

        while (std::chrono::high_resolution_clock::now() < endTime) {
            depth += 1;

            if (depth > 50) {
                break;
            }

            if (depth > 1 && std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime)) {
                break;
            }

            /*if (std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * std::min(0.2, 1.05 - (depth * 0.05))) {
                break;
            }*/

            std::cout << "Searching depth " << depth << "..." << std::endl;

            if (moves.size() == 1) {
                return {moves[0], 0};
            }

            for (int i = 0; i < moves.size(); i++) {
                Move move = moves[i];

                if (depth > 1 && std::chrono::high_resolution_clock::now() > endTime) {
                    break;
                }

                board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

                if (depth == 1) {
                    endTime += std::chrono::minutes(2);
                }

                SearchResult result = search(board, depth, 0, -99999999, 99999999, move, move, endTime);

                if (depth == 1) {
                    endTime -= std::chrono::minutes(2);
                }

                result.score *= -1;
                board.unmakeMove();

                if (depth == 1 || result.score > iterationResult.score ||
                    iterationResult.move.pieceType == PieceType::EMPTY) {
                    iterationResult = result;
                }
            }

            if (depth == 1 || std::chrono::high_resolution_clock::now() < endTime ||
                bestResult.move.pieceType == PieceType::EMPTY) {
                bestResult = iterationResult;
            }

            iterationResult = {};
        }

        tt.clearKillerMoves();
        tt.clearPVMoves();
        isSearching = false;
        return bestResult;
    }

    SearchResult SearchManager::search(Bitboard &board, int depth, int ply, int alpha, int beta, const Move &rootMove,
                                       const Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() > endTime
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) ||
            board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            return quiesce(board, ply, 10, alpha, beta, rootMove, previousMove, endTime);
        }

        bool searchPv = true;
        MoveList moves;
        generateLegalMoves(moves, board, board.getMovingColor());

        for (int i = 0; i < moves.size(); i++) {
            Move move = moves[i];

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            SearchResult result;
            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (tt.isPositionInTable(board.getZobristHash(), depth)) {
                result = {rootMove, tt.getPositionScore(board.getZobristHash())};
            } else {
                if (searchPv) {
                    result = search(board, depth - 1, ply + 1, -beta, -alpha, rootMove, move, endTime);
                    result.score *= -1;
                    tt.addPosition(board.getZobristHash(), depth, result.score);
                } else {
                    result = zwSearch(board, depth - 1, ply + 1, -alpha, rootMove, move, endTime);
                    result.score *= -1;

                    if (result.score > alpha) {
                        result = search(board, depth - 1, ply + 1, -beta, -alpha, rootMove, move, endTime);
                        result.score *= -1;
                        tt.addPosition(board.getZobristHash(), depth, result.score);
                    }
                }
            }

            board.unmakeMove();

            if (result.score >= beta) {
                tt.addKillerMove(move);
                return {rootMove, beta};
            }

            if (result.score > alpha) {
                if (searchPv) {
                    tt.addPVMove(move);
                }

                alpha = result.score;
                searchPv = false;
            }
        }

        return {rootMove, alpha};
    }

    SearchResult
    SearchManager::zwSearch(Bitboard &board, int depth, int ply, int beta, const Move &rootMove,
                            const Move &previousMove,
                            std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() > endTime
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) ||
            board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            return quiesce(board, ply, 10, beta - 1, beta, rootMove, previousMove, endTime);
        }

        MoveList moves;
        generateLegalMoves(moves, board, board.getMovingColor());
        for (int i = 0; i < moves.size(); i++) {
            Move move = moves[i];

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            SearchResult result = zwSearch(board, depth - 1, ply + 1, 1 - beta, rootMove, move, endTime);
            result.score *= -1;
            board.unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }
        }

        return {rootMove, beta - 1};
    }

    int SearchManager::evaluate(Bitboard &board, int ply,
                                std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (std::chrono::high_resolution_clock::now() > endTime) {
            return 0;
        }

        int whiteScore = 0;
        int blackScore = 0;
        int modifier = board.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board.isWinner(board.getMovingColor())) {
            return 10000 - ply;
        } else if (board.isWinner(Bitboard::getOppositeColor(board.getMovingColor()))) {
            return -10000 + ply;
        }

        if (board.isDraw()) {
            return 0;
        }

        int whiteMaterialScore = getWhiteMaterialScore(board);
        int blackMaterialScore = getBlackMaterialScore(board);

        whiteScore += whiteMaterialScore;
        blackScore += blackMaterialScore;

        MoveList moves;

        generateMobilityMoves(moves, board, board.getMovingColor());
        int whiteMobilityScore = moves.size() * 2;

        moves.clear();

        generateMobilityMoves(moves, board, Bitboard::getOppositeColor(board.getMovingColor()));
        int blackMobilityScore = moves.size() * 2;

        whiteScore += whiteMobilityScore;
        blackScore += blackMobilityScore;

        whiteScore += getWhiteConnectivityScore(board);
        blackScore += getBlackConnectivityScore(board);

        return (whiteScore - blackScore) * modifier;
    }

    int SearchManager::getBlackMaterialScore(Bitboard &board) const {
        int blackPawnsScore = popcnt(board.getPieceBoard(BLACK_PAWN)) * 100;
        int blackKnightsScore = popcnt(board.getPieceBoard(BLACK_KNIGHT)) * 350;
        int blackBishopsScore = popcnt(board.getPieceBoard(BLACK_BISHOP)) * 350;
        int blackRooksScore = popcnt(board.getPieceBoard(BLACK_ROOK)) * 525;
        int blackQueensScore = popcnt(board.getPieceBoard(BLACK_QUEEN)) * 1000;
        int blackMaterialScore = blackPawnsScore + blackKnightsScore + blackBishopsScore + blackRooksScore + blackQueensScore;
        return blackMaterialScore;
    }

    int SearchManager::getWhiteMaterialScore(Bitboard &board) const {
        int whitePawnsScore = popcnt(board.getPieceBoard(WHITE_PAWN)) * 100;
        int whiteKnightsScore = popcnt(board.getPieceBoard(WHITE_KNIGHT)) * 350;
        int whiteBishopsScore = popcnt(board.getPieceBoard(WHITE_BISHOP)) * 350;
        int whiteRooksScore = popcnt(board.getPieceBoard(WHITE_ROOK)) * 525;
        int whiteQueensScore = popcnt(board.getPieceBoard(WHITE_QUEEN)) * 1000;
        int whiteMaterialScore = whitePawnsScore + whiteKnightsScore + whiteBishopsScore + whiteRooksScore + whiteQueensScore;
        return whiteMaterialScore;
    }

    SearchResult SearchManager::quiesce(Bitboard &board, int ply, int depth, int alpha, int beta, const Move &rootMove,
                                        const Move &previousMove,
                                        std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        if (std::chrono::high_resolution_clock::now() > endTime) {
            return {rootMove, beta};
        }

        int standPat = evaluate(board, ply, endTime);

        if (standPat >= beta) {
            return {rootMove, beta};
        }

        int delta = 1000;

        if (previousMove.promotionPiece != PieceType::EMPTY) {
            delta += 900;
        }

        if (standPat < alpha - delta) {
            return {rootMove, alpha};
        }

        if (alpha < standPat) {
            alpha = standPat;
        }

        MoveList moves;
        generateQuiescenceMoves(moves, board, board.getMovingColor());
        for (int i = 0; i < moves.size(); i++) {
            Move move = moves[i];

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            SearchResult result = quiesce(board, ply + 1, depth - 1, -beta, -alpha, rootMove, move, endTime);
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
        return senjo::SearchStats();
    }

    int SearchManager::getWhiteConnectivityScore(Bitboard &bitboard) {
        uint64_t whitePieces = bitboard.getWhiteBoard();
        int whiteConnectivityScore = 0;

        while (whitePieces) {
            uint64_t index = bitscanForward(whitePieces);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, PieceColor::WHITE);

            whiteConnectivityScore += popcnt(attacks);
            whitePieces &= ~(1ULL << index);
        }

        return whiteConnectivityScore;
    }

    int SearchManager::getBlackConnectivityScore(Bitboard &bitboard) {
        uint64_t whitePieces = bitboard.getWhiteBoard();
        int whiteConnectivityScore = 0;

        while (whitePieces) {
            uint64_t index = bitscanForward(whitePieces);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, PieceColor::BLACK);

            whiteConnectivityScore += popcnt(attacks);
            whitePieces &= ~(1ULL << index);
        }

        return whiteConnectivityScore;
    }
}

#pragma clang diagnostic pop
