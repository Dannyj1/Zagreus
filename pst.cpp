//
// Created by Danny on 8-1-2023.
//
//
// Created by Danny on 8-1-2023.
//

#include "types.h"

namespace Zagreus {
    // PST tables from https://www.chessprogramming.org/Simplified_Evaluation_Function
    const int pawnTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int knightTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int bishopTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int rookTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int queenTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    const int kingMidgameTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    // TODO: use this in lategame, implement tapered eval.
    const int kingEndgameTable[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5, 5, 10, 25, 25, 10, 5, 5,
            0, 0, 0, 20, 20, 0, 0, 0,
            5, -5, -10, 0, 0, -10, -5, 5,
            5, 10, 10, -20, -20, 10, 10, 5,
            0, 0, 0, 0, 0, 0, 0, 0
    };

    static int pst[12][64]{};

    int getPstValue(PieceType piece, int square) {
        return pst[piece][square];
    }

    void initializePst() {
        for (int piece = 0; piece < 12; piece++) {
            for (int square = 0; square < 64; square++) {
                PieceType pieceType = static_cast<PieceType>(piece);

                switch (pieceType) {
                    case PieceType::WHITE_PAWN:
                        pst[pieceType][square] = pawnTable[square];
                        break;
                    case PieceType::BLACK_PAWN:
                        pst[pieceType][square] = pawnTable[63 - square];
                        break;
                    case PieceType::WHITE_KNIGHT:
                        pst[pieceType][square] = knightTable[square];
                        break;
                    case PieceType::BLACK_KNIGHT:
                        pst[pieceType][square] = knightTable[63 - square];
                        break;
                    case PieceType::WHITE_BISHOP:
                        pst[pieceType][square] = bishopTable[square];
                        break;
                    case PieceType::BLACK_BISHOP:
                        pst[pieceType][square] = bishopTable[63 - square];
                        break;
                    case PieceType::WHITE_ROOK:
                        pst[pieceType][square] = rookTable[square];
                        break;
                    case PieceType::BLACK_ROOK:
                        pst[pieceType][square] = rookTable[63 - square];
                        break;
                    case PieceType::WHITE_QUEEN:
                        pst[pieceType][square] = queenTable[square];
                        break;
                    case PieceType::BLACK_QUEEN:
                        pst[pieceType][square] = queenTable[63 - square];
                        break;
                    case PieceType::WHITE_KING:
                        pst[pieceType][square] = kingMidgameTable[square];
                        break;
                    case PieceType::BLACK_KING:
                        pst[pieceType][square] = kingMidgameTable[63 - square];
                        break;
                }
            }
        }
    }
}