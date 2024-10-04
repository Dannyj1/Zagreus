
/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

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

namespace Zagreus::Bitwise {
inline uint64_t popcnt(uint64_t bb);

inline int bitscanForward(uint64_t bb);

inline int bitscanReverse(uint64_t bb);

inline uint64_t popLsb(uint64_t& bb);

inline uint64_t shiftNorth(uint64_t bb);

inline uint64_t shiftSouth(uint64_t bb);

inline uint64_t shiftEast(uint64_t bb);

inline uint64_t shiftWest(uint64_t bb);

inline uint64_t shiftNorthEast(uint64_t bb);

inline uint64_t shiftNorthWest(uint64_t bb);

inline uint64_t shiftSouthEast(uint64_t bb);

inline uint64_t shiftSouthWest(uint64_t bb);

inline uint64_t shiftNorthNorthEast(uint64_t bb);

inline uint64_t shiftNorthEastEast(uint64_t bb);

inline uint64_t shiftSouthEastEast(uint64_t bb);

inline uint64_t shiftSouthSouthEast(uint64_t bb);

inline uint64_t shiftNorthNorthWest(uint64_t bb);

inline uint64_t shiftNorthWestWest(uint64_t bb);

inline uint64_t shiftSouthWestWest(uint64_t bb);

inline uint64_t shiftSouthSouthWest(uint64_t bb);

template <Direction direction>
uint64_t shift(uint64_t bb);

inline uint64_t whitePawnSinglePush(uint64_t bb, uint64_t empty);

inline uint64_t whitePawnDoublePush(uint64_t bb, uint64_t empty);

inline uint64_t whitePawnWestAttacks(uint64_t bb);

inline uint64_t whitePawnEastAttacks(uint64_t bb);

inline uint64_t whitePawnAttacks(uint64_t bb);

inline uint64_t whitePushablePawns(uint64_t bb, uint64_t empty);

inline uint64_t whiteDoublePushablePawns(uint64_t bb, uint64_t empty);

inline uint64_t blackPawnSinglePush(uint64_t bb, uint64_t empty);

inline uint64_t blackPawnDoublePush(uint64_t bb, uint64_t empty);

inline uint64_t blackPawnWestAttacks(uint64_t bb);

inline uint64_t blackPawnEastAttacks(uint64_t bb);

inline uint64_t blackPawnAttacks(uint64_t bb);

inline uint64_t blackPushablePawns(uint64_t bb, uint64_t empty);

inline uint64_t blackDoublePushablePawns(uint64_t bb, uint64_t empty);

inline uint64_t knightAttacks(uint64_t bb);
} // namespace Zagreus::Bitwise
