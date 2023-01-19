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
#include "bitwise.h"

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
        uint64_t getPieceBoard() {
            return pieceBB[pieceType];
        }

        template<PieceColor color>
        uint64_t getColorBoard() {
            return colorBB[color];
        }

        template<PieceColor color>
        uint64_t getPawnDoublePush(uint64_t pawns) {
            uint64_t singlePush = Zagreus::Bitboard::getPawnSinglePush<color>(pawns);

            if (color == PieceColor::WHITE) {
                return singlePush | (nortOne(singlePush) & getEmptyBoard() & RANK_4);
            } else if (color == PieceColor::BLACK) {
                return singlePush | (soutOne(singlePush) & getEmptyBoard() & RANK_5);
            } else {
                return 0;
            }
        }

        template<PieceColor color>
        uint64_t getPawnAttacks(int8_t square) {
            return pawnAttacks[color][square];
        }

        template<PieceColor color>
        uint64_t getPawnSinglePush(uint64_t pawns) {
            if (color == PieceColor::WHITE) {
                return nortOne(pawns) & getEmptyBoard();
            } else if (color == PieceColor::BLACK) {
                return soutOne(pawns) & getEmptyBoard();
            } else {
                return 0;
            }
        }

        uint64_t getOccupiedBoard();

        uint64_t getEmptyBoard();

        PieceType getPieceOnSquare(int8_t square);

        uint64_t getKingAttacks(int8_t square);

        uint64_t getKnightAttacks(int8_t square);

        uint64_t getQueenAttacks(int8_t square);

        uint64_t getBishopAttacks(int8_t square);

        uint64_t getRookAttacks(int8_t square);

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

        PieceColor getMovingColor() const;

        void setMovingColor(PieceColor movingColor);
    };
}
