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

#include "types.h"

namespace Zagreus {
    // PST tables from https://www.chessprogramming.org/Simplified_Evaluation_Function
    const int pawnTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int knightTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int bishopTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int rookTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int queenTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int kingMidgameTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    // TODO: use this in lategame, implement tapered eval.
    const int kingEndgameTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    static int pst[12][64]{};

    int getPstValue(PieceType piece, int square) {
        return pst[piece][square];
    }

    void initializePst() {
        for (int piece = 0; piece < 12; piece++) {
            for (int square = 0; square < 64; square++) {
                PieceType pieceType = static_cast<PieceType>(piece);

                switch (pieceType) {
                    case PieceType::WHITE_PAWN:
                        pst[pieceType][square] = pawnTable[square];
                        break;
                    case PieceType::BLACK_PAWN:
                        pst[pieceType][square] = pawnTable[63 - square];
                        break;
                    case PieceType::WHITE_KNIGHT:
                        pst[pieceType][square] = knightTable[square];
                        break;
                    case PieceType::BLACK_KNIGHT:
                        pst[pieceType][square] = knightTable[63 - square];
                        break;
                    case PieceType::WHITE_BISHOP:
                        pst[pieceType][square] = bishopTable[square];
                        break;
                    case PieceType::BLACK_BISHOP:
                        pst[pieceType][square] = bishopTable[63 - square];
                        break;
                    case PieceType::WHITE_ROOK:
                        pst[pieceType][square] = rookTable[square];
                        break;
                    case PieceType::BLACK_ROOK:
                        pst[pieceType][square] = rookTable[63 - square];
                        break;
                    case PieceType::WHITE_QUEEN:
                        pst[pieceType][square] = queenTable[square];
                        break;
                    case PieceType::BLACK_QUEEN:
                        pst[pieceType][square] = queenTable[63 - square];
                        break;
                    case PieceType::WHITE_KING:
                        pst[pieceType][square] = kingMidgameTable[square];
                        break;
                    case PieceType::BLACK_KING:
                        pst[pieceType][square] = kingMidgameTable[63 - square];
                        break;
                }
            }
        }
    }
}