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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 98, 134, 61, 94, 68, 126, 34, -10, -6, 7, 26, 31, 65, 56, 25, -19, -13, 13, 6, 21, 23, 12, 16, -23, -26, -1, -5, 12, 16, 5, 9, -25, -25, -3, -4, -9, 2, 3, 32, -12, -34, 0, -20, -22, -15, 23, 38, -22, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 178, 173, 158, 133, 147, 132, 165, 187, 93, 100, 85, 67, 56, 53, 82, 84, 32, 24, 13, 5, -1, 4, 16, 16, 13, 9, -3, -6, -7, -7, 2, -1, 4, 7, -6, 1, 0, -4, -1, -7, 13, 8, 7, 10, 13, 0, 2, -7, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -88, -33, -48, 61, -96, -14, -106, -72, -40, 72, 36, 23, 62, 7, -16, -46, 60, 37, 65, 84, 128, 73, 44, -8, 17, 19, 53, 37, 69, 18, 22, -12, 4, 16, 13, 28, 19, 21, -7, -22, -8, 12, 10, 19, 17, 25, -16, -28, -53, -11, -3, 0, 18, -14, -19, -105, -20, -57, -32, -17, -28, -18, -23 };
    int endgameKnightTable[64] = { -57, -37, -12, -27, -30, -26, -62, -98, -24, -7, -24, -1, -8, -24, -23, -51, -23, -19, 10, 9, 0, -9, -18, -40, -16, 3, 22, 22, 22, 11, 8, -17, -17, -5, 16, 25, 16, 17, 4, -17, -22, -2, 0, 15, 10, -2, -19, -22, -41, -20, -9, -4, -1, -19, -22, -43, -29, -50, -22, -14, -21, -18, -49, -64 };

    int midgameBishopTable[64] = { -28, 4, -81, -36, -24, -41, 7, -7, -25, 16, -17, -12, 30, 59, 18, -46, -15, 37, 43, 40, 35, 50, 37, -1, -3, 4, 19, 50, 37, 37, 7, -1, -5, 13, 13, 26, 34, 12, 10, 4, 0, 15, 15, 15, 14, 27, 18, 10, 4, 15, 16, 0, 7, 21, 33, 1, -32, -3, -13, -20, -12, -11, -38, -20 };
    int endgameBishopTable[64] = { -13, -20, -10, -7, -6, -8, -16, -23, -7, -3, 7, -11, -2, -12, -3, -13, 2, -7, 0, 0, -1, 6, 0, 4, -2, 8, 12, 9, 14, 10, 3, 2, -5, 3, 13, 19, 7, 10, -2, -8, -11, -2, 8, 10, 13, 3, -6, -14, -13, -17, -6, 0, 4, -8, -14, -26, -22, -8, -22, -4, -8, -15, -4, -16 };

    int midgameRookTable[64] = { 32, 42, 32, 51, 63, 9, 31, 43, 27, 32, 58, 62, 80, 67, 26, 44, -4, 19, 26, 36, 17, 45, 61, 16, -23, -10, 7, 26, 24, 35, -7, -19, -35, -25, -11, 0, 9, -6, 6, -22, -44, -24, -15, -16, 3, 0, -4, -32, -44, -15, -19, -8, 0, 11, -5, -70, -18, -12, 1, 17, 16, 7, -36, -25 };
    int endgameRookTable[64] = { 13, 10, 18, 15, 12, 12, 8, 5, 11, 13, 13, 11, -2, 3, 8, 3, 7, 7, 7, 5, 4, -2, -4, -2, 4, 3, 13, 1, 2, 1, 0, 2, 3, 5, 8, 4, -4, -5, -7, -10, -3, 0, -4, 0, -6, -11, -7, -15, -5, -5, 0, 2, -8, -8, -10, -2, -8, 2, 3, 0, -4, -12, 4, -19 };

    int midgameQueenTable[64] = { -27, 0, 28, 12, 59, 44, 42, 45, -23, -38, -4, 1, -15, 57, 28, 54, -12, -16, 7, 8, 28, 56, 47, 56, -26, -26, -15, -15, 0, 17, -1, 1, -8, -25, -8, -9, -1, -3, 3, -2, -13, 2, -10, -1, -4, 2, 14, 5, -34, -7, 11, 2, 8, 14, -2, 1, 0, -17, -8, 10, -14, -24, -30, -49 };
    int endgameQueenTable[64] = { -8, 22, 22, 27, 27, 19, 9, 20, -16, 20, 32, 41, 58, 25, 30, 0, -19, 6, 9, 49, 46, 35, 19, 8, 3, 22, 24, 45, 57, 40, 57, 36, -17, 28, 19, 47, 31, 34, 39, 23, -15, -26, 15, 6, 9, 17, 10, 5, -21, -22, -29, -15, -15, -23, -35, -31, -32, -27, -21, -42, -4, -31, -19, -40 };

    int midgameKingTable[64] = { -64, 22, 15, -15, -56, -34, 1, 12, 28, -1, -19, -6, -8, -4, -38, -29, -8, 23, 2, -15, -19, 6, 22, -21, -17, -19, -11, -26, -29, -24, -14, -36, -49, 0, -26, -38, -46, -44, -33, -51, -13, -13, -21, -45, -44, -30, -15, -27, 1, 6, -8, -64, -43, -15, 8, 7, -14, 36, 12, -53, 7, -28, 23, 13 };
    int endgameKingTable[64] = { -73, -35, -18, -18, -11, 14, 3, -17, -12, 17, 14, 17, 16, 38, 23, 10, 10, 16, 23, 15, 20, 45, 44, 12, -8, 22, 24, 27, 26, 33, 25, 2, -18, -3, 21, 24, 26, 22, 8, -11, -19, -2, 11, 21, 22, 15, 6, -9, -26, -11, 3, 12, 13, 4, -5, -17, -52, -33, -20, -10, -28, -14, -24, -43 };

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