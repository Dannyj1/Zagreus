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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 103, 121, 65, 96, 67, 120, 39, 5, 15, 15, 45, 41, 57, 73, 38, -4, 0, 13, 21, 60, 42, 24, 46, 11, -14, -2, 9, 10, 12, 27, 23, -5, -21, -26, -31, -25, -15, -20, -14, -23, -36, 0, -11, -16, -17, -18, 29, -8, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 194, 168, 163, 136, 146, 131, 178, 211, 111, 102, 87, 72, 62, 76, 98, 119, 47, 31, 19, 0, -12, 16, 43, 60, 17, 0, -4, -29, -33, -16, 4, 13, -8, -19, -21, -12, -7, -17, -17, -1, 6, -7, 0, 24, 3, 17, 4, 10, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -87, -35, -44, 60, -94, -14, -115, -67, -27, 71, 49, 26, 71, 0, -16, -41, 58, 34, 81, 89, 106, 72, 41, 5, 15, 27, 48, 53, 66, 27, 35, -14, 6, 11, 17, 18, 17, 26, -14, -34, 2, 4, 31, 22, 9, 0, -33, -29, -50, -18, -12, 2, 3, -27, -17, -104, -9, -55, -32, -37, -44, -1, -25 };
    int endgameKnightTable[64] = { -57, -38, -11, -25, -26, -25, -59, -95, -23, -3, -25, 7, 3, -17, -20, -49, -16, -15, 18, 15, 13, -2, -13, -40, -5, 14, 33, 32, 42, 15, 21, -12, -9, -7, 17, 28, 30, 23, 0, -14, -24, -2, -30, 14, 13, -15, -26, -23, -41, -17, -20, -19, -20, -18, -26, -45, -29, -45, -30, -29, -29, -26, -60, -63 };

    int midgameBishopTable[64] = { -29, 2, -72, -42, -26, -39, 0, -13, -24, 4, -7, -10, 22, 44, 16, -30, 17, 45, 47, 48, 33, 42, 40, 12, 0, 2, 17, 46, 34, 25, 5, -17, -1, 12, 0, 24, 28, -7, 1, 15, 6, 7, 5, 15, 14, 27, 6, 15, 0, 27, 21, 0, 8, 32, 20, -2, -22, 0, -8, -19, -17, 12, -20, -15 };
    int endgameBishopTable[64] = { -15, -20, -5, -3, -6, -5, -20, -24, 0, 0, 9, -2, -5, -10, -5, -4, 18, 8, 4, 7, 10, 7, 3, 11, 11, 13, 18, 18, 7, 8, 14, 0, -3, 0, 8, 11, -1, 10, -6, -12, 0, -10, -8, 5, 3, 0, -1, -11, -9, -22, -9, -4, 3, -13, -21, -29, -21, -16, -18, -5, -14, -24, -5, -15 };

    int midgameRookTable[64] = { 30, 39, 30, 43, 55, 9, 31, 45, 30, 33, 60, 62, 72, 70, 38, 49, 7, 28, 36, 39, 39, 56, 69, 20, -13, 2, 15, 28, 27, 42, 1, -7, -41, -24, -19, -6, 8, -11, 0, -14, -40, -18, -14, -8, 2, 0, -2, -33, -59, -16, -27, -22, -15, -4, -7, -66, -41, -16, -4, 11, 2, -9, -6, -30 };
    int endgameRookTable[64] = { 11, 7, 14, 2, 0, 6, 12, 0, 29, 25, 13, 18, 8, 15, 30, 14, 22, 16, 19, 14, 30, 14, 4, 9, 16, 15, 21, 19, 15, 10, 14, 16, -1, 10, 10, -1, -1, -1, -13, -6, 0, 0, 0, -1, -14, -21, -15, -18, -21, -15, -13, -11, -22, -28, -17, -5, -31, -18, -28, -19, -19, -18, 0, -27 };

    int midgameQueenTable[64] = { -16, 4, 28, 9, 62, 39, 43, 31, -11, -43, 5, 3, -17, 43, 0, 34, 7, 1, 4, 2, 31, 45, 45, 57, 0, -13, -11, -12, -1, 10, -9, 8, -18, -11, -13, -9, -2, -15, 2, -4, -13, -9, 0, -8, -11, -6, -10, 0, -20, -12, -16, -11, -3, 2, 11, -9, -5, -24, -20, -1, 28, -19, -21, -32 };
    int endgameQueenTable[64] = { 2, 29, 24, 30, 29, 17, 8, 13, 2, 18, 43, 43, 57, 22, 19, -1, -1, 11, 12, 48, 51, 27, 19, 10, 18, 33, 27, 43, 46, 39, 55, 44, -14, 32, 20, 38, 30, 28, 40, 15, -12, -26, 12, -2, 0, 17, 0, 0, -19, -25, -38, -24, -25, -37, -40, -25, -31, -30, -24, -33, -16, -35, -21, -34 };

    int midgameKingTable[64] = { -64, 19, 15, -13, -53, -32, 0, 11, 28, 0, -16, -4, -5, -2, -32, -28, -4, 27, 6, -10, -17, 11, 25, -20, -13, -11, -6, -23, -23, -19, -6, -35, -44, 0, -17, -29, -33, -36, -20, -45, -18, -5, -14, -48, -49, -30, -13, -28, 35, 29, -26, -61, -59, -29, 16, 3, 15, 36, -28, -17, -32, -14, 5, -8 };
    int endgameKingTable[64] = { -72, -32, -14, -16, -7, 13, 1, -14, -8, 20, 20, 20, 19, 42, 29, 11, 15, 25, 39, 25, 27, 55, 55, 16, -3, 37, 38, 46, 42, 54, 39, 1, -11, 0, 31, 50, 55, 44, 30, -3, -30, -6, 18, 32, 23, 17, 7, -11, -28, -4, -9, 8, 2, 0, -12, -18, -80, -60, -53, -42, -47, -34, -29, -84 };

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