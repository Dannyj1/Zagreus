//
// Created by Danny on 8-5-2022.
//

#include <cstdint>
#include <string>
#include <iostream>
#include <bitset>
#include <popcntintrin.h>

#include "bitboard.h"

namespace Chess {
    Bitboard::Bitboard() {
        uint64_t sqBB = 1;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1) {
            kingAttacks[sq] = calculateKingAttacks(sqBB);
        }

        sqBB = 1;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1) {
            knightAttacks[sq] = calculateKnightAttacks(sqBB);
        }
    }

    uint64_t Bitboard::getPieceBoard(int pieceType) {
        return pieceBB[pieceType];
    }

    uint64_t Bitboard::getWhiteBoard() {
        return whiteBB;
    }

    uint64_t Bitboard::getBlackBoard() {
        return blackBB;
    }

    uint64_t Bitboard::getEmptyBoard() {
        return ~occupiedBB;
    }

    uint64_t Bitboard::getOccupiedBoard() {
        return occupiedBB;
    }

    uint64_t Bitboard::getKingAttacks(int square) {
        return this->kingAttacks[square];
    }

    uint64_t Bitboard::getKnightAttacks(int square) {
        return this->knightAttacks[square];
    }

    uint64_t Bitboard::getQueenAttacks(uint64_t bb) {
        uint64_t queenAttacks = 0;
        uint64_t emptyBB = getEmptyBoard();

        queenAttacks |= nortOccl(bb, emptyBB);
        queenAttacks |= noEaOccl(bb, emptyBB);
        queenAttacks |= eastOccl(bb, emptyBB);
        queenAttacks |= soEaOccl(bb, emptyBB);
        queenAttacks |= soutOccl(bb, emptyBB);
        queenAttacks |= soWeOccl(bb, emptyBB);
        queenAttacks |= westOccl(bb, emptyBB);
        queenAttacks |= noWeOccl(bb, emptyBB);

        return queenAttacks;
    }

    uint64_t Bitboard::getBishopAttacks(uint64_t bb) {
        uint64_t bishopAttacks = 0;
        uint64_t emptyBB = getEmptyBoard();

        bishopAttacks |= noEaOccl(bb, emptyBB);
        bishopAttacks |= noWeOccl(bb, emptyBB);
        bishopAttacks |= soEaOccl(bb, emptyBB);
        bishopAttacks |= soWeOccl(bb, emptyBB);

        return bishopAttacks;
    }

    uint64_t Bitboard::getRookAttacks(uint64_t bb) {
        uint64_t rookAttacks = 0;
        uint64_t emptyBB = getEmptyBoard();
        
        rookAttacks |= nortOccl(bb, emptyBB);
        rookAttacks |= westOccl(bb, emptyBB);
        rookAttacks |= soutOccl(bb, emptyBB);
        rookAttacks |= eastOccl(bb, emptyBB);

        return rookAttacks;
    }

    uint64_t Bitboard::getWhitePawnAttacks(uint64_t wPawns) {
        return getWhitePawnEastAttacks(wPawns) & getWhitePawnWestAttacks(wPawns);
    }

    uint64_t Bitboard::getBlackPawnAttacks(uint64_t wPawns) {
        return getBlackPawnEastAttacks(wPawns) & getBlackPawnWestAttacks(wPawns);
    }

    uint64_t Bitboard::getWhitePawnSinglePush(uint64_t wPawns) {
        return nortOne(wPawns) & getEmptyBoard();
    }

    uint64_t Bitboard::getWhitePawnDoublePush(uint64_t wPawns) {
        uint64_t singlePushs = getWhitePawnSinglePush(wPawns);

        return singlePushs | (nortOne(singlePushs) & getEmptyBoard() & RANK_4);
    }

    uint64_t Bitboard::getBlackPawnSinglePush(uint64_t bPawns) {
        return soutOne(bPawns) & getEmptyBoard();
    }

    uint64_t Bitboard::getBlackPawnDoublePush(uint64_t bPawns) {
        uint64_t singlePushs = getBlackPawnSinglePush(bPawns);

        return singlePushs | (soutOne(singlePushs) & getEmptyBoard() & RANK_5);
    }

    void Bitboard::setPiece(int index, PieceType pieceType) {
        pieceBB[pieceType] |= 1ULL << index;
        occupiedBB |= 1ULL << index;

        if (pieceType % 2 == 0) {
            whiteBB |= 1ULL << index;
        } else {
            blackBB |= 1ULL << index;
        }
    }

    void Bitboard::removePiece(int index, PieceType pieceType) {
        pieceBB[pieceType] &= ~(1ULL << index);
        occupiedBB &= ~(1ULL << index);
        whiteBB &= ~(1ULL << index);
        blackBB &= ~(1ULL << index);
    }

    void Bitboard::makeMove(int fromSquare, int toSquare, PieceType pieceType) {
        removePiece(fromSquare, pieceType);
        setPiece(toSquare, pieceType);
    }

    bool Bitboard::setFromFEN(const std::string &fen) {
        int index = 63;
        int spaces = 0;

        for (const char character : fen) {
            if (character == ' ') {
                spaces++;
                continue;
            }

            if (spaces == 0) {
                if (character == '/') {
                    continue;
                }

                if (character >= '1' && character <= '8') {
                    index += character - '0';
                    continue;
                }

                if (character >= 'A' && character <= 'z') {
                    setPieceFromFENChar(character, index);
                    index--;
                    continue;
                } else {
                    std::cout << "Invalid character!" << std::endl;
                    return false;
                }
            }

            /*if (spaces == 1) {
                gameState = character == 'w' ? GameState::WHITE_TURN : GameState::BLACK_TURN;
                continue;
            }

            if (spaces == 2) {
                if (character == '-') {
                    continue;
                }

                if (character == 'K') {
                    TileLocation whiteRookLoc = getPiecePosition(WHITE_H_ROOK_ID);
                    Piece* piece = getTileUnsafe(whiteRookLoc.x, whiteRookLoc.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                if (character == 'Q') {
                    TileLocation whiteQueenLocation = getPiecePosition(WHITE_A_ROOK_ID);
                    Piece* piece = getTileUnsafe(whiteQueenLocation.x, whiteQueenLocation.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                if (character == 'k') {
                    TileLocation blackKingLocation = getPiecePosition(BLACK_A_ROOK_ID);
                    Piece* piece = getTileUnsafe(blackKingLocation.x, blackKingLocation.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                if (character == 'q') {
                    TileLocation blackQueenLocation = getPiecePosition(BLACK_H_ROOK_ID);
                    Piece* piece = getTileUnsafe(blackQueenLocation.x, blackQueenLocation.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                continue;
            }*/
        }

        return true;
    }

    PieceColor Bitboard::getPieceColor(PieceType type) {
        if (type % 2 == 0) {
            return PieceColor::White;
        } else {
            return PieceColor::Black;
        }
    }

    void Bitboard::setPieceFromFENChar(const char character, int index) {
        // Uppercase = White, lowercase = black
        switch (character) {
            case 'P':
                setPiece(index, PieceType::WhitePawn);
                break;
            case 'p':
                setPiece(index, PieceType::BlackPawn);
                break;
            case 'N':
                setPiece(index, PieceType::WhiteKnight);
                break;
            case 'n':
                setPiece(index, PieceType::BlackKnight);
                break;
            case 'B':
                setPiece(index, PieceType::WhiteBishop);
                break;
            case 'b':
                setPiece(index, PieceType::BlackBishop);
                break;
            case 'R':
                setPiece(index, PieceType::WhiteRook);
                break;
            case 'r':
                setPiece(index, PieceType::BlackRook);
                break;
            case 'Q':
                setPiece(index, PieceType::WhiteQueen);
                break;
            case 'q':
                setPiece(index, PieceType::BlackQueen);
                break;
            case 'K':
                setPiece(index, PieceType::WhiteKing);
                break;
            case 'k':
                setPiece(index, PieceType::BlackKing);
                break;
        }
    }

    void Bitboard::print() {
        int bbAmount = sizeof(pieceBB) / sizeof(uint64_t);
        char boardChars[64];

        std::fill_n(boardChars, 64, ' ');

        for (int i = 0; i < bbAmount; i++) {
            uint64_t bb = pieceBB[i];
            char pieceChar = getCharacterForPieceType(static_cast<PieceType>(i));

            while (bb) {
                uint64_t index = bitscanForward(bb);
                bb &= ~(1ULL << index);

                boardChars[index] = pieceChar;
            }
        }

        std::cout << "---------------------------------" << std::endl;
        std::cout << "| ";

        for (int i = 0; i < 64; i++) {
            char boardChar = boardChars[63 - i];

            std::cout << boardChar << " | ";

            if ((i + 1) % 8 == 0 && i != 63) {
                std::cout << std::endl << "| ";
            }
        }

        std::cout << std::endl << "---------------------------------" << std::endl;
    }

    char Bitboard::getCharacterForPieceType(PieceType pieceType) {
        switch (pieceType) {
            case WhitePawn:
                return 'P';
            case BlackPawn:
                return 'p';
            case WhiteKnight:
                return 'N';
            case BlackKnight:
                return 'n';
            case WhiteBishop:
                return 'B';
            case BlackBishop:
                return 'b';
            case WhiteRook:
                return 'R';
            case BlackRook:
                return 'r';
            case WhiteQueen:
                return 'Q';
            case BlackQueen:
                return 'q';
            case WhiteKing:
                return 'K';
            case BlackKing:
                return 'k';
        }
    }

    uint64_t soutOne(uint64_t b) {
        return b >> 8;
    }

    uint64_t nortOne(uint64_t b) {
        return b << 8;
    }

    uint64_t eastOne(uint64_t b) {
        return (b << 1) & NOT_A_FILE;
    }

    uint64_t noEaOne(uint64_t b) {
        return (b << 9) & NOT_A_FILE;
    }

    uint64_t soEaOne(uint64_t b) {
        return (b >> 7) & NOT_A_FILE;
    }

    uint64_t westOne(uint64_t b) {
        return (b >> 1) & NOT_H_FILE;
    }

    uint64_t soWeOne(uint64_t b) {
        return (b >> 9) & NOT_H_FILE;
    }

    uint64_t noWeOne(uint64_t b) {
        return (b << 7) & NOT_H_FILE;
    }

    uint64_t noNoEa(uint64_t b) {
        return (b << 17) & NOT_A_FILE;
    }

    uint64_t noEaEa(uint64_t b) {
        return (b << 10) & NOT_AB_FILE;
    }

    uint64_t soEaEa(uint64_t b) {
        return (b >>  6) & NOT_AB_FILE;
    }

    uint64_t soSoEa(uint64_t b) {
        return (b >> 15) & NOT_A_FILE;
    }

    uint64_t noNoWe(uint64_t b) {
        return (b << 15) & NOT_H_FILE;
    }

    uint64_t noWeWe(uint64_t b) {
        return (b <<  6) & NOT_GH_FILE;

    }
    uint64_t soWeWe(uint64_t b) {
        return (b >> 10) & NOT_GH_FILE;
    }

    uint64_t soSoWe(uint64_t b) {
        return (b >> 17) & NOT_H_FILE;
    }

    uint64_t popcnt(uint64_t b) {
        return __builtin_popcountll(b);
    }

    unsigned long bitscanForward(uint64_t b) {
        unsigned long index;
        _BitScanForward64(&index, b);
        return index;
    }

    unsigned long bitscanReverse(uint64_t b) {
        unsigned long index;

        _BitScanReverse64(&index, b);
        return index;
    }

    uint64_t soutOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        flood |= pieceBB = (pieceBB >> 8) & empty;
        flood |= pieceBB = (pieceBB >> 8) & empty;
        flood |= pieceBB = (pieceBB >> 8) & empty;
        flood |= pieceBB = (pieceBB >> 8) & empty;
        flood |= pieceBB = (pieceBB >> 8) & empty;
        flood |= (pieceBB >> 8) & empty;
        return flood >> 8;
    }

    uint64_t nortOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        flood |= pieceBB = (pieceBB << 8) & empty;
        flood |= pieceBB = (pieceBB << 8) & empty;
        flood |= pieceBB = (pieceBB << 8) & empty;
        flood |= pieceBB = (pieceBB << 8) & empty;
        flood |= pieceBB = (pieceBB << 8) & empty;
        flood |= (pieceBB << 8) & empty;
        return flood << 8;
    }

    uint64_t eastOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        empty &= NOT_A_FILE;
        flood |= pieceBB = (pieceBB << 1) & empty;
        flood |= pieceBB = (pieceBB << 1) & empty;
        flood |= pieceBB = (pieceBB << 1) & empty;
        flood |= pieceBB = (pieceBB << 1) & empty;
        flood |= pieceBB = (pieceBB << 1) & empty;
        flood |= (pieceBB << 1) & empty;
        return (flood << 1) & NOT_A_FILE;
    }

    uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        empty &= NOT_A_FILE;
        flood |= pieceBB = (pieceBB << 9) & empty;
        flood |= pieceBB = (pieceBB << 9) & empty;
        flood |= pieceBB = (pieceBB << 9) & empty;
        flood |= pieceBB = (pieceBB << 9) & empty;
        flood |= pieceBB = (pieceBB << 9) & empty;
        flood |= (pieceBB << 9) & empty;
        return (flood << 9) & NOT_A_FILE;
    }

    uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        empty &= NOT_A_FILE;
        flood |= pieceBB = (pieceBB >> 7) & empty;
        flood |= pieceBB = (pieceBB >> 7) & empty;
        flood |= pieceBB = (pieceBB >> 7) & empty;
        flood |= pieceBB = (pieceBB >> 7) & empty;
        flood |= pieceBB = (pieceBB >> 7) & empty;
        flood |= (pieceBB >> 7) & empty;
        return (flood >> 7) & NOT_A_FILE;
    }

    uint64_t westOccl(uint64_t rooks, uint64_t empty) {
        uint64_t flood = rooks;
        empty &= NOT_H_FILE;
        flood |= rooks = (rooks >> 1) & empty;
        flood |= rooks = (rooks >> 1) & empty;
        flood |= rooks = (rooks >> 1) & empty;
        flood |= rooks = (rooks >> 1) & empty;
        flood |= rooks = (rooks >> 1) & empty;
        flood |= (rooks >> 1) & empty;
        return (flood >> 1) & NOT_H_FILE;
    }

    uint64_t soWeOccl(uint64_t bishops, uint64_t empty) {
        uint64_t flood = bishops;
        empty &= NOT_H_FILE;
        flood |= bishops = (bishops >> 9) & empty;
        flood |= bishops = (bishops >> 9) & empty;
        flood |= bishops = (bishops >> 9) & empty;
        flood |= bishops = (bishops >> 9) & empty;
        flood |= bishops = (bishops >> 9) & empty;
        flood |= (bishops >> 9) & empty;
        return (flood >> 9) & NOT_H_FILE;
    }

    uint64_t noWeOccl(uint64_t bishops, uint64_t empty) {
        uint64_t flood = bishops;
        empty &= NOT_H_FILE;
        flood |= bishops = (bishops << 7) & empty;
        flood |= bishops = (bishops << 7) & empty;
        flood |= bishops = (bishops << 7) & empty;
        flood |= bishops = (bishops << 7) & empty;
        flood |= bishops = (bishops << 7) & empty;
        flood |= (bishops << 7) & empty;
        return (flood << 7) & NOT_H_FILE;
    }

    uint64_t getWhitePawnEastAttacks(uint64_t wPawns) {
        return noEaOne(wPawns);
    }

    uint64_t getWhitePawnWestAttacks(uint64_t wPawns) {
        return noWeOne(wPawns);
    }

    uint64_t calculateKnightAttacks(uint64_t knights) {
        uint64_t l1 = (knights >> 1) & NOT_H_FILE;
        uint64_t l2 = (knights >> 2) & NOT_GH_FILE;
        uint64_t r1 = (knights << 1) & NOT_A_FILE;
        uint64_t r2 = (knights << 2) & NOT_AB_FILE;
        uint64_t h1 = l1 | r1;
        uint64_t h2 = l2 | r2;
        return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
    }

    uint64_t calculateKingAttacks(uint64_t kingSet) {
        uint64_t attacks = eastOne(kingSet) | westOne(kingSet);
        kingSet |= attacks;
        attacks |= nortOne(kingSet) | soutOne(kingSet);

        return attacks;
    }

    uint64_t getBlackPawnEastAttacks(uint64_t bPawns) {
        return soEaOne(bPawns);
    }

    uint64_t getBlackPawnWestAttacks(uint64_t bPawns) {
        return soWeOne(bPawns);
    }
}