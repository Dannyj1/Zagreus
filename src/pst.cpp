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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 96, 131, 62, 92, 68, 127, 35, -7, 0, 8, 26, 31, 58, 52, 26, -10, -14, 9, -1, 17, 21, 8, 21, -12, -44, -5, -14, 15, 22, 2, 8, -31, -24, -6, -9, -1, 2, 1, 12, -16, -23, 10, -9, -17, -8, 11, 36, -15, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 181, 175, 159, 131, 151, 138, 166, 198, 110, 109, 85, 71, 58, 58, 89, 102, 37, 29, 7, -4, -6, 5, 24, 31, 15, 1, -9, -8, -13, -12, 2, 12, -3, 0, -6, 6, 3, -12, -6, -2, 20, 10, 11, 18, 16, 1, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -164, -89, -34, -48, 59, -96, -14, -106, -70, -41, 78, 33, 22, 67, 6, -18, -45, 58, 35, 70, 85, 123, 71, 45, -9, 17, 22, 44, 39, 65, 17, 21, -15, 3, 12, 17, 23, 19, 20, -13, -23, -4, 12, 14, 18, 15, 16, -20, -23, -47, -5, 11, 11, 18, -12, -11, -105, -24, -58, -30, -20, -29, -14, -21 };
    int endgameKnightTable[64] = { -60, -38, -14, -27, -33, -27, -63, -95, -23, -11, -23, -3, -7, -24, -25, -51, -21, -20, 11, 12, 0, -11, -20, -39, -19, 9, 21, 19, 21, 11, 6, -17, -20, -6, 16, 29, 15, 21, 1, -16, -22, -3, -5, 16, 10, -3, -23, -23, -43, -16, -7, 0, 0, -20, -22, -39, -30, -49, -21, -12, -26, -17, -51, -64 };

    int midgameBishopTable[64] = { -29, 3, -82, -36, -25, -37, 6, -6, -25, 12, -13, -11, 31, 59, 19, -43, -10, 38, 41, 38, 38, 54, 38, 0, -5, 3, 18, 44, 37, 29, 0, -1, -9, 8, 2, 25, 36, 5, 12, 1, 0, 16, 12, 4, 5, 24, 20, 19, 11, 29, 14, 4, 13, 25, 45, 0, -27, 0, -4, -17, -10, 0, -38, -20 };
    int endgameBishopTable[64] = { -11, -19, -9, -4, -3, -5, -18, -23, -9, -4, 9, -6, -5, -16, -4, -11, 8, -8, 0, 0, -1, 6, 0, 3, -3, 7, 8, 9, 10, 4, 1, 0, -6, 2, 8, 15, 6, 8, 0, -12, -10, -3, 3, 11, 8, 0, -5, -15, -12, -7, -5, 0, 8, -11, -7, -29, -20, -8, -7, 0, -8, -7, -1, -15 };

    int midgameRookTable[64] = { 30, 40, 28, 46, 62, 12, 33, 42, 24, 31, 60, 64, 75, 66, 22, 43, -4, 18, 26, 33, 16, 44, 59, 13, -19, -14, 9, 27, 21, 32, -4, -18, -35, -27, -15, -2, 13, -10, 3, -26, -45, -24, -17, -19, 1, 5, -6, -32, -45, -15, -20, -11, -4, 8, -6, -65, -13, -17, 2, 17, 12, 13, -29, -26 };
    int endgameRookTable[64] = { 10, 4, 13, 8, 9, 17, 9, 3, 10, 12, 9, 12, -6, 3, 10, 2, 12, 7, 8, 2, 8, -3, -4, -4, 10, 0, 12, 4, 3, 0, 0, 3, 4, 4, 10, 1, 0, -7, -10, -12, -1, 1, 0, 0, -7, -8, -10, -16, -4, -6, 0, 1, -13, -6, -9, 0, -1, 0, 7, -1, -2, -8, 6, -13 };

    int midgameQueenTable[64] = { -25, -1, 32, 13, 58, 43, 43, 43, -23, -41, -1, 0, -18, 55, 17, 52, -5, -14, 3, 8, 29, 56, 46, 58, -19, -31, -20, -15, 0, 11, -2, 5, -12, -24, -10, -17, -7, -6, 7, -6, -15, -2, -5, -6, -12, -3, 5, 0, -32, -5, 11, 0, 3, 8, -2, -1, -1, -15, -10, 10, 0, -16, -23, -40 };
    int endgameQueenTable[64] = { -5, 22, 26, 29, 25, 19, 8, 17, -13, 20, 36, 41, 57, 29, 23, 0, -17, 9, 6, 45, 48, 31, 17, 7, 5, 22, 25, 41, 53, 38, 56, 35, -13, 32, 16, 39, 29, 34, 38, 22, -13, -24, 15, 2, 9, 16, 5, 0, -18, -20, -31, -11, -17, -25, -34, -31, -30, -26, -22, -35, 5, -26, -20, -36 };

    int midgameKingTable[64] = { -64, 24, 15, -13, -55, -34, 2, 13, 29, 0, -18, -7, -7, -1, -36, -28, -8, 22, 1, -15, -20, 5, 23, -22, -16, -17, -13, -25, -28, -23, -15, -35, -49, -1, -28, -35, -46, -43, -34, -50, -14, -13, -26, -45, -44, -29, -12, -28, 0, 6, -13, -70, -51, -16, 13, 12, -6, 31, 5, -36, -9, -32, 28, 13 };
    int endgameKingTable[64] = { -74, -35, -18, -16, -9, 14, 5, -16, -10, 21, 14, 16, 19, 40, 28, 10, 13, 18, 24, 17, 18, 47, 47, 11, -5, 22, 26, 32, 31, 40, 30, 5, -17, -2, 22, 24, 26, 26, 8, -5, -22, 0, 10, 23, 28, 18, 7, -9, -24, -15, 0, 9, 13, 8, -6, -16, -58, -38, -23, -14, -29, -16, -31, -51 };

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