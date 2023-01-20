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

#include "../senjo/GoParams.h"
#include "types.h"
#include "engine.h"

namespace Zagreus {
    std::chrono::time_point<std::chrono::high_resolution_clock> getEndTime(senjo::GoParams &params, ZagreusEngine &engine, PieceColor movingColor) {
        if (params.infinite || params.depth > 0 || params.nodes > 0) {
            return std::chrono::time_point<std::chrono::high_resolution_clock>::max();
        }

        if (params.movetime > 0) {
            return std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(params.movetime);
        }

        uint64_t movesToGo = std::min(params.movestogo, 80);
        movesToGo = std::max(movesToGo, 7ULL);
        uint64_t timeLeft = engine.getOption("Move Overhead").getIntValue();

        if (movingColor == PieceColor::WHITE) {
            timeLeft += params.wtime;
            timeLeft += params.winc;
        } else {
            timeLeft += params.btime;
            timeLeft += params.binc;
        }

        timeLeft = std::max(timeLeft, 1ULL);
        uint64_t maxTime = timeLeft / 100 * 80;
        uint64_t timePerMove = timeLeft / movesToGo;

        if (timePerMove > maxTime) {
            timePerMove = maxTime;
        }

        return std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(timePerMove);
    }
}