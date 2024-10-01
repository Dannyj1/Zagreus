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

#include "bitboard.h"
#include "bitwise.h"
#include "types.h"

namespace Zagreus {
uint64_t popcnt(const uint64_t bitboard) {
    return Bitwise::popcnt(bitboard);
}

uint64_t popLsb(uint64_t bitboard) {
    return Bitwise::popLsb(bitboard);
}

int bitscanForward(const uint64_t bitboard) {
    return Bitwise::bitscanForward(bitboard);
}

int bitscanReverse(const uint64_t bitboard) {
    return Bitwise::bitscanReverse(bitboard);
}

template <Direction direction>
uint64_t shift(const uint64_t bitboard) {
    switch (direction) {
            using enum Direction;
        case NORTH:
            return Bitwise::shiftNorth(bitboard);
        case SOUTH:
            return Bitwise::shiftSouth(bitboard);
        case EAST:
            return Bitwise::shiftEast(bitboard);
        case WEST:
            return Bitwise::shiftWest(bitboard);
        case NORTH_EAST:
            return Bitwise::shiftNorthEast(bitboard);
        case NORTH_WEST:
            return Bitwise::shiftNorthWest(bitboard);
        case SOUTH_EAST:
            return Bitwise::shiftSouthEast(bitboard);
        case SOUTH_WEST:
            return Bitwise::shiftSouthWest(bitboard);
    }

    return bitboard;
}
} // namespace Zagreus
