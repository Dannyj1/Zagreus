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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 100, 130, 65, 93, 68, 124, 35, -9, 1, 9, 27, 33, 60, 61, 26, -13, -17, 2, -2, 17, 23, 16, 18, -15, -37, -8, -20, 12, 8, 8, 14, -27, -26, 0, -13, -6, 3, 5, 18, -18, -20, 6, 0, -12, -6, 6, 37, -15, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 183, 175, 164, 137, 151, 132, 168, 190, 98, 105, 85, 72, 63, 55, 83, 98, 33, 22, 8, 1, -1, 9, 23, 24, 6, 8, -10, -9, -5, -8, 7, 9, 7, 0, -5, 5, 5, 1, 2, 0, 13, 10, 15, 17, 9, 6, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -165, -87, -31, -45, 59, -94, -13, -102, -71, -40, 77, 38, 26, 65, 7, -15, -46, 59, 37, 65, 91, 130, 76, 44, -5, 13, 22, 47, 32, 61, 20, 24, -10, 4, 12, 13, 32, 19, 23, -5, -23, -8, 16, 17, 14, 4, 19, -15, -21, -47, -9, 5, 8, 19, -12, -12, -104, -31, -55, -35, -17, -28, -13, -22 };
    int endgameKnightTable[64] = { -58, -35, -10, -26, -29, -25, -63, -96, -24, -4, -24, 0, -2, -26, -22, -49, -24, -18, 8, 8, 2, -6, -20, -37, -13, 3, 24, 21, 26, 8, 8, -17, -14, -6, 13, 27, 19, 19, 4, -18, -26, -2, -12, 13, 7, -10, -23, -23, -37, -17, -9, -4, 4, -19, -22, -42, -30, -50, -22, -16, -21, -19, -44, -63 };

    int midgameBishopTable[64] = { -25, 3, -80, -34, -24, -41, 4, -1, -26, 15, -12, -9, 32, 56, 21, -42, -10, 35, 46, 39, 36, 51, 38, 0, -6, 3, 22, 50, 38, 34, 3, 3, 1, 15, 5, 28, 29, 3, 11, 3, 0, 19, 15, 11, 8, 23, 15, 13, 6, 32, 16, 6, 8, 22, 42, 5, -29, -8, -8, -20, -10, -3, -39, -22 };
    int endgameBishopTable[64] = { -14, -20, -6, -6, -4, -5, -17, -18, -5, -4, 6, -8, 1, -11, -3, -12, 1, -7, 1, 0, 0, 10, 0, 1, -4, 7, 12, 2, 10, 7, 5, 4, -3, 3, 14, 15, 9, 5, -1, -5, -9, 0, 12, 11, 9, 1, -7, -13, -10, -5, -3, 4, 6, -6, -10, -24, -18, -9, -11, 0, 0, -11, -3, -17 };

    int midgameRookTable[64] = { 35, 39, 28, 47, 64, 9, 28, 44, 24, 28, 59, 58, 82, 68, 29, 41, -1, 21, 26, 36, 20, 43, 61, 23, -23, -10, 14, 29, 24, 39, -6, -17, -39, -26, -12, -1, 7, -8, 5, -24, -47, -23, -13, -11, 5, 3, -4, -29, -40, -17, -20, -8, -1, 14, -3, -68, -10, -18, 1, 11, 19, 14, -30, -29 };
    int endgameRookTable[64] = { 16, 8, 14, 10, 11, 10, 0, 3, 10, 9, 10, 7, 0, 4, 13, 0, 8, 9, 6, 4, 7, -4, -2, 4, 10, 3, 12, 2, 6, 4, 0, 11, 3, 9, 10, 4, -1, -3, -5, -7, -2, 3, 0, 1, 0, -2, -2, -15, -2, 0, 1, 3, -5, -5, -12, 0, 0, -1, 2, 0, 0, -7, 8, -12 };

    int midgameQueenTable[64] = { -18, 0, 30, 13, 61, 44, 39, 47, -21, -41, -5, 1, -16, 57, 26, 50, -6, -16, 12, 7, 25, 53, 46, 54, -19, -27, -15, -16, 0, 14, 2, 1, -7, -22, -11, -14, -1, 0, 5, -9, -16, -5, -9, -11, -11, -4, 15, 2, -32, -9, 9, 5, 11, 15, -2, 2, 0, -16, -5, 7, 12, -18, -28, -43 };
    int endgameQueenTable[64] = { -2, 23, 24, 30, 30, 20, 9, 22, -9, 21, 33, 44, 57, 26, 32, 1, -12, 4, 13, 51, 45, 33, 19, 9, 6, 25, 23, 43, 59, 32, 58, 38, -10, 28, 17, 44, 31, 34, 40, 22, -16, -22, 16, 0, 7, 12, 13, 7, -19, -26, -26, -14, -12, -23, -33, -27, -30, -28, -16, -34, 7, -25, -13, -37 };

    int midgameKingTable[64] = { -63, 23, 16, -14, -55, -32, 1, 12, 30, 0, -19, -5, -5, -3, -36, -28, -9, 26, 3, -14, -18, 6, 23, -20, -16, -18, -8, -27, -30, -18, -11, -36, -49, 0, -29, -35, -40, -42, -33, -49, -15, -20, -23, -43, -44, -24, -10, -24, 0, 4, -10, -70, -47, -11, 7, 9, -7, 34, -2, -32, -13, -23, 26, 7 };
    int endgameKingTable[64] = { -72, -35, -17, -19, -9, 16, 4, -15, -10, 22, 12, 16, 17, 41, 27, 12, 6, 21, 21, 14, 23, 50, 46, 11, -7, 21, 25, 29, 20, 35, 25, 6, -17, -5, 19, 25, 28, 27, 12, -5, -21, -6, 8, 18, 25, 18, 7, -9, -31, -15, 0, 12, 9, 7, -6, -10, -54, -37, -21, -9, -37, -9, -24, -51 };

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