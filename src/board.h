
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
#include <string_view>

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

#include "bitboard.h"
#include "move.h"
#include "constants.h"
#include "macros.h"

namespace Zagreus {
struct BoardState {
    Move move = 0;
    Piece capturedPiece = Piece::EMPTY;
};

class Board {
private:
    std::array<Piece, SQUARES> board{};
    std::array<uint64_t, PIECES> bitboards{};
    uint64_t occupied = 0;
    std::array<uint64_t, COLORS> colorBoards{};
    PieceColor sideToMove = PieceColor::EMPTY;
    std::array<BoardState, MAX_PLY> history{};
    int ply = 0;

public:
    Board() {
        std::ranges::fill(board, Piece::EMPTY);
        std::ranges::fill(bitboards, 0);
        std::ranges::fill(colorBoards, 0);
        std::ranges::fill(history, BoardState{});
    }

    template <Piece piece>
    [[nodiscard]] uint64_t getBitboard() const {
        return bitboards[TO_INT(piece)];
    }

    template <PieceColor color>
    [[nodiscard]] uint64_t getColorBitboard() const {
        return colorBoards[TO_INT(color)];
    }

    [[nodiscard]] Piece getPieceOnSquare(const int square) const {
        assert(square >= 0 && square < 64);
        return board[square];
    }

    [[nodiscard]] bool isPieceOnSquare(const int square) const {
        return board[square] != Piece::EMPTY;
    }

    [[nodiscard]] uint64_t getOccupiedBitboard() const {
        return occupied;
    }

    [[nodiscard]] uint64_t getEmptyBitboard() const {
        return ~occupied;
    }

    [[nodiscard]] PieceColor getSideToMove() const {
        return sideToMove;
    }

    [[nodiscard]] int getPly() const {
        return ply;
    }

    template <Piece piece>
    void setPiece(const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[TO_INT(piece)] |= squareBB;
        occupied |= squareBB;
        colorBoards[TO_INT(pieceColor(piece))] |= squareBB;
    }

    void setPiece(const Piece piece, const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[TO_INT(piece)] |= squareBB;
        occupied |= squareBB;
        colorBoards[TO_INT(pieceColor(piece))] |= squareBB;
    }

    void removePiece(const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);
        const Piece piece = board[square];

        board[square] = Piece::EMPTY;
        bitboards[TO_INT(piece)] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[TO_INT(pieceColor(piece))] &= ~squareBB;
    }

    template <Piece piece>
    void removePiece(const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = Piece::EMPTY;
        bitboards[TO_INT(piece)] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[TO_INT(pieceColor(piece))] &= ~squareBB;
    }

    void removePiece(const Piece piece, const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = Piece::EMPTY;
        bitboards[TO_INT(piece)] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[TO_INT(pieceColor(piece))] &= ~squareBB;
    }

    void makeMove(const Move& move);

    void unmakeMove();
    void setPieceFromFENChar(char character, uint8_t square);

    template <PieceColor movedColor>
    [[nodiscard]] bool isPositionLegal() const;

    [[nodiscard]] uint64_t getSquareAttackers(uint8_t square) const;

    template <PieceColor color>
    [[nodiscard]] uint64_t getSquareAttackersByColor(const uint8_t square) const {
        return getSquareAttackers(square) & getColorBitboard<color>();
    }

    bool setFromFEN(std::string_view fen);

    void reset();
};
} // namespace Zagreus
