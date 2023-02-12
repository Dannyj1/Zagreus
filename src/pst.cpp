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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 108, 122, 66, 96, 70, 121, 41, 6, 28, 20, 46, 41, 65, 74, 46, 0, 10, 26, 22, 87, 66, 34, 52, 27, -12, -14, 23, 12, 23, 28, 33, 9, -34, -32, -39, -58, -46, -12, -24, -19, -49, -8, -13, -22, -28, -34, 22, -15, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 196, 170, 168, 139, 152, 132, 181, 213, 121, 110, 89, 75, 67, 80, 104, 124, 58, 29, 24, 14, 0, 20, 45, 72, 9, 3, 4, -38, -27, -22, 3, 15, -9, -27, -27, -26, -19, -16, -15, 5, -5, -12, -3, 18, 4, 32, 13, 13, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -86, -34, -44, 58, -93, -14, -113, -66, -27, 71, 48, 27, 68, 0, -15, -40, 56, 38, 87, 89, 108, 73, 42, 3, 19, 29, 52, 53, 64, 23, 36, -20, 10, 11, 23, 21, 15, 27, -22, -32, 0, -4, 34, 27, 11, -6, -35, -28, -52, -20, -14, -2, 0, -35, -14, -103, -12, -54, -31, -39, -51, -4, -26 };
    int endgameKnightTable[64] = { -56, -38, -10, -24, -26, -23, -59, -95, -22, -2, -21, 9, 4, -16, -21, -48, -16, -13, 21, 21, 15, 0, -14, -37, -5, 17, 39, 36, 43, 20, 20, -10, -10, -5, 23, 29, 32, 23, 0, -17, -24, -4, -34, 16, 17, -10, -24, -22, -40, -17, -19, -22, -26, -24, -27, -47, -29, -47, -31, -31, -36, -29, -63, -63 };

    int midgameBishopTable[64] = { -29, 1, -70, -42, -26, -39, 0, -15, -26, 5, -7, -8, 23, 41, 15, -29, 22, 46, 48, 46, 36, 40, 38, 14, 3, 11, 15, 44, 40, 26, 0, -19, 3, 15, 0, 25, 23, -7, 0, 20, 0, 1, 0, 22, 17, 27, 12, 22, -3, 12, 21, -14, 5, 46, 5, 0, -22, 0, -12, -27, -27, 16, -7, -20 };
    int endgameBishopTable[64] = { -15, -20, -2, -1, -6, -4, -20, -25, -1, 0, 10, 0, -2, -8, -3, -4, 22, 10, 5, 9, 13, 6, 3, 10, 16, 17, 19, 15, 12, 13, 14, 0, -1, 2, 9, 11, -3, 13, 0, -7, 0, -14, -8, 7, 6, 2, 0, -7, -11, -30, -12, -5, 0, -14, -30, -29, -24, -21, -26, -11, -17, -23, 0, -19 };

    int midgameRookTable[64] = { 31, 41, 32, 44, 55, 9, 32, 44, 34, 34, 60, 66, 71, 75, 36, 49, 6, 28, 39, 41, 43, 58, 69, 20, -4, 3, 17, 31, 24, 42, 4, -9, -35, -24, -21, 0, 13, -10, 0, -12, -32, -7, -5, 0, 8, 7, 0, -26, -55, -14, -25, -24, -22, 0, -16, -74, -47, -22, -18, 8, 0, -19, -3, -43 };
    int endgameRookTable[64] = { 14, 9, 14, 2, 0, 6, 12, 0, 35, 29, 12, 23, 5, 21, 28, 15, 23, 18, 25, 15, 33, 14, 7, 7, 20, 14, 26, 24, 16, 16, 15, 15, 0, 12, 9, 0, 0, 0, -13, -4, 0, 0, 0, 0, -16, -18, -11, -17, -21, -16, -15, -11, -30, -26, -23, -8, -31, -22, -39, -22, -31, -21, -1, -35 };

    int midgameQueenTable[64] = { -15, 4, 30, 13, 62, 39, 42, 29, -10, -40, 4, 1, -17, 43, 0, 36, 6, 0, 3, 6, 33, 50, 41, 59, 3, -15, -13, -8, 1, 5, -7, 3, -20, -5, -11, -12, 4, -13, 0, -11, -13, -11, 0, -11, -8, -12, -15, 0, -18, -16, -22, -14, 3, -1, 5, -6, 0, -19, -24, -5, 32, -17, -23, -30 };
    int endgameQueenTable[64] = { 3, 31, 28, 32, 31, 16, 8, 13, 1, 23, 41, 43, 55, 23, 18, 0, 0, 11, 14, 50, 53, 32, 17, 12, 21, 32, 26, 47, 48, 37, 55, 41, -13, 33, 19, 33, 32, 27, 40, 15, -11, -26, 11, 0, -1, 14, 0, 0, -18, -23, -36, -31, -29, -43, -44, -24, -28, -29, -24, -41, -21, -38, -23, -33 };

    int midgameKingTable[64] = { -64, 19, 14, -12, -52, -32, 0, 10, 27, 0, -15, -3, -4, -1, -32, -27, -4, 27, 6, -9, -14, 10, 25, -19, -11, -10, -6, -23, -22, -18, -5, -34, -42, 1, -15, -27, -31, -36, -18, -43, -19, 0, -16, -45, -51, -31, -15, -28, 49, 42, -22, -59, -58, -26, 25, 0, 19, 32, -20, -17, -39, -8, -11, -18 };
    int endgameKingTable[64] = { -72, -32, -14, -15, -6, 12, 0, -14, -7, 19, 21, 21, 20, 42, 30, 10, 15, 26, 40, 27, 29, 54, 57, 16, -1, 37, 38, 45, 44, 55, 39, 0, -11, 1, 33, 53, 58, 45, 31, 0, -32, -5, 19, 34, 21, 17, 7, -11, -22, 0, -8, 7, 0, 0, 0, -17, -83, -74, -55, -45, -52, -41, -24, -92 };

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