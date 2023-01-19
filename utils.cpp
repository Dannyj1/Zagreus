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

#include <x86intrin.h>
#include <lzcntintrin.h>

#include "utils.h"

namespace Zagreus {
    uint64_t popcnt(uint64_t b) {
        return __builtin_popcountll(b);
    }

    int8_t bitscanForward(uint64_t b) {
        return b ? _tzcnt_u64(b) : 0;
    }

    int8_t bitscanReverse(uint64_t b) {
        return b ? _lzcnt_u64(b) ^ 63 : 0;
    }

    uint32_t encodeMove(const Move &move) {
        return (move.promotionPiece << 20) | (move.piece << 15) |
               (move.to << 7) | move.from;
    }

    std::string getNotation(int8_t square) {
        std::string notation = "";

        notation += static_cast<char>(square % 8 + 'a');
        notation += static_cast<char>(square / 8 + '1');

        return notation;
    }
}