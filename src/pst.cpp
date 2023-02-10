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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 98, 130, 59, 98, 66, 124, 34, -7, -2, 9, 28, 32, 62, 60, 31, -16, -12, 11, -1, 19, 19, 17, 22, -16, -41, -10, -11, 16, 23, 11, 10, -31, -26, 0, -9, -8, 0, 0, 17, -18, -23, 12, -6, -16, -10, 7, 38, -13, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 185, 175, 159, 136, 145, 130, 173, 191, 104, 106, 86, 70, 60, 62, 91, 89, 35, 23, 5, -4, -8, 5, 24, 27, 6, 1, -7, -14, -9, -13, 4, 5, 2, 3, -11, 0, 0, -4, -3, 0, 14, 11, 11, 15, 14, 4, 0, -6, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -89, -34, -48, 59, -95, -15, -108, -72, -37, 76, 36, 23, 61, 2, -17, -48, 59, 38, 67, 86, 128, 74, 45, -8, 11, 18, 46, 38, 60, 19, 24, -13, 6, 16, 19, 31, 19, 22, -9, -22, -8, 18, 10, 18, 13, 18, -13, -25, -53, -13, 2, 7, 17, -12, -12, -107, -24, -54, -27, -17, -25, -19, -23 };
    int endgameKnightTable[64] = { -59, -38, -13, -29, -33, -26, -64, -97, -27, -8, -22, -1, -11, -27, -24, -52, -22, -18, 10, 13, 0, -6, -20, -41, -16, 9, 19, 21, 22, 10, 4, -18, -19, -6, 17, 23, 18, 15, 4, -15, -26, -4, -6, 15, 12, -7, -21, -25, -40, -20, -11, 0, 0, -19, -22, -41, -30, -46, -20, -13, -21, -16, -48, -65 };

    int midgameBishopTable[64] = { -26, 4, -81, -36, -26, -42, 4, -7, -25, 12, -15, -9, 27, 57, 19, -42, -10, 35, 40, 37, 37, 53, 35, 2, -1, 0, 20, 46, 33, 35, 1, 0, -8, 11, 4, 27, 32, 8, 8, 1, 0, 20, 17, 4, 7, 21, 16, 12, 3, 28, 17, 6, 12, 22, 39, 0, -32, -6, -6, -19, -11, 0, -38, -17 };
    int endgameBishopTable[64] = { -11, -17, -12, -6, -3, -9, -19, -23, -7, -3, 6, -10, -4, -14, -3, -12, 6, -9, 0, 0, 0, 6, 0, 10, 0, 6, 9, 9, 11, 6, 1, 1, -5, 3, 10, 16, 5, 5, -2, -10, -11, -2, 6, 3, 11, 4, -7, -15, -11, -15, -7, 1, 11, -7, -13, -29, -23, -9, -16, 0, -10, -13, -3, -14 };

    int midgameRookTable[64] = { 31, 42, 30, 48, 59, 10, 29, 44, 27, 30, 59, 61, 79, 61, 31, 42, -5, 19, 27, 36, 17, 47, 60, 14, -20, -10, 6, 25, 21, 34, -8, -19, -34, -25, -12, -2, 8, -6, 6, -22, -45, -21, -15, -17, 5, 0, -1, -35, -49, -18, -17, -10, 0, 12, -6, -66, -15, -19, 4, 15, 17, 6, -30, -25 };
    int endgameRookTable[64] = { 14, 9, 13, 12, 5, 13, 4, 4, 10, 9, 13, 9, 0, 0, 8, 1, 7, 8, 8, 4, 5, 0, -5, -1, 5, 4, 13, 2, 0, 6, -3, 6, 5, 4, 7, 2, -5, -6, -6, -8, -2, 2, -3, 0, -9, -15, -4, -14, -3, -6, 1, -1, -4, -7, -10, -2, -2, 0, 0, 0, -4, -6, 9, -18 };

    int midgameQueenTable[64] = { -25, -1, 28, 11, 58, 43, 43, 44, -20, -36, -8, -1, -17, 57, 22, 49, -10, -12, 9, 9, 26, 55, 42, 57, -25, -24, -15, -13, -2, 18, 0, 0, -14, -22, -16, -12, 0, -2, 1, -5, -13, 0, -8, -4, -2, 0, 10, 4, -33, -1, 6, -5, 2, 14, -2, 0, -4, -15, -11, 2, 3, -28, -32, -44 };
    int endgameQueenTable[64] = { -4, 20, 20, 25, 29, 18, 10, 20, -11, 19, 32, 38, 58, 24, 26, 0, -17, 8, 10, 47, 47, 31, 17, 7, 5, 23, 23, 45, 52, 38, 56, 37, -17, 27, 15, 44, 30, 36, 35, 23, -14, -26, 15, 6, 12, 19, 6, 5, -19, -21, -33, -16, -12, -24, -34, -30, -35, -25, -22, -40, 0, -32, -18, -36 };

    int midgameKingTable[64] = { -65, 22, 16, -14, -56, -34, 3, 12, 29, -1, -19, -7, -7, -4, -37, -29, -9, 24, 2, -16, -20, 6, 25, -22, -17, -19, -13, -25, -32, -24, -14, -34, -49, -1, -28, -37, -44, -40, -31, -50, -15, -12, -22, -46, -44, -29, -15, -28, 5, 6, -11, -65, -47, -15, 11, 12, -5, 32, 1, -31, -13, -30, 24, 19 };
    int endgameKingTable[64] = { -74, -34, -17, -16, -11, 13, 6, -17, -12, 19, 15, 18, 15, 35, 25, 11, 10, 16, 24, 11, 21, 43, 49, 13, -7, 22, 23, 27, 21, 33, 29, 4, -19, -1, 18, 22, 31, 32, 11, -9, -21, -2, 13, 20, 23, 19, 6, -11, -26, -8, 0, 18, 16, 7, -6, -17, -48, -43, -25, -10, -34, -17, -25, -46 };

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