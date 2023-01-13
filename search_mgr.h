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
#include "engine.h"

namespace Zagreus {
    struct EvalContext {
        int phase = 0;
        int whiteMidgameScore = 0;
        int blackMidgameScore = 0;
        int whiteEndgameScore = 0;
        int blackEndgameScore = 0;
        uint64_t whitePawnAttacks = 0;
        uint64_t whiteKnightAttacks = 0;
        uint64_t whiteBishopAttacks = 0;
        uint64_t whiteRookAttacks = 0;
        uint64_t whiteQueenAttacks = 0;
        uint64_t whiteCombinedAttacks = 0;
        uint64_t blackPawnAttacks = 0;
        uint64_t blackKnightAttacks = 0;
        uint64_t blackBishopAttacks = 0;
        uint64_t blackRookAttacks = 0;
        uint64_t blackQueenAttacks = 0;
        uint64_t blackCombinedAttacks = 0;
    };

    class SearchManager {
    private:
        bool isSearching = false;
        senjo::SearchStats searchStats{};
    public:
        SearchResult getBestMove(ZagreusEngine &engine, Bitboard &board, PieceColor color);

        SearchResult
        search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove, Move &previousMove,
               std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine);

        SearchResult quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                             Move &previousMove,
                             std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        int evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime);

        bool isCurrentlySearching();

        senjo::SearchStats getSearchStats();

        void getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board) const;

        void getBlackMaterialScore(EvalContext &evalContext, Bitboard &board) const;

        void getWhiteConnectivityScore(EvalContext &evalContext, Bitboard &bitboard);

        void getBlackConnectivityScore(EvalContext &evalContext, Bitboard &bitboard);

        void getWhiteKingScore(EvalContext &evalContext, Bitboard &bitboard);

        void getBlackKingScore(EvalContext &evalContext, Bitboard &bitboard);

        void resetStats();

        void getWhiteBishopScore(EvalContext &evalContext, Bitboard &bitboard);

        void getBlackBishopScore(EvalContext &evalContext, Bitboard &bitboard);

        void getWhiteDevelopmentScore(EvalContext &evalContext, Bitboard &bitboard);

        void getBlackDevelopmentScore(EvalContext &evalContext, Bitboard &bitboard);

        void getWhiteRookScore(EvalContext &evalContext, Bitboard &bitboard);

        void getBlackRookScore(EvalContext &evalContext, Bitboard &bitboard);

        void getPositionalScore(EvalContext &evalContext, Bitboard &bitboard, PieceColor color);

        void getPawnScore(EvalContext &evalContext, Bitboard &bitboard, PieceColor color);

        EvalContext createEvalContext(Bitboard &bitboard);

        void getWhiteMobilityScore(EvalContext &evalContext, Bitboard &bitboard);

        void getBlackMobilityScore(EvalContext &evalContext, Bitboard &bitboard);

        int getGamePhase(Bitboard &bitboard);
    };

    static SearchManager searchManager{};
}
