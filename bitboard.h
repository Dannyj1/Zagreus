//
// Created by Danny on 8-5-2022.
//

#pragma once

#include <cstdint>
#include <stack>
#include <iostream>
#include <vector>

#include "types.h"
#include "collections/move_list.h"
#include "collections/undo_stack.h"

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

    unsigned long bitscanForward(uint64_t b);

    unsigned long bitscanReverse(uint64_t b);

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

    class Bitboard {
    public:
        uint64_t pieceBB[12]{};
        PieceType pieceSquareMapping[64]{};
        uint64_t colorBB[2]{};
        uint64_t occupiedBB{};
        int enPassantSquare[2]{};
        uint8_t castlingRights = 0b00001111;
        uint64_t attacksFrom[64]{};
        uint64_t attacksTo[64]{};

        uint64_t kingAttacks[64]{};
        uint64_t knightAttacks[64]{};
        uint64_t pawnAttacks[2][64]{};

        PieceColor movingColor = PieceColor::WHITE;
        unsigned int movesMade = 0;
        unsigned int halfMoveClock = 0;
        int fullmoveClock = 1;
        uint64_t moveHistory[256]{};
        int moveHistoryIndex = 0;
        uint64_t zobristHash = 0;
        uint64_t zobristConstants[789]{};

        unsigned int whiteTimeMsec = 0;
        unsigned int blackTimeMsec = 0;

        UndoStack undoStack{};
        std::vector<Move> generatedMoves{};
    public:
        Bitboard();

        uint64_t getPieceBoard(int pieceType);

        uint64_t getWhiteBoard();

        uint64_t getBlackBoard();

        uint64_t getOccupiedBoard() const;

        uint64_t getKingAttacks(int square);

        uint64_t getKnightAttacks(int square);

        uint64_t getQueenAttacks(uint64_t bb);

        uint64_t getBishopAttacks(uint64_t bb);

        uint64_t getRookAttacks(uint64_t bb);

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

        void printAvailableMoves(const std::vector<Move> &availableMoves);

        uint64_t getBoardByColor(PieceColor color);

        void unmakeMove();

        PieceType getPieceOnSquare(int square);

        bool isKingInCheck(PieceColor color);

        bool isWinner(PieceColor color);

        static std::string getNotation(int index);

        void handleCastling(Square rookSquare, Square kingSquare, PieceType rookType, PieceType kingType, uint64_t updateMask);

        uint8_t getCastlingRights() const;

        PieceColor getMovingColor() const;

        bool isDraw();

        bool isInsufficientMaterial();

        uint64_t getZobristHash() const;

        int getMovesMade() const;

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

        int getEnPassantSquare(PieceColor color);
    };
}
