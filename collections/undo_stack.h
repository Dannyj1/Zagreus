//
// Created by Danny on 1-1-2023.
//

#pragma once

#include "../types.h"

namespace Zagreus {
    class UndoStack {
    private:
        UndoData stack[128]{};
        int lastElementIndex = 0;

    public:
        void pushMove(const uint64_t pieceBB[12], const PieceType pieceSquareMapping[64], const uint64_t colorBB[2],
                      uint64_t occupiedBB, const int enPassantSquare[2],
                      uint8_t castlingRights, const uint64_t attacksFrom[64], const uint64_t attacksTo[64],
                      uint64_t zobristHash, unsigned int movesMade, unsigned int halfMoveClock, int fullMoveClock);

        void clear();

        UndoData* pop();
    };
}
