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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 98, 130, 60, 94, 69, 126, 40, -9, -3, 10, 26, 28, 61, 61, 28, -14, -16, 9, 0, 22, 18, 11, 20, -20, -42, -13, -11, 15, 21, 7, 11, -33, -25, 3, -12, -5, 2, -1, 17, -15, -22, 10, -10, -17, -12, 11, 38, -14, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 185, 175, 158, 133, 151, 132, 171, 196, 102, 107, 85, 63, 56, 58, 83, 96, 37, 26, 12, -4, -5, 5, 22, 31, 12, 3, -13, -10, -12, -13, 0, 1, 2, -1, -10, 3, -1, -5, -3, -6, 16, 12, 11, 14, 12, 3, 1, -2, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -167, -89, -36, -47, 61, -94, -14, -109, -68, -38, 70, 39, 25, 65, 6, -18, -44, 59, 40, 68, 88, 125, 71, 42, -7, 12, 22, 49, 31, 64, 12, 24, -17, 6, 17, 16, 25, 20, 20, -15, -27, -2, 15, 17, 19, 13, 23, -17, -24, -48, -13, 5, 14, 15, -14, -15, -103, -24, -56, -31, -19, -29, -17, -23 };
    int endgameKnightTable[64] = { -57, -39, -11, -26, -27, -25, -61, -100, -24, -6, -25, 0, -8, -25, -23, -49, -23, -24, 12, 8, 2, -7, -20, -43, -16, 5, 25, 19, 20, 8, 6, -17, -20, -3, 18, 23, 15, 16, 0, -21, -24, -1, -1, 15, 8, -7, -22, -20, -40, -18, -12, -4, -3, -16, -22, -42, -29, -43, -24, -12, -22, -18, -46, -65 };

    int midgameBishopTable[64] = { -27, 3, -79, -35, -25, -43, 3, -5, -29, 15, -17, -12, 31, 57, 15, -41, -9, 35, 43, 38, 34, 54, 38, 1, 0, 0, 13, 43, 33, 33, 0, -3, -7, 17, 1, 27, 32, 0, 8, 0, 2, 8, 17, 4, 6, 27, 14, 14, 5, 34, 16, 8, 13, 22, 47, 4, -29, -4, -6, -21, -12, 1, -38, -25 };
    int endgameBishopTable[64] = { -11, -18, -7, -4, -7, -9, -19, -21, -11, -3, 3, -11, -3, -12, -2, -13, 4, -3, -1, -3, -3, 7, 2, 4, 0, 13, 12, 3, 13, 7, 2, 3, -9, 3, 10, 18, 2, 8, -6, -13, -9, -6, 6, 9, 13, 3, -4, -12, -13, -16, -6, 0, 8, -9, -10, -27, -22, -10, -13, -1, -7, -13, -5, -14 };

    int midgameRookTable[64] = { 32, 40, 27, 53, 62, 11, 31, 41, 25, 27, 57, 62, 77, 65, 26, 45, -1, 17, 23, 35, 20, 44, 58, 16, -26, -11, 3, 21, 24, 33, -10, -15, -38, -26, -15, -2, 12, -3, 3, -24, -45, -25, -15, -12, 5, 0, -7, -36, -45, -16, -18, -9, 0, 6, -4, -67, -14, -11, 0, 18, 16, 9, -23, -24 };
    int endgameRookTable[64] = { 10, 7, 12, 13, 8, 11, 7, 2, 10, 12, 11, 10, -8, 9, 8, 2, 10, 8, 6, 5, 6, 0, -6, 0, 4, 2, 11, -2, 1, 1, 0, 6, 2, 5, 8, 0, -1, 0, -10, -8, -1, -1, -2, -3, -7, -12, -9, -17, -4, -7, 0, 4, -4, -8, -7, 1, -3, 2, 5, -2, -5, -8, 8, -14 };

    int midgameQueenTable[64] = { -24, 0, 28, 10, 61, 44, 42, 42, -22, -42, -4, -4, -14, 59, 20, 53, -10, -15, 9, 7, 31, 55, 43, 53, -24, -19, -11, -14, -8, 20, -2, 0, -14, -20, -10, -17, -7, -10, 2, -8, -12, 3, -9, 0, -8, 0, 12, 3, -31, -9, 6, 1, 4, 13, 1, 3, 0, -15, -14, 3, 2, -22, -30, -42 };
    int endgameQueenTable[64] = { -4, 22, 22, 25, 30, 17, 9, 18, -15, 18, 33, 39, 57, 24, 30, 0, -15, 6, 9, 50, 46, 33, 17, 5, 3, 23, 26, 41, 53, 39, 58, 37, -17, 28, 19, 43, 29, 32, 38, 23, -17, -26, 13, 2, 6, 15, 7, 3, -20, -22, -26, -15, -19, -24, -34, -28, -32, -26, -20, -38, 4, -28, -17, -39 };

    int midgameKingTable[64] = { -65, 23, 16, -15, -55, -34, 1, 13, 29, 0, -18, -5, -7, -3, -36, -28, -8, 25, 3, -15, -17, 5, 20, -22, -16, -18, -11, -26, -30, -24, -14, -36, -47, 0, -26, -40, -45, -44, -31, -50, -11, -12, -20, -48, -40, -29, -14, -27, 6, 6, -12, -68, -42, -19, 10, 6, -7, 31, 2, -32, -7, -29, 26, 10 };
    int endgameKingTable[64] = { -73, -33, -17, -18, -10, 14, 4, -16, -11, 17, 18, 17, 18, 39, 24, 11, 9, 19, 25, 18, 23, 41, 45, 14, -9, 25, 23, 30, 24, 35, 26, 2, -16, -3, 19, 23, 31, 26, 12, -9, -19, -3, 11, 20, 23, 14, 7, -8, -26, -11, 3, 16, 12, 7, -7, -21, -50, -39, -22, -3, -31, -16, -31, -51 };

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