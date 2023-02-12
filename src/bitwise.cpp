/*
 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "bitwise.h"

namespace Zagreus {

    uint64_t soutOne(uint64_t b) {
        return b >> 8ULL;
    }

    uint64_t nortOne(uint64_t b) {
        return b << 8ULL;
    }

    uint64_t eastOne(uint64_t b) {
        return (b << 1ULL) & NOT_A_FILE;
    }

    uint64_t noEaOne(uint64_t b) {
        return (b << 9ULL) & NOT_A_FILE;
    }

    uint64_t soEaOne(uint64_t b) {
        return (b >> 7ULL) & NOT_A_FILE;
    }

    uint64_t westOne(uint64_t b) {
        return (b >> 1ULL) & NOT_H_FILE;
    }

    uint64_t soWeOne(uint64_t b) {
        return (b >> 9ULL) & NOT_H_FILE;
    }

    uint64_t noWeOne(uint64_t b) {
        return (b << 7ULL) & NOT_H_FILE;
    }

    uint64_t noNoEa(uint64_t b) {
        return (b << 17ULL) & NOT_A_FILE;
    }

    uint64_t noEaEa(uint64_t b) {
        return (b << 10ULL) & NOT_AB_FILE;
    }

    uint64_t soEaEa(uint64_t b) {
        return (b >> 6ULL) & NOT_AB_FILE;
    }

    uint64_t soSoEa(uint64_t b) {
        return (b >> 15ULL) & NOT_A_FILE;
    }

    uint64_t noNoWe(uint64_t b) {
        return (b << 15ULL) & NOT_H_FILE;
    }

    uint64_t noWeWe(uint64_t b) {
        return (b << 6ULL) & NOT_GH_FILE;
    }

    uint64_t nortFill(uint64_t gen) {
        gen |= (gen <<  8);
        gen |= (gen << 16);
        gen |= (gen << 32);

        return gen;
    }

    uint64_t soutFill(uint64_t gen) {
        gen |= (gen >>  8);
        gen |= (gen >> 16);
        gen |= (gen >> 32);

        return gen;
    }

    uint64_t whiteFrontSpans(uint64_t pawns) {
        return nortOne(nortFill(pawns));
    }

    uint64_t whiteRearSpans(uint64_t pawns) {
        return soutOne(soutFill(pawns));
    }

    uint64_t blackRearSpans(uint64_t pawns) {
        return nortOne(nortFill(pawns));
    }

    uint64_t blackFrontSpans(uint64_t pawns) {
        return soutOne(soutFill(pawns));
    }

    uint64_t soutOccl(uint64_t pieceBB, uint64_t empty) {
        pieceBB |= empty & (pieceBB >> 8ULL);
        empty &= (empty >> 8ULL);
        pieceBB |= empty & (pieceBB >> 16ULL);
        empty &= (empty >> 16ULL);
        pieceBB |= empty & (pieceBB >> 32ULL);
        return soutOne(pieceBB);
    }

    uint64_t nortOccl(uint64_t pieceBB, uint64_t empty) {
        pieceBB |= empty & (pieceBB << 8ULL);
        empty &= (empty << 8ULL);
        pieceBB |= empty & (pieceBB << 16ULL);
        empty &= (empty << 16ULL);
        pieceBB |= empty & (pieceBB << 32ULL);
        return nortOne(pieceBB);
    }

    uint64_t eastOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_A_FILE;
        pieceBB |= empty & (pieceBB << 1ULL);
        empty &= (empty << 1ULL);
        pieceBB |= empty & (pieceBB << 2ULL);
        empty &= (empty << 2ULL);
        pieceBB |= empty & (pieceBB << 4ULL);
        return eastOne(pieceBB);
    }

    uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_A_FILE;
        pieceBB |= empty & (pieceBB << 9ULL);
        empty &= (empty << 9ULL);
        pieceBB |= empty & (pieceBB << 18ULL);
        empty &= (empty << 18ULL);
        pieceBB |= empty & (pieceBB << 36ULL);
        return noEaOne(pieceBB);
    }

    uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_A_FILE;
        pieceBB |= empty & (pieceBB >> 7ULL);
        empty &= (empty >> 7ULL);
        pieceBB |= empty & (pieceBB >> 14ULL);
        empty &= (empty >> 14ULL);
        pieceBB |= empty & (pieceBB >> 28ULL);
        return soEaOne(pieceBB);
    }

    uint64_t westOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_H_FILE;
        pieceBB |= empty & (pieceBB >> 1ULL);
        empty &= (empty >> 1ULL);
        pieceBB |= empty & (pieceBB >> 2ULL);
        empty &= (empty >> 2ULL);
        pieceBB |= empty & (pieceBB >> 4ULL);
        return westOne(pieceBB);
    }

    uint64_t soWeOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_H_FILE;
        pieceBB |= empty & (pieceBB >> 9ULL);
        empty &= (empty >> 9ULL);
        pieceBB |= empty & (pieceBB >> 18ULL);
        empty &= (empty >> 18ULL);
        pieceBB |= empty & (pieceBB >> 36ULL);
        return soWeOne(pieceBB);
    }

    uint64_t noWeOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_H_FILE;
        pieceBB |= empty & (pieceBB << 7ULL);
        empty &= (empty << 7ULL);
        pieceBB |= empty & (pieceBB << 14ULL);
        empty &= (empty << 14ULL);
        pieceBB |= empty & (pieceBB << 28ULL);
        return noWeOne(pieceBB);
    }

    template<PieceColor color>
    uint64_t getPawnEastAttacks(uint64_t pawns) {
        if (color == WHITE) {
            return noEaOne(pawns);
        } else {
            return soEaOne(pawns);
        }
    }

    template<PieceColor color>
    uint64_t getPawnWestAttacks(uint64_t pawns) {
        if (color == WHITE) {
            return noWeOne(pawns);
        } else {
            return soWeOne(pawns);
        }
    }

    uint64_t calculateKnightAttacks(uint64_t knights) {
        uint64_t l1 = (knights >> 1ULL) & NOT_H_FILE;
        uint64_t l2 = (knights >> 2ULL) & NOT_GH_FILE;
        uint64_t r1 = (knights << 1ULL) & NOT_A_FILE;
        uint64_t r2 = (knights << 2ULL) & NOT_AB_FILE;
        uint64_t h1 = l1 | r1;
        uint64_t h2 = l2 | r2;
        return (h1 << 16ULL) | (h1 >> 16ULL) | (h2 << 8ULL) | (h2 >> 8ULL);
    }

    uint64_t calculateKingAttacks(uint64_t kingSet) {
        uint64_t attacks = eastOne(kingSet) | westOne(kingSet);
        kingSet |= attacks;
        attacks |= nortOne(kingSet) | soutOne(kingSet);

        return attacks;
    }

    template<PieceColor color>
    uint64_t calculatePawnAttacks(uint64_t bb) {
        return getPawnEastAttacks<color>(bb) | getPawnWestAttacks<color>(bb);
    }

    template uint64_t calculatePawnAttacks<PieceColor::WHITE>(uint64_t);
    template uint64_t calculatePawnAttacks<PieceColor::BLACK>(uint64_t);
}