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

#include <x86intrin.h>
#include <iostream>

#include "movegen.h"
#include "utils.h"

namespace Zagreus {
    template<PieceColor color>
    MoveList generateMoves(Bitboard &bitboard) {
        MoveList moveList;

        std::cout << "0" << std::endl;
        generatePawnMoves<color>(bitboard, moveList);
        std::cout << "1" << std::endl;
        generateKnightMoves<color>(bitboard, moveList);
        std::cout << "2" << std::endl;
        generateBishopMoves<color>(bitboard, moveList);
        std::cout << "3" << std::endl;
        generateRookMoves<color>(bitboard, moveList);
        std::cout << "4" << std::endl;
        generateQueenMoves<color>(bitboard, moveList);
        std::cout << "5" << std::endl;
        generateKingMoves<color>(bitboard, moveList);
        std::cout << "6" << std::endl;

        return moveList;
    }

    template<PieceColor color>
    void generatePawnMoves(Bitboard &bitboard, MoveList &moveList) {
        uint64_t pawnBB;

        if (color == PieceColor::WHITE) {
            pawnBB = bitboard.getPieceBoard<WHITE_PAWN>();
        } else {
            pawnBB = bitboard.getPieceBoard<BLACK_PAWN>();
        }

        while (pawnBB) {
            int8_t index = bitscanForward(pawnBB);
            pawnBB = _blsr_u64(pawnBB);
            uint64_t genBB = bitboard.getPawnDoublePush<color>(1ULL << index);

            genBB |= bitboard.getPawnAttacks<color>(1ULL << index);
            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() | bitboard.getPieceBoard<BLACK_KING>());

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB = _blsr_u64(genBB);

                if (color == PieceColor::WHITE) {
                    if (genIndex >= Square::A8) {
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_PAWN, PieceType::WHITE_QUEEN };
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_PAWN, PieceType::WHITE_ROOK };
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_PAWN, PieceType::WHITE_BISHOP };
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_PAWN, PieceType::WHITE_KNIGHT };
                    } else {
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_PAWN };
                    }
                } else {
                    if (genIndex <= Square::H1) {
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_PAWN, PieceType::BLACK_QUEEN };
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_PAWN, PieceType::BLACK_ROOK };
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_PAWN, PieceType::BLACK_BISHOP };
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_PAWN, PieceType::BLACK_KNIGHT };
                    } else {
                        moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_PAWN };
                    }
                }
            }
        }
    }

    template<PieceColor color>
    void generateKnightMoves(Bitboard &bitboard, MoveList &moveList) {
        uint64_t knightBB;

        if (color == PieceColor::WHITE) {
            knightBB = bitboard.getPieceBoard<WHITE_KNIGHT>();
        } else {
            knightBB = bitboard.getPieceBoard<BLACK_KNIGHT>();
        }

        while (knightBB) {
            int8_t index = bitscanForward(knightBB);
            knightBB = _blsr_u64(knightBB);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB = _blsr_u64(genBB);

                if (color == PieceColor::WHITE) {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_KNIGHT };
                } else {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_KNIGHT };
                }
            }
        }
    }

    template<PieceColor color>
    void generateBishopMoves(Bitboard &bitboard, MoveList &moveList) {
        uint64_t bishopBB;

        if (color == PieceColor::WHITE) {
            bishopBB = bitboard.getPieceBoard<WHITE_BISHOP>();
        } else {
            bishopBB = bitboard.getPieceBoard<BLACK_BISHOP>();
        }

        while (bishopBB) {
            int8_t index = bitscanForward(bishopBB);
            bishopBB = _blsr_u64(bishopBB);
            uint64_t genBB = bitboard.getBishopAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB = _blsr_u64(genBB);

                if (color == PieceColor::WHITE) {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_BISHOP };
                } else {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_BISHOP };
                }
            }
        }
    }

    template<PieceColor color>
    void generateRookMoves(Bitboard &bitboard, MoveList &moveList) {
        uint64_t rookBB;

        if (color == PieceColor::WHITE) {
            rookBB = bitboard.getPieceBoard<WHITE_ROOK>();
        } else {
            rookBB = bitboard.getPieceBoard<BLACK_ROOK>();
        }

        while (rookBB) {
            int8_t index = bitscanForward(rookBB);
            rookBB = _blsr_u64(rookBB);
            uint64_t genBB = bitboard.getRookAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB = _blsr_u64(genBB);

                if (color == PieceColor::WHITE) {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_ROOK };
                } else {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_ROOK };
                }
            }
        }
    }

    template<PieceColor color>
    void generateQueenMoves(Bitboard &bitboard, MoveList &moveList) {
        uint64_t queenBB;

        if (color == PieceColor::WHITE) {
            queenBB = bitboard.getPieceBoard<WHITE_QUEEN>();
        } else {
            queenBB = bitboard.getPieceBoard<BLACK_QUEEN>();
        }

        while (queenBB) {
            int8_t index = bitscanForward(queenBB);
            queenBB = _blsr_u64(queenBB);
            uint64_t genBB = bitboard.getQueenAttacks(index);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard<WHITE_KING>() |
                       bitboard.getPieceBoard<BLACK_KING>());

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB = _blsr_u64(genBB);

                if (color == PieceColor::WHITE) {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_QUEEN };
                } else {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_QUEEN };
                }
            }
        }
    }

    template<PieceColor color>
    void generateKingMoves(Bitboard &bitboard, MoveList &moveList) {
        uint64_t kingBB;
        uint64_t opponentKingBB;

        if (color == PieceColor::WHITE) {
            kingBB = bitboard.getPieceBoard<WHITE_KING>();
            opponentKingBB = bitboard.getPieceBoard<BLACK_KING>();
        } else {
            kingBB = bitboard.getPieceBoard<BLACK_KING>();
            opponentKingBB = bitboard.getPieceBoard<WHITE_KING>();
        }

        while (kingBB) {
            int8_t index = bitscanForward(kingBB);
            kingBB = _blsr_u64(kingBB);
            uint64_t genBB = bitboard.getKingAttacks(index);
            int8_t opponentKingSquare = bitscanForward(opponentKingBB);

            genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getKingAttacks(opponentKingSquare));

            while (genBB) {
                int8_t genIndex = bitscanForward(genBB);
                genBB = _blsr_u64(genBB);

                if (color == PieceColor::WHITE) {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::WHITE_KING };
                } else {
                    moveList.moves[moveList.count++] = { index, genIndex, PieceType::BLACK_KING };
                }
            }
        }
    }

    template MoveList generateMoves<PieceColor::WHITE>(Bitboard &bitboard);
    template MoveList generateMoves<PieceColor::BLACK>(Bitboard &bitboard);
}