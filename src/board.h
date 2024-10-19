
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
/**
 * \brief Represents the state of the board at a given ply
 */
struct BoardState {
    Move move = 0;
    Piece capturedPiece = EMPTY;
    uint8_t enPassantSquare = 0;
    uint8_t castlingRights = 0;
};

/**
 * \brief Represents the chess board and provides methods to manipulate it. The board consists of several bitboards.
 */
class Board {
private:
    std::array<Piece, SQUARES> board{};
    std::array<uint64_t, PIECES> bitboards{};
    std::array<uint64_t, COLORS> colorBoards{};
    std::array<BoardState, MAX_PLY> history{};
    std::array<int, PIECES> pieceCounts{};
    PieceColor sideToMove = WHITE;
    uint64_t occupied = 0;
    uint16_t ply = 0;
    uint8_t castlingRights = 0;
    uint8_t enPassantSquare = 0;

public:
    /**
     * \brief Constructs a new Board object and initializes it to the starting position.
     */
    Board() {
        std::ranges::fill(board, EMPTY);
        std::ranges::fill(bitboards, 0);
        std::ranges::fill(colorBoards, 0);
        std::ranges::fill(history, BoardState{});
    }

    /**
     * \brief Retrieves the bitboard for a given piece type.
     * \tparam piece The piece type.
     * \return The bitboard for the given piece type.
     */
    template <Piece piece>
    [[nodiscard]] constexpr uint64_t getBitboard() const {
        return bitboards[piece];
    }

    /**
     * \brief Retrieves the bitboard for a given color.
     * \tparam color The color.
     * \return The bitboard for the given color.
     */
    template <PieceColor color>
    [[nodiscard]] constexpr uint64_t getColorBitboard() const {
        return colorBoards[color];
    }

    /**
     * \brief Retrieves the piece on a given square.
     * \param square The square index (0-63).
     * \return The piece on the given square.
     */
    [[nodiscard]] constexpr Piece getPieceOnSquare(const int square) const {
        assert(square >= 0 && square < 64);
        return board[square];
    }

    /**
     * \brief Checks if there is a piece on a given square.
     * \param square The square index (0-63).
     * \return True if there is a piece on the given square, false otherwise.
     */
    [[nodiscard]] constexpr bool isPieceOnSquare(const int square) const {
        return board[square] != EMPTY;
    }

    /**
     * \brief Retrieves the bitboard representing occupied squares.
     * \return The bitboard representing occupied squares.
     */
    [[nodiscard]] constexpr uint64_t getOccupiedBitboard() const {
        return occupied;
    }

    /**
     * \brief Retrieves the bitboard representing empty squares.
     * \return The bitboard representing empty squares.
     */
    [[nodiscard]] constexpr uint64_t getEmptyBitboard() const {
        return ~occupied;
    }

    /**
     * \brief Retrieves the color of the side to move.
     * \return The color of the side to move.
     */
    [[nodiscard]] constexpr PieceColor getSideToMove() const {
        return sideToMove;
    }

    /**
     * \brief Retrieves the number of plies since the start of the game.
     * \return The number of plies since the start of the game.
     */
    [[nodiscard]] constexpr int getPly() const {
        return ply;
    }

    /**
     * \brief Retrieves the castling rights available.
     * \return The castling rights available.
     */
    [[nodiscard]] constexpr uint8_t getCastlingRights() const {
        return castlingRights;
    }

    /**
     * \brief Retrieves the en passant square, if applicable.
     * \return The en passant square.
     */
    [[nodiscard]] constexpr uint8_t getEnPassantSquare() const {
        return enPassantSquare;
    }

    /**
     * \brief Retrieves the last move made.
     * \return The last move made.
     */
    [[nodiscard]] constexpr Move getLastMove() const {
        return history[ply - 1].move;
    }

    /**
     * \brief Retrieves the count of a specific piece type on the board.
     * \param piece The piece type.
     * \return The count of the specified piece type.
     */
    [[nodiscard]] constexpr int getPieceCount(const Piece piece) const {
        return pieceCounts[piece];
    }

