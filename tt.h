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
    private:
        TTEntry* transpositionTable = new TTEntry[1 << 25]{};
        TTEntry* killerMoves = new TTEntry[1 << 20]{};
        TTEntry* pvMoves = new TTEntry[1 << 20]{};

    public:
        ~TranspositionTable() {
            delete[] transpositionTable;
            delete[] killerMoves;
            delete[] pvMoves;
        }

        void addPosition(uint64_t zobristHash, int depth, int score);

        TTEntry getPosition(uint64_t zobristHash);

        void addKillerMove(uint64_t zobristHash, int depth, int score);

        bool isKillerMove(uint64_t zobristHash);

        void addPVMove(uint64_t zobristHash, int depth, int score);

        bool isPVMove(uint64_t zobristHash);
    };

    static TranspositionTable tt{};
}