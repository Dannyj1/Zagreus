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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 97, 121, 61, 95, 68, 126, 40, -3, -6, 0, 30, 29, 52, 63, 27, -18, -20, 3, 0, 20, 23, 14, 20, -20, -46, -7, -6, 13, 20, 8, 5, -32, -27, -5, -12, 0, 6, 0, 14, -21, -16, 17, -9, -10, -14, 10, 36, -8, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 185, 179, 162, 138, 154, 133, 176, 194, 108, 107, 85, 72, 50, 59, 87, 106, 43, 21, 15, 0, -7, 4, 18, 35, 11, 3, -13, -13, -15, -11, 3, 6, 1, 0, -8, 0, 1, -8, -4, -4, 9, 11, 14, 15, 11, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -164, -87, -37, -45, 59, -96, -15, -107, -65, -34, 78, 39, 27, 70, 2, -21, -47, 59, 36, 64, 82, 123, 71, 42, -4, 11, 20, 43, 37, 65, 14, 25, -17, 1, 20, 16, 27, 15, 20, -9, -36, -3, 14, 7, 18, 17, 13, -23, -23, -47, -6, 12, 14, 13, -8, -9, -107, -17, -55, -29, -22, -26, -13, -22 };
    int endgameKnightTable[64] = { -55, -33, -15, -19, -33, -27, -64, -95, -23, -6, -19, -2, -8, -18, -22, -54, -23, -23, 11, 5, 5, -9, -17, -39, -16, 4, 20, 19, 27, 14, 6, -14, -17, -6, 16, 27, 17, 13, 3, -21, -28, -14, -12, 9, 10, -13, -16, -24, -43, -16, -9, 9, 0, -23, -22, -41, -33, -45, -19, -16, -19, -14, -46, -62 };

    int midgameBishopTable[64] = { -24, 0, -81, -37, -28, -41, 4, -2, -22, 9, -14, -9, 27, 59, 23, -40, -4, 32, 49, 40, 34, 50, 43, 3, -7, 7, 9, 41, 39, 32, 0, -9, -10, 16, 0, 29, 34, 3, 6, 0, 0, 19, 15, 6, 4, 25, 5, 17, 7, 32, 7, 5, 9, 30, 47, 0, -27, 0, -7, -17, -11, 6, -34, -21 };
    int endgameBishopTable[64] = { -6, -21, -5, -11, -2, -6, -20, -22, -6, -7, 4, -8, -5, -14, -2, -9, 10, -6, 3, -1, 0, 5, 0, 17, -5, 11, 0, 1, 12, 1, 4, -2, -8, 0, 5, 14, 3, 9, -5, -9, -9, 0, 3, 6, 6, -2, -3, -12, -9, -9, -9, 8, 7, -8, -7, -23, -23, -5, -15, -2, 0, -8, 0, -21 };

    int midgameRookTable[64] = { 29, 40, 33, 49, 62, 8, 30, 45, 22, 26, 56, 59, 75, 63, 25, 44, -4, 20, 21, 33, 27, 48, 60, 13, -21, -13, 7, 22, 27, 35, -12, -13, -34, -21, -16, 0, 5, -8, 6, -24, -41, -29, -18, -20, 1, 0, -2, -32, -48, -13, -15, -11, -13, 0, -7, -70, -11, -19, 5, 13, 21, 10, -18, -19 };
    int endgameRookTable[64] = { 11, 10, 12, 17, 4, 4, 4, 7, 10, 10, 15, 6, -10, 0, 9, 6, 13, 10, 3, 1, 9, -5, -5, -1, 9, 5, 13, -1, 4, 0, 0, 10, 1, 7, 6, 0, -7, -6, -4, -5, 0, 0, 1, -6, -8, -14, -4, -13, -3, 0, 0, -1, -11, -8, -11, -1, -5, -4, 0, 4, -3, -5, 13, -7 };

    int midgameQueenTable[64] = { -19, 0, 30, 14, 58, 42, 42, 42, -20, -37, 0, -1, -17, 51, 12, 45, -9, -13, 4, 11, 31, 46, 43, 51, -13, -21, -15, -16, 0, 16, -11, -2, -14, -15, -15, -13, -11, -8, 4, -1, -16, -2, -6, -14, -9, 0, 8, 0, -27, -2, 8, -2, 0, 12, 5, 0, -2, -20, -14, 4, 9, -22, -28, -42 };
    int endgameQueenTable[64] = { -1, 20, 23, 25, 24, 19, 9, 22, -8, 19, 36, 40, 58, 19, 20, 0, -16, 9, 6, 55, 46, 25, 17, 6, 12, 22, 23, 45, 51, 34, 51, 33, -14, 29, 17, 38, 35, 30, 38, 25, -13, -23, 18, 0, 5, 22, 6, 1, -13, -17, -30, -16, -14, -30, -28, -30, -31, -24, -19, -35, 0, -24, -17, -31 };

    int midgameKingTable[64] = { -64, 23, 17, -15, -56, -34, 1, 12, 31, -2, -19, -7, -8, -3, -38, -26, -9, 27, 2, -14, -18, 4, 25, -18, -17, -23, -10, -27, -30, -24, -11, -36, -48, 0, -24, -38, -41, -38, -32, -47, -14, -16, -22, -47, -43, -33, -13, -30, 8, 9, -15, -67, -57, -22, 20, 10, -2, 33, -9, -28, -15, -29, 27, 5 };
    int endgameKingTable[64] = { -77, -34, -17, -18, -10, 14, 7, -20, -11, 17, 10, 18, 17, 40, 24, 9, 6, 19, 22, 14, 21, 50, 55, 17, -3, 22, 24, 27, 28, 38, 35, 7, -21, -3, 25, 26, 30, 20, 3, -7, -26, 1, 18, 24, 17, 12, 10, -11, -19, -3, 3, 16, 9, 4, 2, -15, -56, -44, -34, -10, -30, -14, -32, -50 };

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