/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
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

#include "timemanager.h"

#include <algorithm>

#include "../senjo/GoParams.h"
#include "engine.h"
#include "types.h"

namespace Zagreus {
std::chrono::time_point<std::chrono::steady_clock> getEndTime(TimeContext &context,
                                                              senjo::GoParams &params,
                                                              ZagreusEngine &engine,
                                                              PieceColor movingColor) {
  if (params.infinite || params.depth > 0 || params.nodes > 0) {
    return std::chrono::time_point<std::chrono::steady_clock>::max();
  }

  if (params.movetime > 0) {
    return context.startTime +
           std::chrono::milliseconds(params.movetime -
                                     engine.getOption("MoveOverhead").getIntValue());
  }

  int movesToGo = params.movestogo ? params.movestogo : 50ULL;

  uint64_t timeLeft = 0;

  if (movingColor == WHITE) {
    timeLeft += params.wtime;
    timeLeft += params.winc * movesToGo;
  } else {
    timeLeft += params.btime;
    timeLeft += params.binc * movesToGo;
  }

  uint64_t moveOverhead = engine.getOption("MoveOverhead").getIntValue();
  timeLeft -= moveOverhead * movesToGo;

  timeLeft = std::max((uint64_t)timeLeft, (uint64_t)1ULL);
  uint64_t maxTime;

  if (movingColor == WHITE) {
    if (params.wtime > moveOverhead) {
      maxTime = (params.wtime - moveOverhead) / 100 * 80;
    } else {
      maxTime = params.wtime / 2 / 100 * 80;
    }
  } else {
    if (params.btime > moveOverhead) {
      maxTime = (params.btime - moveOverhead) / 100 * 80;
    } else {
      maxTime = params.btime / 2 / 100 * 80;
    }
  }

  uint64_t timePerMove = timeLeft / movesToGo;

  if (context.rootMoveCount == 1) {
    timePerMove /= 2;
  }

  // Based on context.pvChanges, scale timePerMove between 1.0 and 1.5. After 5 or more move
  // changes, timePerMove will be 1.5 times as long.
  if (context.pvChanges > 0) {
    timePerMove =
        timePerMove * (1.0 + std::min(static_cast<double>(context.pvChanges), 5.0) / 10.0);
  }

  // if the score suddenly went from positive to negative or vice versa, increase timePerMove by 50%
  if (context.suddenScoreSwing) {
    timePerMove = timePerMove * 1.5;
  }

  // if the score suddenly dropped by 100cp or more, increase timePerMove by 50%
  if (context.suddenScoreDrop) {
    timePerMove = timePerMove * 1.5;
  }

  if (timePerMove > maxTime) {
    timePerMove = maxTime;
  }

  timePerMove = std::max((uint64_t)timePerMove, (uint64_t)1ULL);
  return context.startTime + std::chrono::milliseconds(timePerMove);
}
}  // namespace Zagreus
