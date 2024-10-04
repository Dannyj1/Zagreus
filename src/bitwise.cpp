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

#ifdef _MSC_VER
#include <intrin.h>
#endif

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

uint64_t shiftNorthNorthEast(const uint64_t bb) {
    return (bb << 17) & NOT_A_FILE;
}

uint64_t shiftNorthEastEast(const uint64_t bb) {
    return (bb << 10) & NOT_AB_FILE;
}

uint64_t shiftSouthEastEast(const uint64_t bb) {
    return (bb >> 6) & NOT_AB_FILE;
}

uint64_t shiftSouthSouthEast(const uint64_t bb) {
    return (bb >> 15) & NOT_A_FILE;
}

uint64_t shiftNorthNorthWest(const uint64_t bb) {
    return (bb << 15) & NOT_H_FILE;
}

uint64_t shiftNorthWestWest(const uint64_t bb) {
    return (bb << 6) & NOT_GH_FILE;
}

uint64_t shiftSouthWestWest(const uint64_t bb) {
    return (bb >> 10) & NOT_GH_FILE;
}

uint64_t shiftSouthSouthWest(const uint64_t bb) {
    return (bb >> 17) & NOT_H_FILE;
}

template <Direction direction>
uint64_t shift(const uint64_t bb) {
    switch (direction) {
        case Direction::NORTH:
            return shiftNorth(bb);
        case Direction::SOUTH:
            return shiftSouth(bb);
        case Direction::EAST:
            return shiftEast(bb);
        case Direction::WEST:
            return shiftWest(bb);
        case Direction::NORTH_EAST:
            return shiftNorthEast(bb);
        case Direction::NORTH_WEST:
            return shiftNorthWest(bb);
        case Direction::SOUTH_EAST:
            return shiftSouthEast(bb);
        case Direction::SOUTH_WEST:
            return shiftSouthWest(bb);
        default:
            return bb;
    }
}

uint64_t whitePawnSinglePush(const uint64_t bb, const uint64_t empty) {
    return shiftNorth(bb) & empty;
}

uint64_t whitePawnDoublePush(const uint64_t bb, const uint64_t empty) {
    const uint64_t singlePush = whitePawnSinglePush(bb, empty);

    return shiftNorth(singlePush) & empty & RANK_4;
}

uint64_t whitePawnWestAttacks(const uint64_t bb) {
    return shiftNorthWest(bb);
}

uint64_t whitePawnEastAttacks(const uint64_t bb) {
    return shiftNorthEast(bb);
}

uint64_t whitePawnAttacks(const uint64_t bb) {
    return whitePawnWestAttacks(bb) | whitePawnEastAttacks(bb);
}

uint64_t whitePushablePawns(const uint64_t bb, const uint64_t empty) {
    return shiftSouth(empty) & bb;
}

uint64_t whiteDoublePushablePawns(const uint64_t bb, const uint64_t empty) {
    const uint64_t emptyRank3 = shiftSouth(empty & RANK_4) & empty;

    return whitePushablePawns(bb, emptyRank3);
}

uint64_t blackPawnSinglePush(const uint64_t bb, const uint64_t empty) {
    return shiftSouth(bb) & empty;
}

uint64_t blackPawnDoublePush(const uint64_t bb, const uint64_t empty) {
    const uint64_t singlePush = blackPawnSinglePush(bb, empty);

    return shiftSouth(singlePush) & empty & RANK_5;
}

uint64_t blackPawnWestAttacks(const uint64_t bb) {
    return shiftSouthWest(bb);
}

uint64_t blackPawnEastAttacks(const uint64_t bb) {
    return shiftSouthEast(bb);
}

uint64_t blackPawnAttacks(const uint64_t bb) {
    return blackPawnWestAttacks(bb) | blackPawnEastAttacks(bb);
}

uint64_t blackPushablePawns(const uint64_t bb, const uint64_t empty) {
    return shiftNorth(empty) & bb;
}

uint64_t blackDoublePushablePawns(const uint64_t bb, const uint64_t empty) {
    const uint64_t emptyRank6 = shiftNorth(empty & RANK_5) & empty;

    return blackPushablePawns(bb, emptyRank6);
}

uint64_t knightAttacks(uint64_t bb) {
    return shiftNorthNorthEast(bb) | shiftNorthEastEast(bb) | shiftSouthEastEast(bb) |
           shiftSouthSouthEast(bb)
           | shiftSouthSouthWest(bb) | shiftSouthWestWest(bb) | shiftNorthWestWest(bb) |
           shiftNorthNorthWest(bb);
}
} // namespace Zagreus::Bitwise

