/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#pragma once

#include <x86intrin.h>

#include <cassert>
#include <cstdint>
#include <string>

#include "bitwise.h"
#include "movelist_pool.h"
#include "types.h"
#include "utils.h"

namespace Zagreus {
class Bitboard {
private:
    uint64_t pieceBB[12]{};
    PieceType pieceSquareMapping[64]{};
    uint64_t colorBB[2]{};
    uint64_t occupiedBB{0ULL};

    PieceColor movingColor = NONE;
    uint16_t ply = 0;
    uint8_t halfMoveClock = 0;
    uint8_t fullmoveClock = 1;
    int8_t enPassantSquare = NO_SQUARE;
    uint8_t castlingRights = 0b00001111;

    uint64_t zobristHash = 0ULL;

    UndoData undoStack[MAX_PLY]{};
    uint64_t moveHistory[MAX_PLY]{};
    Line pvLine{};

    int pstValues[4]{};

    Move previousMove{};
    int materialCount[12]{};

public:
    uint64_t getPieceBoard(PieceType pieceType);

    template <PieceColor color>
    uint64_t getColorBoard() {
        return colorBB[color];
    }

    template <PieceColor color>
    uint64_t getPawnDoublePush(uint64_t pawns) {
        const uint64_t singlePush = getPawnSinglePush<color>(pawns);

        if (color == WHITE) {
            return singlePush | (nortOne(singlePush) & getEmptyBoard() & RANK_4);
        }

        if (color == BLACK) {
            return singlePush | (soutOne(singlePush) & getEmptyBoard() & RANK_5);
        }

        return 0;
    }

    template <PieceColor color>
    uint64_t getPawnSinglePush(uint64_t pawns) {
        if (color == WHITE) {
            return nortOne(pawns) & getEmptyBoard();
        }

        if (color == BLACK) {
            return soutOne(pawns) & getEmptyBoard();
        }

        return 0;
    }

    uint64_t getOccupiedBoard() const;

    uint64_t getEmptyBoard() const;

    PieceType getPieceOnSquare(int8_t square);

    uint64_t getQueenAttacks(int8_t square);

    uint64_t getQueenAttacks(int8_t square, uint64_t occupancy);

    uint64_t getBishopAttacks(int8_t square);

    static uint64_t getBishopAttacks(int8_t square, uint64_t occupancy);

    uint64_t getRookAttacks(int8_t square);

    static uint64_t getRookAttacks(int8_t square, uint64_t occupancy);

    void setPiece(int8_t square, PieceType piece);

    void removePiece(int8_t square, PieceType piece);

    template <PieceType piece>
    int getMaterialCount() {
        return materialCount[piece];
    }

    void makeMove(Move& move);
    int getHalfMoveClock();

    void unmakeMove(Move& move);

    void print();

    void printAvailableMoves(MoveList* moves);

    bool setFromFen(const std::string& fen);

    bool setFromFenTuner(const std::string& fen);

    bool isDraw();

    template <PieceColor color>
    bool isWinner();

    void setPieceFromFENChar(char character, int index);

    PieceColor getMovingColor() const;

    void setMovingColor(PieceColor movingColor);

    uint64_t getSquareAttacks(int8_t square);

    template <PieceColor color>
    uint64_t getSquareAttackersByColor(int8_t square) {
        if (color == WHITE) {
            uint64_t queenBB = getPieceBoard(WHITE_QUEEN);
            uint64_t rookBB = getPieceBoard(WHITE_ROOK);
            uint64_t bishopBB = getPieceBoard(WHITE_BISHOP);

            uint64_t pawnAttacks = getPawnAttacks<BLACK>(square) & getPieceBoard(WHITE_PAWN);
            uint64_t bishopAttacks = getBishopAttacks(square) & bishopBB;
            uint64_t knightAttacks = getKnightAttacks(square) & getPieceBoard(WHITE_KNIGHT);
            uint64_t kingAttacks = getKingAttacks(square) & getPieceBoard(WHITE_KING);
            uint64_t rookAttacks = getRookAttacks(square) & rookBB;
            uint64_t queenAttacks = getQueenAttacks(square) & queenBB;

            return pawnAttacks | bishopAttacks | knightAttacks | rookAttacks | queenAttacks |
                   kingAttacks;
        } else {
            uint64_t queenBB = getPieceBoard(BLACK_QUEEN);
            uint64_t rookBB = getPieceBoard(BLACK_ROOK);
            uint64_t bishopBB = getPieceBoard(BLACK_BISHOP);

            uint64_t pawnAttacks = getPawnAttacks<WHITE>(square) & getPieceBoard(BLACK_PAWN);
            uint64_t bishopAttacks = getBishopAttacks(square) & bishopBB;
            uint64_t knightAttacks = getKnightAttacks(square) & getPieceBoard(BLACK_KNIGHT);
            uint64_t rookAttacks = getRookAttacks(square) & rookBB;
            uint64_t queenAttacks = getQueenAttacks(square) & queenBB;
            uint64_t kingAttacks = getKingAttacks(square) & getPieceBoard(BLACK_KING);

            return pawnAttacks | bishopAttacks | knightAttacks | rookAttacks | queenAttacks |
                   kingAttacks;
        }
    }

