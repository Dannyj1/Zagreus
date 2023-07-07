/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <chrono>
#include <algorithm>
#include <iostream>

#include "../senjo/GoParams.h"
#include "types.h"
#include "engine.h"

namespace Zagreus {
    std::chrono::time_point<std::chrono::steady_clock> getEndTime(std::chrono::time_point<std::chrono::steady_clock> startTime, senjo::GoParams &params, Bitboard &bitboard, ZagreusEngine &engine, PieceColor movingColor) {
        if (params.infinite || params.depth > 0 || params.nodes > 0) {
            return std::chrono::time_point<std::chrono::steady_clock>::max();
        }

        if (params.movetime > 0) {
            return startTime + std::chrono::milliseconds(params.movetime - engine.getOption("MoveOverhead").getIntValue());
        }

        // We assume a match lasts 50 moves
        uint64_t moves = 50ULL;

        if (params.movestogo > 0) {
            moves = params.movestogo;
        }

        int movesToGo = moves - (bitboard.getPly() / 2ULL);

        if (movesToGo < 2) {
            movesToGo = 2;
        }

        uint64_t timeLeft = 0;
        uint64_t maxTime;

        if (movingColor == PieceColor::WHITE) {
            timeLeft += params.wtime;

            if (timeLeft > params.winc * 10) {
                timeLeft += params.winc;
            }

            maxTime = (params.wtime + params.winc) / 100 * 80;
        } else {
            timeLeft += params.btime;

            if (timeLeft > params.binc * 10) {
                timeLeft += params.binc;
            }

            maxTime = (params.btime + params.binc) / 100 * 80;
        }

        uint64_t moveOverhead = engine.getOption("MoveOverhead").getIntValue();
        if (moveOverhead && timeLeft >= moveOverhead + 1) {
            timeLeft -= moveOverhead;
        }

        timeLeft = std::max((uint64_t) timeLeft, (uint64_t) 1ULL);
        uint64_t timePerMove = timeLeft / movesToGo;

        // If we are in the opening, give us 25% more time
        if ((bitboard.getPly() / 2) < 15) {
            timePerMove += (timePerMove / 100 * 25);
        }

        if (timePerMove > maxTime) {
            timePerMove = maxTime;
        }

        std::cout << "Time per move: " << timePerMove << std::endl;
        timePerMove = std::max((uint64_t) timePerMove, (uint64_t) 1ULL);
        return startTime + std::chrono::milliseconds(timePerMove);
    }
}