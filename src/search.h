/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

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
#include <vector>

#include "bitboard.h"
#include "engine.h"
#include "types.h"
#include "../senjo/GoParams.h"

namespace Zagreus {
struct SearchContext {
    std::chrono::time_point<std::chrono::steady_clock> endTime;
    std::vector<Move> pv;
};

template <PieceColor color>
Move getBestMove(Bitboard& board, senjo::GoParams params, ZagreusEngine& engine,
                 senjo::SearchStats& searchStats);

template <PieceColor color, NodeType nodeType>
int search(Bitboard& board, int alpha, int beta, int depth, SearchContext& context);

template <PieceColor color, NodeType nodeType>
int qsearch(Bitboard& board, int alpha, int beta, int depth, SearchContext& context);
} // namespace Zagreus
