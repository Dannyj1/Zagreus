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

            if (std::chrono::system_clock::now() - startTime > (endTime - startTime) * 0.75) {
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
                std::shared_ptr<Piece> piece = move.piece;
                board->makeMove(toTile->getX(), toTile->getY(), piece);

                if (board->isKingChecked(move.piece->getColor())) {
                    board->unmakeMove();
                    continue;
                }

                SearchResult result = search(board, depth, -99999999, 99999999, move, endTime);
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

    SearchResult SearchManager::search(Board* board, int depth, int alpha, int beta, const Move &rootMove, std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (depth == 0 || std::chrono::system_clock::now() > endTime) {
            return {rootMove, evaluate(board)};
        }

        bool searchPv = true;
        std::vector<Move> legalMoves = board->getPseudoLegalMoves(board->getMovingColor());

        for (const Move &move : legalMoves) {
            if (std::chrono::system_clock::now() > endTime){
                break;
            }

            SearchResult result;
            Tile* toTile = move.tile;
            std::shared_ptr<Piece> piece = move.piece;

            board->makeMove(toTile->getX(), toTile->getY(), piece);

            if (board->isKingChecked(move.piece->getColor())) {
                board->unmakeMove();
                continue;
            }

            if (tt.isPositionInTable(board->getZobristHash(), depth)) {
                result = {rootMove, tt.getPositionScore(board->getZobristHash())};
            } else {
                if (searchPv) {
                    result = search(board, depth - 1, -beta, -alpha, rootMove, endTime);
                    result.score *= -1;
                    tt.addPosition(board->getZobristHash(), depth, result.score);
                } else {
                    result = zwSearch(board, depth - 1, -alpha, rootMove, endTime);
                    result.score *= -1;

                    if (result.score > alpha) {
                        result = search(board, depth - 1, -beta, -alpha, rootMove, endTime);
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
    SearchManager::zwSearch(Board* board, int depth, int beta, const Move &rootMove, std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (depth == 0) {
            return {rootMove, evaluate(board)};
        }

        std::vector<Move> legalMoves = board->getPseudoLegalMoves(board->getMovingColor());
        for (const Move &move : legalMoves) {
            if (std::chrono::system_clock::now() > endTime){
                break;
            }

            Tile* toTile = move.tile;
            std::shared_ptr<Piece> piece = move.piece;
            board->makeMove(toTile->getX(), toTile->getY(), piece);

            if (board->isKingChecked(move.piece->getColor())) {
                board->unmakeMove();
                continue;
            }

            SearchResult result = zwSearch(board, depth - 1, 1 - beta, rootMove, endTime);
            result.score *= -1;
            board->unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }
        }

        return {rootMove, beta - 1};
    }

    int SearchManager::evaluate(Board* board) {
        int whiteScore = 0;
        int blackScore = 0;
        int modifier = board->getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board->isDraw()) {
            return 0;
        }

        PieceColor winner = board->getWinner();
        if (winner != PieceColor::NONE) {
            if (winner == board->getMovingColor()) {
                return 100000;
            } else {
                return -100000;
            }
        }

        for (std::shared_ptr<Piece> piece : board->getPieces()) {
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

    int SearchManager::quiesce(Board* board, int alpha, int beta) {
        return 0;
    }

    bool SearchManager::isCurrentlySearching() {
        return isSearching;
    }
}

#pragma clang diagnostic pop