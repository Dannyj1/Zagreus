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

#include "movepicker.h"

namespace Zagreus {
Move MovePicker::getNextMove() {
    int highestScore = moveList->moves[searchStartIndex].score;
    int moveIndex = searchStartIndex;

    for (int i = searchStartIndex + 1; i < moveList->size; i++) {
        int currentScore = moveList->moves[i].score;
        if (currentScore > highestScore) {
            highestScore = currentScore;
            moveIndex = i;
        }
    }

    std::swap(moveList->moves[searchStartIndex], moveList->moves[moveIndex]);
    searchStartIndex++;
    return moveList->moves[searchStartIndex - 1];
}

bool MovePicker::hasNext() { return searchStartIndex < moveList->size; }

int MovePicker::size() { return moveList->size; }

int MovePicker::remaining() { return moveList->size - searchStartIndex; }

int MovePicker::movesSearched() { return searchStartIndex; }
} // namespace Zagreus