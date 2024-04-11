/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
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

#include <vector>

#include "types.h"

namespace Zagreus {
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 81, 90, 53, 95, 70, 108, 35, 5, -11, 4, 37, 24,
                            25, 50, 8, -18, -15, 0, 0, 23, 17, 3, 0, -10, -37, -15, 0, 20, 18, 4,
                            -12, -26, -18, 0, 0, 5, 3, 0, -2, -18, -17, 14, 0, -8, -3, 4, 16, -12,
                            0, 0, 0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 187, 179, 164, 139, 150, 150, 192, 211, 112,
                            106, 85, 84, 77, 76, 102, 106, 30, 27, 17, 6, 7, 14, 24, 24, 0, 0, -8,
                            -9, -11, -12, 1, -3, -8, -7, -6, 1, -1, -10, -8, -6, 0, -7, 3, 15, 2, 4,
                            -7, -2, 0, 0, 0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-168, -86, -46, -37, 48, -77, -17, -119, -59, -24, 76, 59, 41, 74, -8,
                              -25, -30, 47, 35, 61, 85, 83, 55, 34, 18, 17, 20, 38, 34, 27, 14, 33,
                              -7, 5, 16, 16, 21, 20, 18, 0, -30, 5, 10, 18, 20, 9, 5, -20, -3, -21,
                              -6, 22, 21, 15, 2, 4, -94, -12, -29, -17, -10, -18, -12, -34};
int endgameKnightTable[64] = {-55, -39, -22, -14, -27, -20, -58, -88, -28, -16, -30, -6, -12, -23,
                              -24, -43, -17, -32, 6, 0, -6, -12, -29, -29, -12, 9, 18, 21, 25, 15,
                              9, -12, -4, 4, 14, 29, 20, 10, -10, -12, -14, -5, -15, 11, 9, -8, -23,
                              -20, -34, -11, -10, -13, -15, -22, -20, -27, -29, -32, -8, -8, -12,
                              -14, -35, -57};

int midgameBishopTable[64] = {-27, 0, -63, -42, -36, -48, 0, 0, -17, -4, -13, -1, 11, 28, 3, -20, 0,
                              23, 38, 22, 16, 49, 23, 3, -16, 0, -3, 17, 18, -2, 8, -15, -6, 3, 0,
                              25, 37, 4, 9, -10, 20, 21, 23, 12, 15, 34, 17, 30, 14, 46, 20, 19, 23,
                              22, 50, -8, -3, -13, 3, -12, 1, 10, -20, 0};
int endgameBishopTable[64] = {-11, -27, -6, -8, -12, -10, -27, -17, -18, -10, 0, -5, -21, -13, -7,
                              -15, -3, -7, 1, 0, 3, 0, -9, -3, 0, 9, 1, 9, 14, 5, 1, -7, -14, -1,
                              22, 11, 0, 20, -9, -10, -15, -11, 13, 17, 18, 4, -5, -19, -10, -13,
                              -6, 9, 0, -9, -9, -17, -18, -14, -4, 0, -6, -11, -5, -16};

int midgameRookTable[64] = {35, 37, 34, 46, 60, 15, 33, 40, 10, 12, 56, 48, 73, 50, 21, 39, 13, 12,
                            20, 23, 28, 29, 55, 6, -15, -7, 0, 26, 7, 20, 0, -19, -30, -25, -22,
                            -13, -2, -20, 0, -26, -35, -22, -18, -8, -1, -2, -8, -30, -50, -7, -16,
                            0, -7, -16, -9, -37, 0, -11, 11, 9, 8, 10, -5, -5};
int endgameRookTable[64] = {10, 6, 5, 9, -2, 13, 9, 14, 10, 17, 5, 3, -3, 11, 18, 3, 7, 8, 0, -1, 1,
                            3, -5, 0, 6, 10, 8, -3, 4, 0, 7, 12, 4, 9, 7, 0, 8, 6, -4, -2, 1, 3, 0,
                            -2, -5, -9, -11, -6, -1, -2, 1, -4, -1, 1, -4, 6, 0, 9, 0, -2, -8, 0, 0,
                            0};

int midgameQueenTable[64] = {-15, -4, 26, 4, 51, 27, 33, 13, -17, -59, -17, -17, -42, 21, -27, 16,
                             5, 5, 9, 6, 19, 27, 31, 21, 0, -13, -27, -18, -28, 2, -14, -2, -6, -13,
                             -16, -14, -19, -8, 5, -6, -18, 5, 7, -3, -3, 0, 2, 0, -13, 7, 13, 13,
                             17, 12, 23, 5, 1, -4, 0, 22, 18, -6, -9, -5};
