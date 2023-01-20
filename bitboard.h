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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#pragma once


#include <cstdint>
#include <string>
#include <cassert>

#include "types.h"
#include "constants.h"
#include "bitwise.h"
#include "utils.h"

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

        uint64_t zobristConstants[ZOBRIST_CONSTANT_SIZE]{};
        uint64_t zobristHash = 0ULL;

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
            uint64_t singlePush = getPawnSinglePush<color>(pawns);

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

        template<PieceColor color>
        bool isWinner() {
            if (!isKingInCheck<color>()) {
                return false;
            }

            MoveList moves = generateMoves<color>(*this);

            for (int i = 0; i < moves.size; i++) {
                Move move = moves.moves[i];
                assert(move.from != move.to);

                makeMove(move);

                if (isKingInCheck<color>()) {
                    unmakeMove(move);
                    continue;
                }

                unmakeMove(move);
                return false;
            }

            return true;
        };

        void initializeBetweenLookup();

        void setPieceFromFENChar(char character, int index);

        char getCharacterForPieceType(PieceType pieceType);

        PieceColor getMovingColor() const;

        void setMovingColor(PieceColor movingColor);

        uint64_t getSquareAttacks(int square);

        template <PieceColor color>
        uint64_t getSquareAttacksByColor(int square) {
            if (color == PieceColor::WHITE) {
                uint64_t queenBB = getPieceBoard<PieceType::WHITE_QUEEN>();
                uint64_t straightSlidingPieces = getPieceBoard<PieceType::WHITE_ROOK>() | queenBB;
                uint64_t diagonalSlidingPieces = getPieceBoard<PieceType::WHITE_BISHOP>() | queenBB;

                uint64_t pawnAttacks = getPawnAttacks<PieceColor::BLACK>(square) & getPieceBoard<PieceType::WHITE_PAWN>();
                uint64_t rookAttacks = getRookAttacks(square) & straightSlidingPieces;
                uint64_t bishopAttacks = getBishopAttacks(square) & diagonalSlidingPieces;
                uint64_t knightAttacks = getKnightAttacks(square) & getPieceBoard<PieceType::WHITE_KNIGHT>();
                uint64_t kingAttacks = getKingAttacks(square) & getPieceBoard<PieceType::WHITE_KING>();

                return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
            } else {
                uint64_t queenBB = getPieceBoard<PieceType::BLACK_QUEEN>();
                uint64_t straightSlidingPieces = getPieceBoard<PieceType::BLACK_ROOK>() | queenBB;
                uint64_t diagonalSlidingPieces = getPieceBoard<PieceType::BLACK_BISHOP>() | queenBB;

                uint64_t pawnAttacks = getPawnAttacks<PieceColor::WHITE>(square) & getPieceBoard<PieceType::BLACK_PAWN>();
                uint64_t rookAttacks = getRookAttacks(square) & straightSlidingPieces;
                uint64_t bishopAttacks = getBishopAttacks(square) & diagonalSlidingPieces;
                uint64_t knightAttacks = getKnightAttacks(square) & getPieceBoard<PieceType::BLACK_KNIGHT>();
                uint64_t kingAttacks = getKingAttacks(square) & getPieceBoard<PieceType::BLACK_KING>();

                return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
            }
        }

        template <PieceColor color>
        bool isSquareAttackedByColor(int square) {
            return getSquareAttacksByColor<color>(square) != 0;
        }

        template<PieceColor color>
        bool isKingInCheck() {
            if (color == PieceColor::NONE) {
                return true;
            }

            uint64_t kingBB = getPieceBoard<color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING>();
            int kingLocation = bitscanForward(kingBB);

            if (color == PieceColor::WHITE) {
                return isSquareAttackedByColor<PieceColor::BLACK>(kingLocation);
            } else {
                return isSquareAttackedByColor<PieceColor::WHITE>(kingLocation);
            }
        }

        int8_t getEnPassantSquare() const;

        void setEnPassantSquare(int8_t enPassantSquare);

        uint8_t getCastlingRights() const;

        void setCastlingRights(uint8_t castlingRights);

        bool isInsufficientMaterial();

        uint64_t getZobristHash() const;

        void setZobristHash(uint64_t zobristHash);
    };
}

#pragma clang diagnostic pop