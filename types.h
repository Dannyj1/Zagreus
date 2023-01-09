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

#include <cstdint>

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
        BLACK_KING = 11,
    };

    enum Square {
        H1, G1, F1, E1, D1, C1, B1, A1,
        H2, G2, F2, E2, D2, C2, B2, A2,
        H3, G3, F3, E3, D3, C3, B3, A3,
        H4, G4, F4, E4, D4, C4, B4, A4,
        H5, G5, F5, E5, D5, C5, B5, A5,
        H6, G6, F6, E6, D6, C6, B6, A6,
        H7, G7, F7, E7, D7, C7, B7, A7,
        H8, G8, F8, E8, D8, C8, B8, A8
    };

    enum Direction {
        NORTH,
        NORTH_EAST,
        EAST,
        SOUTH_EAST,
        SOUTH,
        SOUTH_WEST,
        WEST,
        NORTH_WEST
    };

    enum PieceColor {
        NONE = -1,
        WHITE = 0,
        BLACK = 1
    };

    enum CastlingRights {
        WHITE_KINGSIDE = 1 << 0,
        WHITE_QUEENSIDE = 1 << 1,
        BLACK_KINGSIDE = 1 << 2,
        BLACK_QUEENSIDE = 1 << 3
    };

    struct Move {
        uint8_t fromSquare = 0;
        uint8_t toSquare = 0;
        PieceType pieceType = PieceType::EMPTY;
        uint64_t zobristHash = 0;
        uint8_t ply = 0;
        int captureScore = 0;
        PieceType promotionPiece = PieceType::EMPTY;
    };


    struct SearchResult {
        Move move{};
        int score = -2147483647 / 2;
    };

    struct UndoData {
        uint64_t pieceBB[12]{0};
        PieceType pieceSquareMapping[64]{PieceType::EMPTY};
        uint64_t colorBB[2]{0};
        uint64_t occupiedBB = 0;
        int8_t enPassantSquare[2]{};
        uint8_t castlingRights = 0;
        uint64_t zobristHash = 0;
        uint8_t ply = 0;
        uint8_t halfMoveClock = 0;
        uint8_t fullMoveClock = 0;
        uint8_t kingInCheck = 0;
        uint32_t previousMoveFrom = 0;
        uint32_t previousMoveTo = 0;
    };
}