int endgameQueenTable[64] = {18, 6, 23, 20, 17, 11, 8, -3, 5, 35, 54, 41, 43, 17, 8, -8, -8, 18, 19,
                             59, 47, 21, 5, -6, 18, 42, 34, 45, 54, 25, 43, 29, 6, 32, 18, 45, 37,
                             25, 33, 21, -8, -12, 14, 0, 12, 29, 16, 6, -1, -20, -18, 6, 0, -26,
                             -23, -12, -22, -18, -23, -28, -13, -10, -14, -9};

int midgameKingTable[64] = {-58, 29, 23, -9, -48, -28, 0, 9, 23, 4, -10, 0, -4, 3, -29, -30, 1, 34,
                            10, -5, -16, 12, 28, -31, -16, -22, 0, -26, -26, -6, -13, -39, -41, 1,
                            -37, -38, -44, -36, -31, -47, -19, -17, -26, -57, -50, -28, -6, -27, 22,
                            16, -28, -69, -64, -35, 16, 5, 12, 19, -37, -2, -6, -35, 14, 0};
int endgameKingTable[64] = {-56, -30, -13, -14, 0, 0, 3, -31, -7, 23, 20, 9, 6, 28, 30, 1, 5, 26,
                            25, 20, 22, 33, 32, 11, -12, 13, 14, 25, 20, 18, 10, 0, -17, -9, 14, 22,
                            24, 14, 0, -13, -16, 0, 12, 30, 23, 15, 0, -17, -29, -6, 16, 30, 28, 19,
                            0, -19, -56, -30, -2, -21, -16, 0, -30, -56};

// Base tables from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
int baseMidgamePawnTable[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 98, 134, 61, 95, 68, 126, 34, -11,
    -6, 7, 26, 31, 65, 56, 25, -20, -14, 13, 6, 21, 23, 12, 17, -23,
    -27, -2, -5, 12, 17, 6, 10, -25, -26, -4, -4, -10, 3, 3, 33, -12,
    -35, -1, -20, -23, -15, 24, 38, -22, 0, 0, 0, 0, 0, 0, 0, 0,
};

int baseEndgamePawnTable[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 178, 173, 158, 134, 147, 132, 165, 187, 94, 100, 85, 67, 56, 53,
    82, 84, 32, 24, 13, 5, -2, 4, 17, 17, 13, 9, -3, -7, -7, -8, 3, -1, 4, 7, -6, 1,
    0, -5, -1, -8, 13, 8, 8, 10, 13, 0, 2, -7, 0, 0, 0, 0, 0, 0, 0, 0,
};

int baseMidgameKnightTable[64] = {
    -167, -89, -34, -49, 61, -97, -15, -107, -73, -41, 72, 36, 23, 62, 7, -17,
    -47, 60, 37, 65, 84, 129, 73, 44, -9, 17, 19, 53, 37, 69, 18, 22,
    -13, 4, 16, 13, 28, 19, 21, -8, -23, -9, 12, 10, 19, 17, 25, -16,
    -29, -53, -12, -3, -1, 18, -14, -19, -105, -21, -58, -33, -17, -28, -19, -23,
};

int baseEndgameKnightTable[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99, -25, -8, -25, -2, -9, -25, -24, -52,
    -24, -20, 10, 9, -1, -9, -19, -41, -17, 3, 22, 22, 22, 11, 8, -18,
    -18, -6, 16, 25, 16, 17, 4, -18, -23, -3, -1, 15, 10, -3, -20, -22,
    -42, -20, -10, -5, -2, -20, -23, -44, -29, -51, -23, -15, -22, -18, -50, -64,
};

int baseMidgameBishopTable[64] = {
    -29, 4, -82, -37, -25, -42, 7, -8, -26, 16, -18, -13, 30, 59, 18, -47,
    -16, 37, 43, 40, 35, 50, 37, -2, -4, 5, 19, 50, 37, 37, 7, -2,
    -6, 13, 13, 26, 34, 12, 10, 4, 0, 15, 15, 15, 14, 27, 18, 10,
    4, 15, 16, 0, 7, 21, 33, 1, -33, -3, -14, -21, -13, -12, -39, -21,
};

int baseEndgameBishopTable[64] = {
    -14, -21, -11, -8, -7, -9, -17, -24, -8, -4, 7, -12, -3, -13, -4, -14,
    2, -8, 0, -1, -2, 6, 0, 4, -3, 9, 12, 9, 14, 10, 3, 2,
    -6, 3, 13, 19, 7, 10, -3, -9, -12, -3, 8, 10, 13, 3, -7, -15,
    -14, -18, -7, -1, 4, -9, -15, -27, -23, -9, -23, -5, -9, -16, -5, -17,
};

