/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>
#include <cstdint>

#include "board.h"
#include "move.h"
#include "types.h"
#include "uci.h"

namespace Zagreus {
enum NodeType {
    ROOT,
    PV,
    REGULAR,
};

struct SearchParams {
    uint32_t whiteTime = 0;
    uint32_t blackTime = 0;
    uint32_t whiteInc = 0;
    uint32_t blackInc = 0;
    uint16_t depth = 0;
    uint64_t max_nodes = 0;
    int moveOverhead = 0;
    bool infinite = false;
};

struct SearchStack {
    std::array<Move, MAX_PLIES> excludedMove{};

    SearchStack() { std::ranges::fill(excludedMove, NO_MOVE); }
};

struct SearchStats {
    PvLine pvLine{0};
    uint64_t nodesSearched = 0;
    uint64_t qNodesSearched = 0;
    int score = 0;
    uint16_t depth = 0;
    uint64_t timeSpentMs = 0;

#ifdef TRACE_SEARCH
    uint64_t ttProbes = 0;
    uint64_t ttHits = 0;
    uint64_t ttWrites = 0;
    uint64_t qTtProbes = 0;
    uint64_t qTtHits = 0;
    uint64_t firstMoveCutoffs = 0;
    uint64_t secondMoveCutoffs = 0;
    uint64_t totalMoveCutoffNumber = 0;
    uint64_t totalCutoffs = 0;
    uint64_t nmpTries = 0;
    uint64_t nmpPrunes = 0;
    uint64_t lmrSearches = 0;
    uint64_t lmrResearches = 0;
    uint64_t futilityPrunes = 0;
    uint64_t reverseFutilityPrunes = 0;
    uint64_t checkExtensions = 0;
    uint64_t singularExtensions = 0;
    uint64_t singularAttempts = 0;
    uint64_t seePrunes = 0;

    void clearTrace();
    void printTrace(Engine& engine, int numPositions = 1) const;
    SearchStats& operator+=(const SearchStats& other);
#endif
};

void initializeSearch();

template <PieceColor color>
[[nodiscard]] Move search(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);

// TODO: Might want to move some args to a struct
template <PieceColor color, NodeType nodeType>
int pvSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
             const std::chrono::time_point<std::chrono::steady_clock>& endTime, PvLine& pvLine,
             SearchStack& searchStack, bool isInCheck);

template <PieceColor color, NodeType nodeType>
[[nodiscard]] int qSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
                          const std::chrono::time_point<std::chrono::steady_clock>& endTime, SearchStack& searchStack,
                          bool isInCheck);
}  // namespace Zagreus