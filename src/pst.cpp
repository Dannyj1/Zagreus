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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 98, 133, 62, 95, 68, 126, 32, -9, 0, 2, 23, 25, 62, 55, 16, -11, -15, 11, 2, 16, 19, 16, 9, -19, -28, 0, -11, 9, 19, 9, 10, -24, -30, 1, -4, 6, 4, -6, 31, -7, -35, -6, -12, -23, -16, 16, 36, -23, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 179, 174, 159, 138, 149, 131, 164, 188, 91, 98, 78, 63, 60, 57, 84, 87, 23, 23, 17, 5, -3, 10, 17, 23, 15, 3, -1, -5, -3, -6, 4, 4, 6, 0, -7, 5, 0, 0, -9, 0, 5, 1, 9, 12, 6, -3, -3, -8, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -168, -89, -33, -49, 58, -99, -15, -106, -72, -41, 76, 35, 22, 61, 10, -15, -47, 62, 31, 62, 82, 121, 67, 44, -10, 19, 20, 51, 32, 71, 13, 26, -9, 0, 18, 16, 24, 17, 24, -7, -25, -13, 6, 12, 17, 17, 25, -17, -28, -49, -17, -3, -1, 21, -15, -18, -104, -25, -58, -35, -22, -27, -21, -22 };
    int endgameKnightTable[64] = { -58, -38, -14, -32, -32, -26, -62, -99, -23, -8, -24, -2, -8, -25, -21, -48, -24, -18, 9, 14, 1, -9, -25, -39, -19, 4, 20, 23, 18, 9, 12, -13, -19, -10, 8, 22, 12, 18, 7, -18, -30, -2, 0, 10, 2, -3, -13, -25, -41, -17, -11, -2, -3, -23, -24, -42, -27, -55, -26, -18, -21, -19, -48, -64 };

    int midgameBishopTable[64] = { -31, 4, -84, -36, -25, -43, 6, -3, -26, 14, -18, -14, 28, 55, 15, -51, -12, 37, 35, 36, 30, 44, 36, 0, -7, 2, 19, 52, 29, 35, 3, -4, -5, 14, 11, 27, 30, 9, 13, 0, -2, 18, 12, 17, 14, 24, 19, 10, 0, 22, 12, 0, 9, 22, 33, -1, -31, -2, -13, -23, -13, -9, -39, -21 };
    int endgameBishopTable[64] = { -14, -17, -12, -6, -5, -10, -14, -17, -10, -4, 6, -16, -6, -13, -7, -19, 0, -10, -10, -5, 0, -2, 0, 7, -4, 10, 6, 8, 6, 4, 0, 3, -4, 0, 9, 20, 3, 11, 0, -10, -14, -1, 4, 13, 18, -1, -5, -11, -15, -14, -5, -8, 12, -10, -15, -24, -21, -6, -21, -7, -9, -17, -7, -18 };

    int midgameRookTable[64] = { 32, 45, 37, 48, 61, 9, 27, 41, 29, 27, 54, 60, 84, 64, 26, 47, -3, 20, 25, 32, 15, 46, 60, 23, -22, -7, 1, 23, 29, 34, -6, -14, -38, -25, -5, -7, 12, -6, 4, -24, -47, -25, -20, -16, 4, 0, -4, -31, -41, -14, -27, -10, -4, 13, -4, -70, -14, -9, -1, 7, 17, 6, -36, -23 };
    int endgameRookTable[64] = { 10, 17, 16, 12, 6, 11, 4, 10, 13, 9, 12, 5, -4, 4, 7, 0, 7, 8, 5, 2, 3, -1, -6, -5, 3, 0, 8, 0, 2, 1, 3, 0, 1, 2, 9, 2, 0, -6, -9, -14, -7, 5, -4, 3, -5, -7, -7, -10, -5, -9, -5, 2, -15, -5, -10, -4, -5, 0, 4, -10, -2, -12, 9, -16 };

    int midgameQueenTable[64] = { -31, 0, 33, 13, 59, 44, 40, 41, -21, -43, -5, 0, -10, 54, 28, 49, -10, -14, 2, 12, 23, 49, 41, 52, -28, -25, -12, -19, 1, 22, 0, 5, -9, -25, -7, -8, -9, -5, 0, 0, -16, 1, -4, -2, -7, -1, 16, 8, -32, -5, 9, -2, 3, 17, -1, 2, -1, -19, -9, 9, -5, -28, -32, -49 };
    int endgameQueenTable[64] = { -12, 22, 23, 28, 27, 20, 8, 18, -14, 18, 32, 38, 60, 24, 25, 0, -16, 6, 5, 54, 44, 36, 16, 6, 2, 24, 27, 45, 51, 42, 59, 37, -14, 28, 19, 51, 26, 31, 38, 27, -15, -27, 21, 2, 6, 12, 15, 5, -22, -21, -33, -19, -15, -21, -35, -30, -32, -30, -22, -45, 0, -31, -21, -40 };

    int midgameKingTable[64] = { -65, 23, 16, -14, -55, -34, 4, 13, 28, 0, -19, -9, -7, -2, -35, -26, -11, 20, 0, -18, -21, 7, 20, -23, -20, -21, -11, -24, -30, -26, -12, -36, -48, 0, -29, -43, -45, -37, -33, -49, -17, -12, -21, -48, -37, -30, -15, -24, 0, 7, -3, -58, -39, -21, 10, 7, -9, 32, 16, -44, 2, -16, 16, 16 };
    int endgameKingTable[64] = { -74, -33, -18, -16, -11, 15, 7, -14, -11, 19, 17, 16, 16, 39, 26, 12, 2, 15, 22, 15, 13, 42, 36, 10, -8, 12, 26, 26, 29, 34, 23, 4, -17, -3, 17, 21, 28, 23, 11, -13, -22, -1, 11, 18, 26, 7, 10, -4, -29, -6, 6, 17, 16, 1, -2, -16, -55, -22, -18, -2, -36, -3, -37, -42 };



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