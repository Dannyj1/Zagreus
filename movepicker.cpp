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

#include "movepicker.h"

namespace Zagreus {

    MovePicker::MovePicker(MoveList moves) {
        this->moveList = moves;
    }

    Move MovePicker::getNextMove() {
        int highestScore = moveList.moves[searchStartIndex].score;
        int moveIndex = searchStartIndex;
        Move move = moveList.moves[searchStartIndex];

        for (int i = searchStartIndex; i < moveList.count; i++) {
            if (moveList.moves[i].score > highestScore) {
                highestScore = moveList.moves[i].score;
                move = moveList.moves[i];
                moveIndex = i;
            }
        }

        Move temp = moveList.moves[searchStartIndex];
        moveList.moves[searchStartIndex] = move;
        moveList.moves[moveIndex] = temp;

        searchStartIndex++;

        return move;
    }

    bool MovePicker::hasNext() const {
        return searchStartIndex < moveList.count;
    }

    int MovePicker::size() {
        return moveList.count;
    }
}