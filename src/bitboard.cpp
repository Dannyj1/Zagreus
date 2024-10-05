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

#include "bitboard.h"

namespace Zagreus {
uint64_t bishopAttacks(const uint8_t square, const uint64_t occupied) {
    uint64_t occupancy = occupied;
    occupancy &= getBishopMask(square);
    occupancy *= getBishopMagic(square);
    occupancy >>= 64 - BBits[square];

    return getBishopMagicAttacks(square, occupancy);
}
} // namespace Zagreus
