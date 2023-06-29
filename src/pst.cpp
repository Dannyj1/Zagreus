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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 96, 130, 60, 95, 67, 124, 35, -9, -6, 6, 27, 31, 59, 58, 22, -19, -14, 13, 0, 22, 21, 13, 22, -20, -39, -7, -10, 14, 20, 9, 10, -28, -22, 0, -9, -3, 3, 3, 19, -16, -29, 6, -12, -15, -11, 8, 38, -16, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 177, 171, 156, 134, 147, 130, 166, 186, 92, 99, 86, 68, 51, 53, 81, 82, 31, 25, 9, 0, -6, 4, 14, 18, 10, 4, -6, -6, -7, -10, 3, 0, 4, 8, -4, 8, 6, -5, -1, -8, 13, 13, 9, 12, 16, 0, 0, -6, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -165, -89, -34, -48, 60, -96, -14, -108, -71, -40, 71, 38, 23, 64, 8, -18, -46, 57, 37, 66, 83, 122, 72, 42, -6, 16, 17, 41, 37, 62, 15, 21, -11, 5, 16, 14, 23, 19, 22, -10, -22, -5, 17, 14, 20, 15, 21, -14, -26, -52, -9, 0, 7, 18, -14, -18, -105, -24, -56, -32, -19, -27, -16, -23 };
    int endgameKnightTable[64] = { -57, -38, -13, -27, -31, -26, -63, -99, -25, -7, -24, -1, -8, -23, -22, -51, -23, -21, 10, 8, 0, -10, -17, -43, -15, 4, 21, 18, 21, 10, 8, -19, -16, -4, 18, 24, 12, 16, 5, -19, -23, -1, -6, 15, 12, -6, -23, -21, -43, -19, -8, -5, -3, -20, -23, -42, -28, -47, -21, -13, -24, -16, -47, -63 };

    int midgameBishopTable[64] = { -28, 4, -81, -36, -26, -41, 5, -5, -23, 19, -16, -12, 30, 54, 18, -44, -12, 36, 44, 39, 34, 52, 36, 0, -6, 0, 16, 43, 34, 30, 5, 0, -3, 11, 7, 22, 32, 6, 9, 0, 1, 15, 14, 10, 9, 26, 16, 13, 5, 28, 15, 2, 8, 17, 35, 0, -31, -5, -9, -19, -10, -1, -38, -18 };
    int endgameBishopTable[64] = { -13, -21, -11, -6, -8, -9, -17, -23, -7, 0, 5, -11, -2, -12, -3, -11, 2, -8, 1, -1, 0, 6, 0, 5, -2, 9, 13, 9, 13, 7, 2, 1, -6, 2, 9, 15, 4, 8, -3, -8, -9, -3, 8, 8, 8, 1, -7, -14, -12, -14, -5, -2, 5, -11, -13, -26, -24, -8, -16, -3, -7, -12, -4, -16 };

    int midgameRookTable[64] = { 32, 41, 31, 51, 62, 7, 31, 43, 24, 32, 55, 62, 78, 65, 25, 41, -1, 17, 24, 34, 19, 43, 59, 15, -23, -10, 8, 27, 22, 33, -9, -20, -37, -26, -14, -4, 8, -8, 3, -24, -45, -24, -18, -17, 1, 0, -5, -32, -46, -15, -21, -8, -3, 7, -4, -69, -12, -18, 2, 8, 13, 7, -25, -25 };
    int endgameRookTable[64] = { 13, 10, 17, 15, 11, 10, 8, 4, 9, 15, 13, 12, -3, 2, 8, 1, 7, 5, 6, 4, 6, -3, -5, -4, 4, 4, 12, 0, 1, 0, -1, 3, 3, 4, 6, 1, -5, -5, -8, -13, -3, 0, -4, -2, -8, -12, -9, -16, -7, -6, 0, 2, -9, -7, -9, -1, -5, 2, 1, -3, -6, -5, 7, -17 };

    int midgameQueenTable[64] = { -27, 0, 29, 11, 58, 43, 42, 43, -20, -39, -7, 0, -17, 53, 20, 49, -6, -13, 5, 6, 30, 54, 45, 56, -22, -27, -15, -18, -7, 14, -5, 0, -11, -27, -12, -11, -8, -5, 4, -4, -14, 0, -6, -5, -7, 0, 10, 4, -33, -8, 11, 1, 2, 14, 0, 2, 0, -18, -11, 2, 0, -19, -26, -43 };
    int endgameQueenTable[64] = { -8, 22, 22, 28, 26, 18, 9, 20, -15, 24, 32, 39, 57, 23, 28, -1, -17, 7, 8, 47, 47, 33, 18, 8, 5, 22, 25, 46, 54, 37, 56, 35, -16, 26, 19, 47, 28, 33, 37, 22, -17, -28, 15, 2, 5, 15, 7, 5, -20, -23, -29, -12, -16, -22, -35, -31, -31, -26, -24, -41, 0, -30, -19, -37 };

    int midgameKingTable[64] = { -65, 22, 16, -15, -56, -34, 2, 13, 29, -1, -20, -7, -7, -3, -37, -29, -8, 24, 1, -16, -19, 6, 22, -21, -17, -20, -11, -26, -30, -24, -14, -36, -49, -1, -27, -39, -46, -44, -33, -51, -15, -15, -24, -48, -45, -31, -13, -25, 3, 4, -10, -65, -45, -16, 10, 8, -3, 27, 2, -32, -8, -28, 26, 20 };
    int endgameKingTable[64] = { -74, -35, -18, -18, -11, 15, 4, -17, -12, 16, 14, 17, 17, 38, 22, 11, 10, 17, 22, 15, 20, 45, 44, 13, -8, 21, 24, 27, 26, 33, 26, 2, -18, -4, 20, 24, 26, 23, 9, -11, -19, -5, 12, 19, 23, 16, 8, -7, -28, -11, 2, 14, 15, 7, -5, -18, -51, -38, -26, -9, -30, -11, -19, -42 };
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