//
// Created by Danny on 30-4-2022.
//

#include <iostream>
#include "tt.h"

namespace Chess {
    void TranspositionTable::addPosition(uint64_t zobristHash, int depth, int score) {
        if (table.contains(zobristHash)) {
            TTEntry entry = table.at(zobristHash);

            if (entry.depth >= depth) {
                table.erase(zobristHash);
                table.insert({zobristHash, {score, depth}});
            }
        } else {
            table.insert({zobristHash, {score, depth}});
        }
    }

    bool TranspositionTable::isPositionInTable(uint64_t zobristHash, int depth) {
        if (table.contains(zobristHash)) {
            TTEntry entry = table.at(zobristHash);

            if (entry.depth >= depth) {
                return true;
            }
        }

        return false;
    }

    int TranspositionTable::getPositionScore(uint64_t zobristHash) {
        if (!table.contains(zobristHash)) {
            return 0;
        }

        return table.at(zobristHash).score;
    }

    void TranspositionTable::clearKillerMoves() {
        killerMoves.clear();
    }

    void TranspositionTable::addKillerMove(const Move &move) {
        killerMoves.push_back(move);
    }

    bool TranspositionTable::isKillerMove(const Move &move) {
        return std::find(killerMoves.begin(), killerMoves.end(), move) != killerMoves.end();
    }

    bool TranspositionTable::isPositionInTable(uint64_t zobristHash) {
        return table.contains(zobristHash);
    }

    void TranspositionTable::clearPVMoves() {
        pvMoves.clear();
    }

    void TranspositionTable::addPVMove(const Move &move) {
        pvMoves.push_back(move);
    }

    bool TranspositionTable::isPVMove(const Move &move) {
        return std::find(pvMoves.begin(), pvMoves.end(), move) != pvMoves.end();
    }

}
