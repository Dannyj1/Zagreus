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

#include <array>

enum class Direction {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST
};

enum class PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY = 255 };

enum class PieceColor : uint8_t { WHITE, BLACK, EMPTY = 255 };

enum class Piece : uint8_t {
    WHITE_PAWN,
    BLACK_PAWN,
    WHITE_KNIGHT,
    BLACK_KNIGHT,
    WHITE_BISHOP,
    BLACK_BISHOP,
    WHITE_ROOK,
    BLACK_ROOK,
    WHITE_QUEEN,
    BLACK_QUEEN,
    WHITE_KING,
    BLACK_KING,
    EMPTY = 255
};

inline constexpr PieceColor operator!(const PieceColor color) {
    return color == PieceColor::WHITE ? PieceColor::BLACK : PieceColor::WHITE;
}

inline constexpr PieceColor pieceColor(const Piece piece) {
    return static_cast<PieceColor>(static_cast<uint8_t>(piece) / 2);
}

inline constexpr PieceType pieceType(const Piece piece) {
    return static_cast<PieceType>(static_cast<uint8_t>(piece) % 2);
}
