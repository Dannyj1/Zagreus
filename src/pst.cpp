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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 96, 130, 63, 93, 70, 123, 39, -9, -6, 4, 24, 30, 62, 59, 27, -15, -17, 9, 1, 17, 19, 15, 23, -20, -35, -10, -10, 14, 25, 10, 11, -29, -21, 0, -10, 0, -3, 2, 20, -12, -26, 4, -10, -11, -14, 6, 37, -19, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 180, 175, 161, 132, 147, 132, 165, 189, 100, 100, 79, 65, 53, 53, 87, 92, 30, 23, 12, 1, 0, 5, 22, 26, 6, 7, -9, -9, -12, -9, 4, 4, 5, 2, -11, 6, 3, -4, -1, -6, 8, 5, 7, 14, 14, 0, 0, -4, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -164, -88, -35, -50, 61, -97, -15, -109, -74, -38, 68, 39, 28, 63, 9, -18, -48, 57, 34, 62, 84, 128, 73, 43, -7, 19, 21, 50, 32, 62, 21, 24, -16, 3, 17, 9, 24, 18, 19, -11, -26, -3, 21, 12, 22, 12, 22, -18, -27, -48, -9, 5, 4, 18, -15, -12, -103, -29, -55, -28, -12, -26, -21, -23 };
    int endgameKnightTable[64] = { -57, -36, -15, -30, -31, -27, -63, -101, -26, -10, -24, 0, -8, -23, -21, -50, -26, -22, 8, 8, -1, -9, -20, -40, -13, 4, 22, 18, 24, 11, 12, -17, -17, -4, 16, 28, 16, 14, 2, -16, -22, -6, -5, 16, 13, -7, -23, -22, -42, -19, -7, -2, 0, -18, -23, -43, -29, -52, -24, -16, -19, -15, -56, -63 };

    int midgameBishopTable[64] = { -29, 3, -79, -37, -25, -43, 2, -7, -29, 14, -16, -11, 30, 55, 19, -44, -9, 37, 46, 42, 34, 54, 34, 2, -4, 6, 18, 47, 32, 35, 1, 0, -3, 13, 5, 24, 26, 9, 15, 1, -4, 19, 13, 12, 6, 23, 22, 11, 4, 24, 16, 5, 7, 25, 40, 0, -31, -2, -14, -18, -6, -3, -37, -18 };
    int endgameBishopTable[64] = { -12, -19, -9, -6, -6, -7, -21, -23, -10, -5, 6, -9, -5, -15, -3, -15, 4, -8, 0, 0, -4, 5, -2, 5, 0, 8, 9, 9, 15, 9, 3, 1, -5, 1, 9, 17, 6, 9, 0, -9, -12, -3, 2, 9, 8, 0, -6, -12, -12, -14, -7, -3, 3, -8, -8, -25, -21, -13, -22, 0, -2, -15, -3, -16 };

    int midgameRookTable[64] = { 31, 42, 30, 51, 60, 10, 31, 41, 28, 29, 59, 58, 79, 67, 27, 41, -3, 21, 27, 35, 17, 45, 60, 18, -22, -10, 6, 26, 25, 33, -4, -18, -39, -25, -14, -5, 8, -7, 7, -26, -45, -26, -16, -19, 5, 0, -6, -34, -47, -15, -16, -11, 0, 12, -6, -70, -13, -11, 0, 8, 16, 6, -30, -25 };
    int endgameRookTable[64] = { 12, 12, 16, 14, 7, 12, 7, 4, 10, 8, 11, 7, -1, 2, 9, 2, 10, 8, 5, 4, 8, 0, -4, -3, 4, 5, 10, 0, 0, 0, 0, 6, 1, 8, 8, 0, -4, -6, -7, -9, -2, 0, -5, 0, -4, -11, -10, -15, -8, -5, 0, 0, -7, -7, -12, 0, -5, 1, 1, -2, -4, -13, 5, -18 };

    int midgameQueenTable[64] = { -26, 0, 28, 13, 58, 43, 41, 44, -22, -37, -4, 0, -16, 58, 25, 50, -8, -17, 12, 6, 30, 54, 44, 57, -19, -28, -18, -17, 0, 18, -5, 5, -15, -21, -13, -7, -9, -6, 3, 0, -17, 0, -10, -8, -4, -6, 11, 2, -29, -6, 7, 0, 4, 22, 0, 2, 0, -15, -9, 0, 0, -20, -32, -44 };
    int endgameQueenTable[64] = { -8, 25, 20, 29, 27, 17, 9, 18, -13, 20, 33, 42, 57, 26, 29, 0, -17, 7, 12, 50, 47, 31, 17, 12, 4, 20, 24, 42, 54, 38, 54, 36, -19, 27, 14, 47, 27, 34, 40, 21, -16, -26, 12, 2, 7, 12, 10, 4, -18, -20, -30, -18, -20, -20, -36, -27, -31, -26, -21, -44, 1, -28, -20, -37 };

    int midgameKingTable[64] = { -64, 22, 15, -14, -55, -33, 2, 13, 29, 0, -20, -6, -8, -3, -37, -28, -7, 25, 3, -14, -19, 5, 21, -22, -15, -19, -10, -27, -29, -24, -13, -35, -49, -3, -27, -39, -46, -42, -33, -53, -12, -15, -24, -44, -42, -29, -14, -26, 1, 10, -15, -64, -42, -16, 13, 10, -4, 31, 1, -32, -8, -24, 23, 14 };
    int endgameKingTable[64] = { -74, -35, -15, -16, -11, 15, 5, -18, -12, 17, 15, 17, 16, 37, 22, 10, 10, 19, 22, 14, 21, 44, 41, 14, -8, 20, 23, 27, 24, 33, 26, 6, -17, -4, 17, 25, 28, 26, 7, -11, -19, -3, 11, 25, 24, 18, 7, -10, -25, -10, 1, 14, 15, 8, 0, -15, -52, -39, -27, -6, -36, -10, -30, -44 };

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