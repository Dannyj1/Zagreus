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
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 63, 66, 36, 89, 66, 95, 29, 0, -5, 0, 37, 18,
                            24, 51, 12, -13, -19, 0, 6, 24, 18, 6, 5, -5, -35, -15, 0, 20, 20, 5,
                            -14, -27, -22, -3, -4, 0, -1, -2, -2, -19, -19, 12, -5, -10, -9, 1, 17,
                            -12, 0, 0, 0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 169, 138, 142, 131, 143, 107, 155, 192, 90, 90,
                            78, 67, 41, 41, 77, 73, 47, 26, 5, 0, 0, 9, 0, 19, 15, 9, -3, -6, -6,
                            -7, 1, 8, 9, 5, 9, 20, 26, 2, -4, 4, 18, 6, 26, 30, 23, 20, -6, 7, 0, 0,
                            0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-172, -88, -50, -48, 47, -76, -18, -120, -46, -26, 52, 40, 18, 53, -9,
                              -27, -31, 43, 19, 61, 75, 52, 44, 40, 17, 10, 13, 34, 35, 24, 15, 39,
                              -2, 10, 23, 20, 24, 22, 17, -2, -26, 6, 13, 22, 19, 11, 8, -17, 0,
                              -21, -5, 25, 22, 13, 2, 9, -89, -11, -30, -13, -8, -10, -8, -27};
int endgameKnightTable[64] = {-52, -41, -21, -21, -40, -14, -59, -92, -29, 1, -45, -6, -10, -24,
                              -30, -37, -10, -27, 25, 3, -8, -18, -19, -43, -4, 25, 33, 23, 33, 15,
                              17, -17, -5, -7, 6, 31, 19, 16, 0, -3, -6, -13, -15, 2, 9, -13, -22,
                              -15, -18, -15, -9, -17, -13, -24, -11, -32, -19, -20, -2, -9, -9, -12,
                              -28, -65};

int midgameBishopTable[64] = {-25, -5, -57, -28, -32, -43, -10, -3, -49, -8, -29, -1, 10, 15, 2,
                              -41, 0, 13, 17, 11, 6, 19, 19, 6, -8, 2, -5, 9, 18, -4, 12, -13, -4,
                              0, 3, 25, 36, 9, 2, -10, 20, 18, 25, 15, 15, 33, 12, 29, 21, 44, 19,
                              20, 22, 29, 52, 2, 0, -7, 4, -12, 5, 8, -9, 2};
int endgameBishopTable[64] = {-9, -24, -3, -6, -14, 0, -27, -11, -11, -16, -8, -3, -18, -36, -18,
                              -6, 14, -17, -15, -26, -3, -10, -12, -4, -7, 23, 3, 6, 12, -6, 7, -8,
                              -27, 2, 16, 14, -1, 12, 1, 1, 0, 2, 18, 26, 18, 2, -5, -8, -17, 0, 10,
                              14, 9, -19, -8, -28, -13, -19, 7, 18, -5, 6, 3, -13};

int midgameRookTable[64] = {25, 41, 30, 55, 61, 10, 35, 49, 2, -4, 49, 30, 55, 26, -6, 20, 10, 11,
                            9, 24, 23, 27, 41, -3, -5, -6, 3, 27, 10, 16, 0, -12, -26, -29, -31,
                            -20, -10, -19, -14, -26, -36, -19, -16, -7, 4, 0, -5, -24, -44, -6, -14,
                            -5, -7, -13, -13, -35, -4, -10, 6, 11, 14, 7, -7, -8};
int endgameRookTable[64] = {13, 6, 21, 22, 5, 12, 8, -3, 1, 16, 24, 11, -15, 2, 22, -4, 17, 6, 7, 0,
                            19, -6, -1, -6, 3, 5, 17, -1, -6, 9, 2, 13, 13, 7, 3, -6, -7, -6, -12,
                            -14, 0, 6, -2, 1, -7, -18, -13, -8, -4, -2, 3, 2, -2, -12, -5, 17, 5,
                            10, 12, -5, -9, 11, 7, 9};

int midgameQueenTable[64] = {-28, -11, 20, 3, 51, 28, 34, 16, -28, -63, -31, -31, -48, 8, -33, 8, 0,
                             -7, -15, 0, 23, 0, 25, 16, 1, -12, -23, -23, -37, -5, -18, -1, -4, -10,
                             -15, -14, -17, -9, 3, -6, -19, 7, 10, -5, -1, -2, 6, -2, -3, 6, 11, 15,
                             17, 11, 22, 11, 6, -2, 0, 19, 17, 0, -3, 0};
int endgameQueenTable[64] = {-7, 12, 18, 20, 17, 12, 0, 11, -5, 31, 27, 35, 41, -1, 12, -18, -13,
                             19, 5, 46, 39, 18, 9, -4, 18, 33, 26, 38, 53, 23, 59, 36, -2, 26, 21,
                             56, 36, 30, 43, 32, -18, -21, 31, 19, 20, 31, 2, 5, -14, -19, -9, 14,
                             8, -14, -38, -23, -24, -21, -11, -8, -11, -17, -11, -8};

int midgameKingTable[64] = {-65, 22, 15, -15, -57, -34, 1, 12, 27, -2, -22, -7, -7, -4, -38, -29,
                            -7, 26, 0, -14, -18, 8, 20, -21, -15, -23, -12, -31, -34, -31, -13, -36,
                            -47, -9, -40, -39, -52, -45, -31, -53, -19, -21, -21, -63, -54, -27, -9,
                            -30, 24, 15, -26, -72, -73, -44, 9, 6, 17, 18, -31, 0, -7, -33, 13, -2};
int endgameKingTable[64] = {-75, -35, -18, -18, -12, 14, 3, -17, -14, 15, 11, 17, 17, 39, 22, 10, 7,
                            19, 20, 16, 18, 46, 41, 12, -7, 18, 27, 22, 20, 25, 28, 2, -19, -8, 12,
                            32, 21, 30, 14, -17, -20, -6, 26, 27, 25, 22, 25, -7, -31, 0, 18, 42,
                            39, 28, -2, -26, -61, -36, -14, -29, -23, 2, -25, -53};

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