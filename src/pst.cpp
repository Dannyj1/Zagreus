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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 123, 124, 75, 104, 81, 128, 42, 8, 67, 69, 50, 73, 94, 88, 80, 39, 44, 81, 85, 164, 143, 73, 82, 78, 0, 12, 76, 4, 57, 58, 73, 34, -32, -79, -63, -137, -138, -52, -75, -38, -96, -63, -50, -26, -85, -69, -7, -22, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 211, 170, 177, 146, 160, 137, 179, 213, 163, 145, 100, 97, 87, 93, 130, 156, 93, 47, 43, 48, 21, 48, 72, 118, 10, 16, 13, -35, -30, -22, 35, 34, -38, -67, -7, -87, -48, -31, -76, -8, -48, -43, 3, 16, -14, 45, -4, -18, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -85, -31, -40, 61, -89, -13, -112, -62, -22, 76, 52, 35, 62, -7, -16, -33, 59, 62, 105, 126, 117, 72, 43, 0, 21, 35, 70, 64, 56, 14, 58, -19, 39, -7, 49, 45, 11, 27, -58, -38, 0, -19, 37, 54, 35, -39, -28, -44, -71, -15, -19, -18, -10, -55, -3, -105, -13, -59, -38, -53, -90, -17, -33 };
    int endgameKnightTable[64] = { -55, -38, -9, -20, -23, -20, -57, -95, -17, -4, -16, 14, 4, -15, -21, -47, -14, -3, 38, 30, 39, 0, -5, -34, -11, 25, 54, 45, 58, 29, 22, 0, -21, 12, 17, 36, 42, 36, -2, -28, -30, -13, -51, 18, 33, -16, -30, -16, -44, -24, -32, -45, -47, -30, -42, -46, -29, -57, -37, -34, -60, -53, -74, -64 };

    int midgameBishopTable[64] = { -31, 3, -72, -39, -27, -38, -1, -11, -26, 0, -8, -11, 18, 35, 0, -31, 47, 57, 61, 54, 46, 34, 33, 27, -18, 17, 18, 46, 55, 43, 12, -11, 7, 8, 0, 20, 14, 11, 3, 23, -34, -46, -10, 50, 37, 5, -4, 20, 0, -7, 32, -76, 13, 59, -36, 4, -13, 28, 31, -40, -70, 37, 34, -27 };
    int endgameBishopTable[64] = { -18, -18, -2, -2, -6, -1, -21, -26, 1, -6, 13, 0, -2, -12, -5, -4, 30, 16, 15, 13, 14, 7, 10, 18, 3, 42, 24, 24, 34, 26, 37, 3, 2, -7, 19, 6, -2, 28, 6, -9, -14, -24, -29, 6, -14, -9, 1, -4, -16, -65, -5, -27, 3, -16, -64, -28, -22, -5, -30, -18, -40, -20, 3, -25 };

    int midgameRookTable[64] = { 33, 45, 41, 48, 51, 14, 29, 36, 38, 37, 64, 70, 83, 80, 43, 48, 18, 29, 53, 56, 57, 72, 64, 21, 2, 26, 22, 49, 29, 58, 20, -10, -42, -8, -16, 15, 22, -2, 13, -7, -16, 1, 9, 42, 16, 21, 38, 3, -65, -11, -23, -36, -23, 0, 0, -68, -88, -30, -56, -14, -31, -3, 4, -100 };
    int endgameRookTable[64] = { 19, 12, 22, 5, 0, 14, 13, -1, 39, 37, 16, 25, 13, 28, 42, 26, 37, 26, 32, 16, 44, 27, 11, 9, 26, 22, 29, 32, 23, 24, 22, 14, 0, 16, 9, 12, 14, 0, -9, 2, -5, 3, -9, 20, -2, -11, 3, -12, -30, -22, -30, -16, -38, -25, -21, -12, -60, -57, -78, -32, -52, -21, 0, -62 };

    int midgameQueenTable[64] = { -13, 4, 31, 9, 63, 41, 44, 26, -13, -44, 10, 0, -15, 34, -10, 26, 9, 6, 0, 12, 37, 49, 34, 54, 3, -24, -12, -7, 11, 4, -15, 1, -42, -14, -3, -8, 3, -15, -3, -11, -18, -29, 0, -13, 0, -15, -21, 23, -18, -26, -7, -41, 16, -20, -8, 0, 0, 3, -25, 39, 19, -43, -43, -24 };
    int endgameQueenTable[64] = { 6, 32, 29, 33, 31, 17, 12, 12, 2, 22, 42, 42, 52, 23, 15, -2, 0, 13, 15, 52, 59, 35, 15, 13, 27, 29, 27, 50, 64, 42, 54, 42, -18, 34, 23, 32, 35, 25, 43, 21, -6, -22, 18, 1, -2, 19, -5, 6, -13, -29, -46, -39, -41, -63, -54, -24, -31, -27, -32, -47, -30, -52, -27, -30 };

    int midgameKingTable[64] = { -64, 19, 14, -12, -51, -32, 0, 10, 27, 0, -14, -3, -4, -1, -32, -26, -3, 27, 6, -8, -14, 10, 26, -18, -11, -10, -6, -23, -21, -15, -4, -32, -50, 2, -17, -27, -29, -34, -9, -42, -14, -1, -16, -48, -61, -28, -5, -12, 81, 63, -19, -41, -49, -18, 82, 19, 13, -29, -1, -83, 0, -17, -9, -3 };
    int endgameKingTable[64] = { -72, -31, -14, -15, -5, 12, 0, -14, -7, 19, 21, 20, 20, 42, 29, 10, 15, 26, 39, 28, 28, 54, 59, 15, 0, 37, 38, 44, 44, 59, 40, 1, -11, 6, 33, 52, 63, 49, 44, 3, -25, -7, 24, 35, 14, 14, 14, 0, -6, 19, -10, 19, 16, -13, 31, 1, -117, -122, -52, -57, -56, -46, -36, -103 };

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