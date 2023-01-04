//
// Created by Danny on 30-4-2022.
//

#include <iostream>

#include "tt.h"
#include "bitboard.h"

namespace Zagreus {
    void TranspositionTable::addPosition(uint64_t zobristHash, int depth, int score) {
        uint32_t index = (zobristHash & 0x1FFFFFF);
        transpositionTable[index] = TTEntry{score, (uint8_t) depth, zobristHash};
    }

    bool TranspositionTable::isPositionInTable(uint64_t zobristHash, int depth) {
        if (zobristHash == 0ULL) {
            return false;
        }

        uint32_t index = (zobristHash & 0x1FFFFFF);
        TTEntry entry = transpositionTable[index];

        return entry.zobristHash == zobristHash && entry.depth >= depth;
    }

    int TranspositionTable::getPositionScore(uint64_t zobristHash) {
        uint32_t index = (zobristHash & 0x1FFFFFF);

        return transpositionTable[index].score;
    }

    bool TranspositionTable::isPositionInTable(uint64_t zobristHash) {
        uint32_t index = (zobristHash & 0x1FFFFFF);
        TTEntry entry = transpositionTable[index];

        return entry.zobristHash == zobristHash;
    }

    void TranspositionTable::addKillerMove(uint64_t zobristHash, int depth, int score) {
        uint32_t index = (zobristHash & 0xFFFFF);
        killerMoves[index] = TTEntry{score, (uint8_t) depth, zobristHash};
    }

    bool TranspositionTable::isKillerMove(uint64_t zobristHash) {
        if (zobristHash == 0ULL) {
            return false;
        }

        uint32_t index = (zobristHash & 0xFFFFF);
        TTEntry entry = killerMoves[index];

        return entry.zobristHash == zobristHash;
    }

    void TranspositionTable::addPVMove(uint64_t zobristHash, int depth, int score) {
        uint32_t index = (zobristHash & 0xFFFFF);
        pvMoves[index] = TTEntry{score, (uint8_t) depth, zobristHash};
    }

    bool TranspositionTable::isPVMove(uint64_t zobristHash) {
        if (zobristHash == 0ULL) {
            return false;
        }

        uint32_t index = (zobristHash & 0xFFFFF);
        TTEntry entry = pvMoves[index];

        return entry.zobristHash == zobristHash;
    }
}
