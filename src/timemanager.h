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

#include <chrono>

#include "types.h"
#include "engine.h"
#include "../senjo/GoParams.h"

namespace Zagreus {
    std::chrono::time_point<std::chrono::high_resolution_clock> getEndTime(senjo::GoParams &params, Bitboard &bitboard, ZagreusEngine &engine, PieceColor movingColor);
}
