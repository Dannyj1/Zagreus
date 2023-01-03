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
        int depth;
        uint64_t zobristHash;
    };

    class TranspositionTable {
    private:
        TTEntry table[1 << 25]{};
        std::vector<Move> killerMoves{};
        std::vector<Move> pvMoves{};
    public:
        TranspositionTable();

        void addPosition(uint64_t zobristHash, int depth, int score);

        bool isPositionInTable(uint64_t zobristHash);

        bool isPositionInTable(uint64_t zobristHash, int depth);

        int getPositionScore(uint64_t zobristHash);

        void clearKillerMoves();

        void addKillerMove(Move move);

        bool isKillerMove(Move move);

        void clearPVMoves();

        void addPVMove(Move move);

        bool isPVMove(Move move);
    };

    static TranspositionTable tt{};
}
