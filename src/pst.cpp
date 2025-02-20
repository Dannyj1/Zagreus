/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

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

#include "pst.h"

#include <iostream>

#include "constants.h"
#include "eval.h"
#include "types.h"

namespace Zagreus {
// Midgame pawn PST
int mg_pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    86, 120, 54, 96, 68, 107, 21, -22,
    -11, 4, 22, 28, 61, 54, 21, -23,
    -16, 12, 1, 13, 16, 11, 15, -29,
    -27, -1, -3, 11, 16, 8, 10, -26,
    -22, -2, -1, -4, 11, 3, 39, -7,
    -23, 11, -16, -19, -9, 39, 50, -7,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Midgame knight PST
int mg_knight_table[64] = {
    -167, -91, -34, -49, 61, -97, -16, -107,
    -73, -41, 72, 36, 23, 62, 7, -17,
    -47, 59, 32, 65, 84, 119, 73, 44,
    -9, 17, 19, 53, 36, 69, 18, 22,
    -13, 4, 16, 14, 27, 19, 20, -8,
    -23, -9, 16, 10, 19, 24, 25, -16,
    -29, -53, -12, 1, 1, 18, -14, -19,
    -105, -17, -58, -33, -17, -28, -18, -23
};

// Midgame bishop PST
int mg_bishop_table[64] = {
    -29, 4, -80, -37, -25, -45, 7, -8,
    -26, 18, -19, -15, 28, 59, 18, -47,
    -16, 37, 43, 36, 33, 49, 36, -1,
    -4, 5, 18, 49, 37, 34, 7, -2,
    -6, 12, 13, 26, 33, 11, 9, 4,
    0, 15, 15, 14, 14, 27, 18, 11,
    4, 18, 16, 2, 12, 21, 40, 1,
    -33, -3, 0, -21, -13, -7, -39, -21
};

// Midgame rook PST
int mg_rook_table[64] = {
    17, 42, 31, 49, 62, 1, 31, 35,
    27, 32, 58, 62, 80, 67, 26, 44,
    -5, 19, 26, 36, 17, 45, 61, 16,
    -24, -11, 7, 26, 24, 35, -8, -20,
    -36, -26, -12, -1, 9, -7, 6, -23,
    -45, -25, -16, -17, 3, 0, -5, -33,
    -44, -16, -20, -9, -1, 11, -6, -71,
    -3, -12, 3, 17, 17, 15, -36, -18
};

// Midgame queen PST
int mg_queen_table[64] = {
    -28, 0, 29, 1, 59, 44, 43, 45,
    -24, -39, -6, 1, -16, 57, 28, 54,
    -13, -17, 7, 7, 29, 56, 47, 57,
    -27, -27, -16, -16, -1, 17, -2, 1,
    -9, -26, -9, -10, -2, -4, 3, -3,
    -14, 3, -11, -2, -5, 2, 14, 5,
    -35, -8, 12, 3, 9, 15, -3, 1,
    -1, -18, -9, 21, -15, -25, -31, -50
};

// Midgame king PST
int mg_king_table[64] = {
    -65, 23, 16, -15, -65, -34, -8, 13,
    29, -1, -20, -7, -8, -4, -38, -29,
    -9, 24, 2, -16, -20, 6, 22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49, -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
    1, 7, -8, -64, -43, -16, 10, 9,
    -15, 36, 12, -54, 16, -28, 34, 14
};

// Endgame pawn PST
int eg_pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    175, 170, 157, 134, 147, 127, 161, 184,
    94, 99, 84, 67, 55, 53, 81, 83,
    32, 24, 12, 3, -4, 3, 16, 16,
    15, 10, -3, -7, -7, -7, 4, 0,
    6, 8, -5, 3, 2, -4, 1, -5,
    16, 11, 9, 10, 14, 5, 6, -3,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Endgame knight PST
int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25, -8, -25, -2, -10, -25, -24, -52,
    -24, -20, 9, 9, -1, -11, -19, -41,
    -17, 3, 22, 22, 22, 11, 8, -18,
    -18, -6, 16, 25, 15, 17, 4, -18,
    -23, -3, -1, 15, 10, -3, -20, -22,
    -42, -20, -10, -5, -2, -20, -23, -44,
    -29, -50, -23, -15, -22, -18, -50, -64
};

