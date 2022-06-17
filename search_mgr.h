//
// Created by Danny on 26-4-2022.
//

#pragma once

#include "board.h"

namespace Chess {
    struct SearchResult {
        Move move{};
        int score = -2147483647;
    };

    class SearchManager {
    private:
        bool isSearching;
    public:
        SearchResult getBestMove(Board* board, PieceColor color);

        SearchResult search(Board* board, int depth, int ply, int alpha, int beta, const Move &rootMove, const Move &previousMove,
                            std::chrono::time_point<std::chrono::system_clock> endTime);

        SearchResult zwSearch(Board* board, int depth, int ply, int beta, const Move &rootMove, const Move &previousMove,
                              std::chrono::time_point<std::chrono::system_clock> endTime);

        SearchResult quiesce(Board* board, int ply, int alpha, int beta, const Move &rootMove, const Move &previousMove,
                             std::chrono::time_point<std::chrono::system_clock> endTime);

        int evaluate(Board* board, int ply, std::chrono::time_point<std::chrono::system_clock> endTime);

        bool isCurrentlySearching();
    };

    static SearchManager searchManager{};
}
