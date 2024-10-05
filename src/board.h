
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

#pragma once

#include <array>
#include <cassert>
#include <utility>

#include "bitboard.h"

namespace Zagreus {
class Board {
private:
    std::array<Piece, SQUARES> board{};
    std::array<uint64_t, PIECES> bitboards{};
    uint64_t occupied = 0;
    std::array<uint64_t, COLORS> colorBoards{};

public:
    template <Piece piece>
    [[nodiscard]] uint64_t getBitboard() const {
        return bitboards[std::to_underlying(piece)];
    }

    template <PieceColor color>
    [[nodiscard]] uint64_t getColorBitboard() const {
        return colorBoards[std::to_underlying(color)];
    }

    [[nodiscard]] Piece getPieceOnSquare(const int square) const {
        assert(square >= 0 && square < 64);
        return board[square];
    }

    [[nodiscard]] uint64_t getOccupiedBitboard() const {
        return occupied;
    }

    [[nodiscard]] uint64_t getEmptyBitboard() const {
        return ~occupied;
    }
};
} // namespace Zagreus
