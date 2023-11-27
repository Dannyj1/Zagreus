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

#include "types.h"

#include <vector>

namespace Zagreus {
    int midgamePawnTable[64] = { 13, 13, 13, 13, 13, 13, 13, 13, 109, 140, 73, 106, 83, 135, 47, 4, 4, 18, 37, 42, 66, 68, 38, -7, -6, 10, 13, 23, 28, 21, 29, -9, -40, -7, -4, 19, 23, 10, 9, -25, -17, 0, -5, 4, 2, 7, 13, -18, -20, 11, -5, -13, -11, 6, 31, -13, 13, 13, 13, 13, 13, 13, 13, 13 };
    int endgamePawnTable[64] = { 13, 13, 13, 13, 13, 13, 13, 13, 190, 182, 169, 145, 161, 143, 177, 202, 107, 112, 97, 78, 67, 67, 95, 94, 43, 34, 28, 0, 0, 9, 26, 29, 13, 14, -5, 0, -3, -9, 9, 11, 10, 12, 4, 17, 11, 0, -10, 0, 20, 11, 22, 20, 29, 8, 0, -4, 13, 13, 13, 13, 13, 13, 13, 13 };

    int midgameKnightTable[64] = { -152, -75, -22, -34, 73, -82, -2, -96, -60, -28, 86, 52, 37, 79, 14, -5, -34, 68, 46, 77, 95, 126, 80, 54, 5, 21, 30, 45, 43, 69, 20, 34, -3, 19, 26, 21, 32, 28, 31, 0, -17, 6, 14, 18, 26, 16, 20, -9, -13, -37, -3, 14, 13, 26, 0, -2, -88, -22, -41, -24, -7, -17, -14, -10 };
    int endgameKnightTable[64] = { -43, -25, -1, -14, -17, -13, -50, -85, -13, 5, -11, 12, 4, -10, -12, -38, -10, -9, 21, 24, 9, 0, -8, -30, -2, 19, 35, 31, 32, 25, 19, -5, -7, 8, 24, 34, 27, 28, 15, -8, -13, 10, 0, 23, 22, 0, -11, -7, -28, -6, 2, 11, 6, -8, -9, -30, -14, -36, -9, -2, -10, -6, -44, -51 };

    int midgameBishopTable[64] = { -14, 16, -68, -24, -12, -29, 19, 6, -17, 16, -7, 0, 40, 63, 27, -29, 0, 47, 49, 45, 45, 63, 48, 7, 3, 0, 26, 44, 42, 39, 5, 5, 2, 21, 8, 34, 43, 7, 15, 6, 15, 27, 22, 14, 8, 30, 27, 22, 16, 33, 30, 12, 20, 33, 43, 10, -17, 7, 0, -7, -1, 0, -22, -8 };
    int endgameBishopTable[64] = { 0, -8, 1, 5, 5, 3, -3, -9, 2, 0, 19, 1, 9, -3, 6, -2, 17, 3, 11, 8, 12, 18, 12, 13, 9, 18, 22, 17, 25, 22, 9, 14, 5, 12, 19, 33, 16, 20, 5, 2, 2, 6, 16, 18, 16, 10, 6, -2, -1, 0, 5, 13, 11, 2, -4, -14, -8, 3, -11, 8, 4, -4, 9, -4 };

    int midgameRookTable[64] = { 45, 54, 44, 64, 75, 22, 43, 55, 35, 35, 69, 69, 90, 76, 40, 54, 13, 29, 37, 48, 33, 55, 76, 28, -11, 2, 18, 37, 33, 41, 5, -5, -23, -14, -9, 10, 20, 5, 16, -15, -31, -12, -6, -3, 9, 13, 5, -21, -36, -3, -7, 4, 11, 12, 6, -56, -3, -12, 0, 8, 10, 4, -12, -11 };
    int endgameRookTable[64] = { 26, 21, 31, 28, 24, 25, 21, 16, 21, 20, 24, 21, 8, 14, 24, 13, 22, 19, 21, 19, 19, 7, 10, 11, 16, 16, 25, 12, 13, 13, 13, 18, 15, 16, 18, 14, 8, 6, 4, 2, 9, 13, 7, 11, 1, 2, 3, -6, 6, 4, 14, 13, 3, 1, 0, 6, 4, 5, 6, 9, -2, -6, 19, -7 };

