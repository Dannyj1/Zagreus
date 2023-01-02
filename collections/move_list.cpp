//
// Created by Danny on 1-1-2023.
//

#include <cassert>
#include <iostream>

#include "../types.h"
#include "move_list.h"

namespace Zagreus {
    void MoveList::addMove(uint64_t fromSquare, uint64_t toSquare, PieceType pieceType, uint64_t zobristHash, PieceType promotionPiece) {
        moves[lastElementIndex].fromSquare = fromSquare;
        moves[lastElementIndex].toSquare = toSquare;
        moves[lastElementIndex].pieceType = pieceType;
        moves[lastElementIndex].promotionPiece = promotionPiece;
        moves[lastElementIndex].zobristHash = zobristHash;

        lastElementIndex++;
        assert(lastElementIndex <= 128);
    }

    Move* MoveList::popBack() {
        lastElementIndex--;
        assert(lastElementIndex >= 0);
        return &moves[lastElementIndex];
    }

    void MoveList::clear() {
        lastElementIndex = 0;
    }

    const Move* MoveList::getMoves() const {
        return moves;
    }

    int MoveList::size() const {
        return lastElementIndex;
    }

    Move &MoveList::operator[](int index) {
        assert(index >= 0 && index < lastElementIndex);
        return moves[index];
    }
}