    /**
     * \brief Retrieves the count of a specific piece type on the board.
     * \tparam piece The piece type.
     * \return The count of the specified piece type.
     */
    template <Piece piece>
    [[nodiscard]] constexpr int getPieceCount() const {
        return pieceCounts[piece];
    }

    /**
     * \brief Sets a piece on a given square.
     * \tparam piece The piece type.
     * \param square The square index (0-63).
     */
    template <Piece piece>
    void setPiece(const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == Piece::EMPTY);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[piece] |= squareBB;
        occupied |= squareBB;
        colorBoards[getPieceColor(piece)] |= squareBB;
        pieceCounts[piece] += 1;
    }

    /**
     * \brief Sets a piece on a given square.
     * \param piece The piece type.
     * \param square The square index (0-63).
     */
    void setPiece(const Piece piece, const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == Piece::EMPTY);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[piece] |= squareBB;
        occupied |= squareBB;
        colorBoards[getPieceColor(piece)] |= squareBB;
        pieceCounts[piece] += 1;
    }

    /**
     * \brief Removes a piece from a given square.
     * \param square The square index (0-63).
     */
    void removePiece(const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);
        const Piece piece = board[square];
        assert(piece != Piece::EMPTY);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;
        pieceCounts[piece] -= 1;
    }

    /**
     * \brief Removes a piece from a given square.
     * \tparam piece The piece type.
     * \param square The square index (0-63).
     */
    template <Piece piece>
    void removePiece(const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == piece);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;
        pieceCounts[piece] -= 1;
    }

    /**
     * \brief Removes a piece from a given square.
     * \param piece The piece type.
     * \param square The square index (0-63).
     */
    void removePiece(const Piece piece, const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == piece);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;
        pieceCounts[piece] -= 1;
    }

    /**
     * \brief Makes a move on the board.
     * \param move The move to make.
     */
    void makeMove(const Move& move);

    /**
     * \brief Unmakes the last move on the board.
     */
    void unmakeMove();

    /**
     * \brief Sets a piece on the board from a FEN character.
     * \param character The FEN character representing the piece.
     * \param square The square index (0-63).
     */
    void setPieceFromFENChar(char character, uint8_t square);

    /**
     * \brief Checks if the current position is legal based on the color that just made a move.
     * \tparam movedColor The color of the player who just moved.
     * \return True if the position is legal, false otherwise.
     */
    template <PieceColor movedColor>
    [[nodiscard]] bool isPositionLegal() const;

    /**
     * \brief Retrieves the attackers of a given square.
     * \param square The square index (0-63).
     * \return A bitboard representing the attackers.
     */
    [[nodiscard]] uint64_t getSquareAttackers(uint8_t square) const;

    /**
     * \brief Retrieves the attackers of a given square by color.
     * \tparam color The color of the attackers.
     * \param square The square index (0-63).
     * \return A bitboard representing the attackers of the given color.
     */
    template <PieceColor color>
    [[nodiscard]] uint64_t getSquareAttackersByColor(const uint8_t square) const {
        return getSquareAttackers(square) & getColorBitboard<color>();
    }

    /**
     * \brief Checks if castling is possible for the given side.
     * \tparam side The side to check for castling (WHITE\_KINGSIDE, WHITE\_QUEENSIDE, BLACK\_KINGSIDE, BLACK\_QUEENSIDE).
     * \return True if castling is possible, false otherwise.
     */
    template <CastlingRights side>
    [[nodiscard]] bool canCastle() const;

    /**
     * \brief Sets the board state from a FEN string.
     * \param fen The FEN string representing the board state.
     * \return True if the FEN string was valid, false otherwise.
     */
    [[nodiscard]] bool setFromFEN(std::string_view fen);

    /**
     * \brief Resets the board to the initial state.
     */
    void reset();

    /**
     * \brief Prints the current state of the board to the console.
     */
    void print() const;
};
} // namespace Zagreus
