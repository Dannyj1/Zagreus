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

#include <iostream>

#include "tt.h"

namespace Zagreus {
    void TranspositionTable::addPosition(uint64_t zobristHash, int depth, int score, bool isPVMove) {
        if (score >= 90000000 || score <= -90000000) {
            return;
        }

        uint32_t index = (zobristHash & hashSize);
        transpositionTable[index] = TTEntry{score, (uint8_t) depth, zobristHash, isPVMove};
    }

    TTEntry* TranspositionTable::getPosition(uint64_t zobristHash) {
        uint32_t index = (zobristHash & hashSize);

        return &transpositionTable[index];
    }

    void TranspositionTable::setTableSize(int megaBytes) {
        if ((megaBytes & (megaBytes - 1)) != 0) {
            megaBytes = 1 << (int) (log2(megaBytes));
        }

        int byteSize = megaBytes * 1024 * 1024;
        int entryCount = byteSize / sizeof(TTEntry);

        delete[] transpositionTable;
        transpositionTable = new TTEntry[entryCount]{};
        hashSize = entryCount - 1;
    }
}
