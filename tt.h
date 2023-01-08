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
        bool isPVMove = false;
    };

    class TranspositionTable {
    public:
        TTEntry* transpositionTable = new TTEntry[1 << 26]{};
        uint32_t** killerMoves = new uint32_t*[3]{};
        // TODO: make 1d
        uint32_t** historyMoves = new uint32_t*[12]{};
        uint32_t** counterMoves = new uint32_t*[64]{};

        TranspositionTable() {
            for (int i = 0; i < 3; i++) {
                killerMoves[i] = new uint32_t[128]{};
            }

            for (int i = 0; i < 12; i++) {
                historyMoves[i] = new uint32_t[64]{};
            }

            for (int i = 0; i < 64; i++) {
                counterMoves[i] = new uint32_t[64]{};
            }
        }

        ~TranspositionTable() {
            delete[] transpositionTable;

            for (int i = 0; i < 3; i++) {
                delete[] killerMoves[i];
            }

            for (int i = 0; i < 12; i++) {
                delete[] historyMoves[i];
            }

            for (int i = 0; i < 64; i++) {
                delete[] counterMoves[i];
            }

            delete[] killerMoves;
            delete[] historyMoves;
            delete[] counterMoves;
        }

        void addPosition(uint64_t zobristHash, int depth, int score, bool isPVMove);

        TTEntry* getPosition(uint64_t zobristHash);
    };

    static TranspositionTable tt{};
}