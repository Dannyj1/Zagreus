//
// Created by Danny on 26-4-2022.
//

#pragma once

#include "senjo/SearchStats.h"
#include "types.h"
#include "collections/move_list.h"
#include "bitboard.h"

namespace Zagreus {
    class SearchManager {
    private:
        bool isSearching;
        senjo::SearchStats searchStats;
    public:
        SearchResult getBestMove(Bitboard &board, PieceColor color);

        SearchResult
        search(Bitboard &board, int depth, int ply, int alpha, int beta, const Move &rootMove, const Move &previousMove,
               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        SearchResult
        zwSearch(Bitboard &board, int depth, int ply, int beta, const Move &rootMove, const Move &previousMove,
                 std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        SearchResult quiesce(Bitboard &board, int ply, int depth, int alpha, int beta, const Move &rootMove,
                             const Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        int evaluate(Bitboard &board, int ply, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        bool isCurrentlySearching();

        senjo::SearchStats getSearchStats();

        int getWhiteMaterialScore(Bitboard &board) const;

        int getBlackMaterialScore(Bitboard &board) const;

        int getWhiteConnectivityScore(Bitboard &bitboard);

        int getBlackConnectivityScore(Bitboard &bitboard);
    };

    static SearchManager searchManager{};
}
