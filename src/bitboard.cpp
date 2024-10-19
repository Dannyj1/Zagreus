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
#include "magics.h"

namespace Zagreus {

// Lookup tables for attack patterns
static std::array<std::array<uint64_t, SQUARES>, COLORS> pawnAttacksTable{};
static std::array<uint64_t, SQUARES> knightAttacksTable{};
static std::array<uint64_t, SQUARES> kingAttacksTable{};

/**
 * \brief Initializes the attack lookup tables for pawns, knights, and kings.
 */
void initializeAttackLookupTables() {
    for (uint8_t square = 0; square < SQUARES; ++square) {
        const uint64_t bb = squareToBitboard(square);

        pawnAttacksTable[WHITE][square] = calculateWhitePawnAttacks(bb);
        pawnAttacksTable[BLACK][square] = calculateBlackPawnAttacks(bb);
        knightAttacksTable[square] = calculateKnightAttacks(bb);
        kingAttacksTable[square] = calculateKingAttacks(bb);
    }
}

/**
 * \brief Retrieves the pawn attacks for a given square and color.
 * \tparam color The color of the pawn (WHITE or BLACK).
 * \param square The square index (0-63).
 * \return A bitboard representing the attack pattern.
 */
template <PieceColor color>
uint64_t getPawnAttacks(const uint8_t square) {
    assert(square < SQUARES);
    return pawnAttacksTable[color][square];
}

// Explicit template instantiation for WHITE and BLACK pawns
template uint64_t getPawnAttacks<WHITE>(uint8_t square);
template uint64_t getPawnAttacks<BLACK>(uint8_t square);

/**
 * \brief Retrieves the knight attacks for a given square.
 * \param square The square index (0-63).
 * \return A bitboard representing the attack pattern.
 */
uint64_t getKnightAttacks(const uint8_t square) {
    assert(square < SQUARES);
    return knightAttacksTable[square];
}

/**
 * \brief Retrieves the king attacks for a given square.
 * \param square The square index (0-63).
 * \return A bitboard representing the attack pattern.
 */
uint64_t getKingAttacks(const uint8_t square) {
    assert(square < SQUARES);
    return kingAttacksTable[square];
}

/**
 * \brief Retrieves the bishop attacks for a given square and occupied squares.
 * \param square The square index (0-63).
 * \param occupied A bitboard representing the occupied squares.
 * \return A bitboard representing the attack pattern.
 */
uint64_t getBishopAttacks(const uint8_t square, uint64_t occupied) {
    assert(square < SQUARES);
    occupied &= getBishopMask(square);
    occupied *= getBishopMagic(square);
    occupied >>= 64 - BBits[square];

    return getBishopMagicAttacks(square, occupied);
}

/**
 * \brief Retrieves the rook attacks for a given square and occupied squares.
 * \param square The square index (0-63).
 * \param occupied A bitboard representing the occupied squares.
 * \return A bitboard representing the attack pattern.
 */
uint64_t getRookAttacks(const uint8_t square, uint64_t occupied) {
    assert(square < SQUARES);
    occupied &= getRookMask(square);
    occupied *= getRookMagic(square);
    occupied >>= 64 - RBits[square];

    return getRookMagicAttacks(square, occupied);
}

/**
 * \brief Retrieves the queen attacks for a given square and occupied squares.
 * \param square The square index (0-63).
 * \param occupied A bitboard representing the occupied squares.
 * \return A bitboard representing the attack pattern.
 */
uint64_t queenAttacks(const uint8_t square, const uint64_t occupied) {
    assert(square < SQUARES);
    return getBishopAttacks(square, occupied) | getRookAttacks(square, occupied);
}

} // namespace Zagreus