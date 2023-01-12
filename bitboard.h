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
#include <stack>
#include <iostream>
#include <vector>

#include "types.h"

namespace Zagreus {
    constexpr uint64_t A_FILE = 0x0101010101010101;
    constexpr uint64_t H_FILE = 0x8080808080808080;
    constexpr uint64_t NOT_A_FILE = 0XFEFEFEFEFEFEFEFE;
    constexpr uint64_t NOT_AB_FILE = 0XFCFCFCFCFCFCFCFC;
    constexpr uint64_t NOT_GH_FILE = 0X3F3F3F3F3F3F3F3F;
    constexpr uint64_t NOT_H_FILE = 0X7F7F7F7F7F7F7F7F;
    constexpr uint64_t RANK_1 = 0x00000000000000FF;
    constexpr uint64_t RANK_4 = 0x00000000FF000000;
    constexpr uint64_t RANK_5 = 0x000000FF00000000;
    constexpr uint64_t RANK_8 = 0xFF00000000000000;
    constexpr uint64_t A1_H8_DIAG = 0x8040201008040201;
    constexpr uint64_t H1_A8_DIAG = 0x0102040810204080;
    constexpr uint64_t LIGHT_SQUARES = 0x55AA55AA55AA55AA;
    constexpr uint64_t DARK_SQUARES = 0xAA55AA55AA55AA55;

    constexpr uint8_t WHITE_KING_CHECK_BIT = 1 << 0;
    constexpr uint8_t BLACK_KING_CHECK_BIT = 1 << 1;
    constexpr uint8_t WHITE_KING_CHECK_RESET_BIT = 1 << 2;
    constexpr uint8_t BLACK_KING_CHECK_RESET_BIT = 1 << 3;

