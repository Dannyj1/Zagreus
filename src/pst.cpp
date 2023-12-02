/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <vector>

#include "types.h"

namespace Zagreus {
int midgamePawnTable[64] = {0,   0,   0,  0,   0,  0,  0,  0,   88,  113, 57, 89, 66, 116, 41, -5,
                            -5,  0,   36, 23,  44, 53, 19, -11, -12, 0,   3,  24, 20, 12,  13, -7,
                            -39, -10, 0,  18,  19, 7,  -3, -28, -21, -3,  -6, 0,  0,  -1,  2,  -19,
                            -21, 12,  -6, -13, -9, 5,  25, -10, 0,   0,   0,  0,  0,  0,   0,  0};
int endgamePawnTable[64] = {
    0,  0,  0,   0,  0,  0, 0,  0,  173, 162, 149, 131, 147, 121, 165, 190, 96, 101, 88, 68, 40, 51,
    81, 79, 37,  20, 11, 0, -5, 9,  6,   21,  16,  6,   -8,  -6,  -10, -16, 0,  6,   6,  8,  3,  16,
    17, -5, -12, -1, 13, 2, 18, 32, 25,  10,  -8,  0,   0,   0,   0,   0,   0,  0,   0,  0};

int midgameKnightTable[64] = {-170, -89, -42, -43, 54,  -86, -17, -111, -62, -26, 77,  49, 33,
                              76,   0,   -24, -35, 52,  42,  63,  83,   95,  59,  39,  0,  16,
                              15,   41,  42,  33,  17,  31,  -9,  9,    18,  18,  21,  20, 20,
                              0,    -31, 5,   9,   22,  13,  8,   11,   -17, -11, -35, -7, 18,
                              16,   12,  -4,  -5,  -99, -19, -43, -25,  -13, -21, -13, -24};
int endgameKnightTable[64] = {-53, -38, -19, -24, -34, -21, -62, -96, -24, -4,  -25, 0,   -3,
                              -17, -24, -46, -14, -25, 14,  8,   0,   -15, -20, -42, -9,  12,
                              27,  20,  28,  5,   11,  -13, -16, -8,  14,  27,  14,  20,  1,
                              -19, -16, -4,  -23, 15,  9,   -9,  -23, -17, -35, -18, -12, -5,
                              -10, -18, -23, -39, -28, -35, -11, -16, -18, -17, -37, -64};

int midgameBishopTable[64] = {-26, 0,  -71, -39, -28, -44, 5,   -6,  -19, 1,  -18, -7, 24,
                              45,  9,  -26, -3,  28,  39,  31,  21,  52,  32, 0,   -5, 0,
                              5,   25, 25,  8,   12,  -12, -13, 4,   5,   17, 29,  7,  8,
                              -6,  11, 22,  13,  11,  11,  29,  13,  25,  13, 39,  18, 15,
                              19,  24, 46,  0,   -20, -8,  0,   -16, 0,   2,  -25, -9};
int endgameBishopTable[64] = {-13, -24, -7, -7,  -13, -9, -20, -15, -9,  -13, 4,  -5, -3,
                              -24, -13, -4, 9,   -11, 3,  -10, -7,  3,   -7,  2,  0,  5,
                              3,   4,   10, 0,   5,   -1, -15, 0,   13,  16,  0,  5,  -5,
                              -11, -5,  -7, 11,  22,  17, 6,   -8,  -11, -14, -6, -8, 4,
                              10,  -11, -9, -28, -14, -8, -9,  9,   -8,  -8,  -4, -14};

int midgameRookTable[64] = {32,  41,  29,  53,  60,  12,  33,  43,  19,  14,  53,  48,  63,
                            52,  17,  37,  1,   14,  20,  37,  26,  37,  55,  10,  -17, -10,
                            0,   16,  14,  23,  -4,  -16, -31, -26, -24, -14, 0,   -10, 0,
                            -22, -36, -19, -19, -13, 3,   -12, -6,  -28, -49, -13, -19, -12,
                            -5,  -7,  -2,  -46, -4,  -13, 1,   3,   7,   3,   -12, -8};
int endgameRookTable[64] = {17, 9,  21, 22, 8,  13,  7,   4,   1, 14, 8,  9,  -11, 0,   10,  0,
                            12, 7,  6,  5,  10, -4,  -4,  -11, 3, 0,  10, -4, -5,  0,   0,   4,
                            -1, 6,  3,  -1, -9, -8,  -11, -16, 0, 1,  -6, 0,  -12, -15, -10, -6,
                            -8, -3, 0,  1,  -9, -18, -11, 10,  0, 11, 4,  -5, -6,  1,   7,   0};

int midgameQueenTable[64] = {-20, 0,   26,  11, 54,  44,  41, 32,  -19, -57, -10, -13, -27,
                             44,  -2,  30,  -5, 0,   8,   10, 24,  38,  37,  35,  -9,  -20,
                             -25, -23, -21, 3,  -11, -3,  -9, -15, -18, -15, -15, -8,  2,
                             -4,  -20, 0,   0,  -8,  -11, -2, 5,   2,   -19, -7,  6,   12,
                             15,  9,   15,  7,  0,   -16, -9, 10,  8,   -12, -21, -20};
int endgameQueenTable[64] = {-4, 22,  22,  26,  24,  20,  5,   14,  -5,  26,  29,  37,  49,
                             14, 21,  -4,  -13, 10,  8,   50,  49,  23,  15,  0,   11,  25,
                             24, 40,  50,  30,  53,  28,  -9,  31,  19,  43,  23,  35,  38,
                             26, -14, -21, 22,  7,   6,   27,  4,   3,   -13, -20, -20, -2,
                             -3, -22, -28, -25, -33, -29, -23, -26, -12, -31, -11, -23};

int midgameKingTable[64] = {-65, 22,  16,  -15, -56, -34, 2,   13,  28,  -1,  -20, -7,  -8,
                            -3,  -37, -28, -8,  24,  1,   -14, -19, 6,   21,  -21, -17, -21,
                            -10, -28, -31, -27, -15, -38, -51, -4,  -29, -39, -47, -46, -32,
                            -52, -14, -17, -20, -53, -51, -28, -9,  -27, 13,  4,   -15, -73,
                            -67, -25, 10,  7,   8,   18,  -25, -7,  -6,  -32, 8,   3};
int endgameKingTable[64] = {-74, -35, -18, -18, -11, 15,  4,   -17, -12, 16,  13,  16, 17,
                            38,  22,  11,  9,   18,  21,  16,  20,  44,  43,  13,  -7, 20,
                            26,  26,  25,  30,  26,  1,   -19, -3,  20,  26,  26,  26, 10,
                            -11, -19, -1,  21,  20,  23,  21,  14,  -11, -28, -4,  9,  30,
                            29,  15,  -2,  -24, -58, -41, -29, -13, -24, -11, -31, -56};

// Base tables from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
int baseMidgamePawnTable[64] = {
    0,   0,  0,   0,   0,   0,  0,  0,   98,  134, 61, 95,  68, 126, 34, -11,
    -6,  7,  26,  31,  65,  56, 25, -20, -14, 13,  6,  21,  23, 12,  17, -23,
    -27, -2, -5,  12,  17,  6,  10, -25, -26, -4,  -4, -10, 3,  3,   33, -12,
    -35, -1, -20, -23, -15, 24, 38, -22, 0,   0,   0,  0,   0,  0,   0,  0,
};

int baseEndgamePawnTable[64] = {
    0,  0,  0,  0,  0,  0, 0,  0,  178, 173, 158, 134, 147, 132, 165, 187, 94, 100, 85, 67, 56, 53,
    82, 84, 32, 24, 13, 5, -2, 4,  17,  17,  13,  9,   -3,  -7,  -7,  -8,  3,  -1,  4,  7,  -6, 1,
    0,  -5, -1, -8, 13, 8, 8,  10, 13,  0,   2,   -7,  0,   0,   0,   0,   0,  0,   0,  0,
};

int baseMidgameKnightTable[64] = {
    -167, -89, -34, -49, 61, -97, -15, -107, -73,  -41, 72,  36,  23,  62,  7,   -17,
    -47,  60,  37,  65,  84, 129, 73,  44,   -9,   17,  19,  53,  37,  69,  18,  22,
    -13,  4,   16,  13,  28, 19,  21,  -8,   -23,  -9,  12,  10,  19,  17,  25,  -16,
    -29,  -53, -12, -3,  -1, 18,  -14, -19,  -105, -21, -58, -33, -17, -28, -19, -23,
};

int baseEndgameKnightTable[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99, -25, -8,  -25, -2,  -9,  -25, -24, -52,
    -24, -20, 10,  9,   -1,  -9,  -19, -41, -17, 3,   22,  22,  22,  11,  8,   -18,
    -18, -6,  16,  25,  16,  17,  4,   -18, -23, -3,  -1,  15,  10,  -3,  -20, -22,
    -42, -20, -10, -5,  -2,  -20, -23, -44, -29, -51, -23, -15, -22, -18, -50, -64,
};

int baseMidgameBishopTable[64] = {
    -29, 4,  -82, -37, -25, -42, 7,  -8, -26, 16, -18, -13, 30,  59,  18,  -47,
    -16, 37, 43,  40,  35,  50,  37, -2, -4,  5,  19,  50,  37,  37,  7,   -2,
    -6,  13, 13,  26,  34,  12,  10, 4,  0,   15, 15,  15,  14,  27,  18,  10,
    4,   15, 16,  0,   7,   21,  33, 1,  -33, -3, -14, -21, -13, -12, -39, -21,
};

int baseEndgameBishopTable[64] = {
    -14, -21, -11, -8, -7, -9, -17, -24, -8,  -4, 7,   -12, -3, -13, -4, -14,
    2,   -8,  0,   -1, -2, 6,  0,   4,   -3,  9,  12,  9,   14, 10,  3,  2,
    -6,  3,   13,  19, 7,  10, -3,  -9,  -12, -3, 8,   10,  13, 3,   -7, -15,
    -14, -18, -7,  -1, 4,  -9, -15, -27, -23, -9, -23, -5,  -9, -16, -5, -17,
};

int baseMidgameRookTable[64] = {
    32,  42,  32,  51, 63, 9,  31, 43,  27,  32,  58,  62,  80, 67, 26,  44,
    -5,  19,  26,  36, 17, 45, 61, 16,  -24, -11, 7,   26,  24, 35, -8,  -20,
    -36, -26, -12, -1, 9,  -7, 6,  -23, -45, -25, -16, -17, 3,  0,  -5,  -33,
    -44, -16, -20, -9, -1, 11, -6, -71, -19, -13, 1,   17,  16, 7,  -37, -26,
};

int baseEndgameRookTable[64] = {
    13, 10,  18, 15,  12, 12, 8, 5, 11, 13, 13,  11, -3, 3, 8,  3,  7,  7,   7,  5,   4,  -3,
    -5, -3,  4,  3,   13, 1,  2, 1, -1, 2,  3,   5,  8,  4, -5, -6, -8, -11, -4, 0,   -5, -1,
    -7, -12, -8, -16, -6, -6, 0, 2, -9, -9, -11, -3, -9, 2, 3,  -1, -5, -13, 4,  -20,
};

int baseMidgameQueenTable[64] = {
    -28, 0,   29, 12,  59, 44, 43, 45, -24, -39, -5,  1,   -16, 57,  28,  54,
    -13, -17, 7,  8,   29, 56, 47, 57, -27, -27, -16, -16, -1,  17,  -2,  1,
    -9,  -26, -9, -10, -2, -4, 3,  -3, -14, 2,   -11, -2,  -5,  2,   14,  5,
    -35, -8,  11, 2,   8,  15, -3, 1,  -1,  -18, -9,  10,  -15, -25, -31, -50,
};

int baseEndgameQueenTable[64] = {
    -9,  22,  22,  27,  27,  19,  10,  20,  -17, 20,  32,  41,  58, 25,  30,  0,
    -20, 6,   9,   49,  47,  35,  19,  9,   3,   22,  24,  45,  57, 40,  57,  36,
    -18, 28,  19,  47,  31,  34,  39,  23,  -16, -27, 15,  6,   9,  17,  10,  5,
    -22, -23, -30, -16, -16, -23, -36, -32, -33, -28, -22, -43, -5, -32, -20, -41,
};

int baseMidgameKingTable[64] = {
    -65, 23, 16,  -15, -56, -34, 2,   13,  29,  -1,  -20, -7,  -8,  -4,  -38, -29,
    -9,  24, 2,   -16, -20, 6,   22,  -22, -17, -20, -12, -27, -30, -25, -14, -36,
    -49, -1, -27, -39, -46, -44, -33, -51, -14, -14, -22, -46, -44, -30, -15, -27,
    1,   7,  -8,  -64, -43, -16, 9,   8,   -15, 36,  12,  -54, 8,   -28, 24,  14,
};

int baseEndgameKingTable[64] = {-74, -35, -18, -18, -11, 15,  4,   -17, -12, 17,  14,  17, 17,
                                38,  23,  11,  10,  17,  23,  15,  20,  45,  44,  13,  -8, 22,
                                24,  27,  26,  33,  26,  3,   -18, -4,  21,  24,  27,  23, 9,
                                -11, -19, -3,  11,  21,  23,  16,  7,   -9,  -27, -11, 4,  13,
                                14,  4,   -5,  -17, -53, -34, -21, -11, -28, -14, -24, -43};

static int midgamePst[12][64]{};
static int endgamePst[12][64]{};
static int baseMidgamePst[12][64]{};
static int baseEndgamePst[12][64]{};

int getMidgamePstValue(PieceType piece, int8_t square) { return midgamePst[piece][square]; }

int getEndgamePstValue(PieceType piece, int8_t square) { return endgamePst[piece][square]; }

void setMidgamePstValue(PieceType piece, int8_t square, int value) {
  midgamePst[piece][square] = value;
}

void setEndgamePstValue(PieceType piece, int8_t square, int value) {
  endgamePst[piece][square] = value;
}

std::vector<int> getBaseMidgameValues() {
  std::vector<int> values;

  for (int i = 1; i < 12; i += 2) {
    for (int j = 0; j < 64; j++) {
      values.emplace_back(baseMidgamePst[i][j]);
    }
  }

  return values;
}

std::vector<int> getBaseEndgameValues() {
  std::vector<int> values;

  for (int i = 1; i < 12; i += 2) {
    for (int j = 0; j < 64; j++) {
      values.emplace_back(baseEndgamePst[i][j]);
    }
  }

  return values;
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
        case WHITE_PAWN:
          midgamePst[pieceType][square] = midgamePawnTable[63 - square];
          endgamePst[pieceType][square] = endgamePawnTable[63 - square];
          baseMidgamePst[pieceType][square] = baseMidgamePawnTable[63 - square];
          baseEndgamePst[pieceType][square] = baseEndgamePawnTable[63 - square];
          break;
        case BLACK_PAWN:
          midgamePst[pieceType][square] = midgamePawnTable[square];
          endgamePst[pieceType][square] = endgamePawnTable[square];
          baseMidgamePst[pieceType][square] = baseMidgamePawnTable[square];
          baseEndgamePst[pieceType][square] = baseEndgamePawnTable[square];
          break;
        case WHITE_KNIGHT:
          midgamePst[pieceType][square] = midgameKnightTable[63 - square];
          endgamePst[pieceType][square] = endgameKnightTable[63 - square];
          baseMidgamePst[pieceType][square] = baseMidgameKnightTable[63 - square];
          baseEndgamePst[pieceType][square] = baseEndgameKnightTable[63 - square];
          break;
        case BLACK_KNIGHT:
          midgamePst[pieceType][square] = midgameKnightTable[square];
          endgamePst[pieceType][square] = endgameKnightTable[square];
          baseMidgamePst[pieceType][square] = baseMidgameKnightTable[square];
          baseEndgamePst[pieceType][square] = baseEndgameKnightTable[square];
          break;
        case WHITE_BISHOP:
          midgamePst[pieceType][square] = midgameBishopTable[63 - square];
          endgamePst[pieceType][square] = endgameBishopTable[63 - square];
          baseMidgamePst[pieceType][square] = baseMidgameBishopTable[63 - square];
          baseEndgamePst[pieceType][square] = baseEndgameBishopTable[63 - square];
          break;
        case BLACK_BISHOP:
          midgamePst[pieceType][square] = midgameBishopTable[square];
          endgamePst[pieceType][square] = endgameBishopTable[square];
          baseMidgamePst[pieceType][square] = baseMidgameBishopTable[square];
          baseEndgamePst[pieceType][square] = baseEndgameBishopTable[square];
          break;
        case WHITE_ROOK:
          midgamePst[pieceType][square] = midgameRookTable[63 - square];
          endgamePst[pieceType][square] = endgameRookTable[63 - square];
          baseMidgamePst[pieceType][square] = baseMidgameRookTable[63 - square];
          baseEndgamePst[pieceType][square] = baseEndgameRookTable[63 - square];
          break;
        case BLACK_ROOK:
          midgamePst[pieceType][square] = midgameRookTable[square];
          endgamePst[pieceType][square] = endgameRookTable[square];
          baseMidgamePst[pieceType][square] = baseMidgameRookTable[square];
          baseEndgamePst[pieceType][square] = baseEndgameRookTable[square];
          break;
        case WHITE_QUEEN:
          midgamePst[pieceType][square] = midgameQueenTable[63 - square];
          endgamePst[pieceType][square] = endgameQueenTable[63 - square];
          baseMidgamePst[pieceType][square] = baseMidgameQueenTable[63 - square];
          baseEndgamePst[pieceType][square] = baseEndgameQueenTable[63 - square];
          break;
        case BLACK_QUEEN:
          midgamePst[pieceType][square] = midgameQueenTable[square];
          endgamePst[pieceType][square] = endgameQueenTable[square];
          baseMidgamePst[pieceType][square] = baseMidgameQueenTable[square];
          baseEndgamePst[pieceType][square] = baseEndgameQueenTable[square];
          break;
        case WHITE_KING:
          midgamePst[pieceType][square] = midgameKingTable[63 - square];
          endgamePst[pieceType][square] = endgameKingTable[63 - square];
          baseMidgamePst[pieceType][square] = baseMidgameKingTable[63 - square];
          baseEndgamePst[pieceType][square] = baseEndgameKingTable[63 - square];
          break;
        case BLACK_KING:
          midgamePst[pieceType][square] = midgameKingTable[square];
          endgamePst[pieceType][square] = endgameKingTable[square];
          baseMidgamePst[pieceType][square] = baseMidgameKingTable[square];
          baseEndgamePst[pieceType][square] = baseEndgameKingTable[square];
          break;
      }
    }
  }
}
}  // namespace Zagreus