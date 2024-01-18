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
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 91, 116, 59, 92, 66, 120, 34, -8, -6, 6, 30, 24,
                            49, 56, 24, -12, -14, 7, 4, 18, 17, 19, 19, -11, -37, -10, -6, 14, 19,
                            9, 3, -29, -22, -5, -10, 1, 0, 0, 5, -20, -19, 14, -2, -9, -7, 10, 33,
                            -8, 0, 0, 0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 175, 163, 151, 133, 145, 128, 165, 187, 92, 95,
                            82, 66, 43, 49, 76, 82, 29, 18, 8, -4, 0, 4, 10, 21, 9, 0, 0, -4, -15,
                            -13, -2, 7, 0, 5, -2, 8, 4, -9, -12, -8, 13, 7, 12, 21, 17, 0, -4, -8,
                            0, 0, 0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-164, -87, -40, -44, 56, -89, -16, -109, -63, -34, 59, 35, 23, 65, 3,
                              -20, -43, 52, 35, 68, 82, 109, 69, 38, -1, 13, 19, 36, 34, 54, 14, 27,
                              -8, 6, 18, 16, 20, 17, 20, -7, -25, -1, 14, 14, 13, 11, 16, -17, -21,
                              -45, -8, 14, 16, 19, -7, -8, -102, -19, -50, -29, -17, -27, -21, -22};
int endgameKnightTable[64] = {-54, -37, -13, -23, -33, -23, -62, -100, -28, -2, -30, -1, -6, -19,
                              -25, -47, -22, -30, 10, 9, 0, -13, -18, -40, -13, 7, 23, 19, 22, 10,
                              7, -12, -16, -5, 14, 24, 15, 22, 4, -18, -20, -8, -13, 14, 12, -7,
                              -20, -23, -36, -22, -6, -4, -5, -21, -18, -43, -30, -43, -18, -12,
                              -14, -18, -49, -62};

int midgameBishopTable[64] = {-24, 3, -78, -37, -25, -36, 5, -7, -31, 9, -17, -5, 27, 50, 16, -41,
                              0, 31, 35, 32, 26, 44, 30, 3, -6, 1, 9, 32, 27, 20, 12, -4, -5, 9, 0,
                              22, 31, 5, 9, -1, 9, 19, 14, 11, 14, 26, 9, 23, 7, 34, 16, 11, 17, 22,
                              46, 1, -24, -5, -5, -17, -11, 0, -28, -13};
int endgameBishopTable[64] = {-10, -23, -8, -9, -9, -6, -22, -20, -11, -10, 3, -7, -5, -19, -9, -9,
                              9, -15, -2, -1, -9, 0, -4, 0, -7, 8, 4, 0, 8, 0, -2, -5, -12, 0, 11,
                              15, 0, 0, -2, -8, -10, -4, 8, 15, 9, 0, -6, -5, -14, -7, 0, 7, 11,
                              -10, -8, -24, -24, -12, -8, 0, -7, -12, -2, -14};

int midgameRookTable[64] = {29, 43, 31, 52, 63, 10, 31, 41, 20, 19, 55, 51, 74, 57, 28, 36, 0, 16,
                            26, 34, 19, 43, 56, 13, -20, -8, 7, 26, 22, 30, -7, -20, -34, -21, -18,
                            -6, 2, -7, 4, -24, -45, -19, -16, -16, 1, -1, -2, -34, -52, -17, -18,
                            -7, -1, -2, -6, -58, -1, -13, 0, 4, 5, 0, -13, -11};
int endgameRookTable[64] = {11, 9, 17, 15, 11, 11, 9, 2, 4, 9, 11, 9, -7, 0, 7, 0, 9, 5, 4, 5, 10,
                            -1, -3, -6, 3, 4, 14, -1, 0, 0, -2, 0, 6, 8, 5, 2, -9, -7, -12, -12, -1,
                            0, -7, -1, -9, -9, -11, -15, -4, -3, 0, 1, -12, -15, -8, 2, -7, 5, 3,
                            -7, -13, -9, 14, -10};

int midgameQueenTable[64] = {-29, 0, 29, 10, 60, 41, 41, 39, -26, -50, -14, -4, -23, 51, 8, 40, 0,
                             -16, -3, 1, 29, 43, 38, 46, -8, -20, -12, -20, -14, 11, -4, -4, -15,
                             -19, -19, -13, -11, -5, 1, -5, -16, -4, -3, -8, -6, -3, 7, 0, -26, -13,
                             3, 7, 9, 10, 3, 4, -3, -20, -11, 7, 10, -12, -22, -23};
int endgameQueenTable[64] = {-8, 21, 20, 25, 26, 17, 10, 19, -13, 20, 27, 39, 53, 23, 22, -8, -19,
                             8, 6, 52, 45, 30, 17, 4, 12, 23, 26, 44, 47, 34, 54, 35, -13, 37, 17,
                             46, 35, 32, 41, 22, -16, -32, 23, 6, 8, 21, 6, 4, -17, -21, -31, -13,
                             -9, -23, -34, -29, -31, -27, -23, -35, -7, -28, -13, -30};

