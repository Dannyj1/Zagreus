//
// Created by Danny on 30-4-2022.
//

#pragma once


#include <cstdint>
#include <map>
#include "bitboard.h"

namespace Zagreus {
    struct TTEntry {
        int score;
        uint8_t depth;
        uint64_t zobristHash;
    };

    class TranspositionTable {
    public:
        TTEntry* transpositionTable = new TTEntry[1 << 25]{};
        TTEntry* pvMoves = new TTEntry[1 << 16]{};
        uint32_t killerMoves[3][128]{};
        uint32_t historyMoves[12][64]{};

        ~TranspositionTable() {
            delete[] transpositionTable;
        }

        void addPosition(uint64_t zobristHash, int depth, int score);

        TTEntry getPosition(uint64_t zobristHash);

        void addPVMove(uint64_t zobristHash, int depth, int score);

        bool isPVMove(uint64_t zobristHash);
    };

    static TranspositionTable tt{};
}