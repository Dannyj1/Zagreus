//
// Created by Danny on 1-1-2023.
//

#pragma once

#include "../types.h"

namespace Zagreus {
    class UndoStack {
    private:
        UndoData stack[256]{};
        int lastElementIndex = 0;

    public:
        void pushMove(uint64_t pieceBB[12], PieceType pieceSquareMapping[64], uint64_t colorBB[2],
                      uint64_t occupiedBB, int whiteEnPassantSquare, int blackEnPassantSquare,
                      uint8_t castlingRights, uint64_t attacksFrom[64], uint64_t attacksTo[64],
                      uint64_t zobristHash, int movesMade, int halfMoveClock, int fullMoveClock);

        void clear();

        UndoData* pop();
    };
}
