/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 96, 128, 60, 93, 68, 123, 36, -10, -2, 4, 31, 32, 59, 57, 22, -16, -10, 11, 2, 23, 22, 15, 21, -16, -40, -9, -7, 12, 18, 10, 4, -29, -21, -2, -3, -5, -1, 7, 16, -15, -26, 5, -8, -16, -14, 7, 31, -16, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 178, 171, 157, 132, 147, 130, 166, 188, 95, 99, 87, 66, 52, 54, 82, 86, 33, 26, 13, -1, -6, 6, 15, 19, 10, 7, -4, -7, -10, -10, 0, 2, 6, 5, -5, 9, 9, -6, -8, -4, 17, 12, 15, 16, 16, 0, 0, -4, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -165, -89, -36, -49, 59, -94, -15, -107, -70, -38, 74, 39, 23, 65, 6, -17, -45, 57, 36, 66, 82, 122, 71, 43, -5, 14, 17, 43, 36, 62, 17, 24, -16, 3, 15, 16, 24, 20, 20, -11, -29, -3, 16, 15, 21, 15, 20, -16, -25, -51, -11, 7, 10, 18, -13, -16, -103, -26, -53, -34, -20, -28, -21, -24 };
    int endgameKnightTable[64] = { -57, -38, -13, -28, -31, -26, -63, -100, -25, -6, -24, 0, -7, -21, -24, -52, -23, -21, 9, 11, 0, -10, -19, -41, -15, 6, 21, 20, 23, 9, 9, -17, -17, -6, 15, 28, 14, 17, 3, -20, -24, -1, -2, 16, 13, -3, -23, -22, -41, -20, -9, -5, -1, -20, -23, -44, -28, -46, -22, -16, -22, -17, -51, -64 };

    int midgameBishopTable[64] = { -30, 4, -80, -35, -26, -41, 7, -7, -24, 10, -16, -11, 26, 56, 19, -40, -6, 36, 43, 37, 34, 51, 36, 0, -2, 1, 15, 42, 33, 33, 7, -2, -6, 8, 4, 24, 29, 5, 9, 3, 1, 17, 13, 7, 8, 25, 16, 16, 5, 30, 15, 6, 11, 23, 39, 0, -30, -6, -13, -22, -13, -4, -36, -17 };
    int endgameBishopTable[64] = { -14, -20, -10, -7, -7, -8, -16, -21, -5, -5, 7, -9, -4, -15, -4, -9, 3, -8, 0, -2, -1, 6, 0, 4, -4, 9, 10, 5, 13, 8, 4, 0, -9, 2, 9, 18, 1, 8, -2, -9, -10, -4, 8, 9, 9, 3, -6, -13, -14, -10, -6, 0, 8, -9, -10, -27, -21, -10, -15, -3, -6, -12, -5, -15 };

    int midgameRookTable[64] = { 33, 41, 31, 51, 62, 9, 31, 43, 26, 28, 56, 59, 77, 63, 24, 40, -3, 17, 25, 34, 17, 45, 61, 16, -22, -11, 5, 24, 21, 32, -9, -18, -36, -26, -12, -4, 10, -7, 5, -23, -46, -25, -15, -18, 1, 0, -4, -34, -47, -16, -19, -10, -4, 6, -5, -65, -10, -17, 2, 7, 8, 6, -24, -22 };
    int endgameRookTable[64] = { 13, 9, 18, 16, 11, 11, 7, 5, 10, 11, 13, 9, -5, 0, 9, 3, 8, 6, 6, 4, 4, -3, -4, -1, 4, 1, 12, 0, 0, 2, -1, 1, 1, 5, 6, 1, -6, -7, -7, -12, -5, 0, -4, -1, -6, -10, -10, -16, -9, -5, -2, 1, -9, -11, -9, -1, 0, 0, 5, -5, -11, -9, 9, -13 };

    int midgameQueenTable[64] = { -27, 0, 29, 12, 60, 42, 42, 42, -22, -45, -5, -2, -18, 54, 20, 52, -10, -13, 7, 8, 29, 53, 46, 52, -18, -27, -15, -16, -7, 14, -4, 0, -17, -25, -11, -14, -6, -4, 0, -4, -16, 0, -5, -6, -6, -3, 12, 3, -32, -7, 5, 3, 7, 15, 0, 2, 0, -20, -13, 1, 3, -21, -28, -41 };
    int endgameQueenTable[64] = { -8, 22, 21, 27, 27, 18, 9, 18, -15, 19, 31, 39, 56, 22, 27, 0, -18, 7, 8, 48, 46, 34, 18, 6, 5, 22, 24, 43, 53, 37, 56, 36, -20, 28, 18, 45, 29, 33, 41, 21, -17, -28, 17, 2, 9, 16, 8, 5, -23, -23, -30, -10, -14, -22, -31, -30, -32, -25, -24, -39, 4, -29, -18, -36 };

    int midgameKingTable[64] = { -65, 23, 16, -15, -56, -34, 2, 13, 28, -1, -19, -7, -7, -4, -37, -29, -9, 24, 1, -15, -20, 6, 21, -22, -17, -19, -12, -27, -30, -25, -14, -35, -49, 0, -27, -39, -47, -44, -32, -50, -14, -14, -21, -48, -46, -31, -14, -28, 7, 6, -9, -65, -45, -19, 7, 5, -3, 32, 1, -26, -14, -30, 20, 11 };
    int endgameKingTable[64] = { -74, -35, -18, -18, -11, 15, 4, -17, -12, 16, 14, 17, 17, 38, 22, 10, 10, 17, 22, 15, 19, 45, 43, 12, -8, 21, 23, 26, 25, 33, 26, 3, -18, -3, 20, 23, 25, 23, 10, -11, -18, -3, 12, 20, 22, 14, 6, -8, -24, -12, 7, 19, 15, 4, -4, -19, -48, -37, -27, 1, -34, -14, -24, -48 };

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