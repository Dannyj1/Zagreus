
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

#include <stdint.h>
#include <array>
#include <cassert>
#include <utility>

#include "bitboard.h"
#include "move.h"
#include "constants.h"
#include "macros.h"

namespace Zagreus {
class Board {
private:
    std::array<Piece, SQUARES> board{};
    std::array<uint64_t, PIECES> bitboards{};
    uint64_t occupied = 0;
    std::array<uint64_t, COLORS> colorBoards{};

public:
    template <Piece piece>
    [[nodiscard]] uint64_t getBitboard() const {
        return bitboards[IDX(piece)];
    }

    template <PieceColor color>
    [[nodiscard]] uint64_t getColorBitboard() const {
        return colorBoards[IDX(color)];
    }

    [[nodiscard]] Piece getPieceOnSquare(const int square) const {
        assert(square >= 0 && square < 64);
        return board[square];
    }

    [[nodiscard]] uint64_t getOccupiedBitboard() const {
        return occupied;
    }

    [[nodiscard]] uint64_t getEmptyBitboard() const {
        return ~occupied;
    }

    template <Piece piece>
    void setPiece(uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[IDX(piece)] |= squareBB;
        occupied |= squareBB;
        colorBoards[IDX(pieceColor(piece))] |= squareBB;
    }

    void removePiece(uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);
        const Piece piece = board[square];

        board[square] = Piece::EMPTY;
        bitboards[IDX(piece)] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[IDX(pieceColor(piece))] &= ~squareBB;
    }

    template <Piece piece>
    void removePiece(uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = Piece::EMPTY;
        bitboards[IDX(piece)] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[IDX(pieceColor(piece))] &= ~squareBB;
    }

    void makeMove(const Move& move);

    template <PieceColor movedColor>
    [[nodiscard]] bool isPositionLegal() const;

    [[nodiscard]] uint64_t getSquareAttackers(uint8_t square) const;

    template <PieceColor color>
    [[nodiscard]] uint64_t getSquareAttackersByColor(const uint8_t square) const {
        return getSquareAttackers(square) & getColorBitboard<color>();
    }
};
} // namespace Zagreus