int baseMidgameRookTable[64] = {
    32, 42, 32, 51, 63, 9, 31, 43, 27, 32, 58, 62, 80, 67, 26, 44,
    -5, 19, 26, 36, 17, 45, 61, 16, -24, -11, 7, 26, 24, 35, -8, -20,
    -36, -26, -12, -1, 9, -7, 6, -23, -45, -25, -16, -17, 3, 0, -5, -33,
    -44, -16, -20, -9, -1, 11, -6, -71, -19, -13, 1, 17, 16, 7, -37, -26,
};

int baseEndgameRookTable[64] = {
    13, 10, 18, 15, 12, 12, 8, 5, 11, 13, 13, 11, -3, 3, 8, 3, 7, 7, 7, 5, 4, -3,
    -5, -3, 4, 3, 13, 1, 2, 1, -1, 2, 3, 5, 8, 4, -5, -6, -8, -11, -4, 0, -5, -1,
    -7, -12, -8, -16, -6, -6, 0, 2, -9, -9, -11, -3, -9, 2, 3, -1, -5, -13, 4, -20,
};

int baseMidgameQueenTable[64] = {
    -28, 0, 29, 12, 59, 44, 43, 45, -24, -39, -5, 1, -16, 57, 28, 54,
    -13, -17, 7, 8, 29, 56, 47, 57, -27, -27, -16, -16, -1, 17, -2, 1,
    -9, -26, -9, -10, -2, -4, 3, -3, -14, 2, -11, -2, -5, 2, 14, 5,
    -35, -8, 11, 2, 8, 15, -3, 1, -1, -18, -9, 10, -15, -25, -31, -50,
};

int baseEndgameQueenTable[64] = {
    -9, 22, 22, 27, 27, 19, 10, 20, -17, 20, 32, 41, 58, 25, 30, 0,
    -20, 6, 9, 49, 47, 35, 19, 9, 3, 22, 24, 45, 57, 40, 57, 36,
    -18, 28, 19, 47, 31, 34, 39, 23, -16, -27, 15, 6, 9, 17, 10, 5,
    -22, -23, -30, -16, -16, -23, -36, -32, -33, -28, -22, -43, -5, -32, -20, -41,
};

int baseMidgameKingTable[64] = {
    -65, 23, 16, -15, -56, -34, 2, 13, 29, -1, -20, -7, -8, -4, -38, -29,
    -9, 24, 2, -16, -20, 6, 22, -22, -17, -20, -12, -27, -30, -25, -14, -36,
    -49, -1, -27, -39, -46, -44, -33, -51, -14, -14, -22, -46, -44, -30, -15, -27,
    1, 7, -8, -64, -43, -16, 9, 8, -15, 36, 12, -54, 8, -28, 24, 14,
};

int baseEndgameKingTable[64] = {-74, -35, -18, -18, -11, 15, 4, -17, -12, 17, 14, 17, 17,
                                38, 23, 11, 10, 17, 23, 15, 20, 45, 44, 13, -8, 22,
                                24, 27, 26, 33, 26, 3, -18, -4, 21, 24, 27, 23, 9,
                                -11, -19, -3, 11, 21, 23, 16, 7, -9, -27, -11, 4, 13,
                                14, 4, -5, -17, -53, -34, -21, -11, -28, -14, -24, -43};

static int midgamePst[12][64]{};
static int endgamePst[12][64]{};
static int baseMidgamePst[12][64]{};
static int baseEndgamePst[12][64]{};

int getMidgamePstValue(PieceType piece, int8_t square) {
    return midgamePst[piece][square];
}

int getEndgamePstValue(PieceType piece, int8_t square) {
    return endgamePst[piece][square];
}

void setMidgamePstValue(PieceType piece, int8_t square, int value) {
    midgamePst[piece][square] = value;
}

void setEndgamePstValue(PieceType piece, int8_t square, int value) {
    endgamePst[piece][square] = value;
}

std::vector<int> getBaseMidgameValues() {
    std::vector<int> values;

    for (int i = 1; i < 12; i += 2) {
        for (int j = 0; j < 64; j++) {
            values.emplace_back(baseMidgamePst[i][j]);
        }
    }

    return values;
}

std::vector<int> getBaseEndgameValues() {
    std::vector<int> values;

    for (int i = 1; i < 12; i += 2) {
        for (int j = 0; j < 64; j++) {
            values.emplace_back(baseEndgamePst[i][j]);
        }
    }

    return values;
}

