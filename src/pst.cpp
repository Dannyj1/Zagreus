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
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 75, 86, 54, 84, 65, 102, 44, 2, -15, 1, 39, 28,
                            17, 44, 17, -16, -16, -5, 2, 20, 15, 2, 0, -15, -37, -15, 0, 19, 19, 0,
                            -15, -29, -20, 0, -2, 1, 0, -3, -2, -18, -17, 14, 0, -9, -7, 5, 21, -10,
                            0, 0, 0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 190, 182, 164, 141, 148, 150, 187, 204, 109,
                            106, 84, 79, 83, 78, 98, 105, 31, 28, 15, 6, 10, 14, 27, 30, 4, 0, -8,
                            -10, -11, -7, 2, 0, -2, -7, -6, 2, 4, -10, -3, -3, 0, -8, 6, 12, 5, 4,
                            -10, -5, 0, 0, 0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-167, -84, -46, -38, 53, -86, -19, -117, -54, -23, 74, 58, 33, 77,
                              -11, -28, -36, 42, 42, 67, 77, 77, 52, 40, 14, 16, 22, 34, 37, 30, 15,
                              36, -3, 7, 17, 15, 22, 16, 17, -4, -27, 7, 11, 15, 19, 9, 8, -18, -3,
                              -17, -8, 23, 24, 13, 0, 0, -89, -11, -29, -14, -12, -14, -8, -31};
int endgameKnightTable[64] = {-53, -37, -24, -16, -35, -17, -60, -93, -35, -15, -30, -8, -10, -23,
                              -24, -39, -18, -28, 5, 0, 0, -10, -27, -33, -16, 14, 18, 23, 22, 14,
                              9, -15, -7, 0, 15, 34, 18, 9, -2, -13, -18, -3, -16, 11, 13, -9, -22,
                              -22, -33, -11, -6, -19, -19, -21, -19, -25, -31, -36, -13, -19, -12,
                              -16, -41, -58};

int midgameBishopTable[64] = {-28, 0, -61, -43, -40, -43, 0, 0, -18, -8, -17, -4, 8, 25, 3, -19, 0,
                              25, 37, 25, 19, 49, 24, 1, -10, 0, 0, 17, 15, -5, 10, -19, -8, 6, 0,
                              18, 29, 5, 1, -8, 14, 17, 25, 12, 17, 30, 12, 31, 17, 43, 15, 18, 22,
                              20, 49, 0, -4, -14, 5, -10, 5, 8, -17, -8};
int endgameBishopTable[64] = {-13, -26, -4, -2, -9, -9, -24, -19, -14, -10, 0, 0, -17, -21, -5, -23,
                              -2, -4, 1, -5, 3, -1, -8, -3, 0, 5, 2, 11, 18, 2, 2, -3, -17, 0, 19,
                              16, 4, 19, -5, -14, -9, -7, 7, 15, 17, 11, 2, -16, -14, -11, -5, 1, 0,
                              -10, -8, -16, -15, -18, -6, 0, -12, -10, -6, -12};

int midgameRookTable[64] = {32, 38, 37, 40, 52, 15, 35, 45, 0, 8, 46, 48, 64, 54, 9, 33, 8, 17, 19,
                            33, 30, 33, 52, 3, -15, -10, 5, 25, 10, 25, -2, -17, -32, -27, -26, -15,
                            -4, -10, -11, -31, -33, -22, -20, -14, 0, -7, -4, -23, -49, -11, -9, -5,
                            -4, -11, -11, -39, 0, -6, 10, 9, 8, 9, -6, -4};
int endgameRookTable[64] = {4, 7, 3, 6, -6, 15, 6, 7, 10, 16, 6, 0, -8, 3, 17, 0, 7, 9, 4, -5, 4,
                            -1, -5, 2, 5, 11, 7, -4, 3, 5, 5, 14, 5, 10, 8, 4, 5, 1, 0, 0, 3, 0, 2,
                            0, -5, -11, -10, -13, 1, -4, -1, -1, -8, 0, -2, 5, -5, 2, 0, 0, -3, 4,
                            4, -6};

int midgameQueenTable[64] = {-18, -2, 24, 4, 47, 28, 36, 20, -21, -53, -10, -21, -36, 29, -32, 17,
                             0, 5, 13, 3, 19, 26, 28, 21, 5, -16, -25, -13, -28, -4, -17, -2, -3,
                             -13, -19, -13, -20, -3, 6, -5, -21, 8, 12, 0, -1, 0, 0, 0, -5, 4, 11,
                             15, 15, 9, 16, 9, 2, -7, -3, 21, 17, -3, -6, -1};
int endgameQueenTable[64] = {14, 9, 19, 21, 18, 8, 4, -7, 7, 28, 44, 49, 41, 16, 11, -10, -10, 19,
                             15, 58, 43, 25, 6, -8, 12, 40, 39, 41, 56, 33, 49, 31, 2, 28, 21, 43,
                             33, 26, 28, 16, 0, -17, 16, 4, 0, 27, 9, 0, -9, -13, -15, 7, 4, -12,
                             -19, -12, -24, -18, -18, -31, -17, -18, -12, -12};

int midgameKingTable[64] = {-55, 22, 23, -8, -51, -33, 3, 8, 26, 0, -13, 0, -9, 0, -24, -28, -1, 33,
                            9, -7, -14, 7, 23, -22, -14, -20, -10, -18, -33, -13, -9, -31, -44, 1,
                            -36, -40, -42, -45, -30, -51, -20, -15, -30, -53, -47, -22, -11, -26,
                            21, 19, -28, -66, -61, -38, 11, 7, 12, 16, -35, 0, -7, -35, 15, -3};
int endgameKingTable[64] = {-58, -28, -11, -13, -3, 0, 0, -30, -3, 19, 19, 10, 15, 28, 27, 1, 1, 29,
                            23, 21, 21, 31, 30, 4, -17, 12, 15, 20, 25, 22, 10, -3, -14, -7, 12, 24,
                            22, 15, 1, -14, -14, 0, 16, 28, 22, 13, 2, -16, -23, -1, 16, 28, 27, 20,
                            0, -20, -57, -32, -5, -19, -20, 1, -30, -52};

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