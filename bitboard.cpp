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

#include <random>

#include "bitboard.h"
#include "bitwise.h"
#include "magics.h"
#include "utils.h"
#include "senjo/Output.h"

namespace Zagreus {
    Bitboard::Bitboard() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        gen.seed(24420221607);
        std::uniform_int_distribution<uint64_t> dis;

/*        for (uint64_t &zobristConstant : zobristConstants) {
            zobristConstant = dis(gen);
        }*/

        for (PieceType &type : pieceSquareMapping) {
            type = PieceType::EMPTY;
        }

        uint64_t sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            kingAttacks[sq] = calculateKingAttacks(sqBB) & ~sqBB;
        }

        sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            knightAttacks[sq] = calculateKnightAttacks(sqBB) & ~sqBB;
        }

        sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            pawnAttacks[PieceColor::WHITE][sq] = calculatePawnAttacks<PieceColor::WHITE>(sqBB) & ~sqBB;
            pawnAttacks[PieceColor::BLACK][sq] = calculatePawnAttacks<PieceColor::BLACK>(sqBB) & ~sqBB;
        }

        initializeBetweenLookup();
    }

    uint64_t Bitboard::getOccupiedBoard() {
        return occupiedBB;
    }

    uint64_t Bitboard::getEmptyBoard() {
        return ~occupiedBB;
    }

    PieceColor Bitboard::getMovingColor() const {
        return movingColor;
    }

    void Bitboard::setMovingColor(PieceColor movingColor) {
        Bitboard::movingColor = movingColor;
    }

    PieceType Bitboard::getPieceOnSquare(int8_t square) {
        assert(square >= 0 && square < 64);
        return pieceSquareMapping[square];
    }

    uint64_t Bitboard::getKingAttacks(int8_t square) {
        assert(square >= 0 && square < 64);
        return kingAttacks[square];
    }

    uint64_t Bitboard::getKnightAttacks(int8_t square) {
        assert(square >= 0 && square < 64);
        return knightAttacks[square];
    }

    uint64_t Bitboard::getQueenAttacks(int8_t square) {
        assert(square >= 0 && square < 64);
        return getBishopAttacks(square) | getRookAttacks(square);
    }

    uint64_t Bitboard::getBishopAttacks(int8_t square) {
        assert(square >= 0 && square < 64);
        uint64_t occupancy = getOccupiedBoard();
        occupancy &= getBishopMask(square);
        occupancy *= getBishopMagic(square);
        occupancy >>= 64 - BBits[square];

        return getBishopMagicAttacks(square, occupancy);
    }

    uint64_t Bitboard::getRookAttacks(int8_t square) {
        assert(square >= 0 && square < 64);
        uint64_t occupancy = getOccupiedBoard();
        occupancy &= getRookMask(square);
        occupancy *= getRookMagic(square);
        occupancy >>= 64 - RBits[square];

        return getRookMagicAttacks(square, occupancy);
    }

    void Bitboard::setPiece(int8_t square, PieceType piece) {
        assert(square >= 0 && square < 64);
        pieceBB[piece] |= 1ULL << square;
        occupiedBB |= 1ULL << square;
        colorBB[piece % 2] |= 1ULL << square;
        pieceSquareMapping[square] = piece;
    }

    void Bitboard::removePiece(int8_t square, PieceType piece) {
        assert(square >= 0 && square < 64);
        pieceBB[piece] &= ~(1ULL << square);
        occupiedBB &= ~(1ULL << square);
        colorBB[piece % 2] &= ~(1ULL << square);
        pieceSquareMapping[square] = PieceType::EMPTY;
    }

    void Bitboard::makeMove(Move &move) {
        assert(move.from >= 0 && move.from < 64);
        assert(move.to >= 0 && move.to < 64);

        PieceType capturedPiece = getPieceOnSquare(move.to);

        ply += 1;
        undoStack[ply].capturedPiece = capturedPiece;
        undoStack[ply].halfMoveClock = halfMoveClock;
        undoStack[ply].enPassantSquare = enPassantSquare;
        undoStack[ply].castlingRights = castlingRights;

        if (capturedPiece != PieceType::EMPTY) {
            removePiece(move.to, capturedPiece);
            halfMoveClock = 0;
        }

        removePiece(move.from, move.piece);

        if (move.promotionPiece != PieceType::EMPTY) {
            setPiece(move.to, move.promotionPiece);
        } else {
            setPiece(move.to, move.piece);
        }

        if (movingColor == PieceColor::BLACK) {
            fullmoveClock += 1;
        }

        halfMoveClock += 1;
        movingColor = getOppositeColor(movingColor);
    }

    void Bitboard::unmakeMove(Move &move) {
        assert(move.from >= 0 && move.from < 64);
        assert(move.to >= 0 && move.to < 64);

        UndoData undoData = undoStack[ply];

        if (move.promotionPiece != PieceType::EMPTY) {
            removePiece(move.to, move.promotionPiece);
        } else {
            removePiece(move.to, move.piece);
        }

        if (undoData.capturedPiece != PieceType::EMPTY) {
            setPiece(move.to, undoData.capturedPiece);
            halfMoveClock = undoData.halfMoveClock;
        }

        setPiece(move.from, move.piece);

        ply -= 1;
        halfMoveClock = undoData.halfMoveClock;
        enPassantSquare = undoData.enPassantSquare;
        castlingRights = undoData.castlingRights;
        movingColor = getOppositeColor(movingColor);

        if (movingColor == PieceColor::BLACK) {
            fullmoveClock -= 1;
        }
    }

    void Bitboard::print() {
        std::cout << "  ---------------------------------";

        for (int index = 0; index < 64; index++) {
            if (index % 8 == 0) {
                std::cout << std::endl << (index / 8) + 1 << " | ";
            }

            std::cout << getCharacterForPieceType(pieceSquareMapping[index]) << " | ";
        }

        std::cout << std::endl << "  ---------------------------------" << std::endl;
        std::cout << "    a   b   c   d   e   f   g   h  " << std::endl;
    }

    char Bitboard::getCharacterForPieceType(PieceType pieceType) {
        switch (pieceType) {
            case WHITE_PAWN:
                return 'P';
            case BLACK_PAWN:
                return 'p';
            case WHITE_KNIGHT:
                return 'N';
            case BLACK_KNIGHT:
                return 'n';
            case WHITE_BISHOP:
                return 'B';
            case BLACK_BISHOP:
                return 'b';
            case WHITE_ROOK:
                return 'R';
            case BLACK_ROOK:
                return 'r';
            case WHITE_QUEEN:
                return 'Q';
            case BLACK_QUEEN:
                return 'q';
            case WHITE_KING:
                return 'K';
            case BLACK_KING:
                return 'k';
            case EMPTY:
                return ' ';
        }
    }

    void Bitboard::printAvailableMoves(MoveList &moves) {
        std::cout << "  ---------------------------------";

        for (int index = 0; index < 64; index++) {
            if (index % 8 == 0) {
                std::cout << std::endl << (index / 8) + 1 << " | ";
            }

            bool didPrint = false;

            for (int i = 0; i < moves.count; i++) {
                Move move = moves.moves[i];

                if (move.to == index) {
                    std::cout << 'X' << " | ";
                    didPrint = true;
                    break;
                }
            }

            if (!didPrint) {
                std::cout << getCharacterForPieceType(pieceSquareMapping[index]) << " | ";
            }
        }

        std::cout << std::endl << "  ---------------------------------" << std::endl;
        std::cout << "    a   b   c   d   e   f   g   h  " << std::endl;
    }

    bool Bitboard::setFromFen(std::string fen) {
        int index = Square::A8;
        int spaces = 0;

        for (PieceType &type : pieceSquareMapping) {
            type = PieceType::EMPTY;
        }

        for (uint64_t &bb : pieceBB) {
            bb = 0;
        }

        for (uint64_t &bb : colorBB) {
            bb = 0;
        }

        for (uint64_t &hash : moveHistory) {
            hash = 0;
        }

        for (UndoData &undo : undoStack) {
            undo = {};
        }

        occupiedBB = 0;
        movingColor = PieceColor::NONE;
        ply = 0;
        halfMoveClock = 0;
        fullmoveClock = 1;
        enPassantSquare = Square::NO_SQUARE;
        castlingRights = 0;

        for (char character : fen) {
            if (character == ' ') {
                spaces++;
                continue;
            }

            if (spaces == 0) {
                if (character == '/') {
                    index -= 16;
                    continue;
                }

                if (character >= '1' && character <= '8') {
                    index += character - '0';
                    continue;
                }

                if (character >= 'A' && character <= 'z') {
                    setPieceFromFENChar(character, index);
                    index++;
                } else {
                    senjo::Output(senjo::Output::InfoPrefix) << "Invalid piece character!";
                    return false;
                }
            }

            if (spaces == 1) {
                if (tolower(character) == 'w') {
                    movingColor = PieceColor::WHITE;
                } else if (tolower(character) == 'b') {
                    movingColor = PieceColor::BLACK;
                } else {
                    senjo::Output(senjo::Output::InfoPrefix) << "Invalid color to move!";
                    return false;
                }
            }

            if (spaces == 2) {
                if (character == '-') {
                    continue;
                } else if (character == 'K') {
                    castlingRights |= CastlingRights::WHITE_KINGSIDE;
//                    zobristHash ^= zobristConstants[768 + 1];
                    continue;
                } else if (character == 'Q') {
                    castlingRights |= CastlingRights::WHITE_QUEENSIDE;
//                    zobristHash ^= zobristConstants[768 + 2];
                    continue;
                } else if (character == 'k') {
                    castlingRights |= CastlingRights::BLACK_KINGSIDE;
//                    zobristHash ^= zobristConstants[768 + 3];
                    continue;
                } else if (character == 'q') {
                    castlingRights |= CastlingRights::BLACK_QUEENSIDE;
//                    zobristHash ^= zobristConstants[768 + 4];
                    continue;
                }

                senjo::Output(senjo::Output::InfoPrefix) << "Invalid castling rights!";
                return false;
            }

            if (spaces == 3) {
                if (character == '-') {
                    continue;
                }

                if (tolower(character < 'a') || tolower(character) > 'h') {
                    senjo::Output(senjo::Output::InfoPrefix) << "Invalid en passant square!";
                    return false;
                }

                int8_t file = tolower(character) - 'a'; // NOLINT(cppcoreguidelines-narrowing-conversions)

                if (file < 0 || file > 7) {
                    senjo::Output(senjo::Output::InfoPrefix) << "Invalid en passant square!";
                    return false;
                }

                enPassantSquare = file;
                index += 2;
            }

            if (spaces == 4) {
                halfMoveClock = character - '0';
            }

            if (spaces == 5) {
                fullmoveClock = character - '0';
            }
        }

        return true;
    }

    bool Bitboard::isDraw() {
        return false;
    }

    bool Bitboard::isWinner(PieceColor color) {
        return false;
    }

    void Bitboard::initializeBetweenLookup() {
        for (int from = 0; from < 64; from++) {
            for (int to = 0; to < 64; to++) {
                uint64_t m1 = -1ULL;
                uint64_t a2a7 = 0x0001010101010100ULL;
                uint64_t b2g7 = 0x0040201008040200ULL;
                uint64_t h1b7 = 0x0002040810204080ULL;
                uint64_t btwn, line, rank, file;

                btwn = (m1 << from) ^ (m1 << to);
                file = (to & 7) - (from & 7);
                rank = ((to | 7) - from) >> 3;
                line = ((file & 7) - 1) & a2a7; /* a2a7 if same file */
                line += 2 * (((rank & 7) - 1) >> 58); /* b1g1 if same rank */
                line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
                line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
                line *= btwn & -btwn; /* mul acts like shift by smaller square */

                betweenTable[from][to] = line & btwn;   /* return the bits on that line in-between */
            }
        }
    }

    void Bitboard::setPieceFromFENChar(char character, int index) {
        // Uppercase = WHITE, lowercase = black
        switch (character) {
            case 'P':
                setPiece(index, PieceType::WHITE_PAWN);
                break;
            case 'p':
                setPiece(index, PieceType::BLACK_PAWN);
                break;
            case 'N':
                setPiece(index, PieceType::WHITE_KNIGHT);
                break;
            case 'n':
                setPiece(index, PieceType::BLACK_KNIGHT);
                break;
            case 'B':
                setPiece(index, PieceType::WHITE_BISHOP);
                break;
            case 'b':
                setPiece(index, PieceType::BLACK_BISHOP);
                break;
            case 'R':
                setPiece(index, PieceType::WHITE_ROOK);
                break;
            case 'r':
                setPiece(index, PieceType::BLACK_ROOK);
                break;
            case 'Q':
                setPiece(index, PieceType::WHITE_QUEEN);
                break;
            case 'q':
                setPiece(index, PieceType::BLACK_QUEEN);
                break;
            case 'K':
                setPiece(index, PieceType::WHITE_KING);
                break;
            case 'k':
                setPiece(index, PieceType::BLACK_KING);
                break;
        }
    }

    uint64_t Bitboard::getSquareAttacks(int square) {
        uint64_t queenBB = getPieceBoard<PieceType::WHITE_QUEEN>() | getPieceBoard<PieceType::BLACK_QUEEN>();
        uint64_t straightSlidingPieces =
                getPieceBoard<PieceType::WHITE_ROOK>() | getPieceBoard<PieceType::BLACK_ROOK>() | queenBB;
        uint64_t diagonalSlidingPieces =
                getPieceBoard<PieceType::WHITE_BISHOP>() | getPieceBoard<PieceType::BLACK_BISHOP>() | queenBB;

        uint64_t pawnAttacks = getPawnAttacks<PieceColor::BLACK>(square) & getPieceBoard<PieceType::WHITE_PAWN>();
        pawnAttacks |= getPawnAttacks<PieceColor::WHITE>(square) & getPieceBoard<PieceType::BLACK_PAWN>();
        uint64_t rookAttacks = getRookAttacks(square) & straightSlidingPieces;
        uint64_t bishopAttacks = getBishopAttacks(square) & diagonalSlidingPieces;
        uint64_t knightAttacks = getKnightAttacks(square) &
                                 (getPieceBoard<PieceType::WHITE_KNIGHT>() | getPieceBoard<PieceType::BLACK_KNIGHT>());
        uint64_t kingAttacks =
                getKingAttacks(square) & (getPieceBoard<PieceType::WHITE_KING>() | getPieceBoard<PieceType::BLACK_KING>());

        return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
    }
}