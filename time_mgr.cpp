//
// Created by Danny on 28-4-2022.
//

#include <chrono>
#include "time_mgr.h"
#include "bitboard.h"

namespace Zagreus {
    std::chrono::time_point<std::chrono::high_resolution_clock>
    TimeManager::getEndTime(Bitboard &bitboard, PieceColor movingColor) {
        int movesLeft = 80 - bitboard.getPly();
        uint64_t timeLeft =
                movingColor == PieceColor::WHITE ? bitboard.getWhiteTimeMsec() : bitboard.getBlackTimeMsec();
        timeLeft -= 100;
        uint64_t increment =
                movingColor == PieceColor::WHITE ? bitboard.getWhiteTimeIncrement() : bitboard.getBlackTimeIncrement();

        timeLeft += increment / 2;

        if (movesLeft < 6) {
            movesLeft = 6;
        }

        if (timeLeft == 0) {
            timeLeft = 100;
        }

        uint64_t timePerMove = timeLeft / movesLeft;

        if (bitboard.getPly() <= 25) {
            timePerMove *= 2;
        }

        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        now += std::chrono::milliseconds(timePerMove);
        return now;
    }
}
