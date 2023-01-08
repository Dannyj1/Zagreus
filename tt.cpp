//
// Created by Danny on 30-4-2022.
//

#include <iostream>

#include "tt.h"

namespace Zagreus {
    void TranspositionTable::addPosition(uint64_t zobristHash, int depth, int score, bool isPVMove) {
        if (score >= 90000000 || score <= -90000000) {
            return;
        }

        uint32_t index = (zobristHash & 0x3FFFFFF);
        transpositionTable[index] = TTEntry{score, (uint8_t) depth, zobristHash, isPVMove};
    }

    TTEntry* TranspositionTable::getPosition(uint64_t zobristHash) {
        uint32_t index = (zobristHash & 0x3FFFFFF);

        return &transpositionTable[index];
    }
}