int midgameKingTable[64] = {-65, 23, 16, -15, -56, -34, 2, 13, 28, -1, -20, -6, -7, -4, -38, -29,
                            -8, 24, 1, -15, -19, 6, 21, -21, -16, -18, -12, -28, -31, -25, -13, -35,
                            -49, -1, -27, -40, -49, -45, -33, -51, -15, -15, -23, -51, -49, -31,
                            -12, -23, 12, 8, -11, -73, -56, -17, 6, 0, 3, 20, -15, -13, -9, -33, 15,
                            -4};
int endgameKingTable[64] = {-74, -35, -18, -18, -11, 15, 4, -17, -12, 16, 14, 17, 17, 38, 23, 10,
                            10, 17, 22, 15, 19, 45, 43, 12, -7, 22, 24, 25, 24, 30, 27, 2, -18, -4,
                            19, 23, 25, 24, 9, -11, -21, -3, 17, 19, 25, 20, 11, -6, -29, -10, 7,
                            23, 20, 13, -7, -23, -53, -41, -29, -7, -23, -14, -32, -52};

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

int getMidgamePstValue(PieceType piece, int8_t square) { return midgamePst[piece][square]; }

int getEndgamePstValue(PieceType piece, int8_t square) { return endgamePst[piece][square]; }

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
                    midgamePst[pieceType][square] = midgamePawnTable[63 - square];
                    endgamePst[pieceType][square] = endgamePawnTable[63 - square];
                    baseMidgamePst[pieceType][square] = baseMidgamePawnTable[63 - square];
                    baseEndgamePst[pieceType][square] = baseEndgamePawnTable[63 - square];
                    break;
                case BLACK_PAWN:
                    midgamePst[pieceType][square] = midgamePawnTable[square];
                    endgamePst[pieceType][square] = endgamePawnTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgamePawnTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgamePawnTable[square];
                    break;
                case WHITE_KNIGHT:
                    midgamePst[pieceType][square] = midgameKnightTable[63 - square];
                    endgamePst[pieceType][square] = endgameKnightTable[63 - square];
                    baseMidgamePst[pieceType][square] = baseMidgameKnightTable[63 - square];
                    baseEndgamePst[pieceType][square] = baseEndgameKnightTable[63 - square];
                    break;
                case BLACK_KNIGHT:
                    midgamePst[pieceType][square] = midgameKnightTable[square];
                    endgamePst[pieceType][square] = endgameKnightTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameKnightTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameKnightTable[square];
                    break;
                case WHITE_BISHOP:
                    midgamePst[pieceType][square] = midgameBishopTable[63 - square];
                    endgamePst[pieceType][square] = endgameBishopTable[63 - square];
                    baseMidgamePst[pieceType][square] = baseMidgameBishopTable[63 - square];
                    baseEndgamePst[pieceType][square] = baseEndgameBishopTable[63 - square];
                    break;
                case BLACK_BISHOP:
                    midgamePst[pieceType][square] = midgameBishopTable[square];
                    endgamePst[pieceType][square] = endgameBishopTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameBishopTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameBishopTable[square];
                    break;
                case WHITE_ROOK:
                    midgamePst[pieceType][square] = midgameRookTable[63 - square];
                    endgamePst[pieceType][square] = endgameRookTable[63 - square];
                    baseMidgamePst[pieceType][square] = baseMidgameRookTable[63 - square];
                    baseEndgamePst[pieceType][square] = baseEndgameRookTable[63 - square];
                    break;
                case BLACK_ROOK:
                    midgamePst[pieceType][square] = midgameRookTable[square];
                    endgamePst[pieceType][square] = endgameRookTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameRookTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameRookTable[square];
                    break;
                case WHITE_QUEEN:
                    midgamePst[pieceType][square] = midgameQueenTable[63 - square];
                    endgamePst[pieceType][square] = endgameQueenTable[63 - square];
                    baseMidgamePst[pieceType][square] = baseMidgameQueenTable[63 - square];
                    baseEndgamePst[pieceType][square] = baseEndgameQueenTable[63 - square];
                    break;
                case BLACK_QUEEN:
                    midgamePst[pieceType][square] = midgameQueenTable[square];
                    endgamePst[pieceType][square] = endgameQueenTable[square];
                    baseMidgamePst[pieceType][square] = baseMidgameQueenTable[square];
                    baseEndgamePst[pieceType][square] = baseEndgameQueenTable[square];
                    break;
                case WHITE_KING:
                    midgamePst[pieceType][square] = midgameKingTable[63 - square];
                    endgamePst[pieceType][square] = endgameKingTable[63 - square];
                    baseMidgamePst[pieceType][square] = baseMidgameKingTable[63 - square];
                    baseEndgamePst[pieceType][square] = baseEndgameKingTable[63 - square];
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
