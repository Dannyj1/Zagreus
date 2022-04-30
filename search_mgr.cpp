#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by Danny on 26-4-2022.
//

#include <iostream>
#include <chrono>
#include "search_mgr.h"
#include "time_mgr.h"

namespace Chess {
    SearchResult SearchManager::getBestMove(Board* board) {
        isSearching = true;
        SearchResult bestResult;
        SearchResult iterationResult;
        std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> endTime = timeManager.getEndTime(board, board->getMovingColor());
        std::vector<Move> legalMoves = board->getLegalMoves(board->getMovingColor(), true);
        int depth = 0;

        while (std::chrono::system_clock::now() < endTime) {
            depth += 1;

            if (std::chrono::system_clock::now() - startTime > (endTime - startTime) * 0.5) {
                break;
            }

            std::cout << "Searching depth " << depth << "..." << std::endl;

            if (legalMoves.size() == 1) {
                return {legalMoves[0], 0};
            }

            for (const Move &move : legalMoves) {
                Tile* toTile = move.tile;
                board->makeMove(toTile->getX(), toTile->getY(), move.piece);

                if (board->isKingChecked(move.piece->getColor())) {
                    board->unmakeMove();
                    continue;
                }

                SearchResult result = search(board, depth, -99999999, 99999999, move, endTime);
                result.score *= -1;
                board->unmakeMove();

                if (result.score > iterationResult.score) {
                    iterationResult = result;
                }

                if (std::chrono::system_clock::now() > endTime) {
                    break;
                }
            }

            if (std::chrono::system_clock::now() < endTime) {
                bestResult = iterationResult;
            }

            iterationResult = {};
        }

        isSearching = false;
        return bestResult;
    }

    SearchResult SearchManager::search(Board* board, int depth, int alpha, int beta, const Move &rootMove, std::chrono::time_point<std::chrono::system_clock> endTime) {
        if (depth == 0 || std::chrono::system_clock::now() > endTime) {
            return {rootMove, evaluate(board)};
        }

        bool searchPv = true;
        std::vector<Move> legalMoves = board->getLegalMoves(board->getMovingColor(), true);

        for (const Move &move : legalMoves) {
            if (std::chrono::system_clock::now() > endTime){
                break;
            }

            SearchResult result;
            Tile* toTile = move.tile;
            board->makeMove(toTile->getX(), toTile->getY(), move.piece);

            if (board->isKingChecked(move.piece->getColor())) {
                board->unmakeMove();
                continue;
            }

            if (searchPv) {
                result = search(board, depth - 1, -beta, -alpha, rootMove, endTime);
                result.score *= -1;
            } else {
                result = zwSearch(board, depth - 1, -alpha, rootMove, endTime);
                result.score *= -1;

                if (result.score > alpha) {
                    result = search(board, depth - 1, -beta, -alpha, rootMove, endTime);
                    result.score *= -1;
                }
            }

            board->unmakeMove();

            if (result.score >= beta) {
                return {rootMove, beta};
            }

            if (result.score > alpha) {
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

        std::vector<Move> legalMoves = board->getLegalMoves(board->getMovingColor(), true);
        for (const Move &move : legalMoves) {
            if (std::chrono::system_clock::now() > endTime){
                break;
            }

            Tile* toTile = move.tile;
            board->makeMove(toTile->getX(), toTile->getY(), move.piece);

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
        int whiteMaterialScore = 0;
        int blackMaterialScore = 0;
        int whiteMobilityScore = 0;
        int blackMobilityScore = 0;
        int modifier = board->getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board->isDraw()) {
            return 0;
        }

        PieceColor winner = board->getWinner();
        if (winner != PieceColor::NONE) {
            if (winner == board->getMovingColor()) {
                return 10000;
            } else {
                return -10000;
            }
        }

        for (const std::shared_ptr<Piece> &piece : board->getPieces()) {
            if (piece->getColor() == PieceColor::WHITE) {
                whiteMaterialScore += piece->getWeight(board);
                whiteMobilityScore += piece->getMobilityScore(board);
            } else {
                blackMaterialScore += piece->getWeight(board);
                blackMobilityScore += piece->getMobilityScore(board);
            }
        }

        int whiteScore = whiteMaterialScore + whiteMobilityScore;
        int blackScore = blackMaterialScore + blackMobilityScore;
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