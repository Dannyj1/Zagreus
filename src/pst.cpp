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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 99, 133, 59, 94, 71, 124, 38, -9, -4, 7, 34, 31, 66, 59, 31, -16, -16, 7, 0, 20, 15, 12, 23, -18, -43, -7, -10, 14, 22, 9, 5, -38, -21, -4, -11, -5, 0, 0, 20, -13, -22, 13, -10, -17, -8, 10, 38, -10, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 183, 182, 157, 135, 149, 134, 172, 194, 104, 104, 90, 67, 62, 57, 88, 102, 40, 23, 10, -7, 0, 6, 14, 29, 5, 2, -12, -8, -13, -17, 0, 6, 4, 0, -9, 4, 6, -1, -4, -4, 13, 6, 15, 15, 18, 3, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -167, -89, -33, -49, 58, -95, -15, -110, -70, -42, 83, 38, 26, 66, 12, -19, -47, 56, 38, 65, 87, 131, 69, 42, -8, 15, 18, 45, 31, 64, 16, 23, -19, 7, 17, 17, 22, 15, 18, -7, -29, -2, 13, 18, 22, 19, 14, -22, -27, -51, -12, 2, 12, 19, -9, -12, -107, -24, -54, -33, -22, -26, -19, -22 };
    int endgameKnightTable[64] = { -56, -38, -12, -28, -34, -29, -59, -101, -28, -7, -22, 0, -9, -24, -24, -52, -25, -21, 8, 11, 2, -4, -19, -41, -15, 1, 20, 20, 24, 18, 8, -18, -16, -5, 16, 26, 13, 15, 2, -17, -27, -2, -10, 10, 11, -9, -22, -28, -39, -15, -11, -1, 2, -16, -25, -44, -26, -39, -17, -16, -21, -18, -47, -61 };

    int midgameBishopTable[64] = { -30, 5, -79, -35, -25, -44, 5, -5, -20, 14, -16, -12, 33, 54, 21, -43, -13, 36, 46, 37, 32, 57, 35, 8, -5, 0, 16, 43, 31, 35, 8, -1, -4, 12, 8, 22, 37, 2, 9, 0, 4, 19, 9, 6, 5, 20, 16, 13, 6, 29, 12, 9, 11, 22, 44, 3, -34, -7, -9, -15, -4, -1, -33, -21 };
    int endgameBishopTable[64] = { -17, -15, -8, -6, -5, -10, -18, -23, -7, 0, 8, -9, 0, -19, -3, -12, 7, -10, 0, -3, 0, 10, -4, 8, 0, 11, 10, 5, 9, 7, 0, 7, -6, 0, 8, 13, 0, 7, -2, -6, -12, 0, 4, 5, 16, 0, -8, -13, -12, -11, -10, 0, 7, -8, -10, -29, -23, -12, -14, -3, -7, -10, -1, -12 };

    int midgameRookTable[64] = { 33, 41, 30, 46, 64, 10, 30, 44, 24, 32, 57, 60, 78, 67, 22, 39, -3, 18, 28, 35, 20, 47, 60, 16, -21, -7, 3, 25, 22, 34, -10, -16, -30, -26, -16, -5, 9, -8, 4, -22, -43, -30, -14, -21, 0, -5, -6, -38, -48, -17, -25, -10, -3, 7, -9, -65, -15, -13, 7, 16, 9, 12, -26, -23 };
    int endgameRookTable[64] = { 11, 12, 10, 9, 9, 8, 6, 9, 10, 12, 10, 5, 0, 7, 7, 0, 9, 6, 8, 4, 6, 0, -4, -3, 11, 8, 12, 0, 2, 3, 0, 8, 6, 7, 3, 2, -5, -6, -4, -11, -1, 1, 0, 0, -8, -15, -9, -18, -3, -4, -1, 0, -7, -8, -9, -3, -8, 4, 3, 0, 1, -5, 10, -19 };

    int midgameQueenTable[64] = { -20, 0, 28, 13, 59, 42, 41, 43, -19, -38, -2, -1, -21, 52, 19, 46, -8, -15, 8, 7, 32, 50, 45, 56, -20, -33, -18, -11, -1, 17, -1, 5, -14, -23, -17, -17, -6, -6, 3, -3, -20, 0, 0, -6, -4, -4, 19, 3, -32, -4, 9, 0, 7, 8, 2, 1, 1, -17, -12, 0, 4, -21, -34, -48 };
    int endgameQueenTable[64] = { -4, 25, 22, 28, 27, 18, 9, 19, -13, 21, 36, 44, 54, 20, 25, -4, -11, 3, 11, 49, 49, 34, 17, 9, 4, 22, 23, 41, 54, 35, 57, 37, -20, 33, 17, 41, 25, 36, 37, 21, -14, -24, 16, 6, 9, 20, 13, 3, -22, -19, -29, -18, -16, -26, -29, -30, -28, -26, -20, -40, 3, -32, -19, -39 };

    int midgameKingTable[64] = { -64, 23, 16, -16, -55, -32, 1, 12, 27, 1, -19, -7, -7, -3, -37, -28, -7, 24, 2, -16, -21, 8, 21, -20, -18, -21, -14, -27, -33, -21, -14, -37, -50, 0, -26, -33, -48, -43, -30, -50, -13, -12, -22, -43, -48, -28, -12, -27, 6, 10, -13, -70, -52, -18, 6, 12, -9, 36, 2, -32, -10, -30, 25, 13 };
    int endgameKingTable[64] = { -72, -35, -18, -18, -9, 16, 4, -19, -9, 24, 14, 14, 12, 40, 20, 14, 11, 18, 23, 17, 23, 48, 44, 14, -7, 23, 21, 32, 20, 39, 26, 2, -20, -4, 22, 27, 25, 29, 8, -10, -19, 0, 14, 27, 21, 11, 10, -7, -26, -6, 5, 13, 10, 3, -4, -17, -52, -37, -27, -7, -37, -12, -27, -47 };



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