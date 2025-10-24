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
#include "eval_features.h"
#include "types.h"

namespace Zagreus {
// Tuned PeSTO's piece-square tables from: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
// For every table: The first square in the table [0] is square A8 and the last square [63] is H1
// Midgame pawn PST
int mg_pawn_table[64] = {0,   0,   0,   0,   0,  0,  0,  0,   22,  39,  23,  33,  82, 31,  -12, -123,
                         -42, -22, -9,  -16, -4, 43, 1,  -53, -44, -18, -20, -6,  4,  -3,  -9,  -46,
                         -54, -25, -19, 1,   -2, -9, -9, -61, -48, -21, -13, -14, 5,  -13, 27,  -26,
                         -47, -9,  -19, -11, -1, 24, 47, -22, 0,   0,   0,   0,   0,  0,   0,   0};

// Midgame knight PST
int mg_knight_table[64] = {-187, -103, -6, -56, 21,  -82, -110, -93, -8,  36, 88, 103, 67, 134, 90, 40,
                           55,   72,   89, 127, 145, 135, 113,  59,  81,  88, 83, 126, 94, 112, 76, 101,
                           68,   66,   90, 94,  102, 109, 88,   55,  54,  66, 94, 88,  97, 92, 96, 59,
                           52,   42,   79, 96,  92,  83,  77,   70,  -18, 62, 28, 64,  59, 67,  55, -24};

// Midgame bishop PST
int mg_bishop_table[64] = {-38, -65, -86, -60, -94, -45, -46, -96, -21, 3,  -12, -6, 5,  -1, -13, -36,
                           -17, -2,  0,   10,  38,  30,  19,  38,  -15, 3,  13,  21, 27, 5,  13,  7,
                           -5,  23,  14,  37,  44,  14,  11,  -17, 32,  28, 30,  20, 24, 34, 17,  27,
                           -15, 43,  19,  21,  25,  34,  58,  14,  19,  4,  23,  9,  -4, 0,  18,  3};

// Midgame rook PST
int mg_rook_table[64] = {43,  56,  32,  -5, 58,  30, 108, 4,   -18, -12, 18, 44, 11, 57, 34,  51,
                         -16, -8,  -3,  34, 68,  80, 63,  61,  -45, -27, 6,  23, 3,  15, 4,   15,
                         -39, -46, -39, -8, -10, 0,  -3,  -8,  -39, -8,  -5, 19, 12, 7,  9,   -17,
                         -46, -23, -14, -7, -6,  -6, 6,   -52, -15, -11, 9,  18, 21, 8,  -28, -4};

// Midgame queen PST
int mg_queen_table[64] = {-90, -101, -93, -54, 25, 8, -19, -51, -14, -46, -47, -67, -65, 8,  -39, 53,
                          -26, -22,  -41, 6,   11, 75, 100, 73,  -19, -7,  -6,  -14, 0,   -3, -9,  29,
                          -6,  -13,  -2,  -6,  2,  1,  12,  1,   -6,  18,  3,   9,   11,  20, 13,  3,
                          11,  15,   21,  28,  29, 46, 44,  34,  20,  9,   18,  33,  17,  -2, -14, 5};

// Midgame king PST
int mg_king_table[64] = {36,  74,  92,  104,  -17, 66,   32,  18,  55,  71, 48,  -15,  4,   72,  -39, -58,
                         -27, 100, 26,  20,   -48, 13,   62,  16,  -21, -6, 9,   -78,  -59, -55, -14, -59,
                         21,  7,   19,  -108, -77, -110, -37, -70, 2,   26, -82, -104, -88, -72, -19, -39,
                         39,  -16, -32, -86,  -73, -49,  18,  21,  -44, 26, 1,   -71,  -1,  -42, 40,  28};

// Endgame pawn PST
int eg_pawn_table[64] = {0,   0,   0,   0,  0,  0,  0,   0,   186, 176, 169, 135, 127, 138, 176, 169,
                         130, 123, 113, 95, 83, 70, 100, 111, 54,  41,  26,  14,  6,   14,  31,  32,
                         26,  22,  3,   -4, -7, -2, 10,  8,   19,  12,  4,   9,   4,   8,   4,   6,
                         31,  22,  15,  19, 12, 10, 3,   -1,  0,   0,   0,   0,   0,   0,   0,   0};

// Endgame knight PST
int eg_knight_table[64] = {-63, -94, -93, -64, -86, -86, -76, -128, -87,  -75, -78, -82, -90, -106, -90, -108,
                           -92, -78, -52, -60, -93, -74, -82, -96,  -73,  -58, -51, -44, -48, -52,  -59, -84,
                           -76, -62, -49, -42, -45, -60, -75, -61,  -74,  -71, -75, -48, -53, -67,  -78, -88,
                           -83, -76, -81, -75, -71, -77, -90, -77,  -100, -85, -64, -67, -86, -76,  -95, -86};

// Endgame bishop PST
int eg_bishop_table[64] = {-68, -67, -53, -56, -60, -62, -73, -50, -79, -68, -67, -68, -62, -73, -62, -79,
                           -52, -53, -55, -52, -71, -59, -67, -70, -48, -50, -59, -65, -65, -69, -65, -74,
                           -59, -65, -52, -64, -64, -45, -60, -69, -71, -63, -60, -49, -43, -59, -69, -81,
                           -60, -78, -69, -60, -58, -59, -72, -61, -84, -74, -73, -62, -49, -53, -88, -81};

// Endgame rook PST
int eg_rook_table[64] = {-7, -15, -10, -3,  -15, -20, -32, -10,
    -2,  4,  -5,  -11, -13, -26, -15, -25,
                         0,  -5,  -7,  -12, -31, -31, -23, -32, 6,   2,  -8,  -18, -11, -4,  -15, -10,
                         -2, -3,  5,   -1,  -5,  -17, -21, -22, -10, -8, -18, -22, -19, -26, -28, -21,
                         -5, -15, -1,  -7,  -8,  -6,  -15, -17, -7,  -8, -6,  -3,  -16, -11, 0,   -33};

// Endgame queen PST
int eg_queen_table[64] = {
    -25, 20, 0, 25, -11, -13, -46, -8,
    -8, -5, 20, 59, 68, 31, 28, -42,
    -17, -19, 43, 0, 36, -6, -50, -23,
    -23, -4, -24, 13, 16, 45, 36, 21,
    0, 2, -6, 12, 23, 14, 15, 21,
    -25, -35, -1, 6, 15, 19, 13, 31,
    -12, -15, -9, -20, -3, -61, -33, -14,
    -27, -8, -25, -41, -18, -8, -29, -36
};

// Endgame king PST
int eg_king_table[64] = {
    -84, -44, -22, -35, -9, -8, -2, -33,
    -46, -4, -1, 0, 4, 17, 49, 30,
    -12, 1, 9, 9, 22, 25, 36, 16,
    -21, 5, 14, 27, 21, 32, 30, 10,
    -39, -9, 9, 34, 34, 43, 15, 7,
    -33, -9, 26, 35, 37, 36, 14, -1,
    -34, -1, 17, 30, 30, 28, 2, -13,
    -17, -27, -15, -5, -22, 0, -36, -54
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