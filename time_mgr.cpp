/*
//
// Created by Danny on 28-4-2022.
//

#include <chrono>
#include "time_mgr.h"

namespace Chess {
    std::chrono::time_point<std::chrono::system_clock> TimeManager::getEndTime(Board* board, PieceColor movingColor) {
        int movesLeft = 80 - board->getMovesMade();

        if (movesLeft < 6) {
            movesLeft = 6;
        }

        uint64_t timeLeft = movingColor == PieceColor::WHITE ? board->getWhiteTimeMsec() : board->getBlackTimeMsec();

        if (timeLeft == 0) {
            timeLeft = 300000;
        }

        uint64_t timePerMove = timeLeft / movesLeft;

        if (board->getMovesMade() <= 30) {
            timePerMove += 500 * (30 - board->getMovesMade());
        }

        if (timePerMove > timeLeft * 0.15) {
            timePerMove = timeLeft * 0.15;
        }

        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        now += std::chrono::milliseconds(timePerMove);
        return now;
    }
}
*/
