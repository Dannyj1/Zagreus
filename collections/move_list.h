//
// Created by Danny on 1-1-2023.
//

#pragma once

#include "../types.h"

namespace Zagreus {
    class MoveList {
    private:
        int lastElementIndex = 0;

    public:
        Move moves[128]{};

        void addMove(uint64_t fromSquare, uint64_t toSquare, PieceType pieceType, uint64_t zobristHash,
                     PieceType promotionPiece = PieceType::EMPTY);

        Move* popBack();

        void clear();

        const Move* getMoves() const;

        int size() const;

        Move &operator[](int index);
    };
}
