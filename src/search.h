/*
 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../senjo/SearchStats.h"
#include "types.h"
#include "bitboard.h"
#include "engine.h"

namespace Zagreus {
    class SearchManager {
    private:
        bool isSearching = false;
        senjo::SearchStats searchStats{};
        EvalContext evalContext;
    public:
        Move getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board);

        int
        search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove, Move &previousMove,
               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine, ZagreusEngine &engine, bool isPv, bool canNull);

        int quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                             Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine);

        int evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine);

        bool isCurrentlySearching();

        senjo::SearchStats getSearchStats();

        void getBlackMaterialScore(Bitboard &board);
        void getWhiteMaterialScore(Bitboard &board);

        void getWhitePositionalScore(Bitboard &bitboard);
        void getBlackPositionalScore(Bitboard &bitboard);

        void getWhiteMobilityScore(Bitboard &bitboard);
        void getBlackMobilityScore(Bitboard &bitboard);

        void getWhiteKingScore(Bitboard &bitboard);
        void getBlackKingScore(Bitboard &bitboard);

        void getWhiteConnectivityScore(Bitboard &bitboard);
        void getBlackConnectivityScore(Bitboard &bitboard);

        void getWhiteRookScore(Bitboard &bitboard);
        void getBlackRookScore(Bitboard &bitboard);

        void getWhiteBishopScore(Bitboard &bitboard);
        void getBlackBishopScore(Bitboard &bitboard);

        template<PieceColor color>
        void getPawnScore(Bitboard &bitboard);

        void initEvalContext(Bitboard &bitboard);

        template<PieceColor color>
        void getOutpostScore(Bitboard &bitboard);
    };

    static SearchManager searchManager{};

    int getGamePhase(Bitboard &bitboard);
}
