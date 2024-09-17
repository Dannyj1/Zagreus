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

#include "bitwise.h"
#include "constants.h"

#include <intrin.h>

namespace Zagreus::Bitwise {
uint64_t popcnt(uint64_t bb) {
#ifdef _MSC_VER
    return __popcnt64(bb);
#else
    return __builtin_popcountll(bb);
#endif
}

int bitscanForward(uint64_t bb) {
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward64(&index, bb);
    return (int) index;
#else
    return __builtin_ctzll(bb);
#endif
}

int bitscanReverse(const uint64_t bb) {
#ifdef _MSC_VER
    unsigned long index;
    _BitScanReverse64(&index, bb);
    return (int) index;
#else
    return 63 ^ __builtin_clzll(bb);
#endif
}

uint64_t popLsb(uint64_t& bb) {
    int lsb = bitscanForward(bb);

    bb &= bb - 1;
    return lsb;
}

uint64_t shiftNorth(const uint64_t bb) {
    return bb << 8;
}

uint64_t shiftSouth(const uint64_t bb) {
    return bb >> 8;
}

uint64_t shiftEast(const uint64_t bb) {
    return (bb << 1) & NOT_A_FILE;
}

uint64_t shiftWest(const uint64_t bb) {
    return (bb >> 1) & NOT_H_FILE;
}

uint64_t shiftNorthEast(const uint64_t bb) {
    return (bb << 9) & NOT_A_FILE;
}

uint64_t shiftNorthWest(const uint64_t bb) {
    return (bb << 7) & NOT_H_FILE;
}

uint64_t shiftSouthEast(const uint64_t bb) {
    return (bb >> 7) & NOT_A_FILE;
}

uint64_t shiftSouthWest(const uint64_t bb) {
    return (bb >> 9) & NOT_H_FILE;
}

uint64_t whitePawnSinglePush(uint64_t bb, uint64_t empty) {
}

uint64_t whitePawnDoublePush(uint64_t bb, uint64_t empty) {
}

uint64_t whitePawnAttack(uint64_t bb) {
}

uint64_t blackPawnSinglePush(uint64_t bb, uint64_t empty) {
}

uint64_t blackPawnDoublePush(uint64_t bb, uint64_t empty) {
}

uint64_t blackPawnAttack(uint64_t bb) {
}
} // namespace Zagreus::Bitwise

