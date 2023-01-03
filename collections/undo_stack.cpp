//
// Created by Danny on 1-1-2023.
//

#include <cassert>
#include <iostream>

#include "../types.h"
#include "undo_stack.h"

namespace Zagreus {
    void UndoStack::pushMove(const uint64_t pieceBB[12], const PieceType pieceSquareMapping[64], const uint64_t colorBB[2],
                             uint64_t occupiedBB, const int enPassantSquare[2],
                             uint8_t castlingRights, const uint64_t attacksFrom[64], const uint64_t attacksTo[64],
                             uint64_t zobristHash, unsigned int movesMade, unsigned int halfMoveClock, int fullMoveClock) {
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
        stack[lastElementIndex].enPassantSquare[0] = enPassantSquare[0];
        stack[lastElementIndex].enPassantSquare[1] = enPassantSquare[1];
        stack[lastElementIndex].castlingRights = castlingRights;
        stack[lastElementIndex].zobristHash = zobristHash;
        stack[lastElementIndex].movesMade = movesMade;
        stack[lastElementIndex].halfMoveClock = halfMoveClock;
        stack[lastElementIndex].fullMoveClock = fullMoveClock;

        lastElementIndex++;
        assert(lastElementIndex < 512);
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