
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
#include <utility>

#include "bitwise.h"
#include "constants.h"
#include "types.h"

namespace Zagreus {
void initializeAttackLookupTables();

inline uint64_t shiftNorth(const uint64_t bb) {
    return bb << 8;
}

inline uint64_t shiftSouth(const uint64_t bb) {
    return bb >> 8;
}

inline uint64_t shiftEast(const uint64_t bb) {
    return (bb << 1) & NOT_A_FILE;
}

inline uint64_t shiftWest(const uint64_t bb) {
    return (bb >> 1) & NOT_H_FILE;
}

inline uint64_t shiftNorthEast(const uint64_t bb) {
    return (bb << 9) & NOT_A_FILE;
}

inline uint64_t shiftNorthWest(const uint64_t bb) {
    return (bb << 7) & NOT_H_FILE;
}

inline uint64_t shiftSouthEast(const uint64_t bb) {
    return (bb >> 7) & NOT_A_FILE;
}

inline uint64_t shiftSouthWest(const uint64_t bb) {
    return (bb >> 9) & NOT_H_FILE;
}

inline uint64_t shiftNorthNorthEast(const uint64_t bb) {
    return (bb << 17) & NOT_A_FILE;
}

inline uint64_t shiftNorthEastEast(const uint64_t bb) {
    return (bb << 10) & NOT_AB_FILE;
}

inline uint64_t shiftSouthEastEast(const uint64_t bb) {
    return (bb >> 6) & NOT_AB_FILE;
}

inline uint64_t shiftSouthSouthEast(const uint64_t bb) {
    return (bb >> 15) & NOT_A_FILE;
}

inline uint64_t shiftNorthNorthWest(const uint64_t bb) {
    return (bb << 15) & NOT_H_FILE;
}

inline uint64_t shiftNorthWestWest(const uint64_t bb) {
    return (bb << 6) & NOT_GH_FILE;
}

inline uint64_t shiftSouthWestWest(const uint64_t bb) {
    return (bb >> 10) & NOT_GH_FILE;
}

inline uint64_t shiftSouthSouthWest(const uint64_t bb) {
    return (bb >> 17) & NOT_H_FILE;
}

template <Direction direction>
constexpr uint64_t shift(const uint64_t bb) {
    switch (direction) {
        case NORTH:
            return shiftNorth(bb);
        case SOUTH:
            return shiftSouth(bb);
        case EAST:
            return shiftEast(bb);
        case WEST:
            return shiftWest(bb);
        case NORTH_EAST:
            return shiftNorthEast(bb);
        case NORTH_WEST:
            return shiftNorthWest(bb);
        case SOUTH_EAST:
            return shiftSouthEast(bb);
        case SOUTH_WEST:
            return shiftSouthWest(bb);
        case NORTH_NORTH_EAST:
            return shiftNorthNorthEast(bb);
        case NORTH_EAST_EAST:
            return shiftNorthEastEast(bb);
        case SOUTH_EAST_EAST:
            return shiftSouthEastEast(bb);
        case SOUTH_SOUTH_EAST:
            return shiftSouthSouthEast(bb);
        case SOUTH_SOUTH_WEST:
            return shiftSouthSouthWest(bb);
        case SOUTH_WEST_WEST:
            return shiftSouthWestWest(bb);
        default:
            assert(false);
            return bb;
    }
}

inline uint64_t whitePawnSinglePush(const uint64_t bb, const uint64_t empty) {
    return shiftNorth(bb) & empty;
}

inline uint64_t whitePawnDoublePush(const uint64_t bb, const uint64_t empty) {
    const uint64_t singlePush = whitePawnSinglePush(bb, empty);

    return shiftNorth(singlePush) & empty & RANK_4;
}

inline uint64_t whitePawnWestAttacks(const uint64_t bb) {
    return shiftNorthWest(bb);
}

inline uint64_t whitePawnEastAttacks(const uint64_t bb) {
    return shiftNorthEast(bb);
}

inline uint64_t calculateWhitePawnAttacks(const uint64_t bb) {
    return whitePawnWestAttacks(bb) | whitePawnEastAttacks(bb);
}

inline uint64_t whitePushablePawns(const uint64_t bb, const uint64_t empty) {
    return shiftSouth(empty) & bb;
}

inline uint64_t whiteDoublePushablePawns(const uint64_t bb, const uint64_t empty) {
    const uint64_t emptyRank3 = shiftSouth(empty & RANK_4) & empty;

    return whitePushablePawns(bb, emptyRank3);
}

inline uint64_t blackPawnSinglePush(const uint64_t bb, const uint64_t empty) {
    return shiftSouth(bb) & empty;
}

inline uint64_t blackPawnDoublePush(const uint64_t bb, const uint64_t empty) {
    const uint64_t singlePush = blackPawnSinglePush(bb, empty);

    return shiftSouth(singlePush) & empty & RANK_5;
}

inline uint64_t blackPawnWestAttacks(const uint64_t bb) {
    return shiftSouthWest(bb);
}

inline uint64_t blackPawnEastAttacks(const uint64_t bb) {
    return shiftSouthEast(bb);
}

inline uint64_t calculateBlackPawnAttacks(const uint64_t bb) {
    return blackPawnWestAttacks(bb) | blackPawnEastAttacks(bb);
}

inline uint64_t blackPushablePawns(const uint64_t bb, const uint64_t empty) {
    return shiftNorth(empty) & bb;
}

inline uint64_t blackDoublePushablePawns(const uint64_t bb, const uint64_t empty) {
    const uint64_t emptyRank6 = shiftNorth(empty & RANK_5) & empty;

    return blackPushablePawns(bb, emptyRank6);
}

inline uint64_t calculateKnightAttacks(const uint64_t bb) {
    return shiftNorthNorthEast(bb) | shiftNorthEastEast(bb) | shiftSouthEastEast(bb) |
           shiftSouthSouthEast(bb) | shiftSouthSouthWest(bb) | shiftSouthWestWest(bb) |
           shiftNorthWestWest(bb) | shiftNorthNorthWest(bb);
}

template <PieceColor color>
uint64_t pawnAttacks(uint8_t square);

uint64_t knightAttacks(uint8_t square);

uint64_t kingAttacks(uint8_t square);

uint64_t bishopAttacks(uint8_t square, uint64_t occupied);

uint64_t rookAttacks(uint8_t square, uint64_t occupied);

uint64_t queenAttacks(uint8_t square, uint64_t occupied);

inline uint64_t calculateKingAttacks(uint64_t bb) {
    const uint64_t attacks = shiftEast(bb) | shiftWest(bb);
    bb |= attacks;

    return attacks | shiftNorth(bb) | shiftSouth(bb);
}

inline uint64_t squareToBitboard(const uint8_t square) {
    return 1ULL << square;
}

inline uint8_t bitboardToSquare(const uint64_t bb) {
    return bitscanForward(bb);
}
} // namespace Zagreus
