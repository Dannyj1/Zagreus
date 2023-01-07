//
// Created by Danny on 30-4-2022.
//

#include <iostream>

#include "tt.h"

namespace Zagreus {
    void TranspositionTable::addPosition(uint64_t zobristHash, int depth, int score) {
        uint32_t index = (zobristHash & 0x1FFFFFF);
        transpositionTable[index] = TTEntry{score, (uint8_t) depth, zobristHash};
    }

    TTEntry* TranspositionTable::getPosition(uint64_t zobristHash) {
        uint32_t index = (zobristHash & 0x1FFFFFF);

        return &transpositionTable[index];
    }

    void TranspositionTable::addPVMove(uint64_t zobristHash, int depth, int score) {
        uint32_t index = (zobristHash & 0xFFFF);
        pvMoves[index] = TTEntry{score, (uint8_t) depth, zobristHash};
    }

    bool TranspositionTable::isPVMove(uint64_t zobristHash) {
        if (zobristHash == 0ULL) {
            return false;
        }

        uint32_t index = (zobristHash & 0xFFFF);

        return pvMoves[index].zobristHash == zobristHash;
    }
}
