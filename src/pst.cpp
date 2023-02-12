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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 113, 147, 73, 105, 86, 135, 42, 5, 68, 76, 58, 77, 104, 108, 76, 57, 80, 105, 63, 120, 111, 114, 132, 128, 40, 29, 56, 20, 37, 73, 128, 34, -109, -139, -61, -109, -85, -41, -109, -40, -137, -134, -44, -31, -73, -50, -43, -68, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 190, 187, 171, 145, 158, 139, 172, 194, 153, 148, 106, 90, 90, 87, 123, 140, 125, 79, 43, 12, 3, 44, 85, 131, 32, 10, 9, -23, -40, -21, 56, 24, -79, -69, -22, -53, -15, -27, -84, -37, -75, -39, 40, 20, 10, 51, -21, -66, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -169, -89, -33, -48, 66, -94, -12, -107, -66, -37, 76, 41, 32, 61, 8, -12, -29, 58, 32, 84, 97, 147, 93, 52, -11, 24, 42, 99, 70, 63, 47, 41, -14, 27, 32, 49, 106, 44, 36, -12, -38, -23, -58, 55, 32, 21, -16, -7, -43, -63, -31, -13, -29, 24, -32, -62, -109, -39, -89, -63, -66, -91, -59, -34 };
    int endgameKnightTable[64] = { -59, -37, -11, -25, -26, -25, -61, -100, -22, -5, -24, 0, 0, -17, -22, -50, -13, -18, 3, 30, 7, 4, -5, -37, -21, 10, 41, 41, 52, 21, 21, -5, -10, 2, 43, 31, 48, 47, 12, -15, -37, -14, -26, 14, 34, -31, -32, -24, -62, -30, -27, -15, -32, -35, -28, -62, -32, -63, -45, -32, -38, -54, -56, -67 };

    int midgameBishopTable[64] = { -29, 4, -82, -35, -26, -40, 5, -16, -29, 20, -14, -15, 33, 56, 17, -46, 9, 61, 66, 58, 54, 45, 44, 24, -12, 29, 26, 58, 60, 37, 11, -7, 0, 32, 1, 29, 43, -17, 6, 32, -10, -23, -2, 14, 32, 25, 7, 26, -17, -70, 21, -39, -3, 82, -32, -16, -42, 15, 17, -70, -48, 72, 5, -49 };
    int endgameBishopTable[64] = { -13, -21, -13, -5, -3, -6, -19, -27, -10, 3, 12, -8, -1, -10, 0, -12, 17, 0, 14, 13, 8, 7, 5, 22, -12, 27, 18, 23, 29, 18, 16, -2, -4, 10, 21, 14, 14, 30, -4, -5, -14, -20, -15, 9, 16, 0, 0, -8, -21, -75, -16, -28, -1, -9, -55, -35, -39, -4, -31, -31, -26, 0, 0, -38 };

    int midgameRookTable[64] = { 50, 55, 38, 60, 72, 12, 27, 55, 46, 47, 77, 81, 92, 65, 33, 55, 13, 46, 51, 73, 56, 60, 77, 41, 5, 25, 22, 54, 61, 44, 3, -3, -21, -6, -13, 24, 23, 1, 31, -11, -9, 19, 8, 4, -16, -19, 35, 5, -58, -18, -59, -52, -27, 13, -7, -67, -79, 24, -46, -66, -50, -53, -17, -77 };
    int endgameRookTable[64] = { 29, 22, 23, 21, 8, 23, 5, 12, 33, 23, 27, 23, -3, 15, 21, 21, 32, 24, 25, 30, 34, 8, 9, 11, 7, 21, 19, 23, 24, 17, 6, 21, 3, 11, 6, 10, 0, 0, -1, -2, -7, 8, 0, 0, -20, -30, 2, -11, -28, -24, -32, -34, -47, -15, -24, -4, -42, 2, -37, -48, -56, -61, 1, -47 };

    int midgameQueenTable[64] = { -19, 5, 32, 14, 62, 48, 44, 42, -17, -32, -2, 1, -15, 53, 9, 55, 6, 0, 8, 18, 37, 55, 35, 57, 17, -29, -13, -32, 0, 14, -11, -6, 9, -18, -19, -5, -14, 3, 32, 7, 7, -3, 0, -38, -15, 9, 15, 14, -16, -4, -59, -43, 20, -44, -9, 12, 7, 0, -44, 72, -12, -65, -31, -42 };
    int endgameQueenTable[64] = { -6, 27, 28, 28, 31, 20, 12, 16, -14, 20, 31, 44, 61, 25, 27, 3, -6, 16, 19, 50, 55, 40, 24, 24, 28, 27, 25, 53, 59, 45, 57, 38, -11, 23, 28, 61, 36, 37, 51, 29, -2, -27, 23, 5, -2, 20, 8, 4, -19, -27, -57, -52, -13, -52, -50, -39, -23, -24, -51, -59, -33, -60, -26, -40 };

    int midgameKingTable[64] = { -65, 23, 15, -14, -56, -34, 1, 12, 28, 0, -20, -7, -8, -3, -38, -29, -10, 24, 1, -15, -21, 6, 21, -21, -15, -19, -12, -27, -30, -25, -12, -35, -50, 0, -24, -38, -39, -41, -30, -48, -20, -7, -16, -41, -40, -28, 6, -20, 59, 105, -15, -49, -37, -22, 81, 64, -9, -67, 16, -84, 40, -34, -19, -21 };
    int endgameKingTable[64] = { -74, -34, -18, -17, -10, 14, 3, -17, -12, 18, 14, 17, 16, 38, 23, 10, 9, 17, 25, 16, 19, 43, 43, 14, -6, 22, 24, 26, 25, 34, 28, 1, -19, 1, 27, 29, 36, 29, 13, -9, -24, 0, 23, 32, 35, 19, 18, -4, -19, 24, -5, 37, 47, -13, 16, 39, -97, -110, -27, -9, -42, -29, -34, -95 };

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
                        baseMidgamePst[pieceType][square] = baseMidgamePawnTable[63 - square];
                        baseEndgamePst[pieceType][square] = baseEndgamePawnTable[63 - square];
                        break;
                    case PieceType::BLACK_PAWN:
                        midgamePst[pieceType][square] = midgamePawnTable[square];
                        endgamePst[pieceType][square] = endgamePawnTable[square];
                        baseMidgamePst[pieceType][square] = baseMidgamePawnTable[square];
                        baseEndgamePst[pieceType][square] = baseEndgamePawnTable[square];
                        break;
                    case PieceType::WHITE_KNIGHT:
                        midgamePst[pieceType][square] = midgameKnightTable[63 - square];
                        endgamePst[pieceType][square] = endgameKnightTable[63 - square];
                        baseMidgamePst[pieceType][square] = baseMidgameKnightTable[63 - square];
                        baseEndgamePst[pieceType][square] = baseEndgameKnightTable[63 - square];
                        break;
                    case PieceType::BLACK_KNIGHT:
                        midgamePst[pieceType][square] = midgameKnightTable[square];
                        endgamePst[pieceType][square] = endgameKnightTable[square];
                        baseMidgamePst[pieceType][square] = baseMidgameKnightTable[square];
                        baseEndgamePst[pieceType][square] = baseEndgameKnightTable[square];
                        break;
                    case PieceType::WHITE_BISHOP:
                        midgamePst[pieceType][square] = midgameBishopTable[63 - square];
                        endgamePst[pieceType][square] = endgameBishopTable[63 - square];
                        baseMidgamePst[pieceType][square] = baseMidgameBishopTable[63 - square];
                        baseEndgamePst[pieceType][square] = baseEndgameBishopTable[63 - square];
                        break;
                    case PieceType::BLACK_BISHOP:
                        midgamePst[pieceType][square] = midgameBishopTable[square];
                        endgamePst[pieceType][square] = endgameBishopTable[square];
                        baseMidgamePst[pieceType][square] = baseMidgameBishopTable[square];
                        baseEndgamePst[pieceType][square] = baseEndgameBishopTable[square];
                        break;
                    case PieceType::WHITE_ROOK:
                        midgamePst[pieceType][square] = midgameRookTable[63 - square];
                        endgamePst[pieceType][square] = endgameRookTable[63 - square];
                        baseMidgamePst[pieceType][square] = baseMidgameRookTable[63 - square];
                        baseEndgamePst[pieceType][square] = baseEndgameRookTable[63 - square];
                        break;
                    case PieceType::BLACK_ROOK:
                        midgamePst[pieceType][square] = midgameRookTable[square];
                        endgamePst[pieceType][square] = endgameRookTable[square];
                        baseMidgamePst[pieceType][square] = baseMidgameRookTable[square];
                        baseEndgamePst[pieceType][square] = baseEndgameRookTable[square];
                        break;
                    case PieceType::WHITE_QUEEN:
                        midgamePst[pieceType][square] = midgameQueenTable[63 - square];
                        endgamePst[pieceType][square] = endgameQueenTable[63 - square];
                        baseMidgamePst[pieceType][square] = baseMidgameQueenTable[63 - square];
                        baseEndgamePst[pieceType][square] = baseEndgameQueenTable[63 - square];
                        break;
                    case PieceType::BLACK_QUEEN:
                        midgamePst[pieceType][square] = midgameQueenTable[square];
                        endgamePst[pieceType][square] = endgameQueenTable[square];
                        baseMidgamePst[pieceType][square] = baseMidgameQueenTable[square];
                        baseEndgamePst[pieceType][square] = baseEndgameQueenTable[square];
                        break;
                    case PieceType::WHITE_KING:
                        midgamePst[pieceType][square] = midgameKingTable[63 - square];
                        endgamePst[pieceType][square] = endgameKingTable[63 - square];
                        baseMidgamePst[pieceType][square] = baseMidgameKingTable[63 - square];
                        baseEndgamePst[pieceType][square] = baseEndgameKingTable[63 - square];
                        break;
                    case PieceType::BLACK_KING:
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