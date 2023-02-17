/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
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

#include <x86intrin.h>
#include <cassert>

#include "bitboard.h"
#include "movegen.h"
#include "utils.h"
#include "tt.h"

namespace Zagreus {
    int scoreMove(Bitboard &bitboard, Line &previousPv, Move &move, uint32_t moveCode, uint32_t bestMoveCode, TranspositionTable* tt) {
        for (int i = 0; i < previousPv.moveCount; i++) {
            if (moveCode == encodeMove(previousPv.moves[i])) {
                return 50000 - i;
            }
        }

        if (moveCode == bestMoveCode) {
            return 25000;
        }

        if (move.captureScore >= 0) {
            return 10000 + move.captureScore;
        }

        if (tt->killerMoves[0][bitboard.getPly()] == moveCode) {
            return 5000;
        }

        if (tt->killerMoves[1][bitboard.getPly()] == moveCode) {
            return 4000;
        }

        if (tt->killerMoves[2][bitboard.getPly()] == moveCode) {
            return 3000;
        }

        Move previousMove = bitboard.getPreviousMove();
        if (tt->counterMoves[previousMove.from][previousMove.to] == moveCode) {
            return 2000;
        }

        if (move.captureScore < -1) {
            return move.captureScore - 5000;
        }

        return tt->historyMoves[move.piece][move.to];
    }

    template<PieceColor color>
    MoveList generateMoves(Bitboard &bitboard) {
        MoveList moveList{};

        generatePawnMoves<color>(bitboard, moveList);
        generateKnightMoves<color>(bitboard, moveList);
        generateBishopMoves<color>(bitboard, moveList);
        generateRookMoves<color>(bitboard, moveList);
        generateQueenMoves<color>(bitboard, moveList);
        generateKingMoves<color>(bitboard, moveList);

        assert(moveList.size <= MAX_MOVES);
        TranspositionTable* tt = TranspositionTable::getTT();
        Line previousPv = bitboard.getPreviousPvLine();
        TTEntry entry = tt->getEntry(bitboard.getZobristHash());
        uint32_t bestMoveCode = 0;

        if (entry.zobristHash == bitboard.getZobristHash()) {
            bestMoveCode = entry.bestMoveCode;
        }

        for (int i = 0; i < moveList.size; i++) {
            Move &move = moveList.moves[i];
            move.score = scoreMove(bitboard, previousPv, move, encodeMove(move), bestMoveCode, tt);
        }

        return moveList;
    }

    template<PieceColor color>
    MoveList generateQuiescenceMoves(Bitboard &bitboard) {
        MoveList moveList{};

        generatePawnMoves<color>(bitboard, moveList, true);
        generateKnightMoves<color>(bitboard, moveList, true);
        generateBishopMoves<color>(bitboard, moveList, true);
        generateRookMoves<color>(bitboard, moveList, true);
        generateQueenMoves<color>(bitboard, moveList, true);
        generateKingMoves<color>(bitboard, moveList, true);

        assert(moveList.size <= MAX_MOVES);
        TranspositionTable* tt = TranspositionTable::getTT();
        Line previousPv = bitboard.getPreviousPvLine();
        TTEntry entry = tt->getEntry(bitboard.getZobristHash());
        uint32_t bestMoveCode = 0;

        if (entry.zobristHash == bitboard.getZobristHash()) {
            bestMoveCode = entry.bestMoveCode;
        }

        for (int i = 0; i < moveList.size; i++) {
            Move &move = moveList.moves[i];
            move.score = scoreMove(bitboard, previousPv, move, encodeMove(move), bestMoveCode, tt);
        }

        return moveList;
    }

