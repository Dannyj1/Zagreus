//
// Created by Danny on 30-4-2022.
//

#pragma once


#include <cstdint>
#include <map>
#include "board.h"

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

        void addKillerMove(const Move& move);

        bool isKillerMove(const Move& move);

        void clearPVMoves();

        void addPVMove(const Move& move);

        bool isPVMove(const Move& move);
    };

    static TranspositionTable tt{};
}
