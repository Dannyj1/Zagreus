//
// Created by Danny on 26-4-2022.
//

#include <iostream>
#include <chrono>

#include "search_mgr.h"
#include "time_mgr.h"
#include "tt.h"
#include "move_gen.h"
#include "senjo/Output.h"

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

        while (std::chrono::high_resolution_clock::now() < endTime) {
            depth += 1;
            searchStats.depth = depth;
            searchStats.seldepth = 0;

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

            for (const Move &move : moves) {
                assert(move.fromSquare != move.toSquare);

                if (depth > 1 && std::chrono::high_resolution_clock::now() > endTime) {
                    break;
                }

                board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

                if (depth == 1 && board.getWhiteTimeMsec() > 30000) {
                    endTime += std::chrono::minutes(2);
                }

                SearchResult result = search(board, depth, 0, -99999999, 99999999, move, move, endTime);

                if (depth == 1 && board.getWhiteTimeMsec() > 30000) {
                    endTime -= std::chrono::minutes(2);
                }

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
        searchStats.msecs = duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
        tt.clearKillerMoves();
        tt.clearPVMoves();
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
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

        searchStats.nodes += 1;

        bool searchPv = true;
        std::vector<Move> moves = generateLegalMoves(board, board.getMovingColor());

        for (const Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

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

        searchStats.nodes += 1;

        std::vector<Move> moves = generateLegalMoves(board, board.getMovingColor());
        for (const Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

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

        std::vector<Move> moves = generateMobilityMoves(board, board.getMovingColor());
        int whiteMobilityScore = moves.size() * 3;

        moves = generateMobilityMoves(board, Bitboard::getOppositeColor(board.getMovingColor()));
        int blackMobilityScore = moves.size() * 3;

        whiteScore += whiteMobilityScore;
        blackScore += blackMobilityScore;

        whiteScore += getWhiteConnectivityScore(board);
        blackScore += getBlackConnectivityScore(board);

        whiteScore += getWhiteKingSafetyScore(board);
        blackScore += getBlackKingSafetyScore(board);

        return (whiteScore - blackScore) * modifier;
    }

    int SearchManager::getBlackMaterialScore(Bitboard &board) const {
        int blackPawnsScore = popcnt(board.getPieceBoard(BLACK_PAWN)) * 100;
        int blackKnightsScore = popcnt(board.getPieceBoard(BLACK_KNIGHT)) * 350;
        int blackBishopsScore = popcnt(board.getPieceBoard(BLACK_BISHOP)) * 350;
        int blackRooksScore = popcnt(board.getPieceBoard(BLACK_ROOK)) * 525;
        int blackQueensScore = popcnt(board.getPieceBoard(BLACK_QUEEN)) * 1000;
        int blackMaterialScore =
                blackPawnsScore + blackKnightsScore + blackBishopsScore + blackRooksScore + blackQueensScore;
        return blackMaterialScore;
    }

    int SearchManager::getWhiteMaterialScore(Bitboard &board) const {
        int whitePawnsScore = popcnt(board.getPieceBoard(WHITE_PAWN)) * 100;
        int whiteKnightsScore = popcnt(board.getPieceBoard(WHITE_KNIGHT)) * 350;
        int whiteBishopsScore = popcnt(board.getPieceBoard(WHITE_BISHOP)) * 350;
        int whiteRooksScore = popcnt(board.getPieceBoard(WHITE_ROOK)) * 525;
        int whiteQueensScore = popcnt(board.getPieceBoard(WHITE_QUEEN)) * 1000;
        int whiteMaterialScore =
                whitePawnsScore + whiteKnightsScore + whiteBishopsScore + whiteRooksScore + whiteQueensScore;
        return whiteMaterialScore;
    }

    SearchResult SearchManager::quiesce(Bitboard &board, int ply, int depth, int alpha, int beta, const Move &rootMove,
                                        const Move &previousMove,
                                        std::chrono::time_point<std::chrono::high_resolution_clock> &endTime) {
        searchStats.qnodes += 1;

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

        std::vector<Move> moves = generateQuiescenceMoves(board, board.getMovingColor());
        for (const Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

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
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t whitePieces = bitboard.getWhiteBoard() & ~kingBB;
        int whiteConnectivityScore = 0;

        while (whitePieces) {
            uint64_t index = bitscanForward(whitePieces);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, PieceColor::WHITE) & ~kingBB;

            whiteConnectivityScore += popcnt(whitePieces & attacks);
            whitePieces &= ~(1ULL << index);
        }

        return whiteConnectivityScore * 2;
    }

    int SearchManager::getBlackConnectivityScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::BLACK_KING);
        uint64_t blackPieces = bitboard.getBlackBoard() & ~kingBB;
        int blackConnectivityScore = 0;

        while (blackPieces) {
            uint64_t index = bitscanForward(blackPieces);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, PieceColor::BLACK) & ~kingBB;

            blackConnectivityScore += popcnt(blackPieces & attacks);
            blackPieces &= ~(1ULL << index);
        }

        return blackConnectivityScore * 2;
    }

    int SearchManager::getWhiteKingSafetyScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        int whiteKingSafetyScore = std::min(30, (int) (popcnt(pawnShield) * 10));

        if (kingLocation <= 15) {
            whiteKingSafetyScore += 10;
        }

        return whiteKingSafetyScore;
    }

    int SearchManager::getBlackKingSafetyScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::BLACK_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        int blackKingSafetyScore = std::min(30, (int) (popcnt(pawnShield) * 10));

        if (kingLocation >= 48) {
            blackKingSafetyScore += 10;
        }

        return blackKingSafetyScore;
    }

    void SearchManager::resetStats() {
        searchStats = {};
    }
}
