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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 97, 129, 61, 95, 67, 125, 31, -13, 0, 12, 29, 35, 62, 57, 28, -11, -12, 9, 4, 22, 21, 11, 18, -13, -40, -15, -13, 12, 23, 6, 10, -37, -27, 0, -14, -11, 5, 0, 26, -12, -26, 9, -12, -11, -8, 18, 38, -14, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 183, 177, 160, 135, 151, 138, 171, 192, 107, 107, 92, 77, 58, 60, 90, 99, 33, 23, 18, 9, -4, 10, 24, 26, 4, 2, -11, -10, -10, -11, -2, 0, 5, 0, -12, 2, 1, -4, 0, -11, 14, 10, 15, 19, 10, 5, 3, 6, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -165, -87, -32, -48, 59, -99, -16, -107, -74, -41, 74, 35, 16, 59, 0, -18, -50, 58, 37, 60, 89, 130, 71, 47, -8, 15, 21, 49, 39, 69, 16, 24, -10, 7, 15, 15, 30, 16, 24, -6, -29, -8, 18, 0, 20, 17, 23, -18, -22, -48, -18, 1, 10, 15, -14, -16, -106, -24, -62, -33, -20, -31, -13, -25 };
    int endgameKnightTable[64] = { -58, -34, -14, -26, -33, -29, -62, -99, -28, -9, -22, -6, -12, -27, -25, -50, -27, -25, 12, 6, 1, -4, -21, -39, -18, 3, 21, 18, 24, 13, 5, -11, -14, -6, 15, 34, 15, 11, 2, -19, -26, -1, -2, 16, 9, -3, -19, -29, -43, -19, -11, -1, 3, -27, -23, -43, -33, -51, -23, -12, -21, -19, -50, -64 };

    int midgameBishopTable[64] = { -21, 4, -78, -32, -22, -39, 1, -1, -24, 15, -16, -11, 30, 57, 20, -43, -14, 36, 51, 41, 37, 47, 42, -3, -6, -9, 21, 42, 38, 32, 0, -3, -3, 10, 14, 29, 31, 7, 8, -1, 0, 18, 17, 12, 9, 22, 13, 15, 3, 31, 17, 2, 11, 19, 38, 2, -32, -5, -8, -19, -12, 0, -39, -17 };
    int endgameBishopTable[64] = { -9, -22, -8, -1, -2, -8, -20, -20, -9, -3, 5, -11, -6, -16, -4, -12, 7, -2, 0, 0, -6, 4, -1, 3, -6, 4, 6, 11, 13, 10, 3, 4, -3, 0, 13, 19, 5, 13, -2, -9, -8, -9, 6, 12, 12, 1, -12, -16, -10, -10, -6, -2, 1, -9, -10, -26, -22, -7, -14, -2, -6, -4, -5, -18 };

    int midgameRookTable[64] = { 32, 42, 26, 47, 58, 6, 30, 44, 26, 32, 59, 57, 74, 75, 30, 44, -6, 15, 29, 38, 20, 53, 61, 17, -21, -11, 7, 28, 23, 34, -3, -20, -37, -27, -16, 4, 11, -4, 4, -22, -42, -26, -16, -13, 2, 0, -7, -37, -46, -14, -25, -9, -12, 11, -4, -64, -16, -17, 11, 6, 17, 6, -30, -20 };
    int endgameRookTable[64] = { 11, 8, 16, 13, 8, 1, 5, 10, 8, 9, 11, 9, -5, 9, 10, 2, 10, 2, 3, 4, 4, 1, -2, 0, 10, 3, 11, 0, 2, 0, 2, 0, 8, 7, 3, 7, -4, 0, -9, -8, 0, 2, -5, -5, -4, -11, -6, -17, -7, 0, -5, 4, -13, -13, -12, 9, 0, 4, 8, -1, 1, -5, 4, -14 };

    int midgameQueenTable[64] = { -25, 3, 32, 14, 58, 44, 43, 41, -25, -46, -6, 4, -23, 52, 22, 45, -10, -12, 8, 10, 31, 48, 47, 59, -20, -20, -15, -11, -3, 15, -3, 0, -12, -28, -6, -7, -8, -12, 6, -8, -15, 2, -7, -5, -6, -1, 12, 1, -38, -6, 9, 3, 4, 11, 3, 5, -2, -16, -15, 2, 0, -19, -34, -47 };
    int endgameQueenTable[64] = { -5, 22, 24, 27, 28, 16, 11, 19, -13, 15, 29, 44, 54, 22, 25, 0, -19, 5, 10, 51, 47, 32, 17, 11, 4, 28, 25, 46, 54, 37, 60, 36, -15, 36, 20, 49, 32, 29, 39, 27, -17, -30, 20, 0, 5, 19, 7, 2, -21, -22, -30, -19, -17, -20, -37, -30, -32, -25, -19, -42, 5, -29, -20, -36 };

    int midgameKingTable[64] = { -65, 22, 17, -13, -52, -36, 1, 13, 27, 0, -19, -6, -8, -3, -34, -26, -10, 23, 1, -12, -21, 9, 24, -24, -15, -21, -8, -26, -30, -22, -16, -34, -46, 0, -30, -38, -44, -40, -27, -50, -11, -11, -23, -43, -44, -30, -12, -26, 4, 5, -7, -69, -56, -16, 7, 11, -15, 41, 1, -41, 0, -31, 22, 7 };
    int endgameKingTable[64] = { -73, -34, -16, -17, -9, 11, 4, -15, -12, 19, 16, 16, 14, 40, 28, 11, 12, 15, 25, 20, 25, 50, 51, 8, -7, 24, 25, 22, 29, 33, 27, 1, -15, -6, 19, 26, 29, 29, 10, -3, -21, -2, 12, 26, 22, 16, 12, -12, -30, -17, 2, 13, 13, 5, -5, -17, -57, -33, -28, -8, -34, -18, -30, -50 };


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