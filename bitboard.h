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
#include <string>

#include "types.h"
#include "constants.h"

namespace Zagreus {
    class Bitboard {
    private:
        uint64_t pieceBB[12]{ 0ULL };
        PieceType pieceSquareMapping[64]{ PieceType::EMPTY };
        uint64_t colorBB[2]{ 0ULL };
        uint64_t occupiedBB{ 0ULL };

        PieceColor movingColor = PieceColor::NONE;
        uint8_t ply = 0;
        uint8_t halfMoveClock = 0;
        uint8_t fullmoveClock = 1;
        int8_t enPassantSquare = Square::NO_SQUARE;
        uint8_t castlingRights = 0b00001111;

        uint64_t kingAttacks[64]{};
        uint64_t knightAttacks[64]{};
        uint64_t pawnAttacks[2][64]{};
        uint64_t betweenTable[64][64]{};

        UndoData undoStack[MAX_PLY]{};
        uint64_t moveHistory[MAX_PLY]{};
    public:
        Bitboard();

        template<PieceType pieceType>
        uint64_t getPieceBoard();

        template<PieceColor color>
        uint64_t getColorBoard();

        uint64_t getOccupiedBoard();

        uint64_t getEmptyBoard();

        PieceType getPieceOnSquare(int8_t square);

        uint64_t getKingAttacks(int8_t square);

        uint64_t getKnightAttacks(int8_t square);

        uint64_t getQueenAttacks(int8_t square, uint64_t occupancy);

        uint64_t getBishopAttacks(int8_t square, uint64_t occupancy);

        uint64_t getRookAttacks(int8_t square, uint64_t occupancy);

        template<PieceColor color>
        uint64_t getPawnAttacks(uint64_t pawns);

        void setPiece(int8_t square, PieceType piece);

        void removePiece(int8_t square, PieceType piece);

        void makeMove(Move &move);

        void unmakeMove(Move &move);

        void print();

        void printAvailableMoves(MoveList &moves);

        bool setFromFen(std::string fen);

        bool isDraw();

        bool isWinner(PieceColor color);

        void initializeBetweenLookup();

        void setPieceFromFENChar(char character, int index);

        char getCharacterForPieceType(PieceType pieceType);
    };

    template<PieceType pieceType>
    uint64_t getPieceBoard();

    template<PieceColor color>
    uint64_t getColorBoard();

    template<PieceColor color>
    uint64_t getPawnAttacks(uint64_t pawns);
}
