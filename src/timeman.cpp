/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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

#include "timeman.h"

#include <algorithm>

namespace Zagreus {
template <PieceColor color>
TimeLimits calculateSearchTime(SearchParams& params) {
    const int timeLeft = color == WHITE ? params.whiteTime : params.blackTime;
    const int timeInc = color == WHITE ? params.whiteInc : params.blackInc;

    if (timeLeft <= 0) {
        return TimeLimits{};
    }

    const int usableTime = std::max(timeLeft - params.moveOverhead, 1);
    const int softMs = std::clamp(usableTime / 20 + timeInc / 2, 1, usableTime);
    const int hardMs = std::clamp(softMs * 2, softMs, usableTime);

    return TimeLimits{softMs, hardMs};
}

template TimeLimits calculateSearchTime<WHITE>(SearchParams& params);
template TimeLimits calculateSearchTime<BLACK>(SearchParams& params);
}  // namespace Zagreus