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
    int midgamePawnTable[64] = { 5, 5, 5, 5, 5, 5, 5, 5, 100, 135, 65, 99, 72, 129, 41, -3, -3, 10, 32, 32, 59, 61, 29, -12, -15, 7, 2, 18, 17, 13, 20, -15, -44, -11, -8, 12, 17, 6, 5, -31, -23, 0, -12, -2, 0, 0, 17, -20, -27, 10, -5, -15, -12, 4, 32, -15, 5, 5, 5, 5, 5, 5, 5, 5 };
    int endgamePawnTable[64] = { 5, 5, 5, 5, 5, 5, 5, 5, 182, 176, 162, 138, 151, 135, 170, 192, 97, 105, 90, 69, 56, 57, 86, 88, 33, 26, 11, 0, -5, 4, 16, 21, 13, 10, -3, -7, -10, -8, 3, 4, 5, 12, -4, 7, 8, -9, -3, -7, 13, 10, 13, 16, 21, 6, 0, -4, 5, 5, 5, 5, 5, 5, 5, 5 };

    int midgameKnightTable[64] = { -161, -83, -30, -43, 65, -90, -9, -103, -66, -34, 76, 42, 30, 69, 10, -13, -40, 61, 41, 68, 88, 126, 73, 48, 0, 13, 23, 43, 36, 61, 17, 25, -11, 6, 21, 14, 24, 24, 24, -9, -24, 0, 14, 16, 23, 13, 19, -16, -18, -45, -9, 5, 8, 19, -7, -9, -99, -25, -51, -30, -14, -22, -20, -18 };
    int endgameKnightTable[64] = { -52, -32, -9, -21, -26, -21, -57, -93, -20, -3, -21, 4, -2, -18, -18, -46, -18, -16, 15, 12, 4, -5, -13, -36, -10, 6, 29, 21, 25, 13, 12, -13, -11, 0, 20, 28, 18, 22, 7, -14, -19, 1, 0, 18, 15, -3, -17, -18, -35, -15, -6, -1, 4, -17, -16, -37, -23, -45, -16, -9, -18, -11, -48, -58 };

    int midgameBishopTable[64] = { -23, 8, -74, -32, -20, -37, 10, -2, -20, 12, -14, -5, 32, 60, 20, -38, -6, 40, 45, 42, 37, 52, 41, 4, -2, 0, 16, 42, 34, 33, 4, -2, -2, 15, 3, 25, 35, 3, 13, 4, 7, 21, 16, 11, 11, 27, 19, 19, 11, 35, 19, 7, 11, 22, 44, 4, -23, 0, -3, -15, -6, 0, -30, -14 };
    int endgameBishopTable[64] = { -8, -15, -4, -1, -2, -3, -12, -16, -1, -3, 10, -5, 1, -11, 0, -8, 9, -4, 4, 3, 1, 9, 4, 8, 1, 13, 14, 10, 17, 11, 4, 6, -2, 8, 10, 22, 11, 9, 0, -3, -4, 2, 10, 14, 13, 4, -3, -8, -8, -9, 0, 7, 5, -6, -7, -21, -16, -4, -15, 0, -3, -9, 0, -11 };

    int midgameRookTable[64] = { 37, 46, 36, 57, 69, 15, 36, 48, 29, 31, 62, 64, 82, 68, 31, 48, 0, 23, 31, 41, 25, 49, 65, 19, -16, -6, 11, 30, 27, 40, -2, -14, -30, -21, -11, 0, 12, -3, 8, -18, -42, -19, -11, -12, 5, 3, 0, -29, -43, -10, -14, -3, 3, 8, 0, -60, -6, -14, 0, 12, 14, 1, -21, -18 };
    int endgameRookTable[64] = { 18, 14, 22, 21, 17, 17, 13, 10, 15, 16, 18, 14, 0, 6, 15, 7, 12, 12, 12, 9, 10, 2, 0, 2, 8, 7, 17, 7, 6, 7, 4, 7, 8, 10, 11, 6, 0, -1, -2, -6, 0, 4, 0, 3, -1, -7, -3, -11, -1, 0, 3, 8, -3, -5, -5, 2, -5, 4, 4, 7, -5, -10, 11, -14 };

    int midgameQueenTable[64] = { -23, 5, 34, 18, 63, 48, 48, 46, -19, -42, -2, 2, -14, 58, 21, 52, -5, -10, 11, 11, 32, 58, 48, 54, -16, -21, -13, -16, -4, 17, 0, 1, -15, -17, -14, -12, -7, 0, 7, 0, -12, 0, -4, -6, -6, 0, 14, 6, -28, -6, 7, 7, 7, 16, 9, 3, 2, -13, -9, 0, 0, -16, -23, -36 };
    int endgameQueenTable[64] = { -3, 27, 27, 32, 32, 24, 15, 25, -11, 23, 37, 45, 62, 28, 32, 4, -11, 11, 13, 54, 51, 39, 23, 11, 11, 27, 28, 49, 59, 43, 62, 38, -13, 34, 22, 51, 33, 37, 44, 27, -12, -25, 20, 8, 12, 20, 14, 7, -15, -17, -27, -8, -8, -20, -28, -26, -29, -23, -20, -41, 4, -26, -14, -31 };

    int midgameKingTable[64] = { -59, 28, 21, -9, -50, -28, 7, 18, 34, 4, -14, -1, -2, 1, -32, -23, -3, 29, 7, -10, -14, 11, 27, -16, -11, -14, -6, -21, -24, -19, -8, -30, -43, 4, -22, -33, -40, -38, -27, -45, -8, -9, -17, -43, -42, -25, -10, -22, 11, 12, -3, -62, -47, -14, 11, 8, 0, 22, 0, -13, 0, -29, 14, 8 };
    int endgameKingTable[64] = { -68, -29, -12, -12, -5, 20, 9, -11, -6, 22, 18, 22, 22, 43, 28, 16, 15, 22, 28, 20, 25, 50, 49, 18, -2, 27, 29, 32, 31, 38, 31, 8, -12, 1, 25, 29, 31, 28, 14, -5, -13, 1, 16, 25, 26, 21, 13, -4, -19, -6, 10, 21, 20, 8, 0, -15, -46, -36, -24, 1, -32, -10, -28, -43 };

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