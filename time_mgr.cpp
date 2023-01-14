/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <chrono>

#include "time_mgr.h"
#include "bitboard.h"
#include "engine.h"

namespace Zagreus {
    std::chrono::time_point<std::chrono::high_resolution_clock>
    TimeManager::getEndTime(ZagreusEngine &engine, Bitboard &bitboard, PieceColor movingColor) {
        int movesLeft = 100 - bitboard.getPly();
        uint64_t timeLeft =
                movingColor == PieceColor::WHITE ? bitboard.getWhiteTimeMsec() : bitboard.getBlackTimeMsec();
        timeLeft -= engine.getOption("Move Overhead").getIntValue();
        uint64_t increment =
                movingColor == PieceColor::WHITE ? bitboard.getWhiteTimeIncrement() : bitboard.getBlackTimeIncrement();

        timeLeft += increment / 2;

        if (movesLeft < 7) {
            movesLeft = 7;
        }

        if (timeLeft == 0) {
            timeLeft = 100;
        }

        uint64_t timePerMove = timeLeft / movesLeft;

        if (bitboard.getPly() <= 25) {
            timePerMove *= 2;
        }

        if (bitboard.getPly() >= 50) {
            timePerMove /= 2;
        }

        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        now += std::chrono::milliseconds(timePerMove);
        return now;
    }
}
