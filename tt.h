//
// Created by Danny on 30-4-2022.
//

#pragma once


#include <cstdint>
#include <map>
#include "bitboard.h"

namespace Zagreus {
    struct TTEntry {
        int score = 0;
        uint8_t depth = 0;
        uint64_t zobristHash = 0;
    };

    class TranspositionTable {
    public:
        TTEntry* transpositionTable = new TTEntry[1 << 25]{};
        TTEntry* pvMoves = new TTEntry[1 << 16]{};
        uint32_t** killerMoves = new uint32_t*[3]{};
        uint32_t** historyMoves = new uint32_t*[12]{};

        TranspositionTable() {
            for (int i = 0; i < 3; i++) {
                killerMoves[i] = new uint32_t[128]{};
            }

            for (int i = 0; i < 12; i++) {
                historyMoves[i] = new uint32_t[64]{};
            }
        }

        ~TranspositionTable() {
            delete[] transpositionTable;
            delete[] pvMoves;

            for (int i = 0; i < 3; i++) {
                delete[] killerMoves[i];
            }

            for (int i = 0; i < 12; i++) {
                delete[] historyMoves[i];
            }

            delete[] killerMoves;
            delete[] historyMoves;
        }

        void addPosition(uint64_t zobristHash, int depth, int score);

        TTEntry* getPosition(uint64_t zobristHash);

        void addPVMove(uint64_t zobristHash, int depth, int score);

        bool isPVMove(uint64_t zobristHash);
    };

    static TranspositionTable tt{};
}