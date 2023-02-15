/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
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

#include <string>

#include "types.h"

namespace Zagreus {
    inline static PieceColor getOppositeColor(PieceColor color) {
        return static_cast<PieceColor>(color ^ 1);
    }

    uint64_t popcnt(uint64_t b);

    int8_t bitscanForward(uint64_t b);

    int8_t bitscanReverse(uint64_t b);

    uint32_t encodeMove(const Move &move);

    std::string getNotation(int8_t square);

    int8_t getSquareFromString(std::string move);

    uint16_t getPieceWeight(PieceType type);

    char getCharacterForPieceType(PieceType pieceType);

    int mvvlva(PieceType attacker, PieceType victim);
}