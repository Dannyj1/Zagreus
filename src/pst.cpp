/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "types.h"

#include <vector>

namespace Zagreus {
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 95, 131, 62, 92, 66, 128, 30, -9, -7, 2, 21, 32, 67, 59, 22, -16, -16, 8, 0, 16, 24, 12, 23, -21, -40, -10, -10, 12, 21, 11, 3, -26, -25, -1, -14, -4, 0, 4, 21, -14, -30, 5, -6, -19, -11, 9, 37, -17, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 176, 183, 159, 133, 143, 136, 170, 190, 98, 97, 84, 70, 60, 59, 89, 89, 35, 21, 7, -1, -8, 1, 24, 20, 5, 6, -7, -11, -10, -3, 3, 5, -1, 0, -10, 4, 4, -3, 0, -5, 13, 9, 13, 16, 10, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -169, -90, -31, -47, 60, -97, -16, -110, -74, -37, 68, 35, 24, 68, 6, -20, -48, 57, 38, 67, 86, 124, 68, 42, -9, 22, 15, 47, 38, 64, 14, 23, -15, 4, 12, 13, 21, 20, 25, -11, -27, -10, 20, 13, 23, 15, 24, -14, -22, -51, -8, 6, 6, 16, -12, -10, -101, -28, -56, -32, -22, -32, -22, -24 };
    int endgameKnightTable[64] = { -58, -39, -13, -27, -33, -24, -63, -99, -27, -8, -26, -5, -8, -26, -24, -54, -23, -20, 13, 9, 2, -10, -22, -40, -14, 4, 19, 19, 24, 4, 9, -19, -18, -1, 16, 28, 14, 20, 5, -15, -22, 1, -6, 14, 14, -8, -24, -22, -38, -18, -10, 0, 2, -22, -26, -43, -31, -47, -20, -13, -27, -20, -51, -64 };

    int midgameBishopTable[64] = { -28, 5, -77, -36, -24, -43, 4, -5, -28, 12, -14, -4, 24, 58, 18, -46, -8, 37, 40, 40, 31, 48, 38, -5, -3, 3, 17, 45, 33, 33, 4, 2, -7, 12, 1, 21, 33, 6, 5, 5, 1, 14, 11, 3, 12, 26, 18, 14, 5, 19, 16, 8, 13, 18, 43, 0, -29, -4, -8, -19, -7, -4, -39, -18 };
    int endgameBishopTable[64] = { -12, -19, -8, -7, -8, -11, -18, -22, -10, -3, 5, -10, -8, -10, -6, -15, 5, -9, 1, 0, 0, 2, 0, 3, 0, 4, 11, 9, 7, 11, 0, 1, -6, 5, 9, 13, 6, 12, -6, -11, -10, -5, 10, 6, 8, 11, -8, -13, -17, -13, -6, 5, 10, -14, -16, -24, -21, -11, -19, -2, -4, -10, -5, -16 };

    int midgameRookTable[64] = { 28, 43, 31, 52, 61, 7, 32, 42, 23, 31, 57, 65, 81, 63, 23, 42, 0, 24, 31, 35, 16, 49, 64, 16, -22, -12, 5, 26, 19, 36, -3, -20, -27, -24, -20, -5, 7, -5, 5, -22, -48, -24, -17, -15, 0, 2, -1, -32, -44, -16, -22, -12, -4, 5, -9, -71, -15, -15, 0, 11, 12, 7, -26, -23 };
    int endgameRookTable[64] = { 12, 9, 16, 15, 6, 9, 10, 4, 3, 8, 11, 13, -3, 0, 7, 0, 14, 12, 8, 0, 1, 0, -3, -5, 10, 0, 12, -2, 0, 6, -4, 3, 6, 6, -1, 4, -3, -4, -5, -9, 0, 0, -4, 0, -12, -10, -4, -17, -3, -5, 0, -1, -12, -12, -12, -4, 1, 1, 2, -1, -1, -6, 6, -17 };

    int midgameQueenTable[64] = { -24, 0, 29, 11, 56, 45, 40, 44, -21, -40, -5, 0, -17, 54, 27, 47, -5, -16, 6, 4, 31, 54, 49, 56, -19, -23, -19, -17, -3, 9, -4, 1, -18, -24, -15, -14, -8, -10, 1, 1, -19, -5, -5, -11, -8, -2, 11, 2, -32, -8, 9, 1, 7, 18, -3, 0, -2, -17, -11, 0, 0, -22, -30, -43 };
    int endgameQueenTable[64] = { -6, 24, 21, 25, 25, 24, 7, 18, -18, 20, 32, 39, 56, 23, 31, -5, -14, 5, 6, 48, 47, 34, 18, 6, 6, 24, 22, 40, 56, 35, 55, 36, -17, 31, 15, 46, 26, 32, 38, 28, -19, -30, 13, 4, 6, 21, 6, 4, -18, -22, -27, -14, -16, -22, -33, -34, -32, -27, -24, -38, 5, -27, -19, -34 };

    int midgameKingTable[64] = { -65, 23, 16, -15, -56, -32, 2, 13, 29, -1, -17, -6, -7, -3, -36, -27, -9, 23, 1, -17, -18, 5, 24, -20, -19, -19, -12, -26, -29, -23, -11, -35, -49, -2, -24, -37, -43, -44, -30, -50, -15, -17, -23, -46, -47, -20, -13, -26, 6, 11, -8, -60, -44, -19, 12, 11, -7, 29, 2, -30, -11, -30, 24, 22 };
    int endgameKingTable[64] = { -74, -34, -18, -20, -10, 16, 4, -17, -10, 16, 21, 17, 16, 39, 29, 12, 7, 16, 26, 13, 19, 45, 46, 13, -11, 18, 21, 25, 25, 39, 29, 4, -17, -10, 22, 28, 28, 24, 16, -10, -18, -4, 11, 17, 24, 23, 7, -7, -28, -13, -5, 8, 14, 1, -6, -10, -54, -41, -27, 0, -33, -11, -24, -36 };

    /* int midgamePawnTable[64] = {
            0,   0,   0,   0,   0,   0,  0,   0,
            98, 134,  61,  95,  68, 126, 34, -11,
            -6,   7,  26,  31,  65,  56, 25, -20,
            -14,  13,   6,  21,  23,  12, 17, -23,
            -27,  -2,  -5,  12,  17,   6, 10, -25,
            -26,  -4,  -4, -10,   3,   3, 33, -12,
            -35,  -1, -20, -23, -15,  24, 38, -22,
            0,   0,   0,   0,   0,   0,  0,   0,
    };

     int endgamePawnTable[64] = {
            0,   0,   0,   0,   0,   0,   0,   0,
            178, 173, 158, 134, 147, 132, 165, 187,
            94, 100,  85,  67,  56,  53,  82,  84,
            32,  24,  13,   5,  -2,   4,  17,  17,
            13,   9,  -3,  -7,  -7,  -8,   3,  -1,
            4,   7,  -6,   1,   0,  -5,  -1,  -8,
            13,   8,   8,  10,  13,   0,   2,  -7,
            0,   0,   0,   0,   0,   0,   0,   0,
    };

     int midgameKnightTable[64] = {
            -167, -89, -34, -49,  61, -97, -15, -107,
            -73, -41,  72,  36,  23,  62,   7,  -17,
            -47,  60,  37,  65,  84, 129,  73,   44,
            -9,  17,  19,  53,  37,  69,  18,   22,
            -13,   4,  16,  13,  28,  19,  21,   -8,
            -23,  -9,  12,  10,  19,  17,  25,  -16,
            -29, -53, -12,  -3,  -1,  18, -14,  -19,
            -105, -21, -58, -33, -17, -28, -19,  -23,
    };

     int endgameKnightTable[64] = {
            -58, -38, -13, -28, -31, -27, -63, -99,
            -25,  -8, -25,  -2,  -9, -25, -24, -52,
            -24, -20,  10,   9,  -1,  -9, -19, -41,
            -17,   3,  22,  22,  22,  11,   8, -18,
            -18,  -6,  16,  25,  16,  17,   4, -18,
            -23,  -3,  -1,  15,  10,  -3, -20, -22,
            -42, -20, -10,  -5,  -2, -20, -23, -44,
            -29, -51, -23, -15, -22, -18, -50, -64,
    };

     int midgameBishopTable[64] = {
            -29,   4, -82, -37, -25, -42,   7,  -8,
            -26,  16, -18, -13,  30,  59,  18, -47,
            -16,  37,  43,  40,  35,  50,  37,  -2,
            -4,   5,  19,  50,  37,  37,   7,  -2,
            -6,  13,  13,  26,  34,  12,  10,   4,
            0,  15,  15,  15,  14,  27,  18,  10,
            4,  15,  16,   0,   7,  21,  33,   1,
            -33,  -3, -14, -21, -13, -12, -39, -21,
    };

     int endgameBishopTable[64] = {
            -14, -21, -11,  -8, -7,  -9, -17, -24,
            -8,  -4,   7, -12, -3, -13,  -4, -14,
            2,  -8,   0,  -1, -2,   6,   0,   4,
            -3,   9,  12,   9, 14,  10,   3,   2,
            -6,   3,  13,  19,  7,  10,  -3,  -9,
            -12,  -3,   8,  10, 13,   3,  -7, -15,
            -14, -18,  -7,  -1,  4,  -9, -15, -27,
            -23,  -9, -23,  -5, -9, -16,  -5, -17,
    };

     int midgameRookTable[64] = {
            32,  42,  32,  51, 63,  9,  31,  43,
            27,  32,  58,  62, 80, 67,  26,  44,
            -5,  19,  26,  36, 17, 45,  61,  16,
            -24, -11,   7,  26, 24, 35,  -8, -20,
            -36, -26, -12,  -1,  9, -7,   6, -23,
            -45, -25, -16, -17,  3,  0,  -5, -33,
            -44, -16, -20,  -9, -1, 11,  -6, -71,
            -19, -13,   1,  17, 16,  7, -37, -26,
    };

     int endgameRookTable[64] = {
            13, 10, 18, 15, 12,  12,   8,   5,
            11, 13, 13, 11, -3,   3,   8,   3,
            7,  7,  7,  5,  4,  -3,  -5,  -3,
            4,  3, 13,  1,  2,   1,  -1,   2,
            3,  5,  8,  4, -5,  -6,  -8, -11,
            -4,  0, -5, -1, -7, -12,  -8, -16,
            -6, -6,  0,  2, -9,  -9, -11,  -3,
            -9,  2,  3, -1, -5, -13,   4, -20,
    };


     int midgameQueenTable[64] = {
            -28,   0,  29,  12,  59,  44,  43,  45,
            -24, -39,  -5,   1, -16,  57,  28,  54,
            -13, -17,   7,   8,  29,  56,  47,  57,
            -27, -27, -16, -16,  -1,  17,  -2,   1,
            -9, -26,  -9, -10,  -2,  -4,   3,  -3,
            -14,   2, -11,  -2,  -5,   2,  14,   5,
            -35,  -8,  11,   2,   8,  15,  -3,   1,
            -1, -18,  -9,  10, -15, -25, -31, -50,
    };

     int endgameQueenTable[64] = {
            -9,  22,  22,  27,  27,  19,  10,  20,
            -17,  20,  32,  41,  58,  25,  30,   0,
            -20,   6,   9,  49,  47,  35,  19,   9,
            3,  22,  24,  45,  57,  40,  57,  36,
            -18,  28,  19,  47,  31,  34,  39,  23,
            -16, -27,  15,   6,   9,  17,  10,   5,
            -22, -23, -30, -16, -16, -23, -36, -32,
            -33, -28, -22, -43,  -5, -32, -20, -41,
    };

     int midgameKingTable[64] = {
            -65,  23,  16, -15, -56, -34,   2,  13,
            29,  -1, -20,  -7,  -8,  -4, -38, -29,
            -9,  24,   2, -16, -20,   6,  22, -22,
            -17, -20, -12, -27, -30, -25, -14, -36,
            -49,  -1, -27, -39, -46, -44, -33, -51,
            -14, -14, -22, -46, -44, -30, -15, -27,
            1,   7,  -8, -64, -43, -16,   9,   8,
            -15,  36,  12, -54,   8, -28,  24,  14,
    };

     int endgameKingTable[64] = {
            -74, -35, -18, -18, -11,  15,   4, -17,
            -12,  17,  14,  17,  17,  38,  23,  11,
            10,  17,  23,  15,  20,  45,  44,  13,
            -8,  22,  24,  27,  26,  33,  26,   3,
            -18,  -4,  21,  24,  27,  23,   9, -11,
            -19,  -3,  11,  21,  23,  16,   7,  -9,
            -27, -11,   4,  13,  14,   4,  -5, -17,
            -53, -34, -21, -11, -28, -14, -24, -43
    };*/
    
    static int midgamePst[12][64]{};
    static int endgamePst[12][64]{};

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
                        break;
                    case PieceType::BLACK_PAWN:
                        midgamePst[pieceType][square] = midgamePawnTable[square];
                        endgamePst[pieceType][square] = endgamePawnTable[square];
                        break;
                    case PieceType::WHITE_KNIGHT:
                        midgamePst[pieceType][square] = midgameKnightTable[63 - square];
                        endgamePst[pieceType][square] = endgameKnightTable[63 - square];
                        break;
                    case PieceType::BLACK_KNIGHT:
                        midgamePst[pieceType][square] = midgameKnightTable[square];
                        endgamePst[pieceType][square] = endgameKnightTable[square];
                        break;
                    case PieceType::WHITE_BISHOP:
                        midgamePst[pieceType][square] = midgameBishopTable[63 - square];
                        endgamePst[pieceType][square] = endgameBishopTable[63 - square];
                        break;
                    case PieceType::BLACK_BISHOP:
                        midgamePst[pieceType][square] = midgameBishopTable[square];
                        endgamePst[pieceType][square] = endgameBishopTable[square];
                        break;
                    case PieceType::WHITE_ROOK:
                        midgamePst[pieceType][square] = midgameRookTable[63 - square];
                        endgamePst[pieceType][square] = endgameRookTable[63 - square];
                        break;
                    case PieceType::BLACK_ROOK:
                        midgamePst[pieceType][square] = midgameRookTable[square];
                        endgamePst[pieceType][square] = endgameRookTable[square];
                        break;
                    case PieceType::WHITE_QUEEN:
                        midgamePst[pieceType][square] = midgameQueenTable[63 - square];
                        endgamePst[pieceType][square] = endgameQueenTable[63 - square];
                        break;
                    case PieceType::BLACK_QUEEN:
                        midgamePst[pieceType][square] = midgameQueenTable[square];
                        endgamePst[pieceType][square] = endgameQueenTable[square];
                        break;
                    case PieceType::WHITE_KING:
                        midgamePst[pieceType][square] = midgameKingTable[63 - square];
                        endgamePst[pieceType][square] = endgameKingTable[63 - square];
                        break;
                    case PieceType::BLACK_KING:
                        midgamePst[pieceType][square] = midgameKingTable[square];
                        endgamePst[pieceType][square] = endgameKingTable[square];
                        break;
                }
            }
        }
    }
}