std::vector<int> getMidgameValues() {
    std::vector<int> values;

    for (int i = 1; i < 12; i += 2) {
        for (int j = 0; j < 64; j++) {
            values.emplace_back(midgamePst[i][j]);
        }
    }

    return values;
}

std::vector<int> getEndgameValues() {
    std::vector<int> values;

    for (int i = 1; i < 12; i += 2) {
        for (int j = 0; j < 64; j++) {
            values.emplace_back(endgamePst[i][j]);
        }
    }

    return values;
}

void initializePst() {
    for (int piece = 0; piece < 12; piece++) {
        PieceType pieceType = static_cast<PieceType>(piece);

        for (int8_t square = 0; square < 64; square++) {
            switch (pieceType) {
                case WHITE_PAWN:
                    midgamePst[pieceType][square] = midgamePawnTable[square ^ 56];
                    endgamePst[pieceType][square] = endgamePawnTable[square ^ 56];
                    baseMidgamePst[pieceType][square] = baseMidgamePawnTable[square ^ 56];
                    baseEndgamePst[pieceType][square] = baseEndgamePawnTable[square ^ 56];
                    break;
                case BLACK_PAWN:
                    midgamePst[pieceType][square] = midgamePawnTable[square];
                    endgamePst[pieceType][square] = endgamePawnTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgamePawnTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgamePawnTable[square];
                    break;
                case WHITE_KNIGHT:
                    midgamePst[pieceType][square] = midgameKnightTable[square ^ 56];
                    endgamePst[pieceType][square] = endgameKnightTable[square ^ 56];
                    baseMidgamePst[pieceType][square] = baseMidgameKnightTable[square ^ 56];
                    baseEndgamePst[pieceType][square] = baseEndgameKnightTable[square ^ 56];
                    break;
                case BLACK_KNIGHT:
                    midgamePst[pieceType][square] = midgameKnightTable[square];
                    endgamePst[pieceType][square] = endgameKnightTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameKnightTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameKnightTable[square];
                    break;
                case WHITE_BISHOP:
                    midgamePst[pieceType][square] = midgameBishopTable[square ^ 56];
                    endgamePst[pieceType][square] = endgameBishopTable[square ^ 56];
                    baseMidgamePst[pieceType][square] = baseMidgameBishopTable[square ^ 56];
                    baseEndgamePst[pieceType][square] = baseEndgameBishopTable[square ^ 56];
                    break;
                case BLACK_BISHOP:
                    midgamePst[pieceType][square] = midgameBishopTable[square];
                    endgamePst[pieceType][square] = endgameBishopTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameBishopTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameBishopTable[square];
                    break;
                case WHITE_ROOK:
                    midgamePst[pieceType][square] = midgameRookTable[square ^ 56];
                    endgamePst[pieceType][square] = endgameRookTable[square ^ 56];
                    baseMidgamePst[pieceType][square] = baseMidgameRookTable[square ^ 56];
                    baseEndgamePst[pieceType][square] = baseEndgameRookTable[square ^ 56];
                    break;
                case BLACK_ROOK:
                    midgamePst[pieceType][square] = midgameRookTable[square];
                    endgamePst[pieceType][square] = endgameRookTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameRookTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameRookTable[square];
                    break;
                case WHITE_QUEEN:
                    midgamePst[pieceType][square] = midgameQueenTable[square ^ 56];
                    endgamePst[pieceType][square] = endgameQueenTable[square ^ 56];
                    baseMidgamePst[pieceType][square] = baseMidgameQueenTable[square ^ 56];
                    baseEndgamePst[pieceType][square] = baseEndgameQueenTable[square ^ 56];
                    break;
                case BLACK_QUEEN:
                    midgamePst[pieceType][square] = midgameQueenTable[square];
                    endgamePst[pieceType][square] = endgameQueenTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameQueenTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameQueenTable[square];
                    break;
                case WHITE_KING:
                    midgamePst[pieceType][square] = midgameKingTable[square ^ 56];
                    endgamePst[pieceType][square] = endgameKingTable[square ^ 56];
                    baseMidgamePst[pieceType][square] = baseMidgameKingTable[square ^ 56];
                    baseEndgamePst[pieceType][square] = baseEndgameKingTable[square ^ 56];
                    break;
                case BLACK_KING:
                    midgamePst[pieceType][square] = midgameKingTable[square];
                    endgamePst[pieceType][square] = endgameKingTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameKingTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameKingTable[square];
                    break;
            }
        }
    }
}
} // namespace Zagreus