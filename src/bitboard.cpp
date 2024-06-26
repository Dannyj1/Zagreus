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

#include "bitboard.h"

#include <random>

#include "../senjo/Output.h"
#include "bitwise.h"
#include "magics.h"
#include "movegen.h"
#include "pst.h"
#include "utils.h"

namespace Zagreus {
uint64_t Bitboard::getOccupiedBoard() const { return occupiedBB; }

uint64_t Bitboard::getEmptyBoard() const { return ~occupiedBB; }

PieceColor Bitboard::getMovingColor() const { return movingColor; }

void Bitboard::setMovingColor(PieceColor movingColor) { this->movingColor = movingColor; }

PieceType Bitboard::getPieceOnSquare(int8_t square) {
    return pieceSquareMapping[square];
}

uint64_t Bitboard::getQueenAttacks(int8_t square) {
    return getBishopAttacks(square) | getRookAttacks(square);
}

uint64_t Bitboard::getQueenAttacks(int8_t square, uint64_t occupancy) {
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}

uint64_t Bitboard::getBishopAttacks(int8_t square) {
    uint64_t occupancy = getOccupiedBoard();
    occupancy &= getBishopMask(square);
    occupancy *= getBishopMagic(square);
    occupancy >>= 64 - BBits[square];

    return getBishopMagicAttacks(square, occupancy);
}

uint64_t Bitboard::getBishopAttacks(int8_t square, uint64_t occupancy) {
    occupancy &= getBishopMask(square);
    occupancy *= getBishopMagic(square);
    occupancy >>= 64 - BBits[square];

    return getBishopMagicAttacks(square, occupancy);
}

uint64_t Bitboard::getRookAttacks(int8_t square) {
    uint64_t occupancy = getOccupiedBoard();
    occupancy &= getRookMask(square);
    occupancy *= getRookMagic(square);
    occupancy >>= 64 - RBits[square];

    return getRookMagicAttacks(square, occupancy);
}

uint64_t Bitboard::getRookAttacks(int8_t square, uint64_t occupancy) {
    occupancy &= getRookMask(square);
    occupancy *= getRookMagic(square);
    occupancy >>= 64 - RBits[square];

    return getRookMagicAttacks(square, occupancy);
}

void Bitboard::setPiece(int8_t square, PieceType piece) {
    pieceBB[piece] |= 1ULL << square;
    occupiedBB |= 1ULL << square;
    colorBB[piece % 2] |= 1ULL << square;
    pieceSquareMapping[square] = piece;
    zobristHash ^= getPieceZobristConstant(piece, square);
    materialCount[piece] += 1;
    pstValues[piece % 2] += getMidgamePstValue(piece, square);
    pstValues[piece % 2 + 2] += getEndgamePstValue(piece, square);
}

void Bitboard::removePiece(int8_t square, PieceType piece) {
    pieceBB[piece] &= ~(1ULL << square);
    occupiedBB &= ~(1ULL << square);
    colorBB[piece % 2] &= ~(1ULL << square);
    pieceSquareMapping[square] = EMPTY;
    zobristHash ^= getPieceZobristConstant(piece, square);
    materialCount[piece] -= 1;
    pstValues[piece % 2] -= getMidgamePstValue(piece, square);
    pstValues[piece % 2 + 2] -= getEndgamePstValue(piece, square);
}

void Bitboard::makeMove(Move& move) {
    PieceType capturedPiece = getPieceOnSquare(move.to);

    undoStack[ply].capturedPiece = capturedPiece;
    undoStack[ply].halfMoveClock = halfMoveClock;
    undoStack[ply].enPassantSquare = enPassantSquare;
    undoStack[ply].castlingRights = castlingRights;
    undoStack[ply].moveType = REGULAR;
    undoStack[ply].zobristHash = zobristHash;
    undoStack[ply].previousMove = previousMove;

    halfMoveClock += 1;

    if (capturedPiece != EMPTY) {
        halfMoveClock = 0;
        removePiece(move.to, capturedPiece);
    }

    if (move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) {
        halfMoveClock = 0;
    }

    removePiece(move.from, move.piece);

    if (enPassantSquare != NO_SQUARE) {
        zobristHash ^= getEnPassantZobristConstant(enPassantSquare % 8);
    }

    if (move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) {
        halfMoveClock = 0;

        if (move.to - move.from == 16) {
            enPassantSquare = move.to - 8;
        } else if (move.to - move.from == -16) {
            enPassantSquare = move.to + 8;
        } else if ((std::abs(move.to - move.from) == 7 || std::abs(move.to - move.from) == 9) &&
                   move.to == enPassantSquare) {
            int8_t enPassantCaptureSquare = move.to - (movingColor == WHITE ? 8 : -8);
            removePiece(enPassantCaptureSquare, getPieceOnSquare(enPassantCaptureSquare));
            undoStack[ply].moveType = EN_PASSANT;
            enPassantSquare = NO_SQUARE;
        } else {
            enPassantSquare = NO_SQUARE;
        }
    } else {
        enPassantSquare = NO_SQUARE;
    }

    if (enPassantSquare != NO_SQUARE) {
        zobristHash ^= getEnPassantZobristConstant(enPassantSquare % 8);
    }

    if (move.piece == WHITE_KING || move.piece == BLACK_KING) {
        if (std::abs(move.to - move.from) == 2) {
            if (move.to == G1) {
                removePiece(H1, WHITE_ROOK);
                setPiece(F1, WHITE_ROOK);
            } else if (move.to == C1) {
                removePiece(A1, WHITE_ROOK);
                setPiece(D1, WHITE_ROOK);
            } else if (move.to == G8) {
                removePiece(H8, BLACK_ROOK);
                setPiece(F8, BLACK_ROOK);
            } else if (move.to == C8) {
                removePiece(A8, BLACK_ROOK);
                setPiece(D8, BLACK_ROOK);
            }

            undoStack[ply].moveType = CASTLING;
        }

        if (move.piece == WHITE_KING) {
            if (castlingRights & WHITE_KINGSIDE) {
                zobristHash ^= getCastleZobristConstant(ZOBRIST_WHITE_KINGSIDE_INDEX);
                castlingRights &= ~WHITE_KINGSIDE;
            }

            if (castlingRights & WHITE_QUEENSIDE) {
                zobristHash ^= getCastleZobristConstant(ZOBRIST_WHITE_QUEENSIDE_INDEX);
                castlingRights &= ~WHITE_QUEENSIDE;
            }
        } else {
            if (castlingRights & BLACK_KINGSIDE) {
                zobristHash ^= getCastleZobristConstant(ZOBRIST_BLACK_KINGSIDE_INDEX);
                castlingRights &= ~BLACK_KINGSIDE;
            }

            if (castlingRights & BLACK_QUEENSIDE) {
                zobristHash ^= getCastleZobristConstant(ZOBRIST_BLACK_QUEENSIDE_INDEX);
                castlingRights &= ~BLACK_QUEENSIDE;
            }
        }
    }

    if (move.piece == WHITE_ROOK) {
        if (move.from == A1 && (castlingRights & WHITE_QUEENSIDE)) {
            zobristHash ^= getCastleZobristConstant(ZOBRIST_WHITE_QUEENSIDE_INDEX);
            castlingRights &= ~WHITE_QUEENSIDE;
        } else if (move.from == H1 && (castlingRights & WHITE_KINGSIDE)) {
            zobristHash ^= getCastleZobristConstant(ZOBRIST_WHITE_KINGSIDE_INDEX);
            castlingRights &= ~WHITE_KINGSIDE;
        }
    } else if (move.piece == BLACK_ROOK) {
        if (move.from == A8 && (castlingRights & BLACK_QUEENSIDE)) {
            zobristHash ^= getCastleZobristConstant(ZOBRIST_BLACK_QUEENSIDE_INDEX);
            castlingRights &= ~BLACK_QUEENSIDE;
        } else if (move.from == H8 && (castlingRights & BLACK_KINGSIDE)) {
            zobristHash ^= getCastleZobristConstant(ZOBRIST_BLACK_KINGSIDE_INDEX);
            castlingRights &= ~BLACK_KINGSIDE;
        }
    }

    if (move.promotionPiece != EMPTY) {
        setPiece(move.to, move.promotionPiece);
    } else {
        setPiece(move.to, move.piece);
    }

    if (movingColor == BLACK) {
        fullmoveClock += 1;
    }

    movingColor = getOppositeColor(movingColor);
    zobristHash ^= getMovingColorZobristConstant();
    ply += 1;
    moveHistory[ply] = getZobristHash();
    previousMove = move;
}

int Bitboard::getHalfMoveClock() {
    return halfMoveClock;
}

void Bitboard::unmakeMove(Move& move) {
    moveHistory[ply] = 0;
    UndoData undoData = undoStack[ply - 1];

    if (move.promotionPiece != EMPTY) {
        removePiece(move.to, move.promotionPiece);
    } else {
        removePiece(move.to, move.piece);
    }

    if (undoData.capturedPiece != EMPTY) {
        setPiece(move.to, undoData.capturedPiece);
    }

    setPiece(move.from, move.piece);

    if (undoData.moveType == EN_PASSANT) {
        int8_t enPassantCaptureSquare = move.to - (getOppositeColor(movingColor) == WHITE ? 8 : -8);
        setPiece(enPassantCaptureSquare,
                 getOppositeColor(movingColor) == WHITE ? BLACK_PAWN : WHITE_PAWN);
    }

    if (undoData.moveType == CASTLING) {
        if (move.to == G1) {
            removePiece(F1, WHITE_ROOK);
            setPiece(H1, WHITE_ROOK);
        } else if (move.to == C1) {
            removePiece(D1, WHITE_ROOK);
            setPiece(A1, WHITE_ROOK);
        } else if (move.to == G8) {
            removePiece(F8, BLACK_ROOK);
            setPiece(H8, BLACK_ROOK);
        } else if (move.to == C8) {
            removePiece(D8, BLACK_ROOK);
            setPiece(A8, BLACK_ROOK);
        }
    }

    ply -= 1;
    halfMoveClock = undoData.halfMoveClock;
    enPassantSquare = undoData.enPassantSquare;
    castlingRights = undoData.castlingRights;
    movingColor = getOppositeColor(movingColor);
    zobristHash = undoData.zobristHash;
    previousMove = undoData.previousMove;

    if (movingColor == BLACK) {
        fullmoveClock -= 1;
    }
}

void Bitboard::makeNullMove() {
    undoStack[ply].capturedPiece = EMPTY;
    undoStack[ply].halfMoveClock = halfMoveClock;
    undoStack[ply].enPassantSquare = enPassantSquare;
    undoStack[ply].castlingRights = castlingRights;
    undoStack[ply].moveType = REGULAR;
    undoStack[ply].zobristHash = zobristHash;
    undoStack[ply].previousMove = previousMove;

    if (enPassantSquare != NO_SQUARE) {
        zobristHash ^= getEnPassantZobristConstant(enPassantSquare % 8);
        enPassantSquare = NO_SQUARE;
    }

    movingColor = getOppositeColor(movingColor);
    zobristHash ^= getMovingColorZobristConstant();
    previousMove = {NO_SQUARE, NO_SQUARE, EMPTY, 0, EMPTY, 0};
    ply += 1;
}

void Bitboard::unmakeNullMove() {
    ply -= 1;
    UndoData undoData = undoStack[ply];

    enPassantSquare = undoData.enPassantSquare;
    castlingRights = undoData.castlingRights;
    movingColor = getOppositeColor(movingColor);
    zobristHash = undoData.zobristHash;
    previousMove = undoData.previousMove;
}

const Move& Bitboard::getPreviousMove() const { return previousMove; }

bool Bitboard::hasMinorOrMajorPieces() {
    return hasMinorOrMajorPieces<WHITE>() || hasMinorOrMajorPieces<BLACK>();
}

int Bitboard::getAmountOfMinorOrMajorPieces() {
    return getAmountOfMinorOrMajorPieces<WHITE>() + getAmountOfMinorOrMajorPieces<BLACK>();
}

void Bitboard::print() {
    std::cout << "  ---------------------------------";

    for (int index = 0; index < 64; index++) {
        if (index % 8 == 0) {
            std::cout << std::endl << index / 8 + 1 << " | ";
        }

        std::cout << getCharacterForPieceType(pieceSquareMapping[index]) << " | ";
    }

    std::cout << std::endl << "  ---------------------------------" << std::endl;
    std::cout << "    a   b   c   d   e   f   g   h  " << std::endl;
}

void Bitboard::printAvailableMoves(MoveList* moves) {
    std::cout << "  ---------------------------------";

    for (int index = 0; index < 64; index++) {
        if (index % 8 == 0) {
            std::cout << std::endl << index / 8 + 1 << " | ";
        }

        bool didPrint = false;

        for (int i = 0; i < moves->size; i++) {
            Move move = moves->moves[i];

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

bool Bitboard::setFromFen(const std::string& fen) {
    int index = A8;
    int spaces = 0;

    for (PieceType& type : pieceSquareMapping) {
        type = EMPTY;
    }

    for (uint64_t& bb : pieceBB) {
        bb = 0;
    }

    for (uint64_t& bb : colorBB) {
        bb = 0;
    }

    for (uint64_t& hash : moveHistory) {
        hash = 0;
    }

    for (UndoData& undo : undoStack) {
        undo = {};
    }

    for (int& count : materialCount) {
        count = 0;
    }

    occupiedBB = 0;
    movingColor = NONE;
    ply = 0;
    halfMoveClock = 0;
    fullmoveClock = 1;
    enPassantSquare = NO_SQUARE;
    castlingRights = 0;
    zobristHash = 0;
    pstValues[0] = 0;
    pstValues[1] = 0;
    pstValues[2] = 0;
    pstValues[3] = 0;

    for (char character : fen) {
        if (character == ' ') {
            spaces++;
            continue;
        }

        if (character == ',') {
            break;
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
                movingColor = WHITE;
            } else if (tolower(character) == 'b') {
                movingColor = BLACK;
                zobristHash ^= getMovingColorZobristConstant();
            } else {
                senjo::Output(senjo::Output::InfoPrefix) << "Invalid color to move!";
                return false;
            }
        }

        if (spaces == 2) {
            if (character == '-') {
                continue;
            } else if (character == 'K') {
                castlingRights |= WHITE_KINGSIDE;
                zobristHash ^= getCastleZobristConstant(ZOBRIST_WHITE_KINGSIDE_INDEX);
                continue;
            } else if (character == 'Q') {
                castlingRights |= WHITE_QUEENSIDE;
                zobristHash ^= getCastleZobristConstant(ZOBRIST_WHITE_QUEENSIDE_INDEX);
                continue;
            } else if (character == 'k') {
                castlingRights |= BLACK_KINGSIDE;
                zobristHash ^= getCastleZobristConstant(ZOBRIST_BLACK_KINGSIDE_INDEX);
                continue;
            } else if (character == 'q') {
                castlingRights |= BLACK_QUEENSIDE;
                zobristHash ^= getCastleZobristConstant(ZOBRIST_BLACK_QUEENSIDE_INDEX);
                continue;
            }

            senjo::Output(senjo::Output::InfoPrefix) << "Invalid castling rights!";
            return false;
        }

        if (spaces == 3) {
            if (character == '-') {
                continue;
            }

            if (tolower(character) < 'a' || tolower(character) > 'h') {
                continue;
            }

            int8_t file = tolower(character) - 'a';
            // NOLINT(cppcoreguidelines-narrowing-conversions)
            int8_t rank = getOppositeColor(movingColor) == WHITE ? 2 : 5;

            if (file < 0 || file > 7) {
                senjo::Output(senjo::Output::InfoPrefix) << "Invalid en passant file!";
                return false;
            }

            enPassantSquare = rank * 8 + file;
            zobristHash ^= getEnPassantZobristConstant(enPassantSquare % 8);

            index += 2;
        }

        if (spaces == 4) {
            halfMoveClock = character - '0';
        }

        if (spaces == 5) {
            fullmoveClock = character - '0';
        }
    }

    moveHistory[ply] = getZobristHash();
    return true;
}

bool Bitboard::setFromFenTuner(const std::string& fen) {
    int index = A8;
    int spaces = 0;

    for (PieceType& type : pieceSquareMapping) {
        type = EMPTY;
    }

    for (uint64_t& bb : pieceBB) {
        bb = 0;
    }

    for (uint64_t& bb : colorBB) {
        bb = 0;
    }

    for (uint64_t& hash : moveHistory) {
        hash = 0;
    }

    for (UndoData& undo : undoStack) {
        undo = {};
    }

    for (int& count : materialCount) {
        count = 0;
    }

    occupiedBB = 0;
    movingColor = NONE;
    ply = 0;
    halfMoveClock = 0;
    fullmoveClock = 1;
    enPassantSquare = NO_SQUARE;
    castlingRights = 0;
    zobristHash = 0;
    pstValues[0] = 0;
    pstValues[1] = 0;
    pstValues[2] = 0;
    pstValues[3] = 0;

    for (char character : fen) {
        if (character == ' ') {
            spaces++;
            continue;
        }

        if (character == ',') {
            break;
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
                movingColor = WHITE;
            } else if (tolower(character) == 'b') {
                movingColor = BLACK;
            } else {
                senjo::Output(senjo::Output::InfoPrefix) << "Invalid color to move!";
                return false;
            }
        }

        if (spaces == 2) {
            if (character == '-') {
                continue;
            } else if (character == 'K') {
                castlingRights |= WHITE_KINGSIDE;
                continue;
            } else if (character == 'Q') {
                castlingRights |= WHITE_QUEENSIDE;
                continue;
            } else if (character == 'k') {
                castlingRights |= BLACK_KINGSIDE;
                continue;
            } else if (character == 'q') {
                castlingRights |= BLACK_QUEENSIDE;
                continue;
            }

            senjo::Output(senjo::Output::InfoPrefix) << "Invalid castling rights!";
            return false;
        }

        if (spaces == 3) {
            if (character == '-') {
                continue;
            }

            if (tolower(character) < 'a' || tolower(character) > 'h') {
                continue;
            }

            int8_t file = tolower(character) - 'a';
            // NOLINT(cppcoreguidelines-narrowing-conversions)
            int8_t rank = getOppositeColor(movingColor) == WHITE ? 2 : 5;

            if (file < 0 || file > 7) {
                senjo::Output(senjo::Output::InfoPrefix) << "Invalid en passant file!";
                return false;
            }

            enPassantSquare = rank * 8 + file;
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
    if (halfMoveClock >= 100) {
        return true;
    }

    if (isInsufficientMaterial()) {
        return true;
    }

    // Check if the same position has occurred 3 times using the movehistory array
    int samePositionCount = 0;
    uint64_t boardHash = getZobristHash();

    for (int i = ply; i >= 0; i--) {
        if (moveHistory[i] == boardHash) {
            samePositionCount++;
        }

        if (samePositionCount >= 3) {
            return true;
        }
    }

    return false;
}

uint64_t Bitboard::getZobristHash() const { return zobristHash; }

void Bitboard::setZobristHash(uint64_t zobristHash) { Bitboard::zobristHash = zobristHash; }

bool Bitboard::isInsufficientMaterial() {
    uint64_t kingBB = getPieceBoard(WHITE_KING) | getPieceBoard(BLACK_KING);
    uint64_t piecesBB = getOccupiedBoard();
    uint64_t piecesWithoutKings = piecesBB & ~kingBB;
    uint64_t sufficientPieces = getPieceBoard(WHITE_QUEEN) | getPieceBoard(BLACK_QUEEN) |
                                getPieceBoard(WHITE_ROOK) | getPieceBoard(BLACK_ROOK) |
                                getPieceBoard(WHITE_PAWN) | getPieceBoard(BLACK_PAWN);

    // If there are queens, rooks or pawns on the board, it can't be a draw
    if (sufficientPieces) {
        return false;
    }

    int pieceCountWithoutKings = popcnt(piecesWithoutKings);

    // If there is only one minor piece on the board, it's a draw. Covers KBvK, KNvK and KvK
    if (pieceCountWithoutKings == 1 || pieceCountWithoutKings == 0) {
        return true;
    }

    return false;
}

void Bitboard::setPieceFromFENChar(char character, int index) {
    // Uppercase = WHITE, lowercase = black
    switch (character) {
        case 'P':
            setPiece(index, WHITE_PAWN);
            break;
        case 'p':
            setPiece(index, BLACK_PAWN);
            break;
        case 'N':
            setPiece(index, WHITE_KNIGHT);
            break;
        case 'n':
            setPiece(index, BLACK_KNIGHT);
            break;
        case 'B':
            setPiece(index, WHITE_BISHOP);
            break;
        case 'b':
            setPiece(index, BLACK_BISHOP);
            break;
        case 'R':
            setPiece(index, WHITE_ROOK);
            break;
        case 'r':
            setPiece(index, BLACK_ROOK);
            break;
        case 'Q':
            setPiece(index, WHITE_QUEEN);
            break;
        case 'q':
            setPiece(index, BLACK_QUEEN);
            break;
        case 'K':
            setPiece(index, WHITE_KING);
            break;
        case 'k':
            setPiece(index, BLACK_KING);
            break;
    }
}

uint64_t Bitboard::getSquareAttacks(int8_t square) {
    uint64_t queenBB = getPieceBoard(WHITE_QUEEN) | getPieceBoard(BLACK_QUEEN);
    uint64_t straightSlidingPieces = getPieceBoard(WHITE_ROOK) | getPieceBoard(BLACK_ROOK) |
                                     queenBB;
    uint64_t diagonalSlidingPieces =
        getPieceBoard(WHITE_BISHOP) | getPieceBoard(BLACK_BISHOP) | queenBB;

    uint64_t pawnAttacks = getPawnAttacks<BLACK>(square) & getPieceBoard(WHITE_PAWN);
    pawnAttacks |= getPawnAttacks<WHITE>(square) & getPieceBoard(BLACK_PAWN);
    uint64_t rookAttacks = getRookAttacks(square) & straightSlidingPieces;
    uint64_t bishopAttacks = getBishopAttacks(square) & diagonalSlidingPieces;
    uint64_t knightAttacks =
        getKnightAttacks(square) & (getPieceBoard(WHITE_KNIGHT) | getPieceBoard(BLACK_KNIGHT));
    uint64_t kingAttacks =
        getKingAttacks(square) & (getPieceBoard(WHITE_KING) | getPieceBoard(BLACK_KING));

    return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
}

uint8_t Bitboard::getCastlingRights() const { return castlingRights; }

void Bitboard::setCastlingRights(uint8_t castlingRights) {
    Bitboard::castlingRights = castlingRights;
}

int8_t Bitboard::getEnPassantSquare() const { return enPassantSquare; }

void Bitboard::setEnPassantSquare(int8_t enPassantSquare) {
    Bitboard::enPassantSquare = enPassantSquare;
}

uint64_t Bitboard::getFile(int8_t square) {
    return getRayAttack(square, NORTH) | getRayAttack(square, SOUTH) | 1ULL << square;
}

bool Bitboard::makeStrMove(const std::string& strMove) {
    int8_t fromSquare = getSquareFromString(strMove.substr(0, 2));
    int8_t toSquare = getSquareFromString(strMove.substr(2, 2));
    PieceType promotionPiece = EMPTY;

    if (strMove.length() == 5) {
        if (strMove.ends_with("q")) {
            promotionPiece = getMovingColor() == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
        } else if (strMove.ends_with("r")) {
            promotionPiece = getMovingColor() == WHITE ? WHITE_ROOK : BLACK_ROOK;
        } else if (strMove.ends_with("b")) {
            promotionPiece = getMovingColor() == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
        } else if (strMove.ends_with("n")) {
            promotionPiece = getMovingColor() == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
        }
    }

    PieceType movingPiece = getPieceOnSquare(fromSquare);
    PieceType capturedPiece = getPieceOnSquare(toSquare);
    int captureScore = -1;

    if (capturedPiece != EMPTY) {
        captureScore = mvvlva(movingPiece, capturedPiece);
    }

    Move move = {fromSquare, toSquare, movingPiece, captureScore, promotionPiece};
    makeMove(move);
    return true;
}

Line Bitboard::getPvLine() { return pvLine; }

void Bitboard::setPvLine(Line& pvLine) {
    Bitboard::pvLine = pvLine;
}

uint16_t Bitboard::getPly() const { return ply; }

void Bitboard::setPly(uint16_t ply) { this->ply = ply; }

bool Bitboard::isOpenFile(int8_t square) {
    uint64_t fileMask = getFile(square);
    uint64_t occupied = getPieceBoard(WHITE_PAWN) | getPieceBoard(BLACK_PAWN);

    return fileMask == (fileMask & ~occupied);
}

int Bitboard::getWhiteMidgamePst() const { return pstValues[0]; }

int Bitboard::getWhiteEndgamePst() const { return pstValues[2]; }

int Bitboard::getBlackMidgamePst() const { return pstValues[1]; }

int Bitboard::getBlackEndgamePst() const { return pstValues[3]; }

uint64_t Bitboard::getPieceBoard(PieceType pieceType) { return pieceBB[pieceType]; }

template <PieceColor color>
bool Bitboard::isWinner() {
    if (color == WHITE) {
        if (!isKingInCheck<BLACK>()) {
            return false;
        }
    } else {
        if (!isKingInCheck<WHITE>()) {
            return false;
        }
    }

    MoveListPool* moveListPool = MoveListPool::getInstance();
    MoveList* moveList = moveListPool->getMoveList();
    generateMoves<color == WHITE ? BLACK : WHITE, NORMAL>(*this, moveList);

    for (int i = 0; i < moveList->size; i++) {
        Move move = moveList->moves[i];

        makeMove(move);

        if (color == WHITE) {
            if (isKingInCheck<BLACK>()) {
                unmakeMove(move);
                continue;
            }
        } else {
            if (isKingInCheck<WHITE>()) {
                unmakeMove(move);
                continue;
            }
        }

        unmakeMove(move);
        moveListPool->releaseMoveList(moveList);
        return false;
    }

    moveListPool->releaseMoveList(moveList);
    return true;
}

template bool Bitboard::isWinner<WHITE>();
template bool Bitboard::isWinner<BLACK>();
} // namespace Zagreus