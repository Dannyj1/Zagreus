/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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

#include <algorithm>
#include <cmath>

#include "constants.h"

namespace Zagreus {
void TranspositionTable::savePosition(const uint64_t zobristHash, const int16_t depth, const int ply, int score,
                                      const Move bestMove, const TTNodeType nodeType) const {
    const uint64_t index = zobristHash & hashSize;
    TTEntry* entry = &transpositionTable[index];

    // Only replace the entry if:
    // 1. Validation hash is 0 (the entry is empty)
    // 2. Entry is from a shallower depth (lower depth value)
    if (entry->zobristHash != zobristHash || entry->depth <= depth) {
        if (score >= (MATE_SCORE - MAX_PLIES)) {
            score += ply;
        } else if (score <= (-MATE_SCORE + MAX_PLIES)) {
            score -= ply;
        }

        score = std::clamp<int>(score, INT16_MIN + 1, INT16_MAX);

        Move moveToSave = bestMove;
        if (bestMove == NO_MOVE && entry->zobristHash == zobristHash) {
            moveToSave = entry->bestMove;
        }

        entry->zobristHash = zobristHash;
        entry->depth = depth;
        entry->bestMove = moveToSave;
        entry->score = score;
        entry->nodeType = nodeType;
    }
}

int16_t TranspositionTable::probePosition(const uint64_t zobristHash, const int16_t depth, const int alpha,
                                          const int beta, const int ply, TTEntry*& ttEntry) const {
    const uint64_t index = zobristHash & hashSize;
    TTEntry* entry = &transpositionTable[index];

    if (entry->zobristHash == zobristHash) {
        ttEntry = entry;

        if (entry->depth >= depth) {
            bool returnScore = false;

            if (entry->nodeType == EXACT) {
                returnScore = true;
            } else if (entry->nodeType == ALPHA) {
                if (entry->score <= alpha) {
                    returnScore = true;
                }
            } else if (entry->nodeType == BETA) {
                if (entry->score >= beta) {
                    returnScore = true;
                }
            }

            if (returnScore) {
                int adjustedScore = entry->score;

                if (adjustedScore >= (MATE_SCORE - MAX_PLIES)) {
                    adjustedScore -= ply;
                } else if (adjustedScore <= (-MATE_SCORE + MAX_PLIES)) {
                    adjustedScore += ply;
                }

                return adjustedScore;
            }
        }
    }

    return NO_TT_SCORE;
}

TTEntry* TranspositionTable::getEntry(const uint64_t zobristHash) const {
    const uint64_t index = zobristHash & hashSize;
    TTEntry* entry = &transpositionTable[index];

    // Check full Zobrist hash to ensure positions match
    if (entry->zobristHash == zobristHash) {
        return entry;
    }

    return nullptr;
}

void TranspositionTable::setTableSize(int megaBytes) {
    if ((megaBytes & (megaBytes - 1)) != 0) {
        megaBytes = 1 << static_cast<int>(log2(megaBytes));
    }

    const uint64_t byteSize = static_cast<uint64_t>(megaBytes) * 1024 * 1024;
    const uint64_t entryCount = byteSize / sizeof(TTEntry);

    delete[] transpositionTable;
    transpositionTable = new TTEntry[entryCount]{};

    hashSize = entryCount - 1;
}

TranspositionTable* TranspositionTable::getTT() {
    static TranspositionTable instance{};
    return &instance;
}

template <PieceColor color>
void TranspositionTable::updateHistory(const Move move, const int value) {
    const Square from = getFromSquare(move);
    const Square to = getToSquare(move);
    const int clampedValue = std::clamp(value, -MAX_HISTORY, MAX_HISTORY);

    history[color][from][to] += clampedValue - history[color][from][to] * std::abs(clampedValue) / MAX_HISTORY;
}

void TranspositionTable::updateCaptureHistory(const Move move, const Piece movedPiece, const Piece capturedPiece,
                                              const int value) {
    const Square to = getToSquare(move);
    const int clampedValue = std::clamp(value, -MAX_HISTORY, MAX_HISTORY);

    captureHistory[movedPiece][to][capturedPiece] +=
        clampedValue - captureHistory[movedPiece][to][capturedPiece] * std::abs(clampedValue) / MAX_HISTORY;
}

template void TranspositionTable::updateHistory<WHITE>(Move move, int value);
template void TranspositionTable::updateHistory<BLACK>(Move move, int value);
}  // namespace Zagreus