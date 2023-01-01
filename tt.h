//
// Created by Danny on 30-4-2022.
//

#pragma once


#include <cstdint>
#include <map>
#include "bitboard.h"
#include "include/tsl/hopscotch_map.h"

namespace Chess {
    struct TTEntry {
        int score;
        int depth;
    };

    class TranspositionTable {
    private:
        std::map<uint64_t, TTEntry> table;
        std::vector<Move> killerMoves;
        std::vector<Move> pvMoves;
    public:
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
