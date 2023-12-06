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

#pragma once

#include <chrono>

#include "../senjo/GoParams.h"
#include "engine.h"
#include "types.h"

namespace Zagreus {
struct TimeContext {
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    int pvChanges = 0;
    int rootMoveCount = 0;
    // A boolean variable that keeps track if the score suddenly went from positive to negative or
    // vice versa
    bool suddenScoreSwing = false;
    // A boolean variable that keeps track if the score suddenly had a big drop (-150 or more)
    bool suddenScoreDrop = false;
};

std::chrono::time_point<std::chrono::steady_clock> getEndTime(TimeContext& context,
                                                              senjo::GoParams& params,
                                                              ZagreusEngine& engine,
                                                              PieceColor movingColor);
} // namespace Zagreus
