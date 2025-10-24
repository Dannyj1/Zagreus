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
// Tuned PeSTO's piece-square tables from: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
// For every table: The first square in the table [0] is square A8 and the last square [63] is H1
// Midgame pawn PST
int mg_pawn_table[64] = {0,   0,  0,   0,   0,   0,  0,  0,   97,  131, 60, 95,  68, 121, 34, -11,
                         -6,  7,  26,  31,  65,  57, 25, -20, -14, 13,  6,  20,  22, 12,  17, -23,
                         -28, -2, -5,  11,  16,  6,  10, -27, -27, -4,  -4, -10, 3,  3,   34, -12,
                         -34, -1, -19, -22, -15, 25, 40, -19, 0,   0,   0,  0,   0,  0,   0,  0};

// Midgame knight PST
int mg_knight_table[64] = {-168, -90, -34, -49, 62, -96, -15, -107, -73,  -41, 70,  35,  23,  63,  7,   -17,
                           -47,  60,  37,  64,  83, 131, 74,  43,   -9,   17,  19,  53,  36,  68,  18,  22,
                           -13,  4,   16,  13,  28, 19,  21,  -8,   -23,  -9,  12,  10,  19,  17,  25,  -16,
                           -29,  -51, -12, -3,  -1, 18,  -14, -19,  -103, -21, -59, -32, -17, -28, -19, -23};

// Midgame bishop PST
int mg_bishop_table[64] = {-28, 4,  -81, -37, -25, -42, 7,  -8, -26, 16, -18, -13, 30,  58,  17,  -47,
                           -16, 37, 42,  39,  34,  50,  37, -2, -4,  5, 19, 49, 36, 37, 7, -2,
                           -6,  13, 13,  26,  34,  12,  10, 4,  0,   15, 15,  14,  14,  27,  18,  10,
                           4,   15, 16,  0,   7,   21,  36, 1,  -33, -3, -13, -21, -13, -12, -39, -21};

// Midgame rook PST
int mg_rook_table[64] = {32,  42,  32,  51, 63, 9,  31, 43,  27,  32,  57,  62,  81, 68, 26,  43,
                         -5,  19,  27,  36, 17, 45, 61, 16,  -23, -11, 7,   26,  24, 35, -8,  -20,
                         -36, -26, -12, -1, 9,  -7, 6,  -23, -45, -25, -16, -17, 3,  0,  -5,  -33,
                         -44, -16, -20, -9, -1, 12, -6, -71, -19, -13, 1,   17,  16, 7,  -37, -27};

// Midgame queen PST
int mg_queen_table[64] = {-28, 0,   29, 12, 59, 44, 44, 44, -24, -40, -5,  1,   -16, 55,  29,  54,
                          -13, -16, 7,  8,   29, 55, 47, 57, -26, -26, -16, -17, -1,  17,  -2,  1,
                          -9,  -26, -9, -10, -2, -4, 3,  -3, -14, 2,   -11, -2,  -5,  2,   14,  5,
                          -35, -8,  11, 2,   8,  15, -3, 1,  -1,  -17, -9,  10,  -15, -25, -31, -50};

// Midgame king PST
int mg_king_table[64] = {-64, 23, 16,  -15, -56, -34, 2,   13,  29,  -1,  -20, -7,  -8,  -4,  -38, -29,
                         -9,  24, 2,   -16, -20, 6,   22,  -22, -17, -20, -12, -27, -30, -25, -14, -36,
                         -49, -1, -27, -39, -45, -44, -33, -52, -14, -14, -22, -46, -45, -30, -15, -27,
                         1,   7,  -8,  -64, -44, -16, 9,   8,   -15, 36,  12,  -54, 8,   -28, 26,  14};

// Endgame pawn PST
int eg_pawn_table[64] = {0,  0,  0,  0,  0,  0, 0,  0,  180, 165, 158, 133, 148, 132, 164, 181, 99, 100, 87, 69, 57, 53,
                         83, 86, 33, 24, 13, 5, -2, 4,  17,  17,  13,  9,   -3,  -7,  -7,  -8,  3,  -1,  4,  7,  -6, 1,
                         0,  -5, -1, -8, 13, 8, 8,  10, 13,  0,   2,   -7,  0,   0,   0,   0,   0,  0,   0,  0};

// Endgame knight PST
int eg_knight_table[64] = {-58, -38, -13, -29, -31, -26, -62, -97, -25, -8,  -25, -2,  -9,  -25, -24, -53,
                           -25, -20, 10,  9,   -1,  -9,  -20, -41, -17, 3,   22,  22,  22,  11,  8,   -18,
                           -18, -6,  16,  24,  16,  17,  4,   -18, -23, -3,  -1,  15,  10,  -3,  -20, -22,
                           -42, -20, -10, -5,  -2,  -20, -23, -44, -28, -51, -23, -15, -22, -18, -50, -64};

// Endgame bishop PST
int eg_bishop_table[64] = {-14, -21, -11, -8, -7, -9, -17, -24, -8,  -4, 7,   -12, -3, -13, -4, -14,
                           2,   -8,  0,   -1, -2, 6,  0,   4,   -3,  9,  12,  9,   14, 10,  3,  2,
                           -6,  3,   13,  19, 7,  10, -3,  -9,  -12, -3, 8,   10,  13, 3,   -7, -15,
                           -14, -18, -7,  -1, 4,  -9, -15, -27, -23, -9, -22, -5,  -9, -16, -5, -17};

// Endgame rook PST
int eg_rook_table[64] = {13, 10,  18, 15,  12, 12, 8, 5, 11, 13, 13,  11, -3, 3, 8,  3,  7,  7,   7,  5,  4,  -3,
                         -5, -3,  4,  3,   13, 1,  2, 1, -1, 2,  3,   5,  8,  4, -5, -6, -8, -11, -4, 0,  -5, -1,
                         -7, -12, -8, -16, -6, -6, 0, 2, -9, -9, -11, -3, -9, 2, 3,  -1, -5, -13, 4,  -20};

// Endgame queen PST
int eg_queen_table[64] = {-9,  22,  22,  26,  26,  19,  10,  20,  -17, 20,  32,  41,  57, 25,  30,  0,
                          -21, 6,   9,   50,  48,  34,  19,  9,   3,   22,  24,  45,  56, 41,  58,  36,
    -18, 28, 19, 46, 30, 34, 39, 23,
    -16, -27, 15, 6, 9, 17, 10, 5,
    -22, -23, -30, -15, -16, -23, -36, -32,
    -34, -27, -22, -42, -5, -32, -20, -41
};

// Endgame king PST
int eg_king_table[64] = {
    -73, -35, -18, -18, -11, 15, 4, -17,
    -12, 17, 14, 17, 17, 38, 23, 11,
    10, 17, 22, 15, 20, 45, 45, 13,
    -8, 22, 24, 27, 26, 33, 26, 3,
    -18, -4, 21, 23, 26, 23, 9, -11,
    -19, -3, 11, 21, 23, 16, 7, -9,
    -27, -11, 4, 13, 14, 4, -5, -17,
    -52, -34, -21, -11, -28, -14, -23, -42
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