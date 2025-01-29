
/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

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

#include "uci.h"
#include "move.h"
#include "board.h"

namespace Zagreus {
enum NodeType {
    ROOT,
    PV,
    REGULAR,
};

template <PieceColor color>
Move search(Engine& engine, Board& board, int whiteTime, int blackTime);

template <PieceColor color, NodeType nodeType>
int pvSearch(Board& board, int alpha, int beta, int depth, const std::chrono::time_point<std::chrono::steady_clock>& endTime);

template <PieceColor color>
int qSearch(Board& board, int alpha, int beta, const std::chrono::time_point<std::chrono::steady_clock>& endTime);
} // namespace Zagreus