    template <PieceColor color>
    bool isSquareAttackedByColor(int8_t square) {
        return getSquareAttackersByColor<color>(square) != 0;
    }

    template <PieceColor color>
    bool isKingInCheck() {
        constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
        uint64_t kingBB = getPieceBoard(color == WHITE ? WHITE_KING : BLACK_KING);
        int8_t kingLocation = bitscanForward(kingBB);

        return isSquareAttackedByColor<OPPOSITE_COLOR>(kingLocation);
    }

    int8_t getEnPassantSquare() const;

    void setEnPassantSquare(int8_t enPassantSquare);

    uint8_t getCastlingRights() const;

    void setCastlingRights(uint8_t castlingRights);

    bool isInsufficientMaterial();

    uint64_t getZobristHash() const;

    void setZobristHash(uint64_t zobristHash);

    bool makeStrMove(const std::string& strMove);

    Line getPvLine();

    void setPvLine(Line& pvLine);

    uint16_t getPly() const;

    void setPly(uint16_t ply);

    bool isOpenFile(int8_t square);

    template <PieceColor color>
    bool isSemiOpenFile(int8_t square) {
        uint64_t fileMask = getFile(square);
        if (color == WHITE) {
            uint64_t ownOccupied = getPieceBoard(WHITE_PAWN);
            uint64_t opponentOccupied = getPieceBoard(BLACK_PAWN);

            return fileMask == (fileMask & ~ownOccupied) && fileMask != (
                       fileMask & ~opponentOccupied);
        } else {
            uint64_t ownOccupied = getPieceBoard(BLACK_PAWN);
            uint64_t opponentOccupied = getPieceBoard(WHITE_PAWN);

            return fileMask == (fileMask & ~ownOccupied) && fileMask != (
                       fileMask & ~opponentOccupied);
        }
    }

    // Also returns true when it is an open file
    template <PieceColor color>
    bool isSemiOpenFileLenient(int8_t square) {
        uint64_t fileMask = getFile(square);

        if (color == WHITE) {
            uint64_t ownOccupied = getPieceBoard(WHITE_PAWN);
            return fileMask == (fileMask & ~ownOccupied);
        } else {
            uint64_t ownOccupied = getPieceBoard(BLACK_PAWN);
            return fileMask == (fileMask & ~ownOccupied);
        }
    }

    template <PieceColor attackingColor>
    int seeCapture(int8_t fromSquare, int8_t toSquare) {
        constexpr PieceColor OPPOSITE_COLOR = attackingColor == WHITE ? BLACK : WHITE;
        PieceType movingPiece = pieceSquareMapping[fromSquare];
        PieceType capturedPieceType = pieceSquareMapping[toSquare];
        int captureScore = mvvlva(movingPiece, capturedPieceType);
        Move move{fromSquare, toSquare, movingPiece, captureScore};

        makeMove(move);
        int score = getPieceWeight(capturedPieceType) - see<OPPOSITE_COLOR>(toSquare);
        unmakeMove(move);

        return score;
    }

    template <PieceColor attackingColor>
    int8_t getSmallestAttackerSquare(int8_t square) {
        uint64_t attacks = getSquareAttackersByColor<attackingColor>(square);
        int8_t smallestAttackerSquare = NO_SQUARE;
        int smallestAttackerWeight = 999999999;

        while (attacks) {
            int attackerSquare = popLsb(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight < smallestAttackerWeight) {
                smallestAttackerWeight = weight;
                smallestAttackerSquare = attackerSquare;
            }
        }

        return smallestAttackerSquare;
    }

    template <PieceColor attackingColor>
    int see(int8_t square) {
        constexpr PieceColor OPPOSITE_COLOR = attackingColor == WHITE ? BLACK : WHITE;
        int score = 0;
        int8_t smallestAttackerSquare = getSmallestAttackerSquare<attackingColor>(square);

        if (smallestAttackerSquare != NO_SQUARE) {
            PieceType movingPiece = pieceSquareMapping[smallestAttackerSquare];
            PieceType capturedPieceType = pieceSquareMapping[square];
            int captureScore = mvvlva(movingPiece, capturedPieceType);
            Move move{smallestAttackerSquare, square, movingPiece, captureScore};
            makeMove(move);
            score = std::max(0, getPieceWeight(capturedPieceType) - see<OPPOSITE_COLOR>(square));
            unmakeMove(move);
        }

        return score;
    }

