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

#include <cstdint>

#include "attacks.h"
#include "bitwise.h"
#include "magics.h"
#include "types.h"

static uint64_t kingAttacks[64]{};
static uint64_t knightAttacks[64]{};
static uint64_t pawnAttacks[2][64]{};
static uint64_t rayAttacks[8][64]{};

namespace Zagreus {
void intializeAttacks() {
    uint64_t sqBB = 1ULL;
    for (int8_t sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
        kingAttacks[sq] = calculateKingAttacks(sqBB) & ~sqBB;
    }

    sqBB = 1ULL;
    for (int8_t sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
        knightAttacks[sq] = calculateKnightAttacks(sqBB) & ~sqBB;
    }

    sqBB = 1ULL;
    for (int8_t sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
        pawnAttacks[WHITE][sq] = calculatePawnAttacks<WHITE>(sqBB) & ~sqBB;
        pawnAttacks[BLACK][sq] = calculatePawnAttacks<BLACK>(sqBB) & ~sqBB;
    }

    initializeRayAttacks();
}

void initializeRayAttacks() {
    uint64_t sqBB = 1ULL;

    for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
        rayAttacks[NORTH][sq] = nortOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[SOUTH][sq] = soutOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[EAST][sq] = eastOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[WEST][sq] = westOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[NORTH_EAST][sq] = noEaOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[NORTH_WEST][sq] = noWeOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[SOUTH_EAST][sq] = soEaOccl(sqBB, ~0ULL) & ~sqBB;
        rayAttacks[SOUTH_WEST][sq] = soWeOccl(sqBB, ~0ULL) & ~sqBB;
    }
}

template <PieceColor color>
uint64_t getPawnDoublePush(uint64_t pawns, uint64_t emptyBB) {
    const uint64_t singlePush = getPawnSinglePush<color>(pawns, emptyBB);

    if (color == WHITE) {
        return singlePush | (nortOne(singlePush) & emptyBB & RANK_4);
    }

    if (color == BLACK) {
        return singlePush | (soutOne(singlePush) & emptyBB & RANK_5);
    }

    return 0;
}

template uint64_t getPawnDoublePush<WHITE>(uint64_t pawns, uint64_t emptyBB);
template uint64_t getPawnDoublePush<BLACK>(uint64_t pawns, uint64_t emptyBB);

uint64_t getPawnAttacks(int8_t square, PieceColor color) {
    return pawnAttacks[color][square];
}

template <PieceColor color>
uint64_t getPawnAttacks(int8_t square) {
    return pawnAttacks[color][square];
}

template uint64_t getPawnAttacks<WHITE>(int8_t square);
template uint64_t getPawnAttacks<BLACK>(int8_t square);

template <PieceColor color>
uint64_t getPawnSinglePush(uint64_t pawns, uint64_t emptyBB) {
    if (color == WHITE) {
        return nortOne(pawns) & emptyBB;
    }

    if (color == BLACK) {
        return soutOne(pawns) & emptyBB;
    }

    return 0;
}

template uint64_t getPawnSinglePush<WHITE>(uint64_t pawns, uint64_t emptyBB);
template uint64_t getPawnSinglePush<BLACK>(uint64_t pawns, uint64_t emptyBB);

uint64_t getKingAttacks(int8_t square) {
    return kingAttacks[square];
}

uint64_t getKnightAttacks(int8_t square) {
    return knightAttacks[square];
}

uint64_t getQueenAttacks(int8_t square, uint64_t occupancy) {
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}

uint64_t getBishopAttacks(int8_t square, uint64_t occupancy) {
    occupancy &= getBishopMask(square);
    occupancy *= getBishopMagic(square);
    occupancy >>= 64 - BBits[square];

    return getBishopMagicAttacks(square, occupancy);
}

uint64_t getRookAttacks(int8_t square, uint64_t occupancy) {
    occupancy &= getRookMask(square);
    occupancy *= getRookMagic(square);
    occupancy >>= 64 - RBits[square];

    return getRookMagicAttacks(square, occupancy);
}

uint64_t getRayAttack(int8_t square, Direction direction) {
    return rayAttacks[direction][square];
}
} // namespace Zagreus
