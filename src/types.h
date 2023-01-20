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
        WHITE_PAWN = 0,
        BLACK_PAWN = 1,
        WHITE_KNIGHT = 2,
        BLACK_KNIGHT = 3,
        WHITE_BISHOP = 4,
        BLACK_BISHOP = 5,
        WHITE_ROOK = 6,
        BLACK_ROOK = 7,
        WHITE_QUEEN = 8,
        BLACK_QUEEN = 9,
        WHITE_KING = 10,
        BLACK_KING = 11
    };

    enum PieceColor {
        NONE = -1,
        WHITE = 0,
        BLACK = 1
    };

    enum Square {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,
        NO_SQUARE = -1
    };

    struct Move {
        int8_t from;
        int8_t to;
        PieceType piece;
        PieceType promotionPiece = PieceType::EMPTY;
        int score = 0;
    };

    enum MoveType {
        REGULAR,
        EN_PASSANT,
        CASTLING
    };

    struct UndoData {
        uint8_t halfMoveClock = 0;
        int8_t enPassantSquare = NO_SQUARE;
        uint8_t castlingRights = 0;
        PieceType capturedPiece = PieceType::EMPTY;
        MoveType moveType = MoveType::REGULAR;
        uint64_t zobristHash;
    };

    struct MoveList {
        Move moves[MAX_MOVES];
        uint8_t size;
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