    // See, but after a move has already been made. We just check if the opponent can win material.
    // This method is basically seeCapture where the move has already been made. It is used to analyze a move already made to see if it appears to be safe.
    template <PieceColor movedColor>
    int seeOpponent(int8_t square) {
        // moved color is the color that just moved
        constexpr PieceColor OPPOSITE_COLOR = movedColor == WHITE ? BLACK : WHITE;
        int score = NO_CAPTURE_SCORE;
        int8_t smallestAttackerSquare = getSmallestAttackerSquare<OPPOSITE_COLOR>(square);

        if (smallestAttackerSquare != NO_SQUARE) {
            PieceType movingPiece = pieceSquareMapping[smallestAttackerSquare];
            PieceType capturedPieceType = pieceSquareMapping[square];
            int captureScore = mvvlva(movingPiece, capturedPieceType);
            Move move{smallestAttackerSquare, square, movingPiece, captureScore};
            makeMove(move);
            score = getPieceWeight(capturedPieceType) - see<OPPOSITE_COLOR>(square);
            unmakeMove(move);
        }

        return score;
    }

    [[nodiscard]] const Move& getPreviousMove() const;

    uint64_t getFile(int8_t square);

    template <PieceColor color>
    uint64_t getPawnsOnSameFile(int8_t square) {
        return pieceBB[WHITE_PAWN + color] & getFile(square);
    }

    template <PieceColor color>
    bool isIsolatedPawn(int8_t square) {
        uint64_t neighborMask = 0;

        if (square % 8 != 0) {
            neighborMask |= getFile(square - 1);
        }

        if (square % 8 != 7) {
            neighborMask |= getFile(square + 1);
        }

        if (color == WHITE) {
            return !(neighborMask & getPieceBoard(WHITE_PAWN));
        } else {
            return !(neighborMask & getPieceBoard(BLACK_PAWN));
        }
    }

    template <PieceColor color>
    bool isPassedPawn(int8_t square) {
        Direction direction = color == WHITE ? NORTH : SOUTH;
        uint64_t neighborMask = getRayAttack(square, direction);
        uint64_t pawnBB = getPieceBoard(color == WHITE ? WHITE_PAWN : BLACK_PAWN);

        if (neighborMask & pawnBB) {
            return false;
        }

        if (square % 8 != 0) {
            // neighboring file
            neighborMask |= getRayAttack(square - 1, direction);
        }

        if (square % 8 != 7) {
            neighborMask |= getRayAttack(square + 1, direction);
        }

        if (color == WHITE) {
            return !(neighborMask & getPieceBoard(BLACK_PAWN));
        } else {
            return !(neighborMask & getPieceBoard(WHITE_PAWN));
        }
    }

    bool hasMinorOrMajorPieces();

    template <PieceColor color>
    bool hasMinorOrMajorPieces() {
        if (color == WHITE) {
            return getColorBoard<color>() & ~(
                       getPieceBoard(WHITE_PAWN) | getPieceBoard(WHITE_KING));
        } else {
            return getColorBoard<color>() & ~(
                       getPieceBoard(BLACK_PAWN) | getPieceBoard(BLACK_KING));
        }
    }

    template <PieceColor color>
    int getAmountOfMinorOrMajorPieces() {
        if (color == WHITE) {
            return popcnt(getColorBoard<color>() &
                          ~(getPieceBoard(WHITE_PAWN) | getPieceBoard(WHITE_KING)));
        } else {
            return popcnt(getColorBoard<color>() &
                          ~(getPieceBoard(BLACK_PAWN) | getPieceBoard(BLACK_KING)));
        }
    }

    template <PieceColor color>
    int getAmountOfPawns() {
        return popcnt(getColorBoard<color>() & getPieceBoard(color == WHITE
                                                                 ? WHITE_PAWN
                                                                 : BLACK_PAWN));
    }

    template <PieceColor color>
    int getAmountOfPieces() {
        return popcnt(getColorBoard<color>());
    }

    void makeNullMove();

    void unmakeNullMove();

    [[nodiscard]] int getWhiteMidgamePst() const;

    [[nodiscard]] int getWhiteEndgamePst() const;

    [[nodiscard]] int getBlackMidgamePst() const;

    [[nodiscard]] int getBlackEndgamePst() const;

    int getAmountOfMinorOrMajorPieces();
};
} // namespace Zagreus

#pragma clang diagnostic pop