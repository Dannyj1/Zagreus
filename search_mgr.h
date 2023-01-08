//
// Created by Danny on 26-4-2022.
//

#pragma once

#include "senjo/SearchStats.h"
#include "types.h"
#include "bitboard.h"

namespace Zagreus {
    class SearchManager {
    private:
        bool isSearching = false;
        senjo::SearchStats searchStats{};
    public:
        SearchResult getBestMove(Bitboard &board, PieceColor color);

        SearchResult
        search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove, Move &previousMove,
               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        SearchResult
        zwSearch(Bitboard &board, int depth, int beta, Move &rootMove, Move &previousMove,
                 std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        SearchResult quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                             Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        int evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        bool isCurrentlySearching();

        senjo::SearchStats getSearchStats();

        int getWhiteMaterialScore(Bitboard &board) const;

        int getBlackMaterialScore(Bitboard &board) const;

        int getWhiteConnectivityScore(Bitboard &bitboard);

        int getBlackConnectivityScore(Bitboard &bitboard);

        int getWhiteKingScore(Bitboard &bitboard);

        int getBlackKingScore(Bitboard &bitboard);

        void resetStats();

        int getWhiteBishopScore(Bitboard &bitboard);

        int getBlackBishopScore(Bitboard &bitboard);

        int getCenterScore(Bitboard &bitboard, PieceColor color);

        int getWhiteDevelopmentScore(Bitboard &bitboard);

        int getBlackDevelopmentScore(Bitboard &bitboard);

        int getWhiteRookScore(Bitboard &bitboard);

        int getBlackRookScore(Bitboard &bitboard);

        int getPositionalScore(Bitboard &bitboard, PieceColor color);

        int getMobilityScore(Bitboard &bitboard, PieceColor color);

        int getPawnScore(Bitboard &bitboard, PieceColor color);
    };

    static SearchManager searchManager{};
}