    int midgameQueenTable[64] = { -13, 11, 41, 25, 71, 57, 57, 54, -13, -42, 3, 7, -7, 63, 28, 62, 0, -2, 17, 16, 41, 61, 54, 60, -8, -11, -5, -5, 4, 25, 1, 10, -6, -11, -6, -12, 0, 7, 14, 10, -2, 8, 0, 0, 0, 2, 15, 13, -20, 2, 10, 10, 13, 19, 10, 10, 7, -5, -4, 8, 9, -11, -14, -28 };
    int endgameQueenTable[64] = { 4, 35, 35, 40, 40, 32, 23, 31, -3, 31, 44, 53, 69, 34, 39, 11, -8, 18, 23, 62, 60, 43, 30, 18, 18, 38, 38, 59, 67, 50, 70, 50, -6, 41, 32, 54, 41, 45, 51, 33, -1, -13, 27, 18, 20, 29, 21, 17, -7, -13, -24, -2, -10, -12, -22, -18, -21, -14, -9, -35, 7, -19, -5, -23 };

    int midgameKingTable[64] = { -51, 36, 29, -1, -42, -20, 15, 26, 42, 12, -6, 6, 5, 9, -24, -15, 4, 37, 15, -2, -6, 19, 35, -8, -3, -6, 1, -13, -16, -11, 0, -22, -35, 13, -14, -24, -33, -30, -19, -37, -1, 0, -10, -36, -33, -18, -1, -13, 18, 20, 0, -60, -39, -5, 18, 17, 3, 22, 0, -16, 0, -24, 16, 12 };
    int endgameKingTable[64] = { -60, -21, -4, -4, 2, 28, 17, -3, 1, 30, 27, 30, 30, 51, 36, 24, 23, 30, 36, 28, 33, 58, 57, 26, 5, 35, 37, 40, 39, 46, 39, 16, -4, 10, 34, 38, 39, 36, 23, 2, -5, 10, 24, 33, 35, 27, 20, 4, -13, 1, 19, 26, 26, 22, 7, -8, -42, -33, -13, 10, -25, -4, -27, -42 };

    // Base tables from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
    int baseMidgamePawnTable[64] = {
            0,   0,   0,   0,   0,   0,  0,   0,
            98, 134,  61,  95,  68, 126, 34, -11,
            -6,   7,  26,  31,  65,  56, 25, -20,
            -14,  13,   6,  21,  23,  12, 17, -23,
            -27,  -2,  -5,  12,  17,   6, 10, -25,
            -26,  -4,  -4, -10,   3,   3, 33, -12,
            -35,  -1, -20, -23, -15,  24, 38, -22,
            0,   0,   0,   0,   0,   0,  0,   0,
    };

    int baseEndgamePawnTable[64] = {
            0,   0,   0,   0,   0,   0,   0,   0,
            178, 173, 158, 134, 147, 132, 165, 187,
            94, 100,  85,  67,  56,  53,  82,  84,
            32,  24,  13,   5,  -2,   4,  17,  17,
            13,   9,  -3,  -7,  -7,  -8,   3,  -1,
            4,   7,  -6,   1,   0,  -5,  -1,  -8,
            13,   8,   8,  10,  13,   0,   2,  -7,
            0,   0,   0,   0,   0,   0,   0,   0,
    };

    int baseMidgameKnightTable[64] = {
            -167, -89, -34, -49,  61, -97, -15, -107,
            -73, -41,  72,  36,  23,  62,   7,  -17,
            -47,  60,  37,  65,  84, 129,  73,   44,
            -9,  17,  19,  53,  37,  69,  18,   22,
            -13,   4,  16,  13,  28,  19,  21,   -8,
            -23,  -9,  12,  10,  19,  17,  25,  -16,
            -29, -53, -12,  -3,  -1,  18, -14,  -19,
            -105, -21, -58, -33, -17, -28, -19,  -23,
    };

    int baseEndgameKnightTable[64] = {
            -58, -38, -13, -28, -31, -27, -63, -99,
            -25,  -8, -25,  -2,  -9, -25, -24, -52,
            -24, -20,  10,   9,  -1,  -9, -19, -41,
            -17,   3,  22,  22,  22,  11,   8, -18,
            -18,  -6,  16,  25,  16,  17,   4, -18,
            -23,  -3,  -1,  15,  10,  -3, -20, -22,
            -42, -20, -10,  -5,  -2, -20, -23, -44,
            -29, -51, -23, -15, -22, -18, -50, -64,
    };

