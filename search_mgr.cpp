/*
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by Danny on 26-4-2022.
//

#include <iostream>
#include <chrono>

#include "search_mgr.h"
#include "time_mgr.h"
#include "tt.h"

namespace Chess {
    SearchResult SearchManager::getBestMove(Board* board, PieceColor color) {
        isSearching = true;
        SearchResult bestResult;
        SearchResult iterationResult;
        std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> endTime = timeManager.getEndTime(board, color);
        std::vector<Move> legalMoves = board->getPseudoLegalMoves(color);
        int depth = 0;

        while (std::chrono::system_clock::now() < endTime) {
            depth += 1;

            if (depth > 30) {
                break;
            }

            if (std::chrono::system_clock::now() - startTime > (endTime - startTime) * std::min(0.2, 0.9 - (depth * 0.1))) {
                break;
            }

            std::cout << "Searching depth " << depth << "..." << std::endl;

            if (legalMoves.size() == 1) {
                return {legalMoves[0], 0};
            }

            for (const Move &move : legalMoves) {
                if (std::chrono::system_clock::now() > endTime) {
                    break;
                }

                Tile* toTile = move.tile;
                Piece* piece = move.piece;
                board->makeMove(toTile->getX(), toTile->getY(), piece, move.promotionPiece);

                if (board->isKingChecked(move.piece->getColor())) {
                    board->unmakeMove();
                    continue;
                }

                SearchResult result = search(board, depth, 0, -99999999, 99999999, move, move, endTime);
                result.score *= -1;
                board->unmakeMove();

                if (result.score > iterationResult.score || iterationResult.move.piece == nullptr) {
                    iterationResult = result;
                }
            }

            if (std::chrono::system_clock::now() < endTime || bestResult.move.piece == nullptr) {
                bestResult = iterationResult;
            }

            iterationResult = {};
        }

        tt.clearKillerMoves();
        tt.clearPVMoves();
        isSearching = false;
        return bestResult;
    }

    SearchResult SearchManager::search(Board* board, int depth, int ply, int alpha, int beta, const Move &rootMove,
                                       const Move &previousMove, std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (depth == 0 || std::chrono::system_clock::now() > endTime || board->getWinner() != PieceColor::NONE || board->isDraw()) {
            return quiesce(board, ply, alpha, beta, rootMove, previousMove, endTime);
        }

        bool searchPv = true;
        std::vector<Move> legalMoves = board->getPseudoLegalMoves(board->getMovingColor());

        for (const Move &move : legalMoves) {
            if (std::chrono::system_clock::now() > endTime) {
                break;
            }

            SearchResult result;
            Tile* toTile = move.tile;
            Piece* piece = move.piece;

            board->makeMove(toTile->getX(), toTile->getY(), piece, move.promotionPiece);

            if (board->isKingChecked(move.piece->getColor())) {
                board->unmakeMove();
                continue;
            }

            if (tt.isPositionInTable(board->getZobristHash(), depth)) {
                result = {rootMove, tt.getPositionScore(board->getZobristHash())};
            } else {
                if (searchPv) {
                    result = search(board, depth - 1, ply + 1, -beta, -alpha, rootMove, move, endTime);
                    result.score *= -1;
                    tt.addPosition(board->getZobristHash(), depth, result.score);
                } else {
                    result = zwSearch(board, depth - 1, ply + 1, -alpha, rootMove, move, endTime);
                    result.score *= -1;

                    if (result.score > alpha) {
                        result = search(board, depth - 1, ply + 1, -beta, -alpha, rootMove, move, endTime);
                        result.score *= -1;
                        tt.addPosition(board->getZobristHash(), depth, result.score);
                    }
                }
            }

            board->unmakeMove();

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
    SearchManager::zwSearch(Board* board, int depth, int ply, int beta, const Move &rootMove, const Move &previousMove,
                            std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (depth == 0 || std::chrono::system_clock::now() > endTime || board->getWinner() != PieceColor::NONE || board->isDraw()) {
            return quiesce(board, ply, beta - 1, beta, rootMove, previousMove, endTime);
        }

        std::vector<Move> legalMoves = board->getPseudoLegalMoves(board->getMovingColor());
        for (const Move &move : legalMoves) {
            if (std::chrono::system_clock::now() > endTime) {
                break;
            }

            Tile* toTile = move.tile;
            Piece* piece = move.piece;
            board->makeMove(toTile->getX(), toTile->getY(), piece, move.promotionPiece);

            if (board->isKingChecked(move.piece->getColor())) {
                board->unmakeMove();
                continue;
            }

            SearchResult result = zwSearch(board, depth - 1, ply + 1, 1 - beta, rootMove, move, endTime);
            result.score *= -1;
            board->unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }
        }

        return {rootMove, beta - 1};
    }

    int SearchManager::evaluate(Board* board, int ply, std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (std::chrono::system_clock::now() > endTime) {
            return 0;
        }

        int whiteScore = 0;
        int blackScore = 0;
        int modifier = board->getMovingColor() == PieceColor::WHITE ? 1 : -1;

        PieceColor winner = board->getWinner();
        if (winner != PieceColor::NONE) {
            if (winner == board->getMovingColor()) {
                return 100000 - ply;
            } else {
                return -100000 + ply;
            }
        }

        if (board->isDraw()) {
            return 0;
        }

        for (Piece* piece : board->getPieces()) {
            if (std::chrono::system_clock::now() > endTime) {
                return 0;
            }

            if (piece->getColor() == PieceColor::WHITE) {
                whiteScore += piece->getWeight(board);
                whiteScore += piece->getMobilityScore(board);
                whiteScore += piece->getTempo(board);
                whiteScore += piece->getEvaluationScore(board);
            } else {
                blackScore += piece->getWeight(board);
                blackScore += piece->getMobilityScore(board);
                blackScore += piece->getTempo(board);
                blackScore += piece->getEvaluationScore(board);
            }
        }

        return (whiteScore - blackScore) * modifier;
    }

    SearchResult SearchManager::quiesce(Board* board, int ply, int alpha, int beta, const Move &rootMove, const Move &previousMove,
                                        std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (std::chrono::system_clock::now() > endTime) {
            return {rootMove, beta};
        }

        int standPat = evaluate(board, ply, endTime);

        if (standPat >= beta) {
            return {rootMove, beta};
        }

        int delta = 1000;

        if (previousMove.promotionPiece) {
            delta += 900;
        }

        if (standPat < alpha - delta) {
            return {rootMove, alpha};
        }

        if (alpha < standPat) {
            alpha = standPat;
        }

        for (Move move : board->getQuiescenceMoves(board->getMovingColor())) {
            if (std::chrono::system_clock::now() > endTime) {
                break;
            }

            Tile* toTile = move.tile;
            Piece* piece = move.piece;

            board->makeMove(toTile->getX(), toTile->getY(), piece, move.promotionPiece);

            if (board->isKingChecked(move.piece->getColor())) {
                board->unmakeMove();
                continue;
            }

            SearchResult result = quiesce(board, ply + 1, -beta, -alpha, rootMove, move, endTime);
            result.score *= -1;
            board->unmakeMove();

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
}

#pragma clang diagnostic pop*/
