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

#include "../senjo/SearchStats.h"
#include "bitboard.h"
#include "engine.h"
#include "types.h"

namespace Zagreus {
class SearchManager {
 private:
  bool isSearching = false;
  senjo::SearchStats searchStats{};
  EvalContext evalContext;
  MoveListPool *moveListPool = MoveListPool::getInstance();

 public:
  Move getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board);

  template <PieceColor color>
  int search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove, Move &previousMove,
             std::chrono::time_point<std::chrono::steady_clock> &endTime, Line &pvLine,
             ZagreusEngine &engine, bool isPv, bool canNull);

  template <PieceColor color>
  int quiesce(Bitboard &board, int alpha, int beta, Move &rootMove, Move &previousMove,
              std::chrono::time_point<std::chrono::steady_clock> &endTime, ZagreusEngine &engine,
              bool isPv, int depth = 0);

  bool isCurrentlySearching() const;

  senjo::SearchStats getSearchStats();
};

static SearchManager searchManager{};
}  // namespace Zagreus
