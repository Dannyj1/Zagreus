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
        search(Bitboard &board, int depth, int ply, int alpha, int beta, const Move &rootMove, const Move &previousMove,
               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime);

        SearchResult
        zwSearch(Bitboard &board, int depth, int ply, int beta, const Move &rootMove, const Move &previousMove,
                 std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime);

        SearchResult quiesce(Bitboard &board, int ply, int depth, int alpha, int beta, const Move &rootMove,
                             const Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime);

        int evaluate(Bitboard &board, int ply, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, std::chrono::time_point<std::chrono::high_resolution_clock> &startTime);

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
    };

    static SearchManager searchManager{};
}
