//
// Created by Danny on 26-4-2022.
//

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
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor()))
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
            || board.isWinner(Bitboard::getOppositeColor(board.getMovingColor()))
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

        int whiteScore = 0;
        int blackScore = 0;
        int modifier = board.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board.isWinner(board.getMovingColor())) {
            return 10000 - board.getPly();
        } else if (board.isWinner(Bitboard::getOppositeColor(board.getMovingColor()))) {
            return -10000 + board.getPly();
        }

        if (board.isDraw()) {
            return 0;
        }

        int whiteMaterialScore = getWhiteMaterialScore(board);
        int blackMaterialScore = getBlackMaterialScore(board);

        whiteScore += whiteMaterialScore;
        blackScore += blackMaterialScore;

        whiteScore += getMobilityScore(board, PieceColor::WHITE);
        blackScore += getMobilityScore(board, PieceColor::BLACK);

        whiteScore += getWhiteConnectivityScore(board);
        blackScore += getBlackConnectivityScore(board);

        whiteScore += getWhiteKingScore(board);
        blackScore += getBlackKingScore(board);

        whiteScore += getPawnScore(board, PieceColor::WHITE);
        blackScore += getPawnScore(board, PieceColor::BLACK);

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

        if (bitboard.getPly() < 40 && bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::WHITE)) {
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

        if (bitboard.getPly() < 40 && kingLocation <= 15) {
            blackKingSafetyScore += 10;
        }

        if (bitboard.getPly() < 40 && bitboard.isSemiOpenFileLenient(kingLocation, PieceColor::BLACK)) {
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

        return score;
    }

    int SearchManager::getBlackBishopScore(Bitboard &bitboard) {
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
            centerPattern = _blsr_u64(centerPattern);
        }

        while (extendedCenterPattern) {
            uint64_t index = bitscanForward(extendedCenterPattern);
            uint64_t attacks = bitboard.getSquareAttacksByColor(index, color);

            score += popcnt(attacks) * 4;
            extendedCenterPattern = _blsr_u64(extendedCenterPattern);
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
        uint64_t minorPiecesOnStart =
                (bitboard.getPieceBoard(PieceType::WHITE_KNIGHT) | bitboard.getPieceBoard(PieceType::WHITE_BISHOP)) &
                whiteMinorPiecesStartBB;
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
        if (bitboard.getPly() > 12) {
            return 0;
        }

        int score = 0;
        uint64_t minorPiecesOnStart =
                (bitboard.getPieceBoard(PieceType::BLACK_KNIGHT) | bitboard.getPieceBoard(PieceType::BLACK_BISHOP)) &
                blackMinorPiecesStartBB;
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
/*        uint64_t occupiedBB = bitboard.getOccupiedBoard();
        uint64_t ownPieces = bitboard.getBoardByColor(color);
        int score = 0;

        while (ownPieces) {
            uint64_t index = bitscanForward(ownPieces);

            if (bitboard.isPinned(index, color)) {
                score += bitboard.getPieceWeight(bitboard.getPieceOnSquare(index)) / 100;
            }

            ownPieces = _blsr_u64(ownPieces);
        }*/

        int score = 0;
        uint64_t occupiedBB = bitboard.getOccupiedBoard();
        while (occupiedBB) {
            uint64_t index = bitscanForward(occupiedBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            score += getPstValue(pieceOnSquare, index);
            occupiedBB = _blsr_u64(occupiedBB);
        }

        return score;
    }

    int SearchManager::getMobilityScore(Bitboard &bitboard, PieceColor color) {
        int score = 0;
        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPawnBB = bitboard.getPieceBoard(PieceType::WHITE_PAWN + Bitboard::getOppositeColor(color));
        uint64_t opponentPawnAttacks = bitboard.calculatePawnAttacks(opponentPawnBB, Bitboard::getOppositeColor(color));
        uint64_t knightBB = bitboard.getPieceBoard(PieceType::WHITE_KNIGHT + color);
        uint64_t bishopBB = bitboard.getPieceBoard(PieceType::WHITE_BISHOP + color);
        uint64_t rookBB = bitboard.getPieceBoard(PieceType::WHITE_ROOK + color);
        uint64_t queenBB = bitboard.getPieceBoard(PieceType::WHITE_QUEEN + color);

        uint64_t knightAttacks = calculateKnightAttacks(knightBB) & ~ownPiecesBB;
        uint64_t bishopAttacks = 0;
        uint64_t rookAttacks = 0;
        uint64_t queenAttacks = 0;

        uint64_t occupied = bitboard.getOccupiedBoard();

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (!bitboard.isPinned(index, color)) {
                bishopAttacks |= bitboard.getBishopAttacks(index, occupied) & ~ownPiecesBB;
            }

            bishopBB &= ~(1ULL << index);
        }

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (!bitboard.isPinned(index, color)) {
                rookAttacks |= bitboard.getRookAttacks(index, occupied) & ~ownPiecesBB;
            }

            rookBB &= ~(1ULL << index);
        }

        while (queenBB) {
            uint64_t index = bitscanForward(queenBB);

            if (!bitboard.isPinned(index, color)) {
                queenAttacks |= bitboard.getQueenAttacks(index, occupied) & ~ownPiecesBB;
            }

            queenBB &= ~(1ULL << index);
        }

        knightAttacks &= ~opponentPawnAttacks;
        bishopAttacks &= ~opponentPawnAttacks;
        rookAttacks &= ~opponentPawnAttacks;
        queenAttacks &= ~opponentPawnAttacks;

        if (bitboard.getFullmoveClock() > 12) {
            score += popcnt(knightAttacks) * 2;
            score += popcnt(bishopAttacks) * 2;
            score += popcnt(rookAttacks) * 5;
            score += popcnt(queenAttacks) * 7;
        } else {
            score += popcnt(knightAttacks) * 5;
            score += popcnt(bishopAttacks) * 5;
            score += popcnt(rookAttacks);
            score += popcnt(queenAttacks) * 2;
        }

        return score;
    }

    int SearchManager::getPawnScore(Bitboard &bitboard, PieceColor color) {
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

        return score;
    }
}

#pragma clang diagnostic pop