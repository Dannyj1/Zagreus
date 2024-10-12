
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

namespace Zagreus {
struct BoardState {
    Move move = 0;
    Piece capturedPiece = EMPTY;
    uint8_t enPassantSquare = 0;
    uint8_t castlingRights = 0;
};

class Board {
private:
    std::array<Piece, SQUARES> board{};
    std::array<uint64_t, PIECES> bitboards{};
    std::array<uint64_t, COLORS> colorBoards{};
    PieceColor sideToMove = WHITE;
    std::array<BoardState, MAX_PLY> history{};
    uint64_t occupied = 0;
    uint16_t ply = 0;
    uint8_t castlingRights = 0;
    uint8_t enPassantSquare = 0;

public:
    Board() {
        std::ranges::fill(board, EMPTY);
        std::ranges::fill(bitboards, 0);
        std::ranges::fill(colorBoards, 0);
        std::ranges::fill(history, BoardState{});
    }

    template <Piece piece>
    [[nodiscard]] constexpr uint64_t getBitboard() const {
        return bitboards[piece];
    }

    template <PieceColor color>
    [[nodiscard]] constexpr uint64_t getColorBitboard() const {
        return colorBoards[color];
    }

    [[nodiscard]] constexpr Piece getPieceOnSquare(const int square) const {
        assert(square >= 0 && square < 64);
        return board[square];
    }

    [[nodiscard]] constexpr bool isPieceOnSquare(const int square) const {
        return board[square] != Piece::EMPTY;
    }

    [[nodiscard]] constexpr uint64_t getOccupiedBitboard() const {
        return occupied;
    }

    [[nodiscard]] constexpr uint64_t getEmptyBitboard() const {
        return ~occupied;
    }

    [[nodiscard]] constexpr PieceColor getSideToMove() const {
        return sideToMove;
    }

    [[nodiscard]] constexpr int getPly() const {
        return ply;
    }

    [[nodiscard]] constexpr uint8_t getCastlingRights() const {
        return castlingRights;
    }

    [[nodiscard]] constexpr uint8_t getEnPassantSquare() const {
        return enPassantSquare;
    }

    [[nodiscard]] constexpr Move getLastMove() const {
        return history[ply - 1].move;
    }

    template <Piece piece>
    void setPiece(const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == Piece::EMPTY);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[piece] |= squareBB;
        occupied |= squareBB;
        colorBoards[getPieceColor(piece)] |= squareBB;
    }

    void setPiece(const Piece piece, const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == Piece::EMPTY);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[piece] |= squareBB;
        occupied |= squareBB;
        colorBoards[getPieceColor(piece)] |= squareBB;
    }

    void removePiece(const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);
        const Piece piece = board[square];
        assert(piece != Piece::EMPTY);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;
    }

    template <Piece piece>
    void removePiece(const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == piece);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = Piece::EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;
    }

    void removePiece(const Piece piece, const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == piece);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = Piece::EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;
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

    template <CastlingRights side>
    bool canCastle() const;

    bool setFromFEN(std::string_view fen);

    void reset();

    void print() const;
};
} // namespace Zagreus
