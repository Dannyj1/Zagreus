/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "types.h"

#include <vector>

namespace Zagreus {
    // PST values from Rofchade: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 97, 133, 60, 94, 67, 125, 33, -11, -6, 6, 25, 30, 64, 55, 24, -20, -14, 12, 5, 19, 21, 11, 16, -23, -27, -2, -4, 11, 16, 5, 9, -25, -26, -4, -3, -9, 3, 2, 33, -11, -35, -2, -21, -22, -14, 24, 36, -22, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 177, 172, 157, 133, 146, 131, 164, 186, 93, 99, 84, 66, 55, 52, 81, 83, 31, 23, 12, 3, -2, 3, 16, 16, 12, 8, -3, -7, -7, -8, 3, 0, 3, 5, -7, 0, 0, -5, -1, -7, 11, 6, 6, 9, 12, 0, 1, -8, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -167, -88, -34, -49, 61, -97, -14, -107, -73, -41, 71, 35, 22, 61, 6, -17, -46, 59, 36, 64, 83, 128, 72, 44, -8, 16, 18, 52, 36, 68, 17, 21, -12, 3, 15, 12, 27, 18, 21, -7, -22, -8, 10, 10, 18, 16, 24, -15, -28, -52, -11, -2, 0, 18, -13, -18, -104, -19, -57, -32, -16, -27, -18, -22 };
    int endgameKnightTable[64] = { -58, -37, -13, -27, -30, -27, -62, -99, -25, -8, -24, -1, -9, -25, -23, -52, -23, -20, 9, 8, -1, -9, -19, -41, -17, 2, 21, 21, 21, 10, 7, -17, -17, -5, 15, 24, 15, 16, 4, -17, -22, -2, -1, 14, 10, -3, -19, -21, -41, -19, -9, -4, -1, -20, -22, -43, -28, -50, -22, -14, -21, -17, -49, -63 };

    int midgameBishopTable[64] = { -28, 3, -82, -37, -25, -42, 6, -8, -25, 15, -18, -13, 29, 58, 17, -47, -16, 36, 42, 39, 34, 49, 36, -2, -4, 5, 18, 49, 36, 36, 6, -2, -6, 12, 12, 25, 33, 11, 9, 3, 0, 15, 14, 13, 12, 27, 17, 10, 4, 15, 15, 0, 7, 20, 34, 1, -32, -2, -12, -20, -12, -11, -38, -20 };
    int endgameBishopTable[64] = { -13, -20, -10, -7, -7, -9, -17, -24, -7, -4, 6, -12, -3, -13, -3, -13, 1, -8, 0, -1, -2, 5, 0, 3, -2, 9, 11, 8, 13, 9, 2, 2, -6, 2, 12, 18, 6, 9, -2, -8, -11, -2, 7, 9, 12, 3, -6, -14, -13, -17, -6, 0, 4, -9, -14, -26, -22, -8, -22, -4, -8, -15, -4, -16 };

    int midgameRookTable[64] = { 31, 41, 31, 50, 62, 8, 31, 42, 26, 31, 57, 61, 79, 66, 25, 43, -5, 19, 25, 35, 16, 44, 60, 15, -24, -11, 6, 25, 23, 34, -8, -20, -35, -26, -12, -1, 8, -6, 5, -23, -44, -25, -16, -16, 2, 0, -4, -33, -43, -16, -20, -8, -1, 11, -5, -70, -17, -12, 0, 16, 14, 8, -36, -26 };
    int endgameRookTable[64] = { 12, 9, 18, 14, 11, 11, 8, 4, 10, 12, 12, 10, -3, 2, 7, 2, 6, 6, 6, 4, 3, -3, -5, -3, 3, 2, 12, 0, 1, 0, -1, 1, 2, 4, 7, 3, -5, -6, -8, -11, -3, 0, -4, 0, -7, -12, -7, -16, -5, -5, 0, 2, -8, -8, -10, -2, -8, 2, 2, -1, -5, -12, 4, -19 };

    int midgameQueenTable[64] = { -28, 0, 29, 11, 58, 43, 42, 44, -24, -39, -5, 0, -16, 56, 27, 53, -13, -17, 6, 7, 28, 55, 46, 56, -26, -26, -16, -16, -1, 16, -2, 1, -8, -25, -8, -10, -2, -4, 3, -2, -13, 2, -11, -1, -5, 1, 13, 4, -34, -7, 11, 2, 8, 14, -3, 1, 0, -17, -8, 10, -15, -25, -30, -49 };
    int endgameQueenTable[64] = { -9, 21, 21, 26, 26, 18, 10, 19, -17, 19, 31, 40, 57, 24, 30, 0, -20, 5, 8, 48, 46, 34, 18, 8, 2, 21, 23, 44, 56, 39, 56, 36, -17, 28, 19, 46, 30, 33, 38, 22, -15, -26, 14, 6, 8, 16, 9, 4, -21, -22, -29, -15, -15, -23, -36, -31, -32, -27, -21, -42, -5, -31, -19, -41 };

    int midgameKingTable[64] = { -64, 23, 15, -15, -56, -33, 2, 12, 29, 0, -19, -7, -8, -3, -38, -29, -9, 23, 2, -16, -20, 5, 21, -21, -16, -20, -12, -27, -30, -25, -14, -36, -48, -1, -27, -39, -46, -43, -32, -50, -14, -14, -22, -45, -43, -29, -14, -26, 0, 6, -7, -63, -42, -15, 9, 8, -14, 33, 12, -53, 9, -27, 25, 14 };
    int endgameKingTable[64] = { -74, -35, -18, -18, -10, 15, 4, -17, -11, 17, 13, 16, 16, 38, 22, 10, 9, 16, 22, 14, 19, 44, 43, 13, -7, 21, 23, 26, 25, 33, 25, 2, -17, -4, 20, 23, 26, 23, 9, -11, -19, -2, 10, 21, 23, 16, 7, -8, -27, -11, 3, 13, 14, 4, -4, -16, -52, -35, -20, -10, -27, -13, -23, -42 };
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