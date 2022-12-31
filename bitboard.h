//
// Created by Danny on 8-5-2022.
//

#pragma once

#include <cstdint>
#include <stack>

namespace Chess {
    enum PieceType {
        EMPTY = -1,
        WHITE_PAWN = 0,
        BLACK_PAWN = 1,
        WHITE_KNIGHT = 2,
        BLACK_KNIGHT = 3,
        WHITE_BISHOP = 4,
        BLACK_BISHOP = 5,
        WHITE_ROOK = 6,
        BLACK_ROOK = 7,
        WHITE_QUEEN = 8,
        BLACK_QUEEN = 9,
        WHITE_KING = 10,
        BLACK_KING = 11,
    };

    enum Square {
        H1, G1, F1, E1, D1, C1, B1, A1,
        H2, G2, F2, E2, D2, C2, B2, A2,
        H3, G3, F3, E3, D3, C3, B3, A3,
        H4, G4, F4, E4, D4, C4, B4, A4,
        H5, G5, F5, E5, D5, C5, B5, A5,
        H6, G6, F6, E6, D6, C6, B6, A6,
        H7, G7, F7, E7, D7, C7, B7, A7,
        H8, G8, F8, E8, D8, C8, B8, A8
    };

    struct Move {
        uint64_t fromSquare;
        uint64_t toSquare;
        PieceType pieceType;
        PieceType promotionPiece = PieceType::EMPTY;
    };

    struct UndoData {
        uint64_t pieceBB[12];
        PieceType pieceSquareMapping[64]{PieceType::EMPTY};
        uint64_t whiteBB;
        uint64_t blackBB;
        uint64_t occupiedBB;
        int whiteEnPassantSquare;
        int blackEnPassantSquare;
        uint8_t castlingRights;
        uint64_t whiteAttackMap;
        uint64_t blackAttackMap;
        uint64_t zobristHash;
        int movesMade;
        int halfMoveClock;
        int fullMoveClock;
    };

    enum PieceColor {
        NONE = -1,
        WHITE = 0,
        BLACK = 1
    };

    enum CastlingRights {
        WHITE_KINGSIDE = 1 << 0,
        WHITE_QUEENSIDE = 1 << 1,
        BLACK_KINGSIDE = 1 << 2,
        BLACK_QUEENSIDE = 1 << 3
    };

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

    constexpr PieceColor oppositeColors[2]{PieceColor::BLACK, PieceColor::WHITE};

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
    private:
        uint64_t pieceBB[12]{};
        PieceType pieceSquareMapping[64]{};
        uint64_t whiteBB{};
        uint64_t blackBB{};
        uint64_t occupiedBB{};
        int blackEnPassantSquare = -1;
        int whiteEnPassantSquare = -1;
        uint8_t castlingRights = 0b00001111;
        uint64_t whiteAttackMap = 0;
        uint64_t blackAttackMap = 0;

        uint64_t kingAttacks[64]{};
        uint64_t knightAttacks[64]{};

        PieceColor movingColor;
        int movesMade = 0;
        int halfmoveClock = 0;
        int fullmoveClock = 1;
        std::vector<uint64_t> moveHistory;
        uint64_t zobristHash = 0;
        uint64_t zobristConstants[789]{};

        unsigned int whiteTimeMsec = 0;
        unsigned int blackTimeMsec = 0;

        std::vector<UndoData> undoStack{};
    public:
        Bitboard();

        uint64_t getPieceBoard(int pieceType);

        uint64_t getWhiteBoard();

        uint64_t getBlackBoard();

        uint64_t getOccupiedBoard();

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

        PieceColor getPieceColor(PieceType type);

        uint64_t getEmptyBoard();

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

        uint64_t getAttackedTilesForColor(PieceColor color);

        bool isKingInCheck(PieceColor color);

        uint64_t calculateAttackedTilesForColor(PieceColor color);

        uint64_t getWhiteAttacksBB();

        uint64_t getBlackAttacksBB();

        bool isWinner(PieceColor color);

        static std::string getNotation(int index);

        int getBlackEnPassantSquare() const;

        int getWhiteEnPassantSquare() const;

        void handleCastling(Square rookSquare, Square kingSquare, PieceType rookType, PieceType kingType);

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
    };
}
