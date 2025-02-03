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

#include "move_picker.h"
#include <array>
#include <iostream>

#include "constants.h"
#include "tt.h"

namespace Zagreus {
/**
 * \brief Checks if there is a next move and retrieves it.
 * \param[out] move The next move if available.
 * \return True if there is a next move, false otherwise.
 */
bool MovePicker::next(Move& move) {
    if (currentIndex >= moveList.size) {
        return false;
    }

    move = moveList.moves[currentIndex];

    if (move == NO_MOVE) {
        return false;
    }

    currentIndex += 1;
    return true;
}

/**
 *\brief Resets the move picker to the beginning of the move list.
 */
void MovePicker::reset() {
    currentIndex = 0;
}

static TranspositionTable* tt = TranspositionTable::getTT();

void MovePicker::sort(Board& board) const {
    const PvLine& pvLine = board.getPreviousPvLine();
    const int currentPly = board.getPly();
    const int pvMoveIndex = currentPly - pvLine.startPly;

    if (pvLine.moveCount >= pvMoveIndex) {
        const Move pvMove = pvLine.moves[pvMoveIndex];

        if (pvMove != NO_MOVE) {
            for (int i = 0; i < moveList.size; i++) {
                if (moveList.moves[i] == pvMove) {
                    std::swap(moveList.moves[0], moveList.moves[i]);
                    break;
                }
            }
        }
    }

    /*uint64_t zobristHash = board.getZobristHash();
    TTEntry* entry = tt->getEntry(zobristHash);

    if (entry != nullptr) {
        Move bestMove = entry->bestMove;

        if (bestMove != NO_MOVE) {
            // Find the best move and put it at the front of the list
            for (int i = 0; i < moveList.size; i++) {
                if (moveList.moves[i] == bestMove) {
                    std::swap(moveList.moves[0], moveList.moves[i]);
                    break;
                }
            }
        }
    }*/
}
} // namespace Zagreus