    template<PieceColor color>
    void generatePawnMoves(Bitboard &bitboard, MoveList &moveList, bool quiesce = false) {
        uint64_t pawnBB;

        if (color == PieceColor::WHITE) {
            pawnBB = bitboard.getPieceBoard<WHITE_PAWN>();
        } else {
            pawnBB = bitboard.getPieceBoard<BLACK_PAWN>();
        }

        while (pawnBB) {
            int8_t index = bitscanForward(pawnBB);
            pawnBB &= ~(1ULL << index);
            uint64_t genBB = bitboard.getPawnDoublePush<color>(1ULL << index);

            uint64_t attackableSquares = bitboard.getColorBoard<color == PieceColor::WHITE ? PieceColor::BLACK : PieceColor::WHITE>();

            if (bitboard.getEnPassantSquare() > NO_SQUARE) {
                attackableSquares |= 1ULL << bitboard.getEnPassantSquare();
            }

            genBB |= (bitboard.getPawnAttacks<color>(index) & attackableSquares);
            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() | bitboard.getPieceBoard<BLACK_KING>());

            if (quiesce) {
                if (color == PieceColor::WHITE) {
                    genBB &= bitboard.getColorBoard<PieceColor::BLACK>();
                } else {
                    genBB &= bitboard.getColorBoard<PieceColor::WHITE>();
                }
            }

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB &= ~(1ULL << genIndex);
                PieceType capturedPiece = bitboard.getPieceOnSquare(genIndex);
                int captureScore = NO_CAPTURE_SCORE;

                if (color == PieceColor::WHITE) {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::WHITE_PAWN, capturedPiece);
                    }

