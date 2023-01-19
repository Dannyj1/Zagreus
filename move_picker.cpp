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

#include "move_picker.h"

namespace Zagreus {

    MovePicker::MovePicker(std::vector<Move> moves) {
        this->moves = moves;
    }

    Move MovePicker::getNextMove() {
        int highestScore = moves[searchStartIndex].score;
        int moveIndex = searchStartIndex;
        Move move = moves[searchStartIndex];

        for (int i = searchStartIndex; i < moves.size(); i++) {
            if (moves[i].score > highestScore) {
                highestScore = moves[i].score;
                move = moves[i];
                moveIndex = i;
            }
        }

        Move temp = moves[searchStartIndex];
        moves[searchStartIndex] = move;
        moves[moveIndex] = temp;

        searchStartIndex++;

        return move;
    }

    bool MovePicker::hasNext() {
        return searchStartIndex < moves.size();
    }

    int MovePicker::size() {
        return moves.size();
    }
}