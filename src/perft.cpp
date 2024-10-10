/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

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

#include "perft.h"

#include <iostream>

#include "move.h"
#include "move_gen.h"

namespace Zagreus {
uint64_t perft(Board &board, const int depth, bool printNodes) {
    assert(depth >= 0);

    if (depth == 0) {
        return 1;
    }

    uint64_t nodes = 0;
    MoveList moveList{};
    const PieceColor sideToMove = board.getSideToMove();

    if (sideToMove == WHITE) {
        generateMoves<WHITE, ALL>(board, moveList);
    } else {
        generateMoves<BLACK, ALL>(board, moveList);
    }

    for (int i = 0; i < moveList.size; i++) {
        board.makeMove(moveList.moves[i]);

        if (sideToMove == WHITE) {
            if (!board.isPositionLegal<WHITE>()) {
                board.unmakeMove();
                continue;
            }
        } else {
            if (!board.isPositionLegal<BLACK>()) {
                board.unmakeMove();
                continue;
            }
        }

        uint64_t perftNodes = perft(board, depth - 1, false);

        if (printNodes) {
            std::string moveNotation = getMoveNotation(moveList.moves[i]);

            std::cout << moveNotation << ": " << perftNodes << std::endl;
        }

        nodes += perftNodes;
        board.unmakeMove();
    }

    return nodes;
}
} // namespace Zagreus