    int baseMidgameBishopTable[64] = {
            -29,   4, -82, -37, -25, -42,   7,  -8,
            -26,  16, -18, -13,  30,  59,  18, -47,
            -16,  37,  43,  40,  35,  50,  37,  -2,
            -4,   5,  19,  50,  37,  37,   7,  -2,
            -6,  13,  13,  26,  34,  12,  10,   4,
            0,  15,  15,  15,  14,  27,  18,  10,
            4,  15,  16,   0,   7,  21,  33,   1,
            -33,  -3, -14, -21, -13, -12, -39, -21,
    };

    int baseEndgameBishopTable[64] = {
            -14, -21, -11,  -8, -7,  -9, -17, -24,
            -8,  -4,   7, -12, -3, -13,  -4, -14,
            2,  -8,   0,  -1, -2,   6,   0,   4,
            -3,   9,  12,   9, 14,  10,   3,   2,
            -6,   3,  13,  19,  7,  10,  -3,  -9,
            -12,  -3,   8,  10, 13,   3,  -7, -15,
            -14, -18,  -7,  -1,  4,  -9, -15, -27,
            -23,  -9, -23,  -5, -9, -16,  -5, -17,
    };

    int baseMidgameRookTable[64] = {
            32,  42,  32,  51, 63,  9,  31,  43,
            27,  32,  58,  62, 80, 67,  26,  44,
            -5,  19,  26,  36, 17, 45,  61,  16,
            -24, -11,   7,  26, 24, 35,  -8, -20,
            -36, -26, -12,  -1,  9, -7,   6, -23,
            -45, -25, -16, -17,  3,  0,  -5, -33,
            -44, -16, -20,  -9, -1, 11,  -6, -71,
            -19, -13,   1,  17, 16,  7, -37, -26,
    };

    int baseEndgameRookTable[64] = {
            13, 10, 18, 15, 12,  12,   8,   5,
            11, 13, 13, 11, -3,   3,   8,   3,
            7,  7,  7,  5,  4,  -3,  -5,  -3,
            4,  3, 13,  1,  2,   1,  -1,   2,
            3,  5,  8,  4, -5,  -6,  -8, -11,
            -4,  0, -5, -1, -7, -12,  -8, -16,
            -6, -6,  0,  2, -9,  -9, -11,  -3,
            -9,  2,  3, -1, -5, -13,   4, -20,
    };


    int baseMidgameQueenTable[64] = {
            -28,   0,  29,  12,  59,  44,  43,  45,
            -24, -39,  -5,   1, -16,  57,  28,  54,
            -13, -17,   7,   8,  29,  56,  47,  57,
            -27, -27, -16, -16,  -1,  17,  -2,   1,
            -9, -26,  -9, -10,  -2,  -4,   3,  -3,
            -14,   2, -11,  -2,  -5,   2,  14,   5,
            -35,  -8,  11,   2,   8,  15,  -3,   1,
            -1, -18,  -9,  10, -15, -25, -31, -50,
    };

    int baseEndgameQueenTable[64] = {
            -9,  22,  22,  27,  27,  19,  10,  20,
            -17,  20,  32,  41,  58,  25,  30,   0,
            -20,   6,   9,  49,  47,  35,  19,   9,
            3,  22,  24,  45,  57,  40,  57,  36,
            -18,  28,  19,  47,  31,  34,  39,  23,
            -16, -27,  15,   6,   9,  17,  10,   5,
            -22, -23, -30, -16, -16, -23, -36, -32,
            -33, -28, -22, -43,  -5, -32, -20, -41,
    };

    int baseMidgameKingTable[64] = {
            -65,  23,  16, -15, -56, -34,   2,  13,
            29,  -1, -20,  -7,  -8,  -4, -38, -29,
            -9,  24,   2, -16, -20,   6,  22, -22,
            -17, -20, -12, -27, -30, -25, -14, -36,
            -49,  -1, -27, -39, -46, -44, -33, -51,
            -14, -14, -22, -46, -44, -30, -15, -27,
            1,   7,  -8, -64, -43, -16,   9,   8,
            -15,  36,  12, -54,   8, -28,  24,  14,
    };

    int baseEndgameKingTable[64] = {
            -74, -35, -18, -18, -11,  15,   4, -17,
            -12,  17,  14,  17,  17,  38,  23,  11,
            10,  17,  23,  15,  20,  45,  44,  13,
            -8,  22,  24,  27,  26,  33,  26,   3,
            -18,  -4,  21,  24,  27,  23,   9, -11,
            -19,  -3,  11,  21,  23,  16,   7,  -9,
            -27, -11,   4,  13,  14,   4,  -5, -17,
            -53, -34, -21, -11, -28, -14, -24, -43
    };
    
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
}