// Endgame bishop PST
int eg_bishop_table[64] = {
    -14, -21, -10, -8, -7, -9, -17, -24,
    -8, -4, 7, -12, -3, -13, -4, -14,
    2, -8, 0, -2, -3, 6, 0, 4,
    -3, 9, 12, 8, 14, 9, 3, 2,
    -6, 3, 13, 18, 6, 10, -3, -9,
    -12, -3, 8, 10, 13, 3, -7, -15,
    -14, -18, -7, -1, 5, -9, -14, -27,
    -23, -9, -21, -5, -9, -15, -5, -17
};

// Endgame rook PST
int eg_rook_table[64] = {
    11, 10, 18, 14, 11, 11, 8, 4,
    11, 13, 13, 11, -3, 3, 8, 3,
    7, 7, 7, 5, 4, -3, -5, -3,
    4, 3, 13, 1, 2, 1, -1, 2,
    3, 5, 8, 4, -5, -6, -8, -11,
    -4, 0, -5, -1, -7, -12, -8, -16,
    -6, -6, 0, 2, -9, -9, -11, -3,
    -6, 3, 3, -1, -5, -11, 4, -19
};

// Endgame queen PST
int eg_queen_table[64] = {
    -9, 22, 22, 26, 27, 19, 10, 20,
    -17, 20, 32, 41, 58, 25, 30, 0,
    -20, 6, 9, 49, 47, 35, 19, 9,
    3, 22, 24, 45, 57, 40, 57, 36,
    -18, 28, 19, 47, 31, 34, 39, 23,
    -16, -27, 15, 6, 9, 17, 10, 5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -42, -5, -32, -20, -41
};

// Endgame king PST
int eg_king_table[64] = {
    -74, -35, -18, -18, -12, 15, 0, -17,
    -12, 17, 14, 17, 17, 38, 24, 11,
    10, 17, 23, 15, 20, 45, 44, 13,
    -8, 22, 24, 27, 25, 33, 26, 3,
    -18, -4, 21, 24, 27, 23, 9, -11,
    -19, -3, 11, 21, 23, 17, 7, -9,
    -27, -11, 4, 13, 15, 5, -4, -16,
    -53, -34, -21, -11, -27, -14, -22, -43
};

int midgamePstTable[PIECES][SQUARES]{};
int endgamePstTable[PIECES][SQUARES]{};

int* getMidgameTable(const PieceType pieceType) {
    switch (pieceType) {
        case PAWN:
            return mg_pawn_table;
        case KNIGHT:
            return mg_knight_table;
        case BISHOP:
            return mg_bishop_table;
        case ROOK:
            return mg_rook_table;
        case QUEEN:
            return mg_queen_table;
        case KING:
            return mg_king_table;
    }
}

int* getEndgameTable(const PieceType pieceType) {
    switch (pieceType) {
        case PAWN:
            return eg_pawn_table;
        case KNIGHT:
            return eg_knight_table;
        case BISHOP:
            return eg_bishop_table;
        case ROOK:
            return eg_rook_table;
        case QUEEN:
            return eg_queen_table;
        case KING:
            return eg_king_table;
    }
}

void initializePst() {
    for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        for (Square square = A1; square <= H8; square++) {
            const int midgamePieceValue = evalMaterialValues[MIDGAME][getPieceType(piece)];
            const int endgamePieceValue = evalMaterialValues[ENDGAME][getPieceType(piece)];
            const PieceColor color = getPieceColor(piece);

            if (color == WHITE) {
                midgamePstTable[piece][square] = midgamePieceValue + getMidgameTable(getPieceType(piece))[square ^ 56];
                endgamePstTable[piece][square] = endgamePieceValue + getEndgameTable(getPieceType(piece))[square ^ 56];
            } else {
                midgamePstTable[piece][square] = midgamePieceValue + getMidgameTable(getPieceType(piece))[square];
                endgamePstTable[piece][square] = endgamePieceValue + getEndgameTable(getPieceType(piece))[square];
            }
        }
    }
}
} // namespace Zagreus
