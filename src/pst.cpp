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
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 94, 124, 58, 92, 64, 123, 34, -10, -5, 7, 27,
                            34, 57, 61, 23, -16, -14, 8, 0, 19, 24, 15, 22, -18, -40, -9, -8, 15,
                            16, 7, 6, -27, -23, 0, -6, 3, 3, 3, 16, -15, -25, 5, -7, -14, -12, 7,
                            30, -16, 0, 0, 0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 177, 169, 157, 133, 146, 129, 162, 187, 90, 93,
                            84, 67, 48, 56, 81, 82, 34, 25, 10, 3, -11, 7, 13, 19, 7, 4, -8, -11,
                            -10, -9, -1, -4, 4, 9, -4, 7, 9, 1, -10, -6, 14, 4, 10, 18, 17, 0, -1,
                            -5, 0, 0, 0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-165, -89, -37, -46, 56, -94, -16, -108, -65, -38, 66, 38, 19, 63, 5,
                              -14, -47, 57, 37, 62, 81, 122, 69, 40, -8, 13, 20, 45, 40, 59, 19, 22,
                              -12, 3, 13, 14, 24, 24, 19, -5, -25, -3, 12, 17, 19, 13, 15, -20, -28,
                              -46, -8, 3, 8, 16, -10, -12, -104, -21, -54, -31, -21, -28, -19, -23};
int endgameKnightTable[64] = {-57, -38, -13, -24, -34, -24, -65, -97, -22, -6, -27, 0, -7, -25, -22,
                              -51, -24, -20, 12, 9, -5, -13, -19, -43, -13, 0, 18, 21, 28, 9, 7,
                              -17, -14, -8, 14, 32, 15, 17, 2, -15, -19, -4, -5, 16, 7, -9, -25,
                              -25, -38, -22, -10, -3, 0, -24, -22, -40, -27, -42, -19, -14, -21,
                              -17, -49, -65};

int midgameBishopTable[64] = {-26, 8, -80, -33, -26, -41, 7, -4, -26, 7, -18, -8, 26, 52, 13, -42,
                              -7, 32, 40, 35, 34, 45, 34, 0, -5, 0, 13, 38, 33, 26, 6, -9, -7, 13,
                              2, 26, 31, 5, 10, 2, 0, 21, 15, 8, 9, 27, 14, 14, 7, 28, 18, 9, 11,
                              21, 41, 0, -34, -3, -8, -23, -9, -2, -36, -15};
int endgameBishopTable[64] = {-12, -22, -9, -7, -6, -8, -17, -19, -7, -9, 3, -8, -4, -18, -8, -13,
                              7, -5, 1, -4, -2, 3, 0, 8, -5, 8, 8, 3, 14, 6, 3, -1, -9, 0, 7, 17, 3,
                              3, -4, -9, -10, -4, 7, 11, 10, 0, -8, -15, -18, -6, -7, 1, 12, -9,
                              -13, -28, -23, -12, -15, -5, -8, -7, -1, -14};

int midgameRookTable[64] = {31, 41, 31, 51, 59, 8, 31, 41, 18, 19, 57, 59, 78, 64, 24, 41, 0, 16,
                            25, 35, 19, 42, 59, 14, -20, -11, 8, 23, 20, 33, -8, -20, -30, -23, -11,
                            -1, 6, -6, 0, -22, -43, -27, -13, -14, 0, 0, -5, -32, -45, -17, -24, -7,
                            -3, 7, -6, -63, -7, -14, 0, 8, 7, 2, -20, -16};
int endgameRookTable[64] = {11, 10, 19, 14, 7, 10, 6, 2, 4, 11, 16, 10, -3, 2, 10, 3, 10, 6, 6, 5,
                            4, -1, -4, -2, 5, 4, 13, 5, 0, 0, 0, 5, 3, 7, 5, 0, -7, -2, -9, -16, -3,
                            0, -3, -4, -8, -13, -7, -18, -9, -8, 0, 3, -13, -12, -7, -2, -2, 4, 0,
                            -2, -10, -14, 7, -11};

int midgameQueenTable[64] = {-24, 0, 27, 11, 58, 42, 42, 40, -21, -47, -6, -2, -16, 51, 17, 49, -7,
                             -15, 3, 5, 30, 52, 47, 51, -22, -22, -18, -20, -6, 11, -6, 0, -14, -21,
                             -17, -15, -7, -6, 4, -1, -14, -1, -4, -9, -6, -1, 11, 4, -31, -9, 6, 5,
                             7, 14, 0, 3, -4, -18, -11, 1, 6, -19, -24, -39};
int endgameQueenTable[64] = {-7, 20, 21, 25, 25, 19, 8, 20, -17, 20, 30, 36, 59, 21, 26, -1, -16, 3,
                             11, 47, 50, 33, 20, 4, 6, 21, 24, 44, 51, 36, 54, 35, -17, 29, 17, 46,
                             31, 38, 38, 26, -13, -28, 18, 1, 5, 15, 7, 4, -19, -24, -27, -11, -19,
                             -22, -32, -29, -34, -27, -18, -39, -2, -29, -20, -30};

int midgameKingTable[64] = {-64, 23, 16, -15, -56, -34, 2, 12, 28, 0, -19, -7, -8, -4, -38, -28, -9,
                            24, 1, -15, -19, 5, 21, -22, -16, -20, -11, -26, -29, -26, -13, -36,
                            -50, -2, -29, -38, -48, -41, -32, -52, -17, -15, -17, -49, -45, -31,
                            -12, -24, 10, 4, -10, -64, -51, -25, 10, 8, -3, 24, 0, -17, -8, -29, 14,
                            11};
int endgameKingTable[64] = {-74, -35, -18, -18, -11, 15, 4, -17, -12, 16, 14, 17, 17, 38, 23, 11, 9,
                            17, 22, 15, 20, 44, 43, 13, -7, 20, 24, 27, 26, 32, 26, 2, -18, -5, 21,
                            25, 25, 26, 9, -10, -20, -4, 11, 23, 21, 20, 11, -6, -23, -13, 5, 16,
                            16, 7, -6, -17, -50, -40, -25, 2, -27, -11, -28, -49};

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