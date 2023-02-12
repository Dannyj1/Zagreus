/*
 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "types.h"

#include <vector>

namespace Zagreus {
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 97, 119, 57, 94, 65, 118, 35, -5, 3, 0, 35, 26, 41, 56, 23, -23, -24, 3, 0, 17, 16, 5, 26, -9, -42, -4, -2, 15, 21, 0, 3, -27, -15, 0, -7, 0, 1, -5, 10, -19, -18, 20, -6, -5, -7, 5, 32, -9, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 183, 162, 156, 135, 142, 129, 171, 199, 98, 93, 81, 64, 48, 56, 83, 94, 32, 25, 3, -4, -12, 4, 17, 29, 12, -2, -17, -17, -18, -19, -4, 9, -5, 3, -7, -2, 4, -9, -2, 0, 9, 2, 8, 18, 12, 5, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -87, -35, -45, 59, -94, -14, -110, -72, -27, 74, 46, 24, 71, 2, -17, -44, 54, 40, 71, 80, 112, 68, 39, 1, 10, 15, 37, 31, 57, 19, 34, -13, 0, 17, 13, 21, 15, 23, -19, -29, 0, 18, 17, 20, 20, 7, -24, -21, -43, -11, 15, 13, 10, -17, -9, -103, -16, -49, -27, -21, -30, -15, -23 };
    int endgameKnightTable[64] = { -57, -38, -13, -26, -27, -26, -60, -93, -23, -2, -26, 3, -4, -21, -22, -52, -20, -20, 11, 9, 6, -5, -18, -43, -8, 8, 23, 14, 32, 6, 11, -14, -9, -12, 12, 26, 17, 19, 0, -15, -21, 0, -16, 10, 16, -13, -28, -16, -35, -16, -9, -2, -7, -18, -21, -41, -27, -44, -18, -16, -21, -14, -47, -61 };

    int midgameBishopTable[64] = { -29, 3, -75, -42, -27, -39, 0, -11, -25, 8, -12, -8, 25, 47, 21, -33, 2, 38, 46, 40, 31, 46, 37, 7, -1, 3, 7, 34, 33, 21, 2, -16, -6, 6, 4, 22, 27, 0, 7, -1, 5, 11, 9, 4, 3, 23, 3, 21, 4, 46, 17, 9, 13, 15, 48, 0, -16, -5, 1, -14, -1, 10, -33, -15 };
    int endgameBishopTable[64] = { -14, -21, -9, -5, -9, -9, -18, -23, 0, -5, 7, -6, -9, -16, -4, -5, 15, -3, 0, 0, 0, 4, 0, 4, 4, 12, 7, 7, 8, 4, 11, 0, -4, -4, 7, 13, -5, 5, -6, -13, -1, -5, 0, 6, 2, 0, -8, -9, -8, -7, -3, 2, 5, -15, -12, -25, -15, -11, -10, 4, -4, -10, -2, -15 };

    int midgameRookTable[64] = { 29, 38, 31, 43, 60, 8, 28, 42, 23, 23, 53, 59, 68, 65, 28, 45, 1, 17, 25, 33, 25, 47, 58, 9, -17, -7, 3, 18, 15, 29, -6, -13, -34, -26, -22, -11, 2, -10, -3, -25, -39, -23, -20, -7, -2, -2, -2, -36, -47, -12, -20, -15, 0, 0, -5, -65, -5, -15, 8, 14, 14, 7, -17, -18 };
    int endgameRookTable[64] = { 13, 4, 14, 3, 2, 5, 6, -1, 15, 12, 3, 10, 0, 2, 16, 1, 13, 3, 4, 4, 15, 0, -4, -2, 7, 4, 7, 5, 3, 1, 2, 7, 0, 9, 6, -5, -7, -6, -12, -11, 0, 0, -1, 3, -10, -14, -11, -13, -10, -5, -1, 3, -6, -17, -9, 1, -10, 1, -2, 5, -10, -4, 10, -5 };

    int midgameQueenTable[64] = { -17, 1, 30, 14, 61, 39, 43, 30, -17, -46, 0, -1, -19, 46, 0, 37, 1, -7, 2, 1, 30, 45, 42, 49, -8, -27, -19, -13, -12, 6, -14, 5, -19, -19, -21, -16, -8, -14, 1, -7, -15, -9, 0, -15, -4, -11, 3, 0, -23, -7, 3, 5, 3, 13, 17, -7, -2, -11, -14, 13, 13, -12, -19, -29 };
    int endgameQueenTable[64] = { 0, 24, 24, 29, 29, 16, 9, 12, -6, 14, 39, 43, 51, 20, 19, -4, -11, 7, 11, 48, 50, 28, 17, 5, 11, 24, 22, 45, 44, 32, 54, 41, -14, 31, 13, 39, 26, 27, 39, 13, -13, -30, 13, -2, 3, 22, 6, 4, -17, -20, -26, -11, -9, -23, -34, -27, -29, -24, -16, -36, 0, -28, -17, -29 };

    int midgameKingTable[64] = { -65, 21, 15, -14, -55, -33, 0, 11, 28, 0, -18, -5, -7, -3, -34, -29, -7, 26, 5, -12, -19, 10, 23, -21, -16, -15, -10, -25, -27, -23, -9, -36, -47, 0, -22, -37, -39, -41, -28, -50, -16, -13, -24, -50, -48, -26, -15, -33, 13, 4, -18, -64, -54, -21, 11, 15, 10, 20, -12, -19, -17, -27, 27, 17 };
    int endgameKingTable[64] = { -73, -33, -16, -17, -10, 13, 2, -16, -11, 20, 16, 18, 16, 40, 28, 12, 12, 22, 31, 19, 25, 48, 50, 14, -6, 28, 29, 37, 34, 44, 31, 1, -16, 0, 28, 35, 43, 31, 18, -9, -23, -10, 10, 28, 27, 18, 8, -14, -26, -13, 1, 17, 12, 5, -7, -23, -63, -46, -33, -22, -34, -18, -35, -59 };

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
                    case PieceType::WHITE_PAWN:
                        midgamePst[pieceType][square] = midgamePawnTable[63 - square];
                        endgamePst[pieceType][square] = endgamePawnTable[63 - square];
                        baseMidgamePst[pieceType][square] = midgamePawnTable[63 - square];
                        baseEndgamePst[pieceType][square] = endgamePawnTable[63 - square];
                        break;
                    case PieceType::BLACK_PAWN:
                        midgamePst[pieceType][square] = midgamePawnTable[square];
                        endgamePst[pieceType][square] = endgamePawnTable[square];
                        baseMidgamePst[pieceType][square] = midgamePawnTable[square];
                        baseEndgamePst[pieceType][square] = endgamePawnTable[square];
                        break;
                    case PieceType::WHITE_KNIGHT:
                        midgamePst[pieceType][square] = midgameKnightTable[63 - square];
                        endgamePst[pieceType][square] = endgameKnightTable[63 - square];
                        baseMidgamePst[pieceType][square] = midgameKnightTable[63 - square];
                        baseEndgamePst[pieceType][square] = endgameKnightTable[63 - square];
                        break;
                    case PieceType::BLACK_KNIGHT:
                        midgamePst[pieceType][square] = midgameKnightTable[square];
                        endgamePst[pieceType][square] = endgameKnightTable[square];
                        baseMidgamePst[pieceType][square] = midgameKnightTable[square];
                        baseEndgamePst[pieceType][square] = endgameKnightTable[square];
                        break;
                    case PieceType::WHITE_BISHOP:
                        midgamePst[pieceType][square] = midgameBishopTable[63 - square];
                        endgamePst[pieceType][square] = endgameBishopTable[63 - square];
                        baseMidgamePst[pieceType][square] = midgameBishopTable[63 - square];
                        baseEndgamePst[pieceType][square] = endgameBishopTable[63 - square];
                        break;
                    case PieceType::BLACK_BISHOP:
                        midgamePst[pieceType][square] = midgameBishopTable[square];
                        endgamePst[pieceType][square] = endgameBishopTable[square];
                        baseMidgamePst[pieceType][square] = midgameBishopTable[square];
                        baseEndgamePst[pieceType][square] = endgameBishopTable[square];
                        break;
                    case PieceType::WHITE_ROOK:
                        midgamePst[pieceType][square] = midgameRookTable[63 - square];
                        endgamePst[pieceType][square] = endgameRookTable[63 - square];
                        baseMidgamePst[pieceType][square] = midgameRookTable[63 - square];
                        baseEndgamePst[pieceType][square] = endgameRookTable[63 - square];
                        break;
                    case PieceType::BLACK_ROOK:
                        midgamePst[pieceType][square] = midgameRookTable[square];
                        endgamePst[pieceType][square] = endgameRookTable[square];
                        baseMidgamePst[pieceType][square] = midgameRookTable[square];
                        baseEndgamePst[pieceType][square] = endgameRookTable[square];
                        break;
                    case PieceType::WHITE_QUEEN:
                        midgamePst[pieceType][square] = midgameQueenTable[63 - square];
                        endgamePst[pieceType][square] = endgameQueenTable[63 - square];
                        baseMidgamePst[pieceType][square] = midgameQueenTable[63 - square];
                        baseEndgamePst[pieceType][square] = endgameQueenTable[63 - square];
                        break;
                    case PieceType::BLACK_QUEEN:
                        midgamePst[pieceType][square] = midgameQueenTable[square];
                        endgamePst[pieceType][square] = endgameQueenTable[square];
                        baseMidgamePst[pieceType][square] = midgameQueenTable[square];
                        baseEndgamePst[pieceType][square] = endgameQueenTable[square];
                        break;
                    case PieceType::WHITE_KING:
                        midgamePst[pieceType][square] = midgameKingTable[63 - square];
                        endgamePst[pieceType][square] = endgameKingTable[63 - square];
                        baseMidgamePst[pieceType][square] = midgameKingTable[63 - square];
                        baseEndgamePst[pieceType][square] = endgameKingTable[63 - square];
                        break;
                    case PieceType::BLACK_KING:
                        midgamePst[pieceType][square] = midgameKingTable[square];
                        endgamePst[pieceType][square] = endgameKingTable[square];
                        baseMidgamePst[pieceType][square] = midgameKingTable[square];
                        baseEndgamePst[pieceType][square] = endgameKingTable[square];
                        break;
                }
            }
        }
    }
}