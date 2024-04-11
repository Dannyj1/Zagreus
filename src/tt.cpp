/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "tt.h"

#include <cmath>
#include <iostream>

#include "search.h"

namespace Zagreus {
void TranspositionTable::addPosition(uint64_t zobristHash, int16_t depth, int score,
                                     TTNodeType nodeType, uint32_t bestMoveCode, int ply,
                                     SearchContext& context) {
    // current time
    auto currentTime = std::chrono::steady_clock::now();
    if (score > MAX_POSITIVE || score < MAX_NEGATIVE || currentTime > context.endTime) {
        return;
    }

    if (depth < INT8_MIN || depth > INT8_MAX) {
        return;
    }

    uint64_t index = zobristHash & hashSize;
    TTEntry* entry = &transpositionTable[index];

    if (depth > entry->depth) {
        int adjustedScore = score;

        if (adjustedScore >= (MATE_SCORE - MAX_PLY)) {
            adjustedScore += ply;
        } else if (adjustedScore <= (-MATE_SCORE + MAX_PLY)) {
            adjustedScore -= ply;
        }

        entry->validationHash = zobristHash >> 32;
        entry->depth = static_cast<int8_t>(depth);
        entry->bestMoveCode = bestMoveCode;
        entry->score = adjustedScore;
        entry->nodeType = nodeType;
    }
}

int TranspositionTable::getScore(uint64_t zobristHash, int16_t depth, int alpha, int beta,
                                 int ply) {
    if (depth < INT8_MIN || depth > INT8_MAX) {
        return INT32_MIN;
    }

    uint64_t index = zobristHash & hashSize;
    // Most significant 16 bits of the zobrist hash
    uint32_t validationHash = zobristHash >> 32;
    TTEntry* entry = &transpositionTable[index];

    if (entry->validationHash == validationHash && entry->depth >= depth) {
        bool returnScore = false;

        if (entry->nodeType == EXACT_NODE) {
            returnScore = true;
        } else if (entry->nodeType == FAIL_LOW_NODE) {
            if (entry->score <= alpha) {
                returnScore = true;
            }
        } else if (entry->nodeType == FAIL_HIGH_NODE) {
            if (entry->score >= beta) {
                returnScore = true;
            }
        }

        if (returnScore) {
            int adjustedScore = entry->score;

            if (adjustedScore >= MATE_SCORE) {
                adjustedScore -= ply;
            } else if (adjustedScore <= -MATE_SCORE) {
                adjustedScore += ply;
            }

            return adjustedScore;
        }
    }

    return INT32_MIN;
}

TTEntry* TranspositionTable::getEntry(uint64_t zobristHash) {
    uint64_t index = zobristHash & hashSize;

    return &transpositionTable[index];
}

void TranspositionTable::setTableSize(int megaBytes) {
    if ((megaBytes & (megaBytes - 1)) != 0) {
        megaBytes = 1 << static_cast<int>(log2(megaBytes));
    }

    uint64_t byteSize = megaBytes * 1024 * 1024;
    uint64_t entryCount = byteSize / sizeof(TTEntry);

    delete[] transpositionTable;
    transpositionTable = new TTEntry[entryCount]{};
    hashSize = entryCount - 1;

    for (uint64_t i = 0; i < entryCount; i++) {
        transpositionTable[i] = {};
    }
}

TranspositionTable* TranspositionTable::getTT() {
    static TranspositionTable instance{};
    return &instance;
}

void TranspositionTable::ageHistoryTable() {
    for (int i = 0; i < PIECE_TYPES; i++) {
        for (int j = 0; j < SQUARES; j++) {
            historyMoves[i][j] /= 8;
        }
    }
}
} // namespace Zagreus