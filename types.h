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

#include "constants.h"

namespace Zagreus {
    enum PieceType {
        EMPTY = -1,
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
        BLACK_KING
    };

    enum PieceColor {
        NONE = -1,
        WHITE = 0,
        BLACK = 1
    };

    enum MoveType {
        REGULAR,
        CASTLING,
        PROMOTION,
        EN_PASSANT
    };

    struct Move {
        int8_t from;
        int8_t to;
        PieceType piece;
        PieceType promotionPiece;
    };

    struct UndoData {
        uint8_t halfMoveClock;
        int8_t enPassantSquare;
        uint8_t castlingRights;
        PieceType capturedPiece;
    };

    struct MoveList {
        Move moves[MAX_MOVES];
        uint8_t count;
    };

    enum CastlingRights {
        WHITE_KINGSIDE = 1 << 0,
        WHITE_QUEENSIDE = 1 << 1,
        BLACK_KINGSIDE = 1 << 2,
        BLACK_QUEENSIDE = 1 << 3
    };

    enum Direction {
        NORTH = 8,
        SOUTH = -8,
        EAST = 1,
        WEST = -1,
        NORTH_EAST = 9,
        NORTH_WEST = 7,
        SOUTH_EAST = -7,
        SOUTH_WEST = -9
    };
}