    static int mvvlvaTable[12][12] = {
            {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
            {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
            {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
            {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
            {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
            {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

            {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
            {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
            {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
            {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
            {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
            {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600}
    };

    class Bitboard {
    private:
        uint64_t pieceBB[12]{};
        PieceType pieceSquareMapping[64]{};
        uint64_t colorBB[2]{};
        uint64_t occupiedBB{};
        int8_t enPassantSquare[2]{};
        uint8_t castlingRights = 0b00001111;
        uint8_t kingInCheck = 0b00001100;
        uint8_t previousMoveFrom = 0;
        uint8_t previousMoveTo = 0;
        Line previousPvLine = {};

        uint64_t kingAttacks[64]{};
        uint64_t knightAttacks[64]{};
        uint64_t pawnAttacks[2][64]{};
        uint64_t rayAttacks[8][64]{};
        uint64_t betweenTable[64][64]{};

        PieceColor movingColor = PieceColor::NONE;
        uint8_t ply = 0;
        uint8_t halfMoveClock = 0;
        uint8_t fullmoveClock = 1;
        uint64_t moveHistory[256]{};
        uint16_t moveHistoryIndex = 0;
        uint64_t zobristHash = 0;
        uint64_t zobristConstants[789]{};

        unsigned int whiteTimeMsec = 0;
        unsigned int blackTimeMsec = 0;
        unsigned int whiteTimeIncrement = 0;
        unsigned int blackTimeIncrement = 0;

        bool hasWhiteCastled = false;
        bool hasBlackCastled = false;

        UndoData undoStack[256]{};
        uint16_t undoStackIndex = 0;
    public:
        Bitboard();

        uint64_t getPieceBoard(int pieceType);

        uint64_t getWhiteBoard();

        uint64_t getBlackBoard();

        uint64_t getOccupiedBoard() const;

        uint64_t getKingAttacks(int square);

        uint64_t getKnightAttacks(int square);

        uint64_t getQueenAttacks(int square, uint64_t occupancy);

        uint64_t getBishopAttacks(int square, uint64_t occupancy);

        uint64_t getRookAttacks(int square, uint64_t occupancy);

        uint64_t getWhitePawnAttacks(uint64_t wPawns);

        uint64_t getBlackPawnAttacks(uint64_t wPawns);

        uint64_t getWhitePawnSinglePush(uint64_t wPawns);

        uint64_t getWhitePawnDoublePush(uint64_t wPawns);

        uint64_t getBlackPawnSinglePush(uint64_t bPawns);

        uint64_t getBlackPawnDoublePush(uint64_t bPawns);

        void setPiece(int index, PieceType pieceType);

        void removePiece(int index, PieceType pieceType);

        void makeMove(int fromSquare, int toSquare, PieceType pieceType, PieceType promotionPiece);

        uint64_t getEmptyBoard() const;

        bool setFromFEN(const std::string &fen);

        void setPieceFromFENChar(char character, int index);

        void print();

        static PieceColor getOppositeColor(PieceColor color);

        static char getCharacterForPieceType(PieceType pieceType);

        void printAvailableMoves(uint64_t availableMoves);

        void printAvailableMoves(const std::vector<Move> &moves);

        uint64_t getBoardByColor(PieceColor color);

        void unmakeMove();

        PieceType getPieceOnSquare(int square);

        bool isKingInCheck(PieceColor color);

        bool isWinner(PieceColor color);

        static std::string getNotation(int index);

        void handleCastling(Square rookSquare, Square kingSquare, PieceType rookType, PieceType kingType);

        uint8_t getCastlingRights() const;

        PieceColor getMovingColor() const;

        bool isDraw();

        bool isInsufficientMaterial();

        uint64_t getZobristHash() const;

        uint8_t getPly() const;

        int getWhiteTimeMsec() const;

        int getBlackTimeMsec() const;

        void setWhiteTimeMsec(unsigned int whiteTimeMsec);

        void setBlackTimeMsec(unsigned int blackTimeMsec);

        void makeStrMove(const std::string &move);

        int getSquareFromString(std::string notation);

        bool isSquareAttackedByColor(int square, PieceColor color);

        uint64_t getSquareAttacks(int square);

        uint64_t getSquareAttacksByColor(int square, PieceColor color);

        uint64_t calculatePawnAttacks(uint64_t bb, PieceColor color);

        uint64_t getPawnAttacks(int square, PieceColor color);

        int8_t getEnPassantSquare(PieceColor color);

        uint16_t getPieceWeight(PieceType type);

        int getMostValuableVictimWeight(int attackedSquare, PieceColor attackingColor);

        int getLeastValuableAttackerWeight(int attackedSquare, PieceColor color);

        static PieceColor getPieceColor(PieceType type);

        int mvvlva(PieceType attacker, PieceType victim);

        uint64_t getRayAttacks(uint64_t occupied, Direction direction, int square);

        uint64_t getNegativeRayAttacks(uint64_t occupied, Direction direction, int square);

        void initializeRayAttacks();

        void
        pushUndoData(const uint64_t pieceBB[12], const PieceType pieceSquareMapping[64],
                     const uint64_t colorBB[2],
                     uint64_t occupiedBB, int8_t enPassantSquare[2],
                     uint8_t castlingRights, uint64_t zobristHash, uint8_t ply,
                     uint8_t halfMoveClock, uint8_t fullMoveClock, uint8_t kingInCheck, uint8_t previousMoveFrom,
                     uint8_t previousMoveTo, bool hasWhiteCastled, bool hasBlackCastled);

        int see(int square, PieceColor attackingColor);

        int getSmallestAttackerSquare(int square, PieceColor attackingColor);

        int seeCapture(int fromSquare, int toSquare, PieceColor attackingColor);

        bool isOpenFile(int square);

        bool isSemiOpenFile(int square, PieceColor color);

        bool isSemiOpenFileLenient(int square, PieceColor color);

        uint8_t getHalfMoveClock() const;

        uint8_t getFullmoveClock() const;

        bool isPinned(int attackedSquare, PieceColor attackingColor);

        bool isPinnedStraight(int attackedSquare, PieceColor attackingColor);

        void initializeBetweenLookup();

        bool isPinnedDiagonally(int attackedSquare, PieceColor attackingColor);

        uint64_t getTilesBetween(int from, int to);

        unsigned int getWhiteTimeIncrement() const;

        void setWhiteTimeIncrement(unsigned int whiteTimeIncrement);

        unsigned int getBlackTimeIncrement() const;

        void setBlackTimeIncrement(unsigned int blackTimeIncrement);

        void makeNullMove();

        bool isPawnEndgame();

        uint64_t getPawnsOnSameFile(int square, PieceColor color);

        bool isIsolatedPawn(int square, PieceColor pawnColor);

        bool isPassedPawn(int square, PieceColor pawnColor);

        uint8_t getPreviousMoveFrom() const;

        uint8_t getPreviousMoveTo() const;

        bool isHasWhiteCastled() const;

        bool isHasBlackCastled() const;

        uint64_t getZobristForMove(int fromSquare, int toSquare, PieceType pieceType, PieceType promotionPiece);

        Line& getPreviousPvLine();

        void setPreviousPvLine(Line &previousPvLine);
    };

    uint64_t soutOne(uint64_t b);

    uint64_t nortOne(uint64_t b);

    uint64_t eastOne(uint64_t b);

    uint64_t noEaOne(uint64_t b);

    uint64_t soEaOne(uint64_t b);

    uint64_t westOne(uint64_t b);

    uint64_t soWeOne(uint64_t b);

    uint64_t noWeOne(uint64_t b);

    uint64_t noNoEa(uint64_t b);

    uint64_t noEaEa(uint64_t b);

    uint64_t soEaEa(uint64_t b);

    uint64_t soSoEa(uint64_t b);

    uint64_t noNoWe(uint64_t b);

    uint64_t noWeWe(uint64_t b);

    uint64_t soWeWe(uint64_t b);

    uint64_t soSoWe(uint64_t b);

    uint64_t popcnt(uint64_t b);

    int bitscanForward(uint64_t b);

    int bitscanReverse(uint64_t b);

    uint64_t soutOccl(uint64_t pieceBB, uint64_t empty);

    uint64_t nortOccl(uint64_t pieceBB, uint64_t empty);

    uint64_t eastOccl(uint64_t pieceBB, uint64_t empty);

    uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty);

    uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty);

    uint64_t westOccl(uint64_t rooks, uint64_t empty);

    uint64_t soWeOccl(uint64_t bishops, uint64_t empty);

    uint64_t noWeOccl(uint64_t bishops, uint64_t empty);

    uint64_t getWhitePawnEastAttacks(uint64_t wPawns);

    uint64_t getWhitePawnWestAttacks(uint64_t wPawns);

    uint64_t getBlackPawnEastAttacks(uint64_t bPawns);

    uint64_t getBlackPawnWestAttacks(uint64_t bPawns);

    uint64_t calculateKnightAttacks(uint64_t knights);

    uint64_t calculateKingAttacks(uint64_t kingSet);

    uint32_t encodeMove(const Move &move);

    uint32_t encodeMove(int fromSquare, int toSquare, PieceType pieceType);
}