                    if (genIndex >= Square::A8) {
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_PAWN, captureScore, PieceType::WHITE_QUEEN };
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_PAWN, captureScore, PieceType::WHITE_ROOK };
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_PAWN, captureScore, PieceType::WHITE_BISHOP };
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_PAWN, captureScore, PieceType::WHITE_KNIGHT };
                    } else {
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_PAWN, captureScore};
                    }
                } else {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::BLACK_PAWN, capturedPiece);
                    }

                    if (genIndex <= Square::H1) {
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_PAWN, captureScore, PieceType::BLACK_QUEEN };
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_PAWN, captureScore, PieceType::BLACK_ROOK };
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_PAWN, captureScore, PieceType::BLACK_BISHOP };
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_PAWN, captureScore, PieceType::BLACK_KNIGHT };
                    } else {
                        moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_PAWN, captureScore };
                    }
                }
            }
        }
    }

    template<PieceColor color>
    void generateKnightMoves(Bitboard &bitboard, MoveList &moveList, bool quiesce = false) {
        uint64_t knightBB;

        if (color == PieceColor::WHITE) {
            knightBB = bitboard.getPieceBoard<WHITE_KNIGHT>();
        } else {
            knightBB = bitboard.getPieceBoard<BLACK_KNIGHT>();
        }

        while (knightBB) {
            int8_t index = bitscanForward(knightBB);
            knightBB &= ~(1ULL << index);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            if (quiesce) {
                if (color == PieceColor::WHITE) {
                    genBB &= bitboard.getColorBoard<PieceColor::BLACK>();
                } else {
                    genBB &= bitboard.getColorBoard<PieceColor::WHITE>();
                }
            }

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB &= ~(1ULL << genIndex);
                PieceType capturedPiece = bitboard.getPieceOnSquare(genIndex);
                int captureScore = NO_CAPTURE_SCORE;

                if (color == PieceColor::WHITE) {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::WHITE_KNIGHT, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_KNIGHT, captureScore };
                } else {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::BLACK_KNIGHT, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_KNIGHT, captureScore };
                }
            }
        }
    }

    template<PieceColor color>
    void generateBishopMoves(Bitboard &bitboard, MoveList &moveList, bool quiesce = false) {
        uint64_t bishopBB;

        if (color == PieceColor::WHITE) {
            bishopBB = bitboard.getPieceBoard<WHITE_BISHOP>();
        } else {
            bishopBB = bitboard.getPieceBoard<BLACK_BISHOP>();
        }

        while (bishopBB) {
            int8_t index = bitscanForward(bishopBB);
            bishopBB &= ~(1ULL << index);
            uint64_t genBB = bitboard.getBishopAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            if (quiesce) {
                if (color == PieceColor::WHITE) {
                    genBB &= bitboard.getColorBoard<PieceColor::BLACK>();
                } else {
                    genBB &= bitboard.getColorBoard<PieceColor::WHITE>();
                }
            }

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB &= ~(1ULL << genIndex);
                PieceType capturedPiece = bitboard.getPieceOnSquare(genIndex);
                int captureScore = NO_CAPTURE_SCORE;

                if (color == PieceColor::WHITE) {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::WHITE_BISHOP, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_BISHOP, captureScore };
                } else {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::BLACK_BISHOP, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_BISHOP, captureScore };
                }
            }
        }
    }

    template<PieceColor color>
    void generateRookMoves(Bitboard &bitboard, MoveList &moveList, bool quiesce = false) {
        uint64_t rookBB;

        if (color == PieceColor::WHITE) {
            rookBB = bitboard.getPieceBoard<WHITE_ROOK>();
        } else {
            rookBB = bitboard.getPieceBoard<BLACK_ROOK>();
        }

        while (rookBB) {
            int8_t index = bitscanForward(rookBB);
            rookBB &= ~(1ULL << index);
            uint64_t genBB = bitboard.getRookAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            if (quiesce) {
                if (color == PieceColor::WHITE) {
                    genBB &= bitboard.getColorBoard<PieceColor::BLACK>();
                } else {
                    genBB &= bitboard.getColorBoard<PieceColor::WHITE>();
                }
            }

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB &= ~(1ULL << genIndex);
                PieceType capturedPiece = bitboard.getPieceOnSquare(genIndex);
                int captureScore = NO_CAPTURE_SCORE;

                if (color == PieceColor::WHITE) {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::WHITE_ROOK, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_ROOK, captureScore };
                } else {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::BLACK_ROOK, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_ROOK, captureScore };
                }
            }
        }
    }

    template<PieceColor color>
    void generateQueenMoves(Bitboard &bitboard, MoveList &moveList, bool quiesce = false) {
        uint64_t queenBB;

        if (color == PieceColor::WHITE) {
            queenBB = bitboard.getPieceBoard<WHITE_QUEEN>();
        } else {
            queenBB = bitboard.getPieceBoard<BLACK_QUEEN>();
        }

        while (queenBB) {
            int8_t index = bitscanForward(queenBB);
            queenBB &= ~(1ULL << index);
            uint64_t genBB = bitboard.getQueenAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            if (quiesce) {
                if (color == PieceColor::WHITE) {
                    genBB &= bitboard.getColorBoard<PieceColor::BLACK>();
                } else {
                    genBB &= bitboard.getColorBoard<PieceColor::WHITE>();
                }
            }

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB &= ~(1ULL << genIndex);
                PieceType capturedPiece = bitboard.getPieceOnSquare(genIndex);
                int captureScore = NO_CAPTURE_SCORE;

                if (color == PieceColor::WHITE) {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::WHITE_QUEEN, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_QUEEN, captureScore };
                } else {
                    if (capturedPiece != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::BLACK_QUEEN, capturedPiece);
                    }

                    moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_QUEEN, captureScore };
                }
            }
        }
    }

    template<PieceColor color>
    void generateKingMoves(Bitboard &bitboard, MoveList &moveList, bool quiesce = false) {
        uint64_t kingBB;
        uint64_t opponentKingBB;

        if (color == PieceColor::WHITE) {
            kingBB = bitboard.getPieceBoard<WHITE_KING>();
            opponentKingBB = bitboard.getPieceBoard<BLACK_KING>();
        } else {
            kingBB = bitboard.getPieceBoard<BLACK_KING>();
            opponentKingBB = bitboard.getPieceBoard<WHITE_KING>();
        }

        int8_t index = bitscanForward(kingBB);
        uint64_t genBB = bitboard.getKingAttacks(index);
        int8_t opponentKingSquare = bitscanForward(opponentKingBB);

        genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getKingAttacks(opponentKingSquare));

        if (quiesce) {
            if (color == PieceColor::WHITE) {
                genBB &= bitboard.getColorBoard<PieceColor::BLACK>();
            } else {
                genBB &= bitboard.getColorBoard<PieceColor::WHITE>();
            }
        }

        while (genBB) {
            int8_t genIndex = bitscanForward(genBB);
            genBB &= ~(1ULL << genIndex);
            PieceType capturedPiece = bitboard.getPieceOnSquare(genIndex);
            int captureScore = NO_CAPTURE_SCORE;

            if (color == PieceColor::WHITE) {
                if (capturedPiece != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::WHITE_KING, capturedPiece);
                }

                moveList.moves[moveList.size++] = {index, genIndex, PieceType::WHITE_KING, captureScore };
            } else {
                if (capturedPiece != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture<color>(index, genIndex) : mvvlva(PieceType::BLACK_KING, capturedPiece);
                }

                moveList.moves[moveList.size++] = {index, genIndex, PieceType::BLACK_KING, captureScore };
            }
        }

        uint64_t occupiedBB = bitboard.getOccupiedBoard();

        if (color == PieceColor::WHITE) {
            if (bitboard.getCastlingRights() & CastlingRights::WHITE_KINGSIDE && (occupiedBB & WHITE_KING_SIDE_BETWEEN) == 0
            && bitboard.getPieceOnSquare(Square::H1) == PieceType::WHITE_ROOK && !bitboard.isKingInCheck<PieceColor::WHITE>()) {
                uint64_t tilesToCheck = WHITE_KING_SIDE_BETWEEN;
                bool canCastle = true;

                while (tilesToCheck) {
                    int8_t tileIndex = bitscanForward(tilesToCheck);
                    tilesToCheck &= ~(1ULL << tileIndex);

                    if (bitboard.isSquareAttackedByColor<PieceColor::BLACK>(tileIndex)) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle) {
                    moveList.moves[moveList.size++] = {index, Square::G1, PieceType::WHITE_KING, -1 };
                }
            }

            if (bitboard.getCastlingRights() & CastlingRights::WHITE_QUEENSIDE && (occupiedBB & WHITE_QUEEN_SIDE_BETWEEN) == 0
                && bitboard.getPieceOnSquare(Square::A1) == PieceType::WHITE_ROOK && !bitboard.isKingInCheck<PieceColor::WHITE>()) {
                uint64_t tilesToCheck = WHITE_QUEEN_SIDE_BETWEEN & ~(1ULL << Square::B1);
                bool canCastle = true;

                while (tilesToCheck) {
                    int8_t tileIndex = bitscanForward(tilesToCheck);
                    tilesToCheck &= ~(1ULL << tileIndex);

                    if (bitboard.isSquareAttackedByColor<PieceColor::BLACK>(tileIndex)) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle) {
                    moveList.moves[moveList.size++] = {index, Square::C1, PieceType::WHITE_KING, -1 };
                }
            }
        } else {
            if (bitboard.getCastlingRights() & CastlingRights::BLACK_KINGSIDE && (occupiedBB & BLACK_KING_SIDE_BETWEEN) == 0
            && bitboard.getPieceOnSquare(Square::H8) == PieceType::BLACK_ROOK && !bitboard.isKingInCheck<PieceColor::BLACK>()) {
                uint64_t tilesToCheck = BLACK_KING_SIDE_BETWEEN;
                bool canCastle = true;

                while (tilesToCheck) {
                    int8_t tileIndex = bitscanForward(tilesToCheck);
                    tilesToCheck &= ~(1ULL << tileIndex);

                    if (bitboard.isSquareAttackedByColor<PieceColor::WHITE>(tileIndex)) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle) {
                    moveList.moves[moveList.size++] = {index, Square::G8, PieceType::BLACK_KING, -1 };
                }
            }

            if (bitboard.getCastlingRights() & CastlingRights::BLACK_QUEENSIDE && (occupiedBB & BLACK_QUEEN_SIDE_BETWEEN) == 0
            && bitboard.getPieceOnSquare(Square::A8) == PieceType::BLACK_ROOK && !bitboard.isKingInCheck<PieceColor::BLACK>()) {
                uint64_t tilesToCheck = BLACK_QUEEN_SIDE_BETWEEN & ~(1ULL << Square::B8);
                bool canCastle = true;

                while (tilesToCheck) {
                    int8_t tileIndex = bitscanForward(tilesToCheck);
                    tilesToCheck &= ~(1ULL << tileIndex);

                    if (bitboard.isSquareAttackedByColor<PieceColor::WHITE>(tileIndex)) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle) {
                    moveList.moves[moveList.size++] = {index, Square::C8, PieceType::BLACK_KING, -1 };
                }
            }
        }
    }

    template MoveList generateMoves<PieceColor::WHITE>(Bitboard &bitboard);
    template MoveList generateMoves<PieceColor::BLACK>(Bitboard &bitboard);

    template MoveList generateQuiescenceMoves<PieceColor::WHITE>(Bitboard &bitboard);
    template MoveList generateQuiescenceMoves<PieceColor::BLACK>(Bitboard &bitboard);
}
