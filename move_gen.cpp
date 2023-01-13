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

#include <vector>
#include <cassert>
#include <immintrin.h>

#include "bitboard.h"
#include "move_gen.h"
#include "tt.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"

namespace Zagreus {
    int scoreMove(Bitboard &bitboard, Move &move, Line &previousPv, TranspositionTable* tt) {
        for (int i = 0; i < previousPv.moveCount; i++) {
            Move &pvMove = previousPv.moves[i];

            if (move.fromSquare == pvMove.fromSquare && move.toSquare == pvMove.toSquare && move.pieceType == pvMove.pieceType) {
                return 50000 - i;
            }
        }

        TTEntry* entry = tt->getEntry(move.zobristHash);

        if (entry->zobristHash == move.zobristHash) {
            return 25000 + entry->score;
        }

        if (move.captureScore >= 0) {
            return 10000 + move.captureScore;
        }

        uint32_t aMoveCode = encodeMove(move);
        if (tt->killerMoves[0][move.ply] == aMoveCode) {
            return 5000;
        }

        if (tt->killerMoves[1][move.ply] == aMoveCode) {
            return 4000;
        }

        if (tt->counterMoves[bitboard.getPreviousMoveFrom()][bitboard.getPreviousMoveTo()] ==
            aMoveCode) {
            return 3000;
        }

        if (move.captureScore < -1) {
            return move.captureScore - 5000;
        }

        return tt->historyMoves[move.pieceType][move.toSquare];
    }

