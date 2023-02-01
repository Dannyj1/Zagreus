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

#include "../senjo/SearchStats.h"
#include "types.h"
#include "bitboard.h"
#include "engine.h"

namespace Zagreus {
    class SearchManager {
    private:
        bool isSearching = false;
        senjo::SearchStats searchStats{};
    public:
        Move getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board);

        int
        search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove, Move &previousMove,
               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine, ZagreusEngine &engine, bool isPv);

        int quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                             Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine);

        int evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine);

        bool isCurrentlySearching();

        senjo::SearchStats getSearchStats();
    };

    static SearchManager searchManager{};

    void getBlackMaterialScore(EvalContext &evalContext, Bitboard &board);
    void getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board);

    void getWhitePositionalScore(EvalContext &evalContext, Bitboard &bitboard);
    void getBlackPositionalScore(EvalContext &evalContext, Bitboard &bitboard);

    void getWhiteMobilityScore(EvalContext &evalContext, Bitboard &bitboard);
    void getBlackMobilityScore(EvalContext &evalContext, Bitboard &bitboard);

    void getWhiteKingScore(EvalContext &evalContext, Bitboard &bitboard);
    void getBlackKingScore(EvalContext &evalContext, Bitboard &bitboard);

    void getWhiteConnectivityScore(EvalContext &evalContext, Bitboard &bitboard);
    void getBlackConnectivityScore(EvalContext &evalContext, Bitboard &bitboard);

    void getWhiteRookScore(EvalContext &evalContext, Bitboard &bitboard);
    void getBlackRookScore(EvalContext &evalContext, Bitboard &bitboard);

    void getWhiteBishopScore(EvalContext &evalContext, Bitboard &bitboard);
    void getBlackBishopScore(EvalContext &evalContext, Bitboard &bitboard);

    void getPawnScore(EvalContext &evalContext, Bitboard &bitboard, PieceColor color);

    EvalContext createEvalContext(Bitboard &bitboard);
}
