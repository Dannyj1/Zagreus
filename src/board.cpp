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

#include "board.h"

#include <utility>

namespace Zagreus {
template <Piece piece>
uint64_t Board::getBitboard() const {
    return bitboards[std::to_underlying(piece)];
}

template <PieceColor color>
uint64_t Board::getColorBitboard() const {
    return colorBoards[std::to_underlying(color)];
}

Piece Board::getPieceOnSquare(const int square) const {
    return board[square];
}

uint64_t Board::getOccupiedBitboard() const {
    return occupied;
}

uint64_t Board::getEmptyBitboard() const {
    return ~occupied;
}
} // namespace Zagreus
