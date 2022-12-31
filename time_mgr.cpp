//
// Created by Danny on 28-4-2022.
//

#include <chrono>
#include "time_mgr.h"
#include "bitboard.h"

namespace Chess {
    std::chrono::time_point<std::chrono::high_resolution_clock> TimeManager::getEndTime(Bitboard bitboard, PieceColor movingColor) {
        int movesLeft = 80 - bitboard.getMovesMade();

        if (movesLeft < 5) {
            movesLeft = 5;
        }

        uint64_t timeLeft = movingColor == PieceColor::WHITE ? bitboard.getWhiteTimeMsec() : bitboard.getBlackTimeMsec();
        uint64_t timePerMove = timeLeft / movesLeft;

        if (bitboard.getMovesMade() <= 30) {
            timePerMove += 500 * (25 - bitboard.getMovesMade());
        }

/*        if (timePerMove > timeLeft * 0.15) {
            timePerMove = timeLeft * 0.15;
        }*/

        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        now += std::chrono::milliseconds(timePerMove);
        return now;
    }
}
