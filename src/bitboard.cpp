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
#include "../senjo/Output.h"
#include "movegen.h"

namespace Zagreus {
    Bitboard::Bitboard() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        gen.seed(0x6C7CCC580A348E7BULL);
        std::uniform_int_distribution<uint64_t> dis;

        for (uint64_t &zobristConstant : zobristConstants) {
            zobristConstant = dis(gen);
        }

        for (PieceType &type : pieceSquareMapping) {
            type = PieceType::EMPTY;
        }

        uint64_t sqBB = 1ULL;
        for (int8_t sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            kingAttacks[sq] = calculateKingAttacks(sqBB) & ~sqBB;
        }

        sqBB = 1ULL;
        for (int8_t sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            knightAttacks[sq] = calculateKnightAttacks(sqBB) & ~sqBB;
        }

        sqBB = 1ULL;
        for (int8_t sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            pawnAttacks[PieceColor::WHITE][sq] = calculatePawnAttacks<PieceColor::WHITE>(sqBB) & ~sqBB;
            pawnAttacks[PieceColor::BLACK][sq] = calculatePawnAttacks<PieceColor::BLACK>(sqBB) & ~sqBB;
        }

        initializeBetweenLookup();
        initializeRayAttacks();
    }

    void Bitboard::initializeRayAttacks() {
        uint64_t sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            for (int direction = 0; direction < 8; direction++) {
                switch (static_cast<Direction>(direction)) {
                    case NORTH:
                        rayAttacks[direction][sq] = nortOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case SOUTH:
                        rayAttacks[direction][sq] = soutOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case EAST:
                        rayAttacks[direction][sq] = eastOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case WEST:
                        rayAttacks[direction][sq] = westOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case NORTH_EAST:
                        rayAttacks[direction][sq] = noEaOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case NORTH_WEST:
                        rayAttacks[direction][sq] = noWeOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case SOUTH_EAST:
                        rayAttacks[direction][sq] = soEaOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case SOUTH_WEST:
                        rayAttacks[direction][sq] = soWeOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                }
            }
        }
    }

    uint64_t Bitboard::getOccupiedBoard() {
        return occupiedBB;
    }

    uint64_t Bitboard::getEmptyBoard() {
        return ~occupiedBB;
    }

    uint64_t Bitboard::getTilesBetween(int8_t from, int8_t to) {
        return betweenTable[from][to];
    }

    PieceColor Bitboard::getMovingColor() const {
        return movingColor;
    }

    void Bitboard::setMovingColor(PieceColor movingColor) {
        Bitboard::movingColor = movingColor;
    }

    PieceType Bitboard::getPieceOnSquare(int8_t square) {
        assert(square >= 0 && square < 64);
        assert(pieceSquareMapping[square] >= -1 && pieceSquareMapping[square] <= 11);
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
        zobristHash ^= zobristConstants[square + 64 * piece];
    }

    void Bitboard::removePiece(int8_t square, PieceType piece) {
        assert(square >= 0 && square < 64);
        pieceBB[piece] &= ~(1ULL << square);
        occupiedBB &= ~(1ULL << square);
        colorBB[piece % 2] &= ~(1ULL << square);
        pieceSquareMapping[square] = PieceType::EMPTY;
        zobristHash ^= zobristConstants[square + 64 * piece];
    }

    void Bitboard::makeMove(Move &move) {
        assert(move.from >= 0 && move.from < 64);
        assert(move.to >= 0 && move.to < 64);
        assert(move.piece % 2 == movingColor);

        PieceType capturedPiece = getPieceOnSquare(move.to);

        undoStack[ply].capturedPiece = capturedPiece;
        undoStack[ply].halfMoveClock = halfMoveClock;
        undoStack[ply].enPassantSquare = enPassantSquare;
        undoStack[ply].castlingRights = castlingRights;
        undoStack[ply].moveType = MoveType::REGULAR;
        undoStack[ply].zobristHash = zobristHash;
        undoStack[ply].kingInCheck = kingInCheck;
        undoStack[ply].previousMove = previousMove;
        halfMoveClock += 1;

        if (capturedPiece != PieceType::EMPTY) {
            removePiece(move.to, capturedPiece);
            halfMoveClock = 0;
        }

        removePiece(move.from, move.piece);

        if (enPassantSquare != NO_SQUARE) {
            zobristHash ^= zobristConstants[ZOBRIST_EN_PASSANT_INDEX + enPassantSquare % 8];
        }

        if (move.piece == PieceType::WHITE_PAWN || move.piece == PieceType::BLACK_PAWN) {
            halfMoveClock = 0;

            if (move.to - move.from == 16) {
                enPassantSquare = move.to - 8;
                assert(enPassantSquare >= 0 && enPassantSquare < 64);
            } else if (move.to - move.from == -16) {
                enPassantSquare = move.to + 8;
                assert(enPassantSquare >= 0 && enPassantSquare < 64);
            } else if ((std::abs(move.to - move.from) == 7 || std::abs(move.to - move.from) == 9) &&
                       move.to == enPassantSquare) {
                int8_t enPassantCaptureSquare = move.to - (movingColor == PieceColor::WHITE ? 8 : -8);
                removePiece(enPassantCaptureSquare, getPieceOnSquare(enPassantCaptureSquare));
                undoStack[ply].moveType = MoveType::EN_PASSANT;
                enPassantSquare = NO_SQUARE;
            } else {
                enPassantSquare = NO_SQUARE;
            }
        } else {
            enPassantSquare = NO_SQUARE;
        }

        if (enPassantSquare != NO_SQUARE) {
            zobristHash ^= zobristConstants[ZOBRIST_EN_PASSANT_INDEX + enPassantSquare % 8];
        }

        if (move.piece == WHITE_KING || move.piece == BLACK_KING) {
            if (std::abs(move.to - move.from) == 2) {
                if (move.to == Square::G1) {
                    removePiece(Square::H1, PieceType::WHITE_ROOK);
                    setPiece(Square::F1, PieceType::WHITE_ROOK);
                    castlingRights |= HAS_WHITE_CASTLED;
                } else if (move.to == Square::C1) {
                    removePiece(Square::A1, PieceType::WHITE_ROOK);
                    setPiece(Square::D1, PieceType::WHITE_ROOK);
                    castlingRights |= HAS_WHITE_CASTLED;
                } else if (move.to == Square::G8) {
                    removePiece(Square::H8, PieceType::BLACK_ROOK);
                    setPiece(Square::F8, PieceType::BLACK_ROOK);
                    castlingRights |= HAS_BLACK_CASTLED;
                } else if (move.to == Square::C8) {
                    removePiece(Square::A8, PieceType::BLACK_ROOK);
                    setPiece(Square::D8, PieceType::BLACK_ROOK);
                    castlingRights |= HAS_BLACK_CASTLED;
                }

                undoStack[ply].moveType = MoveType::CASTLING;
            }

            if (move.piece == WHITE_KING) {
                if (castlingRights & CastlingRights::WHITE_KINGSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_WHITE_KINGSIDE_INDEX];
                }

                if (castlingRights & CastlingRights::WHITE_QUEENSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_WHITE_QUEENSIDE_INDEX];
                }

                castlingRights &= ~(CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE);
            } else {
                if (castlingRights & CastlingRights::BLACK_KINGSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_BLACK_KINGSIDE_INDEX];
                }

                if (castlingRights & CastlingRights::BLACK_QUEENSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_BLACK_QUEENSIDE_INDEX];
                }

                castlingRights &= ~(CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
            }
        }

        if (move.piece == PieceType::WHITE_ROOK) {
            if (move.from == Square::A1 && (castlingRights & CastlingRights::WHITE_QUEENSIDE)) {
                if (castlingRights & CastlingRights::WHITE_QUEENSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_WHITE_QUEENSIDE_INDEX];
                }

                castlingRights &= ~CastlingRights::WHITE_QUEENSIDE;
            } else if (move.from == Square::H1 && (castlingRights & CastlingRights::WHITE_KINGSIDE)) {
                if (castlingRights & CastlingRights::WHITE_KINGSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_WHITE_KINGSIDE_INDEX];
                }

                castlingRights &= ~CastlingRights::WHITE_KINGSIDE;
            }
        } else if (move.piece == PieceType::BLACK_ROOK) {
            if (move.from == Square::A8 && (castlingRights & CastlingRights::BLACK_QUEENSIDE)) {
                if (castlingRights & CastlingRights::BLACK_QUEENSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_BLACK_QUEENSIDE_INDEX];
                }

                castlingRights &= ~CastlingRights::BLACK_QUEENSIDE;
            } else if (move.from == Square::H8 && (castlingRights & CastlingRights::BLACK_KINGSIDE)) {
                if (castlingRights & CastlingRights::BLACK_KINGSIDE) {
                    zobristHash ^= zobristConstants[ZOBRIST_BLACK_KINGSIDE_INDEX];
                }

                castlingRights &= ~CastlingRights::BLACK_KINGSIDE;
            }
        }

        if (move.promotionPiece != PieceType::EMPTY) {
            setPiece(move.to, move.promotionPiece);
        } else {
            setPiece(move.to, move.piece);
        }

        if (movingColor == PieceColor::BLACK) {
            fullmoveClock += 1;
        }

        kingInCheck = 0b00001100;
        movingColor = getOppositeColor(movingColor);
        zobristHash ^= zobristConstants[ZOBRIST_COLOR_INDEX];
        ply += 1;
        moveHistory[ply] = getZobristHash();
        previousMove = move;
    }

    void Bitboard::unmakeMove(Move &move) {
        assert(move.from >= 0 && move.from < 64);
        assert(move.to >= 0 && move.to < 64);

        moveHistory[ply] = 0;
        UndoData undoData = undoStack[ply - 1];

        if (move.promotionPiece != PieceType::EMPTY) {
            removePiece(move.to, move.promotionPiece);
        } else {
            removePiece(move.to, move.piece);
        }

        if (undoData.capturedPiece != PieceType::EMPTY) {
            setPiece(move.to, undoData.capturedPiece);
        }

        setPiece(move.from, move.piece);

        if (undoData.moveType == MoveType::EN_PASSANT) {
            int8_t enPassantCaptureSquare = move.to - (getOppositeColor(movingColor) == PieceColor::WHITE ? 8 : -8);
            setPiece(enPassantCaptureSquare, getOppositeColor(movingColor) == PieceColor::WHITE ? PieceType::BLACK_PAWN
                                                                                                : PieceType::WHITE_PAWN);
        }

        if (undoData.moveType == MoveType::CASTLING) {
            if (move.to == Square::G1) {
                removePiece(Square::F1, PieceType::WHITE_ROOK);
                setPiece(Square::H1, PieceType::WHITE_ROOK);
            } else if (move.to == Square::C1) {
                removePiece(Square::D1, PieceType::WHITE_ROOK);
                setPiece(Square::A1, PieceType::WHITE_ROOK);
            } else if (move.to == Square::G8) {
                removePiece(Square::F8, PieceType::BLACK_ROOK);
                setPiece(Square::H8, PieceType::BLACK_ROOK);
            } else if (move.to == Square::C8) {
                removePiece(Square::D8, PieceType::BLACK_ROOK);
                setPiece(Square::A8, PieceType::BLACK_ROOK);
            }
        }

        ply -= 1;
        halfMoveClock = undoData.halfMoveClock;
        enPassantSquare = undoData.enPassantSquare;
        assert(enPassantSquare >= NO_SQUARE && enPassantSquare < 64);
        castlingRights = undoData.castlingRights;
        movingColor = getOppositeColor(movingColor);
        zobristHash = undoData.zobristHash;
        kingInCheck = undoData.kingInCheck;
        previousMove = undoData.previousMove;

        if (movingColor == PieceColor::BLACK) {
            fullmoveClock -= 1;
        }
    }

    const Move &Bitboard::getPreviousMove() const {
        return previousMove;
    }


    uint64_t Bitboard::getZobristForMove(Move &move) {
        assert(move.from >= 0 && move.from < 64);
        assert(move.to >= 0 && move.to < 64);
        assert(move.piece % 2 == movingColor);

        uint64_t result = getZobristHash();
        PieceType capturedPiece = getPieceOnSquare(move.to);

        if (capturedPiece != PieceType::EMPTY) {
            result ^= zobristConstants[move.to + 64 * capturedPiece];
        }

        result ^= zobristConstants[move.from + 64 * move.piece];

        if (enPassantSquare != NO_SQUARE) {
            result ^= zobristConstants[ZOBRIST_EN_PASSANT_INDEX + enPassantSquare % 8];
        }

        if (move.piece == PieceType::WHITE_PAWN || move.piece == PieceType::BLACK_PAWN) {
            if (move.to - move.from == 16) {
                result ^= zobristConstants[ZOBRIST_EN_PASSANT_INDEX + (move.to - 8) % 8];
                assert(enPassantSquare >= 0 && enPassantSquare < 64);
            } else if (move.to - move.from == -16) {
                result ^= zobristConstants[ZOBRIST_EN_PASSANT_INDEX + (move.to + 8) % 8];
                assert(enPassantSquare >= 0 && enPassantSquare < 64);
            } else if ((std::abs(move.to - move.from) == 7 || std::abs(move.to - move.from) == 9) &&
                       move.to == enPassantSquare) {
                int8_t enPassantCaptureSquare = move.to - (movingColor == PieceColor::WHITE ? 8 : -8);
                result ^= zobristConstants[enPassantCaptureSquare + 64 * getPieceOnSquare(enPassantCaptureSquare)];
            }
        }

        if (move.piece == WHITE_KING || move.piece == BLACK_KING) {
            if (std::abs(move.to - move.from) == 2) {
                if (move.to == Square::G1) {
                    result ^= zobristConstants[Square::H1 + 64 * PieceType::WHITE_ROOK];
                    result ^= zobristConstants[Square::F1 + 64 * PieceType::WHITE_ROOK];
                } else if (move.to == Square::C1) {
                    result ^= zobristConstants[Square::A1 + 64 * PieceType::WHITE_ROOK];
                    result ^= zobristConstants[Square::D1 + 64 * PieceType::WHITE_ROOK];
                } else if (move.to == Square::G8) {
                    result ^= zobristConstants[Square::H8 + 64 * PieceType::BLACK_ROOK];
                    result ^= zobristConstants[Square::F8 + 64 * PieceType::BLACK_ROOK];
                } else if (move.to == Square::C8) {
                    result ^= zobristConstants[Square::A8 + 64 * PieceType::BLACK_ROOK];
                    result ^= zobristConstants[Square::D8 + 64 * PieceType::BLACK_ROOK];
                }
            }

            if (move.piece == WHITE_KING) {
                if (castlingRights & CastlingRights::WHITE_KINGSIDE) {
                    result ^= zobristConstants[ZOBRIST_WHITE_KINGSIDE_INDEX];
                }

                if (castlingRights & CastlingRights::WHITE_QUEENSIDE) {
                    result ^= zobristConstants[ZOBRIST_WHITE_QUEENSIDE_INDEX];
                }
            } else {
                if (castlingRights & CastlingRights::BLACK_KINGSIDE) {
                    result ^= zobristConstants[ZOBRIST_BLACK_KINGSIDE_INDEX];
                }

                if (castlingRights & CastlingRights::BLACK_QUEENSIDE) {
                    result ^= zobristConstants[ZOBRIST_BLACK_QUEENSIDE_INDEX];
                }
            }
        }

        if (move.piece == PieceType::WHITE_ROOK) {
            if (move.from == Square::A1 && (castlingRights & CastlingRights::WHITE_QUEENSIDE)) {
                if (castlingRights & CastlingRights::WHITE_QUEENSIDE) {
                    result ^= zobristConstants[ZOBRIST_WHITE_QUEENSIDE_INDEX];
                }
            } else if (move.from == Square::H1 && (castlingRights & CastlingRights::WHITE_KINGSIDE)) {
                if (castlingRights & CastlingRights::WHITE_KINGSIDE) {
                    result ^= zobristConstants[ZOBRIST_WHITE_KINGSIDE_INDEX];
                }
            }
        } else if (move.piece == PieceType::BLACK_ROOK) {
            if (move.from == Square::A8 && (castlingRights & CastlingRights::BLACK_QUEENSIDE)) {
                if (castlingRights & CastlingRights::BLACK_QUEENSIDE) {
                    result ^= zobristConstants[ZOBRIST_BLACK_QUEENSIDE_INDEX];
                }
            } else if (move.from == Square::H8 && (castlingRights & CastlingRights::BLACK_KINGSIDE)) {
                if (castlingRights & CastlingRights::BLACK_KINGSIDE) {
                    result ^= zobristConstants[ZOBRIST_BLACK_KINGSIDE_INDEX];
                }
            }
        }

        if (move.promotionPiece != PieceType::EMPTY) {
            result ^= zobristConstants[move.to + 64 * move.promotionPiece];
        } else {
            result ^= zobristConstants[move.to + 64 * move.piece];
        }

        result ^= zobristConstants[ZOBRIST_COLOR_INDEX];
        return result;
    }

    bool Bitboard::hasMinorOrMajorPieces() {
        return getPieceBoard<PieceType::WHITE_BISHOP>() | getPieceBoard<PieceType::WHITE_KNIGHT>() |
               getPieceBoard<PieceType::WHITE_QUEEN>() | getPieceBoard<PieceType::WHITE_ROOK>() |
               getPieceBoard<PieceType::BLACK_BISHOP>() | getPieceBoard<PieceType::BLACK_KNIGHT>() |
               getPieceBoard<PieceType::BLACK_QUEEN>() | getPieceBoard<PieceType::BLACK_ROOK>();
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

    void Bitboard::printAvailableMoves(MoveList &moves) {
        std::cout << "  ---------------------------------";

        for (int index = 0; index < 64; index++) {
            if (index % 8 == 0) {
                std::cout << std::endl << (index / 8) + 1 << " | ";
            }

            bool didPrint = false;

            for (int i = 0; i < moves.size; i++) {
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

    bool Bitboard::setFromFen(const std::string &fen) {
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
        zobristHash = 0;

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
                    movingColor = PieceColor::WHITE;
                } else if (tolower(character) == 'b') {
                    movingColor = PieceColor::BLACK;
                    zobristHash ^= zobristConstants[ZOBRIST_COLOR_INDEX];
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
                    zobristHash ^= zobristConstants[ZOBRIST_WHITE_KINGSIDE_INDEX];
                    continue;
                } else if (character == 'Q') {
                    castlingRights |= CastlingRights::WHITE_QUEENSIDE;
                    zobristHash ^= zobristConstants[ZOBRIST_WHITE_QUEENSIDE_INDEX];
                    continue;
                } else if (character == 'k') {
                    castlingRights |= CastlingRights::BLACK_KINGSIDE;
                    zobristHash ^= zobristConstants[ZOBRIST_BLACK_KINGSIDE_INDEX];
                    continue;
                } else if (character == 'q') {
                    castlingRights |= CastlingRights::BLACK_QUEENSIDE;
                    zobristHash ^= zobristConstants[ZOBRIST_BLACK_QUEENSIDE_INDEX];
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

                int8_t file = tolower(character) - 'a'; // NOLINT(cppcoreguidelines-narrowing-conversions)
                int8_t rank = (getOppositeColor(movingColor) == PieceColor::WHITE) ? 2 : 5;

                if (file < 0 || file > 7) {
                    senjo::Output(senjo::Output::InfoPrefix) << "Invalid en passant file!";
                    return false;
                }

                enPassantSquare = (rank * 8) + file;
                zobristHash ^= zobristConstants[ZOBRIST_EN_PASSANT_INDEX + enPassantSquare % 8];

                assert(enPassantSquare >= 0 && enPassantSquare < 64);
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

    // Faster setFromFen version without validity checks and some features the tuner doesn't need
    bool Bitboard::setFromFenTuner(std::string &fen) {
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

        occupiedBB = 0;
        enPassantSquare = Square::NO_SQUARE;
        castlingRights = 0;

        for (char &character : fen) {
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

                setPieceFromFENChar(character, index);
                index++;
            }

            if (spaces == 1) {
                if (tolower(character) == 'w') {
                    movingColor = PieceColor::WHITE;
                } else {
                    movingColor = PieceColor::BLACK;
                }
            }

            if (spaces == 2) {
                if (character == '-') {
                    continue;
                } else if (character == 'K') {
                    castlingRights |= CastlingRights::WHITE_KINGSIDE;
                    continue;
                } else if (character == 'Q') {
                    castlingRights |= CastlingRights::WHITE_QUEENSIDE;
                    continue;
                } else if (character == 'k') {
                    castlingRights |= CastlingRights::BLACK_KINGSIDE;
                    continue;
                } else if (character == 'q') {
                    castlingRights |= CastlingRights::BLACK_QUEENSIDE;
                    continue;
                } else {
                    continue;
                }
            }

            if (spaces == 3) {
                if (character == '-') {
                    continue;
                }

                if (tolower(character) < 'a' || tolower(character) > 'h') {
                    continue;
                }

                int8_t file = tolower(character) - 'a'; // NOLINT(cppcoreguidelines-narrowing-conversions)
                int8_t rank = (getOppositeColor(movingColor) == PieceColor::WHITE) ? 2 : 5;

                enPassantSquare = (rank * 8) + file;

                assert(enPassantSquare >= 0 && enPassantSquare < 64);
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
        MoveList moves;

        if (movingColor == PieceColor::WHITE) {
            moves = generateMoves<PieceColor::WHITE>(*this);
        } else {
            moves = generateMoves<PieceColor::BLACK>(*this);
        }

        bool hasLegalMove = false;

        for (int i = 0; i < moves.size; i++) {
            Move move = moves.moves[i];
            makeMove(move);

            if (movingColor == PieceColor::WHITE) {
                if (!isKingInCheck<PieceColor::BLACK>()) {
                    unmakeMove(move);
                    hasLegalMove = true;
                    break;
                }
            } else {
                if (!isKingInCheck<PieceColor::WHITE>()) {
                    unmakeMove(move);
                    hasLegalMove = true;
                    break;
                }
            }

            unmakeMove(move);
        }

        if (!hasLegalMove) {
            return true;
        }

        if (halfMoveClock >= 100) {
            return true;
        }

        // Check if the same position has occurred 3 times using the movehistory array
        int samePositionCount = 0;
        uint64_t boardHash = getZobristHash();

        for (int i = ply; i >= 0; i--) {
            assert(moveHistory[i] != 0);

            if (moveHistory[i] == boardHash) {
                samePositionCount++;
            }

            if (samePositionCount >= 3) {
                return true;
            }
        }

        return isInsufficientMaterial();
    }

    uint64_t Bitboard::getZobristHash() const {
        return zobristHash;
    }

    void Bitboard::setZobristHash(uint64_t zobristHash) {
        Bitboard::zobristHash = zobristHash;
    }

    bool Bitboard::isInsufficientMaterial() {
        int pieceCount = popcnt(getOccupiedBoard());

        if (pieceCount > 4) {
            return false;
        }

        if (pieceCount == 2) {
            return true;
        }

        if (pieceCount == 3) {
            uint64_t bishopBB = getPieceBoard<WHITE_BISHOP>() | getPieceBoard<BLACK_BISHOP>();
            uint64_t knightBB = getPieceBoard<WHITE_KNIGHT>() | getPieceBoard<BLACK_KNIGHT>();

            if (bishopBB || knightBB) {
                return true;
            }
        }

        if (pieceCount == 4) {
            if (popcnt(getColorBoard<PieceColor::WHITE>()) != 2) {
                return false;
            }

            uint64_t whiteDrawPieces = getPieceBoard<WHITE_BISHOP>() | getPieceBoard<WHITE_KNIGHT>();
            uint64_t blackDrawPieces = getPieceBoard<BLACK_BISHOP>() | getPieceBoard<BLACK_KNIGHT>();

            // King not included in the above boards, so if there is only one piece left, it's a draw
            if (popcnt(whiteDrawPieces) == 1 && popcnt(blackDrawPieces) == 1) {
                return true;
            }
        }

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

    uint64_t Bitboard::getSquareAttacks(int8_t square) {
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
                getKingAttacks(square) &
                (getPieceBoard<PieceType::WHITE_KING>() | getPieceBoard<PieceType::BLACK_KING>());

        return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
    }

    uint8_t Bitboard::getCastlingRights() const {
        return castlingRights;
    }

    void Bitboard::setCastlingRights(uint8_t castlingRights) {
        Bitboard::castlingRights = castlingRights;
    }

    int8_t Bitboard::getEnPassantSquare() const {
        return enPassantSquare;
    }

    void Bitboard::setEnPassantSquare(int8_t enPassantSquare) {
        Bitboard::enPassantSquare = enPassantSquare;
    }

    uint64_t Bitboard::getFile(int8_t square) {
        return rayAttacks[Direction::NORTH][square] | rayAttacks[Direction::SOUTH][square] | (1ULL << square);
    }

    bool Bitboard::makeStrMove(const std::string &strMove) {
        int8_t fromSquare = getSquareFromString(strMove.substr(0, 2));
        int8_t toSquare = getSquareFromString(strMove.substr(2, 2));
        PieceType promotionPiece = PieceType::EMPTY;

        if (strMove.length() == 5) {
            if (strMove.ends_with("q")) {
                promotionPiece =
                        getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN;
            } else if (strMove.ends_with("r")) {
                promotionPiece = getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK;
            } else if (strMove.ends_with("b")) {
                promotionPiece =
                        getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP;
            } else if (strMove.ends_with("n")) {
                promotionPiece =
                        getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT;
            }
        }

        Move move = {fromSquare, toSquare, getPieceOnSquare(fromSquare), -1, promotionPiece};
        makeMove(move);
        return true;
    }

    Line Bitboard::getPreviousPvLine() {
        return previousPvLine;
    }

    void Bitboard::setPreviousPvLine(const Line &previousPvLine) {
        Bitboard::previousPvLine = previousPvLine;
    }

    uint8_t Bitboard::getPly() const {
        return ply;
    }

    void Bitboard::setPly(uint8_t ply) {
        Bitboard::ply = ply;
    }

    bool Bitboard::isOpenFile(int8_t square) {
        uint64_t fileMask = getFile(square);
        uint64_t occupied = getPieceBoard<PieceType::WHITE_PAWN>() | getPieceBoard<PieceType::BLACK_PAWN>();

        return fileMask == (fileMask & occupied);
    }
}