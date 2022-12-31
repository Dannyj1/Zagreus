//
// Created by Danny on 26-4-2022.
//

#pragma once

#include "senjo/SearchStats.h"
#include "bitboard.h"

namespace Chess {
    struct SearchResult {
        Move move{};
        int score = -2147483647 / 2;
    };

    class SearchManager {
    private:
        bool isSearching;
        senjo::SearchStats searchStats;
    public:
        SearchResult getBestMove(Bitboard board, PieceColor color);

        SearchResult search(Bitboard board, int depth, int ply, int alpha, int beta, const Move &rootMove, const Move &previousMove,
                            std::chrono::time_point<std::chrono::high_resolution_clock> endTime);

        SearchResult zwSearch(Bitboard board, int depth, int ply, int beta, const Move &rootMove, const Move &previousMove,
                              std::chrono::time_point<std::chrono::high_resolution_clock> endTime);

        SearchResult quiesce(Bitboard board, int ply, int depth, int alpha, int beta, const Move &rootMove, const Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> endTime);

        int evaluate(Bitboard board, int ply, std::chrono::time_point<std::chrono::high_resolution_clock> endTime);

        bool isCurrentlySearching();

        senjo::SearchStats getSearchStats();
    };

    static SearchManager searchManager{};
}
