/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
