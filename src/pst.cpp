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
    int midgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 96, 134, 62, 97, 66, 128, 33, -5, -3, 0, 24, 28, 66, 52, 25, -19, -11, 1, -8, 13, 15, 11, 23, -17, -39, -7, -11, 11, 20, 4, 0, -34, -25, 0, -15, -1, 5, 0, 16, -16, -27, 7, -10, -10, -13, 6, 40, -15, 0, 0, 0, 0, 0, 0, 0, 0 };
    int endgamePawnTable[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 183, 181, 163, 136, 143, 137, 173, 198, 100, 102, 73, 70, 56, 61, 83, 99, 36, 22, 6, -1, -6, 8, 23, 24, 3, 0, -11, -13, -8, -12, 5, 2, 1, 2, -5, 2, 1, -9, 0, 1, 20, 12, 8, 27, 16, 5, 4, 11, 0, 0, 0, 0, 0, 0, 0, 0 };

    int midgameKnightTable[64] = { -166, -88, -37, -49, 56, -91, -17, -108, -73, -38, 78, 34, 28, 64, 10, -14, -49, 64, 38, 62, 83, 134, 68, 42, -6, 11, 16, 45, 35, 65, 15, 28, -13, 7, 15, 9, 26, 20, 21, -19, -30, -3, 12, 14, 19, 16, 19, -16, -26, -55, -7, 7, 8, 21, -3, -16, -103, -20, -59, -35, -16, -27, -17, -26 };
    int endgameKnightTable[64] = { -57, -36, -17, -33, -30, -28, -66, -97, -27, -10, -26, -10, -7, -26, -21, -49, -27, -19, 12, 9, 1, -9, -19, -44, -18, 3, 16, 15, 24, 8, 9, -19, -17, -6, 21, 24, 17, 15, 0, -15, -25, -1, 2, 16, 10, -2, -26, -26, -42, -14, -9, 0, -4, -14, -22, -43, -29, -48, -22, -11, -24, -18, -47, -68 };

    int midgameBishopTable[64] = { -26, 1, -81, -40, -27, -43, 7, -6, -29, 19, -14, -14, 29, 49, 16, -44, -2, 36, 43, 39, 31, 54, 37, 4, -6, -2, 12, 47, 23, 29, 3, -6, -11, 5, 3, 20, 33, 11, 9, 0, -2, 12, 18, 11, 6, 26, 10, 22, 6, 28, 12, 8, 10, 22, 37, 0, -24, 0, -10, -20, -9, -6, -33, -13 };
    int endgameBishopTable[64] = { -14, -23, -9, -8, -6, -6, -19, -19, -11, -3, 9, -12, 0, -19, -5, -11, 10, -8, -5, 0, -3, 6, -1, 11, -3, 4, 11, 2, 10, 1, 4, 1, -10, 2, 15, 14, 6, 12, 0, 0, -13, -3, 0, 7, 4, 8, -15, -11, -14, -10, -5, 6, 8, -4, -7, -24, -20, -12, -10, 1, -4, -8, -2, -15 };

    int midgameRookTable[64] = { 32, 38, 32, 49, 55, 8, 35, 37, 26, 31, 55, 62, 79, 63, 26, 41, 0, 15, 23, 40, 20, 43, 60, 12, -23, -8, 6, 22, 22, 39, -7, -17, -32, -24, -14, -7, 9, -7, 0, -24, -44, -26, -19, -19, 1, 3, -2, -32, -45, -17, -20, -14, -2, 1, -11, -66, -12, -14, 0, 9, 12, 6, -29, -26 };
    int endgameRookTable[64] = { 9, 6, 11, 9, 3, 10, 10, 1, 10, 17, 7, 6, -7, 4, 15, 4, 13, -1, 1, 0, 1, 1, -1, -2, 7, 8, 15, 0, 8, 0, 0, 8, 4, 8, 3, 0, -1, -2, -7, -11, 0, 4, -1, 1, -13, -12, -7, -17, -5, -5, 2, 0, -14, -7, -9, 0, -3, 5, 3, -1, -7, -7, 13, -10 };

    int midgameQueenTable[64] = { -23, 2, 34, 9, 58, 46, 44, 48, -20, -50, -7, 6, -18, 51, 14, 47, -10, -7, 9, 11, 27, 51, 44, 46, -20, -29, -10, -17, -5, 12, 0, 3, -16, -23, -13, -12, -5, -6, -1, -8, -15, 1, -7, -10, -10, 1, 15, 4, -37, -3, 12, 5, 0, 11, 9, 3, -3, -12, -17, 0, 2, -24, -32, -35 };
    int endgameQueenTable[64] = { -5, 22, 27, 26, 29, 19, 10, 21, -13, 14, 33, 44, 56, 24, 23, 0, -22, 8, 7, 53, 47, 32, 18, 2, 8, 23, 30, 38, 52, 39, 55, 42, -16, 28, 16, 48, 27, 28, 35, 20, -11, -28, 14, 7, -1, 17, 10, 2, -21, -17, -27, -14, -16, -23, -31, -28, -34, -27, -24, -40, 13, -29, -15, -32 };

    int midgameKingTable[64] = { -64, 25, 16, -13, -54, -32, 2, 12, 27, 0, -21, -6, -4, -2, -38, -27, -8, 27, 4, -19, -18, 7, 21, -18, -18, -19, -13, -25, -32, -26, -17, -36, -50, 0, -26, -40, -49, -39, -34, -49, -16, -17, -27, -50, -46, -27, -8, -29, 6, 8, -18, -62, -53, -10, 12, 5, -1, 28, 4, -26, -14, -23, 25, 14 };
    int endgameKingTable[64] = { -74, -35, -15, -13, -8, 18, 3, -18, -12, 20, 8, 18, 21, 37, 25, 14, 11, 16, 28, 11, 21, 43, 47, 15, -9, 23, 27, 30, 23, 35, 19, 0, -23, -6, 17, 24, 25, 36, 8, -9, -19, -4, 14, 30, 19, 20, 10, -11, -25, -5, 7, 15, 20, 7, -1, -21, -49, -40, -29, -5, -34, -19, -29, -45 };

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