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

#include "bitwise.h"

namespace Zagreus {
    uint64_t soutOne(uint64_t b) {
        return 0;
    }

    uint64_t nortOne(uint64_t b) {
        return 0;
    }

    uint64_t eastOne(uint64_t b) {
        return 0;
    }

    uint64_t noEaOne(uint64_t b) {
        return 0;
    }

    uint64_t soEaOne(uint64_t b) {
        return 0;
    }

    uint64_t westOne(uint64_t b) {
        return 0;
    }

    uint64_t soWeOne(uint64_t b) {
        return 0;
    }

    uint64_t noWeOne(uint64_t b) {
        return 0;
    }

    uint64_t noNoEa(uint64_t b) {
        return 0;
    }

    uint64_t noEaEa(uint64_t b) {
        return 0;
    }

    uint64_t soEaEa(uint64_t b) {
        return 0;
    }

    uint64_t soSoEa(uint64_t b) {
        return 0;
    }

    uint64_t noNoWe(uint64_t b) {
        return 0;
    }

    uint64_t noWeWe(uint64_t b) {
        return 0;
    }

    uint64_t soWeWe(uint64_t b) {
        return 0;
    }

    uint64_t soSoWe(uint64_t b) {
        return 0;
    }

    uint64_t soutOccl(uint64_t pieceBB, uint64_t empty) {
        return 0;
    }

    uint64_t nortOccl(uint64_t pieceBB, uint64_t empty) {
        return 0;
    }

    uint64_t eastOccl(uint64_t pieceBB, uint64_t empty) {
        return 0;
    }

    uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty) {
        return 0;
    }

    uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty) {
        return 0;
    }

    uint64_t westOccl(uint64_t rooks, uint64_t empty) {
        return 0;
    }

    uint64_t soWeOccl(uint64_t bishops, uint64_t empty) {
        return 0;
    }

    uint64_t noWeOccl(uint64_t bishops, uint64_t empty) {
        return 0;
    }

    template<PieceColor color>
    uint64_t getPawnEastAttacks(uint64_t pawns) {
        return 0;
    }

    template<PieceColor color>
    uint64_t getPawnWestAttacks(uint64_t pawns) {
        return 0;
    }

    uint64_t calculateKnightAttacks(uint64_t knights) {
        return 0;
    }

    uint64_t calculateKingAttacks(uint64_t kingSet) {
        return 0;
    }

    template<PieceColor color>
    uint64_t calculatePawnAttacks(uint64_t bb) {
        return getPawnEastAttacks<color>(bb) | getPawnWestAttacks<color>(bb);
    }

    template uint64_t calculatePawnAttacks<PieceColor::WHITE>(uint64_t);
    template uint64_t calculatePawnAttacks<PieceColor::BLACK>(uint64_t);
}