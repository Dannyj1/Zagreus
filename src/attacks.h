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
#include <cstdint>

#include "types.h"

namespace Zagreus {
void intializeAttacks();

void initializeRayAttacks();

template <PieceColor color>
uint64_t getPawnDoublePush(uint64_t pawns, uint64_t emptyBB);

template <PieceColor color>
uint64_t getPawnAttacks(int8_t square);

template <PieceColor color>
uint64_t getPawnSinglePush(uint64_t pawns, uint64_t emptyBB);

uint64_t getKingAttacks(int8_t square);

uint64_t getKnightAttacks(int8_t square);

uint64_t getQueenAttacks(int8_t square, uint64_t occupancy);

uint64_t getBishopAttacks(int8_t square, uint64_t occupancy);

uint64_t getRookAttacks(int8_t square, uint64_t occupancy);

uint64_t getRayAttack(int8_t square, Direction direction);
} // namespace Zagreus