    std::vector<Move> generateLegalMoves(Bitboard &bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(50);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        generatePawnMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN);
        generateKnightMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT);
        generateBishopMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP);
        generateRookMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK);
        generateQueenMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);
        generateKingMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING);

        Line &pvLine = bitboard.getPreviousPvLine();
        TranspositionTable* tt = TranspositionTable::getTT();

        std::sort(moves.begin(), moves.end(), [&bitboard, &pvLine, tt](Move &a, Move &b) {
            return scoreMove(bitboard, a, pvLine, tt) > scoreMove(bitboard, b, pvLine, tt);
        });

        return moves;
    }

    bool sortQuiesceMoves(Move &a, Move &b) {
        return a.captureScore > b.captureScore;
    }

    std::vector<Move> generateQuiescenceMoves(Bitboard &bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(50);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        generatePawnMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN, true);
        generateKnightMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT, true);
        generateBishopMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP, true);
        generateRookMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK, true);
        generateQueenMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN, true);
        generateKingMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING, true);

        std::sort(moves.begin(), moves.end(), sortQuiesceMoves);

        return moves;
    }

    void
    generatePawnMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color,
                      PieceType pieceType, bool quiesce) {
        uint64_t pawnBB = bitboard.getPieceBoard(pieceType);

        while (pawnBB) {
            uint8_t index = bitscanForward(pawnBB);
            pawnBB = _blsr_u64(pawnBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB;

            if (pieceType == PieceType::WHITE_PAWN) {
                genBB = bitboard.getWhitePawnDoublePush(1ULL << index);
            } else {
                genBB = bitboard.getBlackPawnDoublePush(1ULL << index);
            }

            uint64_t attackBB = bitboard.getPawnAttacks(index, color);

            if (bitboard.getEnPassantSquare(Bitboard::getOppositeColor(color)) >= 0) {
                attackBB &= opponentPiecesBB | (1ULL << bitboard.getEnPassantSquare(Bitboard::getOppositeColor(color)));
            } else {
                attackBB &= opponentPiecesBB;
            }

            attackBB &= ~ownPiecesBB;
            genBB &= ~ownPiecesBB;
            genBB |= attackBB;

            if (quiesce) {
                genBB &= (opponentPiecesBB | RANK_8 | RANK_1);
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = -1;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) :
                                   bitboard.mvvlva(pieceType, bitboard.getPieceOnSquare(genIndex));
                }

                if (genIndex >= 56 || genIndex <= 7) {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, static_cast<PieceType>(PieceType::WHITE_QUEEN + color)), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_QUEEN + color)});
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, static_cast<PieceType>(PieceType::WHITE_ROOK + color)), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_ROOK + color)});
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, static_cast<PieceType>(PieceType::WHITE_BISHOP + color)), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_BISHOP + color)});
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, static_cast<PieceType>(PieceType::WHITE_KNIGHT + color)), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_KNIGHT + color)});
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, PieceType::EMPTY), bitboard.getPly(),
                                     captureScore});
                }

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void
    generateKnightMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                        PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t knightBB = bitboard.getPieceBoard(pieceType);

        while (knightBB) {
            uint8_t index = bitscanForward(knightBB);
            knightBB = _blsr_u64(knightBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = -1;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) :
                                   bitboard.mvvlva(pieceType, bitboard.getPieceOnSquare(genIndex));
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, PieceType::EMPTY), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void
    generateBishopMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                        PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t bishopBB = bitboard.getPieceBoard(pieceType);

        while (bishopBB) {
            uint8_t index = bitscanForward(bishopBB);
            bishopBB = _blsr_u64(bishopBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getBishopAttacks(index, bitboard.getOccupiedBoard());

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = -1;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) :
                                   bitboard.mvvlva(pieceType, bitboard.getPieceOnSquare(genIndex));
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, PieceType::EMPTY), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void
    generateRookMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color,
                      PieceType pieceType, bool quiesce) {
        uint64_t rookBB = bitboard.getPieceBoard(pieceType);
        uint8_t castlingRights = bitboard.getCastlingRights();
        uint64_t occupiedBB = bitboard.getOccupiedBoard();

        while (rookBB) {
            uint8_t index = bitscanForward(rookBB);
            rookBB = _blsr_u64(rookBB);
            assert(index >= 0 && index < 64);

            // TODO: refactor, is very ugly
            if (!quiesce && !bitboard.isKingInCheck(color)) {
                if (color == PieceColor::WHITE) {
                    if (castlingRights & CastlingRights::WHITE_KINGSIDE && index == Square::H1) {
                        uint64_t tilesBetween = (1ULL << Square::F1) | (1ULL << Square::G1);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H1, Square::E1, PieceType::WHITE_ROOK,
                                                 bitboard.getZobristForMove(Square::H1, Square::E1, pieceType, PieceType::EMPTY), 0, 0});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::WHITE_QUEENSIDE && index == Square::A1) {
                        uint64_t tilesBetween = (1ULL << Square::C1) | (1ULL << Square::D1);

                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B1)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A1, Square::E1, PieceType::WHITE_ROOK,
                                                 bitboard.getZobristForMove(Square::A1, Square::E1, pieceType, PieceType::EMPTY), 0, 0});
                            }
                        }
                    }
                } else {
                    if (castlingRights & CastlingRights::BLACK_KINGSIDE && index == Square::H8) {
                        uint64_t tilesBetween = (1ULL << Square::F8) | (1ULL << Square::G8);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H8, Square::E8, PieceType::BLACK_ROOK,
                                                 bitboard.getZobristForMove(Square::H8, Square::E8, pieceType, PieceType::EMPTY), 0, 0});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::BLACK_QUEENSIDE && index == Square::A8) {
                        uint64_t tilesBetween = (1ULL << Square::C8) | (1ULL << Square::D8);


                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B8)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A8, Square::E8, PieceType::BLACK_ROOK,
                                                 bitboard.getZobristForMove(Square::A8, Square::E8, pieceType, PieceType::EMPTY), 0, 0});
                            }
                        }
                    }
                }
            }

            uint64_t genBB = bitboard.getRookAttacks(index, bitboard.getOccupiedBoard());;

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = -1;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) :
                                   bitboard.mvvlva(pieceType, bitboard.getPieceOnSquare(genIndex));
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, PieceType::EMPTY), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void
    generateQueenMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                       PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t queenBB = bitboard.getPieceBoard(pieceType);

        while (queenBB) {
            uint8_t index = bitscanForward(queenBB);
            queenBB = _blsr_u64(queenBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getQueenAttacks(index, bitboard.getOccupiedBoard());

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = -1;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) :
                                   bitboard.mvvlva(pieceType, bitboard.getPieceOnSquare(genIndex));
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, PieceType::EMPTY), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void
    generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentKingBB = bitboard.getPieceBoard(
                color == PieceColor::WHITE ? PieceType::BLACK_KING : PieceType::WHITE_KING);
        uint8_t index = bitscanForward(kingBB);
        assert(index >= 0 && index < 64);
        uint64_t genBB = bitboard.getKingAttacks(index);
        uint64_t opponentKingAttacks = bitboard.getKingAttacks(bitscanForward(opponentKingBB));

        genBB &= ~ownPiecesBB;

        if (quiesce) {
            genBB &= opponentPiecesBB;
        }

        genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) | bitboard.getPieceBoard(PieceType::BLACK_KING) |
                   opponentKingAttacks);

        while (genBB) {
            uint8_t genIndex = bitscanForward(genBB);
            assert(genIndex >= 0 && genIndex < 64);

            int captureScore = -1;

            if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) :
                               bitboard.mvvlva(pieceType, bitboard.getPieceOnSquare(genIndex));
            }

            moves.push_back({index, genIndex, pieceType, bitboard.getZobristForMove(index, genIndex, pieceType, PieceType::EMPTY), bitboard.getPly(),
                             captureScore});

            genBB &= ~(1ULL << genIndex);
        }
    }
}
#pragma clang diagnostic pop