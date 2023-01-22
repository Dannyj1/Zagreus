/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <map>
#include "bitboard.h"

namespace Zagreus {
    enum NodeType {
        PV_NODE,  // Exact score
        FAIL_LOW_NODE, // Alpha score
        FAIL_HIGH_NODE // Beta score
    };

    struct TTEntry {
        int score = 0;
        uint8_t depth = 0;
        uint64_t zobristHash = 0;
        NodeType nodeType = NodeType::PV_NODE;
    };

    class TranspositionTable {
    public:
        static TranspositionTable* instance;

        TTEntry* transpositionTable = new TTEntry[1]{};
        uint32_t** killerMoves = new uint32_t*[3]{};
        // TODO: make 1d
        uint32_t** historyMoves = new uint32_t*[12]{};
        uint32_t** counterMoves = new uint32_t*[64]{};

        uint64_t hashSize = 0;

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

        TranspositionTable(TranspositionTable &other) = delete;

        void operator=(const TranspositionTable &) = delete;

        static TranspositionTable* getTT();

        void setTableSize(int megaBytes);

        void addPosition(uint64_t zobristHash, int depth, int score, NodeType nodeType);

        int getScore(uint64_t zobristHash, int depth, int alpha, int beta);

        TTEntry getEntry(uint64_t zobristHash);
    };
}