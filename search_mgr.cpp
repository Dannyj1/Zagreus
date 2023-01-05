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

        while (std::chrono::high_resolution_clock::now() - startTime < (endTime - startTime) * 0.75) {
            depth += 1;
            searchStats.depth = depth;
            searchStats.seldepth = 0;

            if (depth > 50) {
                break;
            }

            senjo::Output(senjo::Output::InfoPrefix) << "Searching depth " << depth << "...";

            if (moves.size() == 1) {
                return {moves[0], 0};
            }

            for (const Move &move : moves) {
                assert(move.fromSquare != move.toSquare);

                if (depth > 1 && std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * 2.0) {
                    break;
                }

                board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);
                SearchResult result = search(board, depth, 0, -99999999, 99999999, move, move, endTime, startTime);
                result.score *= -1;
                board.unmakeMove();

                if (result.score > iterationResult.score) {
                    assert(result.move.pieceType != PieceType::EMPTY);
                    iterationResult = result;
                    searchStats.score = iterationResult.score;
                }

                searchStats.msecs = duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
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
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
        return bestResult;
    }

    SearchResult SearchManager::search(Bitboard &board, int depth, int ply, int alpha, int beta, const Move &rootMove,
                                       const Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * 2.0
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) ||
            board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            return quiesce(board, ply, 7, alpha, beta, rootMove, previousMove, endTime, startTime);
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
            TTEntry entry = tt.getPosition(board.getZobristHash());

            if (entry.zobristHash == board.getZobristHash() && entry.depth >= depth) {
                result = {rootMove, entry.score};
            } else {
                if (searchPv) {
                    result = search(board, depth - 1, ply + 1, -beta, -alpha, rootMove, move, endTime, startTime);
                    result.score *= -1;
                    tt.addPosition(board.getZobristHash(), depth, result.score);
                } else {
                    result = zwSearch(board, depth - 1, ply + 1, -alpha, rootMove, move, endTime, startTime);
                    result.score *= -1;

                    if (result.score > alpha) {
                        result = search(board, depth - 1, ply + 1, -beta, -alpha, rootMove, move, endTime, startTime);
                        result.score *= -1;
                        tt.addPosition(board.getZobristHash(), depth, result.score);
                    }
                }
            }

            board.unmakeMove();

            if (result.score >= beta) {
                tt.addKillerMove(board.getZobristHash(), depth, beta);
                return {rootMove, beta};
            }

            if (result.score > alpha) {
                if (searchPv) {
                    tt.addPVMove(board.getZobristHash(), depth, result.score);
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
                            std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime) {
        if (depth == 0 || std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * 2.0
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor())) ||
            board.isWinner(board.getMovingColor())
            || board.isDraw()) {
            return quiesce(board, ply, 7, beta - 1, beta, rootMove, previousMove, endTime, startTime);
        }

        searchStats.nodes += 1;

        std::vector<Move> moves = generateLegalMoves(board, board.getMovingColor());
        for (const Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            SearchResult result = zwSearch(board, depth - 1, ply + 1, 1 - beta, rootMove, move, endTime, startTime);
            result.score *= -1;
            board.unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }
        }

        return {rootMove, beta - 1};
    }

    SearchResult SearchManager::quiesce(Bitboard &board, int ply, int depth, int alpha, int beta, const Move &rootMove,
                                        const Move &previousMove,
                                        std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime) {
        searchStats.qnodes += 1;

        if (std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * 2.0) {
            return {rootMove, beta};
        }

        int standPat = evaluate(board, ply, endTime, startTime);

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

        // TODO: consider not ending search if over endtime.
        std::vector<Move> moves = generateQuiescenceMoves(board, board.getMovingColor());
        for (const Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            if (std::chrono::high_resolution_clock::now() > endTime) {
                break;
            }

            if (move.captureScore < 0) {
                continue;
            }


            board.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            SearchResult result = quiesce(board, ply + 1, depth - 1, -beta, -alpha, rootMove, move, endTime, startTime);
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

    // TODO: create bitboards with default positions to calcualte "tempo"
    int SearchManager::evaluate(Bitboard &board, int ply,
                                std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime) {
        if (std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * 2.0) {
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

        uint64_t whiteMobilityMoves = generateMobilityMoves(board, board.getMovingColor()) & board.getEmptyBoard();
        uint64_t blackMobilityMoves = generateMobilityMoves(board, Bitboard::getOppositeColor(board.getMovingColor())) & board.getEmptyBoard();

        int whiteMobilityScore = popcnt(whiteMobilityMoves & ~blackMobilityMoves) * 10;
        int blackMobilityScore = popcnt(blackMobilityMoves & ~whiteMobilityMoves) * 10;

        whiteScore += whiteMobilityScore;
        blackScore += blackMobilityScore;

        whiteScore += getWhiteConnectivityScore(board);
        blackScore += getBlackConnectivityScore(board);

        whiteScore += getWhiteKingScore(board);
        blackScore += getBlackKingScore(board);

        whiteScore += getWhiteBishopScore(board);
        blackScore += getBlackBishopScore(board);

        whiteScore += getWhiteRookScore(board);
        blackScore += getBlackRookScore(board);

        whiteScore += getCenterScore(board, PieceColor::WHITE);
        blackScore += getCenterScore(board, PieceColor::BLACK);

        whiteScore += getWhiteDevelopmentScore(board);
        blackScore += getBlackDevelopmentScore(board);

        whiteScore += getPositionalScore(board, PieceColor::WHITE);
        blackScore += getPositionalScore(board, PieceColor::BLACK);

        return (whiteScore - blackScore) * modifier;
    }

    int SearchManager::getBlackMaterialScore(Bitboard &board) const {
        int blackPawnsScore = popcnt(board.getPieceBoard(BLACK_PAWN)) * board.getPieceWeight(BLACK_PAWN);
        int blackKnightsScore = popcnt(board.getPieceBoard(BLACK_KNIGHT)) * board.getPieceWeight(BLACK_KNIGHT);
        int blackBishopsScore = popcnt(board.getPieceBoard(BLACK_BISHOP)) * board.getPieceWeight(BLACK_BISHOP);
        int blackRooksScore = popcnt(board.getPieceBoard(BLACK_ROOK)) * board.getPieceWeight(BLACK_ROOK);
        int blackQueensScore = popcnt(board.getPieceBoard(BLACK_QUEEN)) * board.getPieceWeight(BLACK_QUEEN);
        int blackMaterialScore =
                blackPawnsScore + blackKnightsScore + blackBishopsScore + blackRooksScore + blackQueensScore;
        return blackMaterialScore;
    }

    int SearchManager::getWhiteMaterialScore(Bitboard &board) const {
        int whitePawnsScore = popcnt(board.getPieceBoard(WHITE_PAWN)) * board.getPieceWeight(WHITE_PAWN);
        int whiteKnightsScore = popcnt(board.getPieceBoard(WHITE_KNIGHT)) * board.getPieceWeight(WHITE_KNIGHT);
        int whiteBishopsScore = popcnt(board.getPieceBoard(WHITE_BISHOP)) * board.getPieceWeight(WHITE_BISHOP);
        int whiteRooksScore = popcnt(board.getPieceBoard(WHITE_ROOK)) * board.getPieceWeight(WHITE_ROOK);
        int whiteQueensScore = popcnt(board.getPieceBoard(WHITE_QUEEN)) * board.getPieceWeight(WHITE_QUEEN);
        int whiteMaterialScore =
                whitePawnsScore + whiteKnightsScore + whiteBishopsScore + whiteRooksScore + whiteQueensScore;
        return whiteMaterialScore;
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

        return whiteConnectivityScore * 4;
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

        return blackConnectivityScore * 4;
    }

    int SearchManager::getWhiteKingScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::WHITE_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN);
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        int whiteKingSafetyScore = std::min(60, (int) (popcnt(pawnShield) * 20));

        if (kingLocation <= 15) {
            whiteKingSafetyScore += 10;
        }

        if (bitboard.getMovesMade() < 40 && bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::WHITE)) {
            whiteKingSafetyScore -= 50;
        }

        while (kingAttacks) {
            uint64_t index = bitscanForward(kingAttacks);
            uint64_t opponentAttacks = bitboard.getSquareAttacksByColor(index, PieceColor::BLACK);

            while (opponentAttacks) {
                uint64_t opponentIndex = bitscanForward(opponentAttacks);
                PieceType attackerType = bitboard.getPieceOnSquare(opponentIndex);
                int pieceWeight = bitboard.getPieceWeight(attackerType);
                int amountOfTilesBetween = popcnt(bitboard.getTilesBetween(kingLocation, opponentIndex));

                whiteKingSafetyScore -= (10 + (pieceWeight / 100)) - amountOfTilesBetween;
                opponentAttacks &= ~(1ULL << opponentIndex);
            }

            kingAttacks &= ~(1ULL << index);
        }

        return whiteKingSafetyScore;
    }

    // TODO: add proper game phase stuff
    int SearchManager::getBlackKingScore(Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard(PieceType::BLACK_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard(PieceType::BLACK_PAWN);
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= soutOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        int blackKingSafetyScore = std::min(60, (int) (popcnt(pawnShield) * 20));

        if (bitboard.getMovesMade() < 40 && kingLocation <= 15) {
            blackKingSafetyScore += 10;
        }

        if (bitboard.getMovesMade() < 40 && bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::BLACK)) {
            blackKingSafetyScore -= 50;
        }

        while (kingAttacks) {
            uint64_t index = bitscanForward(kingAttacks);
            uint64_t opponentAttacks = bitboard.getSquareAttacksByColor(index, PieceColor::WHITE);

            while (opponentAttacks) {
                uint64_t opponentIndex = bitscanForward(opponentAttacks);
                PieceType attackerType = bitboard.getPieceOnSquare(opponentIndex);
                int pieceWeight = bitboard.getPieceWeight(attackerType);
                int amountOfTilesBetween = popcnt(bitboard.getTilesBetween(kingLocation, opponentIndex));

                blackKingSafetyScore -= (10 + (pieceWeight / 100)) - amountOfTilesBetween;
                opponentAttacks &= ~(1ULL << opponentIndex);
            }

            kingAttacks &= ~(1ULL << index);
        }

        return blackKingSafetyScore;
    }

    void SearchManager::resetStats() {
        searchStats = {};
    }

    int SearchManager::getWhiteBishopScore(Bitboard &bitboard) {
        int score = 0;
        int bishopAmount = popcnt(bitboard.getPieceBoard(PieceType::WHITE_BISHOP));
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::WHITE_BISHOP);
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

        return score;
    }

    int SearchManager::getBlackBishopScore(Bitboard &bitboard) {
        int score = 0;
        int bishopAmount = popcnt(bitboard.getPieceBoard(PieceType::BLACK_BISHOP));
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::BLACK_BISHOP);
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

        return score;
    }

    int SearchManager::getCenterScore(Bitboard &bitboard, PieceColor color) {
        int score = 0;
        uint64_t centerPattern = 0x0000001818000000;
        uint64_t extendedCenterPattern = 0x00003C3C3C3C0000 & ~centerPattern;

        while (centerPattern) {
            uint64_t index = bitscanForward(centerPattern);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, color);

            if (pieceOnSquare != PieceType::EMPTY && bitboard.getPieceColor(pieceOnSquare) == color) {
                score += 15;
            }

            score += popcnt(attacks) * 7;
            centerPattern &= ~(1ULL << index);
        }

        while (extendedCenterPattern) {
            uint64_t index = bitscanForward(extendedCenterPattern);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, color);

            score += popcnt(attacks) * 4;
            extendedCenterPattern &= ~(1ULL << index);
        }

        return score;
    }

    uint64_t whiteMinorPiecesStartBB = 0x66ULL;
    uint64_t whiteRookStartBB = 0x81ULL;
    uint64_t whiteQueenStartBB = 0x10ULL;
    int SearchManager::getWhiteDevelopmentScore(Bitboard &bitboard) {
        if (bitboard.getFullmoveClock() > 12) {
            return 0;
        }

        int score = 0;
        uint64_t minorPiecesOnStart = (bitboard.getPieceBoard(PieceType::WHITE_KNIGHT) | bitboard.getPieceBoard(PieceType::WHITE_BISHOP)) & whiteMinorPiecesStartBB;
        uint64_t minorPiecesOnStartAmount = popcnt(minorPiecesOnStart);
        uint64_t rooksOnStart = bitboard.getPieceBoard(PieceType::WHITE_ROOK) & whiteRookStartBB;
        uint64_t rooksOnStartAmount = popcnt(rooksOnStart);
        uint64_t queenOnStart = bitboard.getPieceBoard(PieceType::WHITE_QUEEN) & whiteQueenStartBB;
        uint64_t queenOnStartAmount = popcnt(queenOnStart);

        score += (4 - minorPiecesOnStartAmount) * 30;

        if (minorPiecesOnStartAmount == 0) {
            score += (2 - rooksOnStartAmount) * 30;
            score += (1 - queenOnStartAmount) * 30;
        } else {
            score -= (1 - queenOnStartAmount) * 30;
        }

        return score;
    }

    uint64_t blackMinorPiecesStartBB = 0x6600000000000000ULL;
    uint64_t blackRookStartBB = 0x8100000000000000ULL;
    uint64_t blackQueenStartBB = 0x1000000000000000ULL;
    int SearchManager::getBlackDevelopmentScore(Bitboard &bitboard) {
        if (bitboard.getMovesMade() > 12) {
            return 0;
        }

        int score = 0;
        uint64_t minorPiecesOnStart = (bitboard.getPieceBoard(PieceType::BLACK_KNIGHT) | bitboard.getPieceBoard(PieceType::BLACK_BISHOP)) & blackMinorPiecesStartBB;
        uint64_t minorPiecesOnStartAmount = popcnt(minorPiecesOnStart);
        uint64_t rooksOnStart = bitboard.getPieceBoard(PieceType::BLACK_ROOK) & blackRookStartBB;
        uint64_t rooksOnStartAmount = popcnt(rooksOnStart);
        uint64_t queenOnStart = bitboard.getPieceBoard(PieceType::BLACK_QUEEN) & blackQueenStartBB;
        uint64_t queenOnStartAmount = popcnt(queenOnStart);

        score += (4 - minorPiecesOnStartAmount) * 30;

        if (minorPiecesOnStartAmount == 0) {
            score += (2 - rooksOnStartAmount) * 30;
            score += (1 - queenOnStartAmount) * 30;
        }

        return score;
    }

    int SearchManager::getWhiteRookScore(Bitboard &bitboard) {
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

        return score;
    }

    int SearchManager::getBlackRookScore(Bitboard &bitboard) {
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

        return score;
    }

    int SearchManager::getPositionalScore(Bitboard &bitboard, PieceColor color) {
        uint64_t opponentPieces = bitboard.getBoardByColor(color);
        int score = 0;

        while (opponentPieces) {
            uint64_t index = bitscanForward(opponentPieces);

            if (bitboard.isPinned(index, color)) {
                score -= 10;
            }

            opponentPieces &= ~(1ULL << index);
        }

        return score;
    }
}
