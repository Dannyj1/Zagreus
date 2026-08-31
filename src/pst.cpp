/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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
#include "eval_features.h"
#include "types.h"

namespace Zagreus {
// PeSTO's piece-square tables from: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
// For every table: The first square in the table [0] is square A8 and the last square [63] is H1
// Midgame pawn PST
int mg_pawn_table[64] = {
    4,   4,   4,   4,   4,  4,  4,  4,   43,  28,  20,  40, 68, 58, -4, -91,
                         -43, -14, -2,  -10, 13, 60, 2,  -38, -30, -11, -15, -1, 8,  3,  -7, -36,
                         -42, -20, -14, 3,   3,  1,  -9, -49, -33, -13, -8,  -7, 13, -4, 28, -17,
                         -27, 1,   -15, 0,   7,  32, 50, -6,  4,   4,   4,   4,  4,  4,  4,  4};

// Midgame knight PST
int mg_knight_table[64] = {-205, -75, -54, -54, 40, -115, -33, -125, -74, -34, 22,  29, 16, 63, 36, 3,
                           -18,  14,  7,   43,  68, 96,   47,  -1,   20,  13,  4,   43, 15, 27, 1,  40,
                           1,    2,   14,  19,  24, 31,   20,  -8,   -13, -1,  16,  13, 30, 19, 26, -8,
                           -3,   -13, 4,   28,  24, 22,   12,  13,   -60, -3,  -26, 0,  -6, 5,  -6, -42};

// Midgame bishop PST
int mg_bishop_table[64] = {-34, -45, -55, -44, -71, -56, -29, -38, -43, -11, -25, -23, -10, -3,  -21, -43,
                           -24, -7,  -14, -15, 13,  25,  13,  30,  -34, -1,  -1,  6,   18,  -21, 11,  -3,
                           -9,  -3,  8,   32,  32,  0,   6,   -9,  29,  27,  27,  14,  23,  30,  13,  19,
                           -2,  44,  17,  21,  28,  36,  55,  16,  17,  1,   27,  17,  5,   6,   11,  13};

// Midgame rook PST
int mg_rook_table[64] = {16,  31,  25,  26,  50,  12,  29, 15,  -22, -43, 8,   41, 27, 49, 28,  30,
                         -12, 1,   -12, 23,  26,  56,  64, 32,  -38, -24, -8,  6,  -7, 9,  3,   3,
                         -41, -38, -40, -24, -26, -14, -8, -15, -40, -28, -19, -1, -3, -6, 1,   -16,
                         -38, -26, -17, -7,  -4,  -2,  10, -58, -16, -11, 9,   15, 20, 8,  -25, -5};

// Midgame queen PST
int mg_queen_table[64] = {-41, -17, 3,   11,  29, 42, 10, -2, -31, -53, -44, -51, -55, 7,   -34, 33,
                          -26, -29, -36, 6,   20, 57, 53, 54, -27, -17, -20, -23, -23, -15, -16, 10,
                          -14, -15, -8,  -15, -1, -5, 6,  -6, -11, 7,   -3,  3,   3,   14,  10,  -2,
                          -3,  9,   11,  19,  25, 37, 30, 23, 16,  17,  25,  31,  17,  3,   -18, -1};

// Midgame king PST
int mg_king_table[64] = {-20, 55, 50,  36,  17,  7,   15,  11,  19,  38,  24,  -14, 4,   38,  -20, -36,
                         7,   90, 8,   13,  -24, 21,  54,  -18, -8,  -10, 14,  -26, -40, -29, 5,   -52,
                         -31, 8,  -7,  -68, -61, -83, -37, -67, -7,  15,  -67, -87, -91, -57, -22, -30,
                         40,  -2, -32, -82, -67, -36, 15,  17,  -25, 37,  12,  -68, 0,   -32, 43,  28};

// Endgame pawn PST
int eg_pawn_table[64] = {-41, -41, -41, -41, -41, -41, -41, -41, 150, 152, 130, 91,  82,  98,  152, 155,
                         93,  84,  70,  51,  34,  25,  64,  73,  14,  1,   -15, -26, -34, -27, -11, -10,
                         -14, -17, -39, -46, -43, -43, -31, -32, -21, -24, -37, -31, -36, -33, -35, -37,
                         -14, -22, -21, -31, -32, -28, -37, -42, -41, -41, -41, -41, -41, -41, -41, -41};

// Endgame knight PST
int eg_knight_table[64] = {-28, -43, -17, -11, -29, -20, -39, -97, -17, 2,   2,  3,  -13, -15, -24, -43,
                           -12, 3,   42,  37,  9,   14,  -8,  -18, -12, 33,  45, 55, 45,  42,  30,  -10,
                           -3,  26,  42,  47,  46,  30,  5,   0,   -8,  14,  14, 38, 33,  16,  -3,  -19,
                           -24, -11, -5,  0,   6,   5,   -21, -25, -45, -10, -1, -2, -5,  1,   -19, -44};

// Endgame bishop PST
int eg_bishop_table[64] = {-5,  -23, -6, -2, -7, 2,  -14, -5,  -7,  4,   5,   0,  3,  -8, 4,   -17,
                           5,   11,  17, 25, 1,  15, 2,   -7,  14,  18,  17,  21, 16, 8,  1,   -6,
                           -5,  7,   19, 11, 15, 22, -2,  -14, -16, 1,   9,   22, 19, 4,  -9,  -16,
                           -11, -20, -8, 0,  1,  -4, -10, -14, -22, -10, -12, -2, -4, -2, -23, -25};

// Endgame rook PST
int eg_rook_table[64] = {14, 4,   6,   1,   -10, 3, -5, 1, 10, 27, 11,  5, -6, -2, 1, -10, 10, 6,   8,  0,  -9, -16,
                         -8, -10, 13,  10,  11,  6, 5,  5, -6, -4, 8,   7, 15, 11, 8, -2,  -7, -13, -2, 1,  -2, -5,
                         -4, -10, -18, -13, -2,  0, 6,  2, -2, -4, -10, 0, 3,  3,  0, 1,   -9, -5,  4,  -26};

// Endgame queen PST
int eg_queen_table[64] = {-6,  6,   -4,  14, -4,  0,   -23, 5,   2,   5,   13,  46,  52,  17,  6,   -28,
                          -23, -3,  21,  13, 33,  3,   -10, -13,
    -17, 9, -19, 15, 39, 51, 37, 33,
    5, 5, -7, 22, 15, 11, 23, 14,
    -27, -39, 4, 0, 12, 13, 18, 5,
    -5, -15, -10, -9, -16, -40, -36, -29,
    -21, -30, -27, -36, -13, -26, -23, -17
};

// Endgame king PST
int eg_king_table[64] = {
    -74, -26, -30, -19, -12, 6, -15, -41,
    -25, 2, 4, 5, 10, 26, 28, 14,
    -15, 7, 15, 12, 23, 31, 33, 16,
    -29, 5, 13, 24, 27, 32, 24, 11,
    -37, -7, 14, 29, 36, 37, 15, 0,
    -34, -8, 20, 33, 37, 29, 12, -7,
    -40, -9, 11, 26, 25, 20, -1, -22,
    -44, -37, -20, -6, -25, -11, -41, -65
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
}  // namespace Zagreus