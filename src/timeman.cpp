/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

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

namespace Zagreus {
template <PieceColor color>
int calculateSearchTime(SearchParams& params, int movesPlayed) {
    int msLeft = color == WHITE ? params.whiteTime : params.blackTime;

    if (msLeft > 0) {
        // wtime or btime was set, so we use that
        if (movesPlayed < 40) {
            return msLeft / (50 - movesPlayed);
        } else {
            return msLeft / 10;
        }
    } else {
        return std::numeric_limits<int>::max();
    }
}

template int calculateSearchTime<WHITE>(SearchParams& params, int movesPlayed);
template int calculateSearchTime<BLACK>(SearchParams& params, int movesPlayed);

} // namespace Zagreus
