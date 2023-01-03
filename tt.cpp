//
// Created by Danny on 30-4-2022.
//

#include <iostream>

#include "tt.h"
#include "bitboard.h"

namespace Zagreus {
    void TranspositionTable::addPosition(uint64_t zobristHash, int depth, int score) {
        uint32_t index = (zobristHash & 0x1FFFFFF);
        table[index] = TTEntry{score, depth, zobristHash};
    }

    bool TranspositionTable::isPositionInTable(uint64_t zobristHash, int depth) {
        uint32_t index = (zobristHash & 0x1FFFFFF);
        TTEntry entry = table[index];

        return entry.zobristHash == zobristHash && entry.depth >= depth;
    }

    int TranspositionTable::getPositionScore(uint64_t zobristHash) {
        uint32_t index = (zobristHash & 0x1FFFFFF);

        return table[index].score;
    }

    void TranspositionTable::clearKillerMoves() {
        killerMoves.clear();
    }

    void TranspositionTable::addKillerMove(Move move) {
        killerMoves.push_back(move);
    }

    bool TranspositionTable::isKillerMove(Move move) {
        for (const Move &killerMove : killerMoves) {
            if (killerMove.fromSquare == move.fromSquare && killerMove.toSquare == move.toSquare) {
                return true;
            }
        }

        return false;
    }

    bool TranspositionTable::isPositionInTable(uint64_t zobristHash) {
        uint32_t index = (zobristHash & 0x1FFFFFF);
        TTEntry entry = table[index];

        return entry.zobristHash == zobristHash;
    }

    void TranspositionTable::clearPVMoves() {
        pvMoves.clear();
    }

    void TranspositionTable::addPVMove(Move move) {
        pvMoves.push_back(move);
    }

    bool TranspositionTable::isPVMove(Move move) {
        for (const Move &pvMove : pvMoves) {
            if (pvMove.fromSquare == move.fromSquare && pvMove.toSquare == move.toSquare) {
                return true;
            }
        }

        return false;
    }

    TranspositionTable::TranspositionTable() {
        pvMoves.reserve(500);
        killerMoves.reserve(500);
    }
}
