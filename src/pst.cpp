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
// PeSTO's piece-square tables from: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
// For every table: The first square in the table [0] is square A8 and the last square [63] is H1
// Midgame pawn PST
int mg_pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    -37, 1, -14, 78, 36, -30, -75, -129,
    -41, -19, -5, 3, 26, 34, -39, -70,
    -26, 8, -17, -9, 1, -6, 5, -46,
    -16, 8, 28, 48, 32, 23, 16, -13,
    16, 18, 29, 28, 67, 15, 89, 38,
    99, 148, 33, -8, 20, 185, 176, 114,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Midgame knight PST
int mg_knight_table[64] = {
    -167, -111, -34, -50, 60, -97, -27, -107,
    -73, -40, 72, 29, 16, 62, 7, -17,
    -49, 56, -12, 64, 83, 68, 71, 43,
    -10, 21, 17, 49, 36, 68, 21, 20,
    -9, 1, 18, 15, 27, 20, 18, -4,
    -21, -5, 50, 11, 20, 77, 27, -14,
    -29, -53, -11, 19, 8, 18, -13, -17,
    -105, 10, -58, -31, -16, -27, 0, -23
};

// Midgame bishop PST
int mg_bishop_table[64] = {
    -29, 4, -135, -38, -26, -76, 7, -8,
    -25, 17, -19, -28, 15, 58, -12, -47,
    -16, 37, 42, 26, 20, 45, 34, -1,
    -3, 8, 14, 50, 37, 24, 11, -2,
    -6, 11, 17, 26, 35, 19, 6, 5,
    1, 16, 18, 23, 25, 32, 21, 11,
    4, 24, 17, 24, 49, 22, 64, 1,
    -33, -3, 58, -19, -12, 36, -39, -21
};

// Midgame rook PST
int mg_rook_table[64] = {
    -105, 32, 18, 30, 46, -66, 28, -36,
    27, 33, 58, 61, 79, 66, 26, 44,
    -5, 19, 26, 36, 17, 45, 61, 16,
    -24, -11, 7, 26, 24, 35, -8, -20,
    -36, -26, -12, -1, 9, -7, 6, -23,
    -45, -25, -16, -17, 3, 1, -5, -33,
    -44, -16, -20, -8, -1, 12, -6, -71,
    122, -5, 14, 32, 28, 69, -35, 64
};

// Midgame queen PST
int mg_queen_table[64] = {
    -28, -1, 26, -61, 57, 43, 43, 45,
    -23, -37, -11, -5, -25, 56, 28, 54,
    -13, -20, 7, 3, 27, 53, 46, 57,
    -29, -27, -17, -17, -1, 17, -3, 2,
    -7, -26, -8, -9, -1, -4, 4, -4,
    -14, 7, -10, 1, -4, 4, 15, 5,
    -35, -7, 18, 8, 13, 15, -3, 1,
    0, -17, -7, 82, -14, -24, -31, -50
};

// Midgame king PST
int mg_king_table[64] = {
    -65, 20, 11, -18, -126, -39, -102, 7,
    29, -2, -21, -9, -12, -7, -44, -32,
    -9, 24, 2, -17, -21, 5, 21, -23,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49, -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -45, -43, -29, -14, -26,
    1, 7, -7, -62, -39, -12, 15, 12,
    -15, 38, 15, -51, 89, -24, 119, 18
};

// Endgame pawn PST
int eg_pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    114, 111, 126, 131, 136, 48, 94, 117,
    71, 77, 64, 56, 34, 35, 39, 47,
    24, 23, 1, -4, -8, -6, 12, 6,
    34, 22, 17, 9, 3, 12, 19, 24,
    36, 34, 19, 18, 39, 16, 53, 39,
    80, 76, 35, 15, 27, 94, 79, 68,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Endgame knight PST
int eg_knight_table[64] = {
    -58, -43, -13, -29, -32, -27, -67, -99,
    -25, -7, -25, -6, -12, -25, -24, -52,
    -24, -21, -3, 8, -2, -27, -20, -41,
    -17, 5, 21, 21, 22, 11, 10, -18,
    -16, -7, 17, 26, 16, 18, 3, -16,
    -22, -1, 9, 15, 11, 14, -19, -21,
    -42, -20, -10, 2, 2, -20, -22, -43,
    -29, -43, -23, -13, -21, -17, -45, -64
};

// Endgame bishop PST
int eg_bishop_table[64] = {
    -13, -21, -25, -9, -7, -19, -17, -24,
    -7, -5, 6, -17, -12, -13, -13, -14,
    3, -8, -1, -7, -9, 3, -1, 4,
    -2, 10, 10, 8, 12, 6, 4, 3,
    -6, 2, 15, 18, 7, 13, -4, -9,
    -11, -2, 10, 14, 19, 6, -5, -14,
    -14, -14, -6, 7, 17, -8, -3, -27,
    -23, -8, -2, -2, -8, -1, -5, -16
};

// Endgame rook PST
int eg_rook_table[64] = {
    -31, 3, 9, -2, 0, -17, 6, -22,
    12, 15, 13, 10, -4, 3, 9, 3,
    8, 8, 7, 5, 4, -3, -5, -3,
    4, 3, 13, 1, 2, 1, -1, 3,
    3, 5, 8, 5, -4, -6, -8, -11,
    -4, 0, -5, -1, -7, -11, -8, -16,
    -7, -7, 0, 2, -9, -9, -11, -3,
    37, 7, 11, 13, 4, 12, 5, 9
};

// Endgame queen PST
int eg_queen_table[64] = {
    -9, 22, 21, 18, 26, 19, 10, 20,
    -17, 20, 31, 39, 56, 25, 30, 0,
    -20, 5, 9, 48, 46, 34, 19, 9,
    3, 22, 24, 45, 57, 40, 57, 36,
    -18, 28, 19, 47, 31, 34, 39, 23,
    -16, -26, 15, 7, 9, 18, 10, 5,
    -22, -23, -28, -14, -15, -23, -36, -32,
    -33, -28, -22, -35, -5, -32, -20, -41
};

// Endgame king PST
int eg_king_table[64] = {
    -74, -38, -23, -22, -31, 8, -59, -24,
    -13, 15, 11, 12, 9, 31, 13, 5,
    10, 16, 21, 12, 16, 40, 41, 11,
    -8, 22, 23, 26, 25, 33, 26, 4,
    -18, -4, 22, 25, 28, 25, 11, -10,
    -19, -2, 12, 24, 27, 22, 12, -6,
    -27, -10, 7, 17, 21, 11, 8, -11,
    -53, -32, -18, -8, -6, -8, 36, -38
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
