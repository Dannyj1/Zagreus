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
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 83, 114, 59, 93, 68, 114, 35, -7, -5, 8, 28, 24,
                            48, 54, 17, -13, -13, 2, 4, 18, 24, 18, 17, -12, -38, -11, -2, 17, 21,
                            7, 0, -25, -19, 0, -6, 4, 0, 1, 7, -17, -24, 14, -7, -11, -7, 6, 26,
                            -14, 0, 0, 0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 171, 162, 154, 133, 146, 124, 162, 190, 94, 99,
                            85, 61, 45, 54, 79, 87, 33, 19, 13, -1, -2, 6, 9, 26, 12, 6, -9, -6,
                            -16, -15, -5, 3, 5, 5, 0, 10, 17, 0, -13, -11, 15, 7, 14, 17, 21, 0, -9,
                            -4, 0, 0, 0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-165, -88, -36, -50, 54, -89, -14, -114, -66, -35, 67, 36, 19, 62, 2,
                              -17, -44, 53, 31, 59, 81, 103, 62, 40, -1, 10, 18, 38, 40, 48, 17, 33,
                              -5, 5, 21, 19, 24, 22, 20, -3, -25, 0, 10, 15, 18, 8, 9, -11, -13,
                              -44, -10, 15, 16, 19, -12, -6, -100, -20, -49, -26, -12, -21, -13,
                              -23};
int endgameKnightTable[64] = {-51, -37, -16, -27, -37, -23, -63, -101, -24, -5, -31, -6, -8, -22,
                              -24, -49, -18, -24, 10, 9, 1, -14, -17, -42, -10, 6, 27, 19, 25, 7, 9,
                              -16, -10, -7, 15, 28, 18, 12, 0, -15, -19, 0, -16, 18, 12, -10, -25,
                              -25, -36, -19, -11, -7, -5, -21, -25, -40, -27, -37, -14, -12, -21,
                              -12, -46, -64};

int midgameBishopTable[64] = {-26, 0, -75, -36, -23, -42, 0, -7, -36, 7, -20, -5, 23, 41, 11, -38,
                              -2, 31, 35, 26, 23, 39, 34, 3, -8, 3, 7, 28, 31, 23, 12, -6, -4, 10,
                              0, 23, 33, 4, 6, 0, 12, 20, 16, 12, 10, 29, 12, 23, 14, 35, 19, 13,
                              14, 16, 44, 0, -22, -4, -2, -14, -7, 1, -30, -15};
int endgameBishopTable[64] = {-11, -20, -8, -5, -6, -7, -20, -21, -12, -13, 1, -8, -6, -26, -11, -6,
                              2, -6, -2, -8, -3, 0, -6, 0, -3, 13, 3, 0, 11, 1, 2, 0, -12, 3, 10,
                              16, 1, 1, -8, -8, -5, -1, 7, 19, 4, 5, -8, -3, -19, -8, -7, 4, 9, -12,
                              -7, -29, -18, -9, -5, 0, -7, -7, 0, -17};

int midgameRookTable[64] = {29, 41, 30, 49, 60, 8, 30, 44, 18, 18, 56, 53, 73, 59, 21, 37, 3, 15,
                            26, 37, 22, 41, 56, 9, -18, -6, 8, 22, 22, 26, -6, -13, -32, -29, -16,
                            -9, 1, -11, 3, -22, -45, -20, -18, -15, 2, -1, -4, -28, -46, -13, -18,
                            -12, -1, -6, -4, -56, -4, -17, 2, 5, 5, 3, -16, -11};
int endgameRookTable[64] = {10, 9, 16, 13, 10, 11, 8, 2, 5, 13, 11, 4, -8, 1, 9, -3, 12, -2, 5, 5,
                            11, -4, -3, 0, 2, 4, 11, -2, 0, 1, 1, 5, 4, 2, 2, -4, -8, -9, -7, -14,
                            -6, 0, -8, 2, -13, -13, -8, -16, -4, 0, 3, 0, -10, -9, -7, 9, 0, 2, 0,
                            -5, -16, -7, 18, -6};

int midgameQueenTable[64] = {-28, 0, 27, 13, 55, 40, 40, 40, -22, -52, -8, -1, -24, 41, 2, 36, -8,
                             -15, 2, 0, 27, 42, 39, 37, -11, -25, -17, -20, -18, 4, -5, -6, -10,
                             -20, -18, -17, -18, -6, 0, -1, -17, 2, 0, -13, -4, -5, 8, 2, -20, -1,
                             4, 8, 7, 9, 7, 0, -4, -13, -10, 7, 9, -10, -17, -24};
int endgameQueenTable[64] = {-9, 18, 20, 29, 23, 16, 9, 17, -11, 22, 30, 40, 55, 15, 15, 0, -15, 4,
                             12, 48, 47, 30, 17, 2, 10, 26, 20, 41, 47, 33, 58, 32, -16, 32, 14, 43,
                             27, 29, 44, 25, -11, -26, 17, 9, 7, 17, 6, 3, -17, -22, -31, -6, -7,
                             -30, -26, -25, -31, -25, -25, -36, -2, -33, -11, -28};

int midgameKingTable[64] = {-65, 22, 16, -15, -56, -34, 2, 13, 28, -1, -20, -7, -7, -4, -37, -29,
                            -8, 24, 1, -16, -19, 6, 20, -22, -17, -21, -12, -30, -31, -25, -13, -35,
                            -49, -1, -29, -38, -46, -44, -36, -52, -14, -18, -22, -51, -47, -29,
                            -14, -24, 14, 16, -14, -72, -64, -30, 12, 5, 10, 19, -15, -10, -8, -34,
                            13, 4};
int endgameKingTable[64] = {-74, -35, -18, -18, -11, 15, 4, -17, -12, 16, 13, 16, 17, 38, 23, 11, 9,
                            18, 22, 14, 20, 44, 42, 12, -8, 20, 25, 25, 25, 31, 26, 2, -18, -4, 20,
                            24, 26, 22, 9, -11, -19, 0, 19, 16, 23, 18, 9, -8, -20, -11, 6, 23, 19,
                            6, -8, -20, -49, -44, -35, -3, -23, -8, -30, -49};

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
