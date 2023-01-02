//
// Created by Danny on 1-1-2023.
//

#include <cassert>

#include "../types.h"
#include "undo_stack.h"

namespace Zagreus {
    void UndoStack::pushMove(uint64_t pieceBB[12], PieceType pieceSquareMapping[64], uint64_t colorBB[2],
                             uint64_t occupiedBB, int whiteEnPassantSquare, int blackEnPassantSquare,
                             uint8_t castlingRights, uint64_t attacksFrom[64], uint64_t attacksTo[64],
                             uint64_t zobristHash, int movesMade, int halfMoveClock, int fullMoveClock) {
        for (int i = 0; i < 12; i++) {
            stack[lastElementIndex].pieceBB[i] = pieceBB[i];
        }

        for (int i = 0; i < 64; i++) {
            stack[lastElementIndex].attacksFrom[i] = attacksFrom[i];
            stack[lastElementIndex].attacksTo[i] = attacksTo[i];
            stack[lastElementIndex].pieceSquareMapping[i] = pieceSquareMapping[i];
        }

        stack[lastElementIndex].colorBB[0] = colorBB[0];
        stack[lastElementIndex].colorBB[1] = colorBB[1];
        stack[lastElementIndex].occupiedBB = occupiedBB;
        stack[lastElementIndex].whiteEnPassantSquare = whiteEnPassantSquare;
        stack[lastElementIndex].blackEnPassantSquare = blackEnPassantSquare;
        stack[lastElementIndex].castlingRights = castlingRights;
        stack[lastElementIndex].zobristHash = zobristHash;
        stack[lastElementIndex].movesMade = movesMade;
        stack[lastElementIndex].halfMoveClock = halfMoveClock;
        stack[lastElementIndex].fullMoveClock = fullMoveClock;

        lastElementIndex++;
        assert(lastElementIndex <= 256);
    }

    UndoData* UndoStack::pop() {
        lastElementIndex--;
        assert(lastElementIndex >= 0);
        return &stack[lastElementIndex];
    }

    void UndoStack::clear() {
        lastElementIndex = 0;
    }
}