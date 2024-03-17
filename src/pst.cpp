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
int midgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 76, 68, 52, 84, 66, 90, 36, 7, -15, -5, 45, 19,
                            17, 38, 6, -22, -13, 3, 4, 24, 21, 6, 0, -8, -32, -9, 2, 21, 22, 5, -15,
                            -27, -15, 0, 2, 5, 3, 1, -2, -15, -10, 12, 0, -7, -4, 9, 15, -11, 0, 0,
                            0, 0, 0, 0, 0, 0};
int endgamePawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0, 187, 186, 161, 146, 146, 151, 188, 202, 108,
                            109, 79, 82, 82, 82, 100, 105, 23, 23, 11, 6, 5, 8, 25, 25, -3, -5, -13,
                            -16, -12, -11, 0, -3, -6, -9, -10, 0, -2, -13, -9, -4, -4, -8, 5, 14, 5,
                            0, -11, -8, 0, 0, 0, 0, 0, 0, 0, 0};

int midgameKnightTable[64] = {-167, -82, -52, -32, 44, -78, -11, -121, -51, -21, 74, 59, 40, 78,
                              -15, -28, -37, 41, 37, 65, 78, 66, 42, 33, 15, 15, 18, 35, 32, 22, 17,
                              41, -5, 10, 23, 19, 22, 23, 21, 1, -26, 2, 12, 18, 20, 12, 6, -14, 0,
                              -15, 0, 23, 24, 13, 3, 7, -89, -9, -24, -16, -11, -13, -10, -31};
int endgameKnightTable[64] = {-53, -37, -18, -17, -31, -18, -59, -86, -28, -13, -28, -6, -9, -27,
                              -21, -38, -11, -29, 3, 0, -9, -12, -21, -31, -9, 14, 20, 22, 24, 13,
                              9, -13, 2, 2, 10, 25, 19, 9, -9, -8, -12, -1, -16, 5, 13, -15, -20,
                              -27, -27, -10, -11, -20, -14, -20, -24, -25, -31, -26, -10, -5, -15,
                              -9, -29, -53};

int midgameBishopTable[64] = {-34, -1, -54, -34, -43, -38, -6, -8, -16, -17, -9, 0, 3, 15, 5, -18,
                              6, 23, 34, 17, 16, 49, 23, 9, -13, 2, -5, 11, 22, -14, 13, -21, -8, 0,
                              0, 27, 36, 6, 3, -10, 18, 16, 25, 12, 17, 33, 14, 34, 22, 47, 23, 27,
                              26, 21, 54, 2, 0, -15, 6, -12, 3, 9, -10, 0};
int endgameBishopTable[64] = {-9, -26, -6, -12, -17, -17, -26, -14, -24, -5, -5, -5, -20, -16, -13,
                              -21, -5, -5, 4, 0, 8, 0, -8, -11, 1, 8, 6, 10, 12, 5, 3, -5, -17, 0,
                              22, 15, 0, 17, -8, -10, -15, -1, 8, 17, 19, 1, -5, -20, -17, -15, -13,
                              4, 1, -9, -11, -17, -19, -19, -7, 0, -9, -6, -9, -20};

int midgameRookTable[64] = {23, 43, 29, 42, 48, 26, 36, 37, -1, -6, 52, 43, 47, 40, 0, 22, 11, 18,
                            18, 21, 33, 21, 52, 7, -8, -3, -1, 25, 9, 20, 0, -15, -21, -23, -28,
                            -18, 1, -14, -9, -31, -27, -18, -18, 0, 3, -4, -2, -17, -43, -5, -10,
                            -2, -7, -16, -6, -33, 5, -4, 13, 13, 14, 14, 0, 3};
int endgameRookTable[64] = {4, 4, 0, 0, -6, 6, 0, 7, 11, 15, 0, -1, 0, 8, 24, 3, 6, 8, 2, 5, 0, 3,
                            -2, 1, 7, 7, 11, 0, 4, 4, 5, 12, 6, 9, 11, 7, 3, 6, 0, 0, 2, 2, 6, -3,
                            -4, -7, -13, -12, -1, 0, -3, 0, 4, 4, -2, 6, 0, 7, -3, 3, -4, 0, 2, -6};

int midgameQueenTable[64] = {-8, -9, 25, 0, 42, 24, 30, 8, -17, -60, -14, -23, -46, 25, -35, 10, 0,
                             12, 17, 6, 21, 20, 29, 23, 7, -7, -18, -17, -30, -3, -13, -5, -2, -11,
                             -16, -7, -19, -4, 3, 0, -13, 11, 12, -2, 1, 0, 4, 0, -6, 7, 17, 17, 17,
                             14, 24, 10, 5, 0, 6, 21, 19, -3, 0, 5};
int endgameQueenTable[64] = {17, 3, 14, 16, 11, 1, 0, -5, 7, 37, 52, 45, 45, 13, 16, -11, -3, 16,
                             12, 58, 38, 22, 10, -12, 17, 43, 36, 37, 56, 28, 47, 35, 6, 36, 21, 42,
                             38, 25, 35, 12, -2, -7, 17, 12, 12, 28, 7, 2, -8, -13, -23, 12, 6, -15,
                             -29, -10, -11, -16, -20, -22, -19, -11, -15, -19};

int midgameKingTable[64] = {-57, 26, 22, -1, -43, -30, 4, 10, 27, 0, -3, 3, -7, 6, -27, -30, -6, 36,
                            4, -4, -22, 19, 32, -19, -13, -9, -17, -30, -22, -23, -12, -45, -49, 0,
                            -22, -45, -40, -37, -41, -45, -17, -16, -32, -54, -56, -25, -11, -27,
                            28, 27, -30, -67, -61, -41, 18, 6, 20, 18, -34, -1, -7, -36, 15, 2};
int endgameKingTable[64] = {-53, -26, -10, -13, -1, 1, 0, -37, -6, 22, 20, 7, 9, 27, 29, 0, 4, 26,
                            20, 20, 22, 27, 28, 4, -10, 12, 19, 20, 22, 20, 9, 0, -18, -8, 10, 26,
                            21, 13, 0, -21, -15, -3, 12, 27, 24, 11, 0, -19, -31, -8, 18, 27, 26,
                            21, -6, -24, -64, -32, -2, -20, -13, 3, -31, -53};

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