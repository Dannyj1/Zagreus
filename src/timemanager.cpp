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
        int moves = 50ULL;

        if (params.movestogo > 0) {
            moves = params.movestogo;
        }

        int movesToGo = moves - (bitboard.getPly() / 2);

        if (movesToGo < 3) {
            movesToGo = 3;
        }

        uint64_t timeLeft = 0;

        if (movingColor == PieceColor::WHITE) {
            timeLeft += params.wtime;

            if (timeLeft > params.winc * 10) {
                timeLeft += params.winc;
            }
        } else {
            timeLeft += params.btime;

            if (timeLeft > params.binc * 10) {
                timeLeft += params.binc;
            }
        }

        uint64_t moveOverhead = engine.getOption("MoveOverhead").getIntValue();
        if (moveOverhead) {
            if (timeLeft >= moveOverhead + 1) {
                timeLeft -= moveOverhead;
            } else {
                timeLeft -= timeLeft / 2;
            }
        }

        timeLeft = std::max((uint64_t) timeLeft, (uint64_t) 1ULL);
        uint64_t maxTime = timeLeft / 100 * 80;
        uint64_t timePerMove = timeLeft / movesToGo;

        if (timePerMove > maxTime) {
            timePerMove = maxTime;
        }

        timePerMove = std::max((uint64_t) timePerMove, (uint64_t) 1ULL);
        return startTime + std::chrono::milliseconds(timePerMove);
    }
}