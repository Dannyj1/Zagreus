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

#include "bitwise.h"

#include <iostream>
#include <random>
#include <algorithm>

namespace Zagreus {
static uint64_t kingAttacks[64]{};
static uint64_t knightAttacks[64]{};
static uint64_t pawnAttacks[2][64]{};
static uint64_t rayAttacks[8][64]{};
static uint64_t betweenTable[64][64]{};

static uint64_t zobristPieceConstants[PIECE_TYPES][SQUARES]{};
static uint64_t zobristMovingColorConstant{};
static uint64_t zobristCastleConstants[4]{};
static uint64_t zobristEnPassantConstants[8]{};

uint64_t soutOne(uint64_t b) { return b >> 8ULL; }

uint64_t nortOne(uint64_t b) { return b << 8ULL; }

uint64_t eastOne(uint64_t b) { return b << 1ULL & NOT_A_FILE; }

uint64_t noEaOne(uint64_t b) { return b << 9ULL & NOT_A_FILE; }

uint64_t soEaOne(uint64_t b) { return b >> 7ULL & NOT_A_FILE; }

uint64_t westOne(uint64_t b) { return b >> 1ULL & NOT_H_FILE; }

uint64_t soWeOne(uint64_t b) { return b >> 9ULL & NOT_H_FILE; }

uint64_t noWeOne(uint64_t b) { return b << 7ULL & NOT_H_FILE; }

uint64_t noNoEa(uint64_t b) { return b << 17ULL & NOT_A_FILE; }

uint64_t noEaEa(uint64_t b) { return b << 10ULL & NOT_AB_FILE; }

uint64_t soEaEa(uint64_t b) { return b >> 6ULL & NOT_AB_FILE; }

uint64_t soSoEa(uint64_t b) { return b >> 15ULL & NOT_A_FILE; }

uint64_t noNoWe(uint64_t b) { return b << 15ULL & NOT_H_FILE; }

uint64_t noWeWe(uint64_t b) { return b << 6ULL & NOT_GH_FILE; }

uint64_t nortFill(uint64_t gen) {
    gen |= gen << 8;
    gen |= gen << 16;
    gen |= gen << 32;

    return gen;
}

uint64_t soutFill(uint64_t gen) {
    gen |= gen >> 8;
    gen |= gen >> 16;
    gen |= gen >> 32;

    return gen;
}

uint64_t whiteFrontSpans(uint64_t pawns) { return nortOne(nortFill(pawns)); }

uint64_t whiteRearSpans(uint64_t pawns) { return soutOne(soutFill(pawns)); }

uint64_t blackRearSpans(uint64_t pawns) { return nortOne(nortFill(pawns)); }

uint64_t blackFrontSpans(uint64_t pawns) { return soutOne(soutFill(pawns)); }

uint64_t soutOccl(uint64_t pieceBB, uint64_t empty) {
    pieceBB |= empty & pieceBB >> 8ULL;
    empty &= empty >> 8ULL;
    pieceBB |= empty & pieceBB >> 16ULL;
    empty &= empty >> 16ULL;
    pieceBB |= empty & pieceBB >> 32ULL;
    return soutOne(pieceBB);
}

uint64_t nortOccl(uint64_t pieceBB, uint64_t empty) {
    pieceBB |= empty & pieceBB << 8ULL;
    empty &= empty << 8ULL;
    pieceBB |= empty & pieceBB << 16ULL;
    empty &= empty << 16ULL;
    pieceBB |= empty & pieceBB << 32ULL;
    return nortOne(pieceBB);
}

uint64_t eastOccl(uint64_t pieceBB, uint64_t empty) {
    empty &= NOT_A_FILE;
    pieceBB |= empty & pieceBB << 1ULL;
    empty &= empty << 1ULL;
    pieceBB |= empty & pieceBB << 2ULL;
    empty &= empty << 2ULL;
    pieceBB |= empty & pieceBB << 4ULL;
    return eastOne(pieceBB);
}

uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty) {
    empty &= NOT_A_FILE;
    pieceBB |= empty & pieceBB << 9ULL;
    empty &= empty << 9ULL;
    pieceBB |= empty & pieceBB << 18ULL;
    empty &= empty << 18ULL;
    pieceBB |= empty & pieceBB << 36ULL;
    return noEaOne(pieceBB);
}

uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty) {
    empty &= NOT_A_FILE;
    pieceBB |= empty & pieceBB >> 7ULL;
    empty &= empty >> 7ULL;
    pieceBB |= empty & pieceBB >> 14ULL;
    empty &= empty >> 14ULL;
    pieceBB |= empty & pieceBB >> 28ULL;
    return soEaOne(pieceBB);
}

uint64_t westOccl(uint64_t pieceBB, uint64_t empty) {
    empty &= NOT_H_FILE;
    pieceBB |= empty & pieceBB >> 1ULL;
    empty &= empty >> 1ULL;
    pieceBB |= empty & pieceBB >> 2ULL;
    empty &= empty >> 2ULL;
    pieceBB |= empty & pieceBB >> 4ULL;
    return westOne(pieceBB);
}

uint64_t soWeOccl(uint64_t pieceBB, uint64_t empty) {
    empty &= NOT_H_FILE;
    pieceBB |= empty & pieceBB >> 9ULL;
    empty &= empty >> 9ULL;
    pieceBB |= empty & pieceBB >> 18ULL;
    empty &= empty >> 18ULL;
    pieceBB |= empty & pieceBB >> 36ULL;
    return soWeOne(pieceBB);
}

uint64_t noWeOccl(uint64_t pieceBB, uint64_t empty) {
    empty &= NOT_H_FILE;
    pieceBB |= empty & pieceBB << 7ULL;
    empty &= empty << 7ULL;
    pieceBB |= empty & pieceBB << 14ULL;
    empty &= empty << 14ULL;
    pieceBB |= empty & pieceBB << 28ULL;
    return noWeOne(pieceBB);
}

template <PieceColor color>
uint64_t calculatePawnEastAttacks(uint64_t pawns) {
    if (color == WHITE) {
        return noEaOne(pawns);
    }
    return soEaOne(pawns);
}

template <PieceColor color>
uint64_t calculatePawnWestAttacks(uint64_t pawns) {
    if (color == WHITE) {
        return noWeOne(pawns);
    }
    return soWeOne(pawns);
}

uint64_t calculateKnightAttacks(uint64_t knights) {
    uint64_t l1 = knights >> 1ULL & NOT_H_FILE;
    uint64_t l2 = knights >> 2ULL & NOT_GH_FILE;
    uint64_t r1 = knights << 1ULL & NOT_A_FILE;
    uint64_t r2 = knights << 2ULL & NOT_AB_FILE;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    return h1 << 16ULL | h1 >> 16ULL | h2 << 8ULL | h2 >> 8ULL;
}

uint64_t calculateKingAttacks(uint64_t kingSet) {
    uint64_t attacks = eastOne(kingSet) | westOne(kingSet);
    kingSet |= attacks;
    attacks |= nortOne(kingSet) | soutOne(kingSet);

    return attacks;
}

void initializeBitboardConstants() {
    std::mt19937_64 gen(0x6C7CCC580A348E7BULL);
    std::uniform_int_distribution<uint64_t> dis(1ULL, UINT64_MAX);
    std::vector<uint64_t> generatedZobristConstants(ZOBRIST_CONSTANT_SIZE);

    for (int pieceType = 0; pieceType < PIECE_TYPES; pieceType++) {
        for (int square = 0; square < SQUARES; square++) {
            uint64_t zobristConstant = dis(gen);
            zobristPieceConstants[pieceType][square] = zobristConstant;

            // if constant already generated, generate a new one
            if (std::ranges::find(generatedZobristConstants, zobristConstant) !=
                generatedZobristConstants.end()) {
                zobristConstant = dis(gen);
            }

            generatedZobristConstants.push_back(zobristConstant);
        }
    }

    zobristMovingColorConstant = dis(gen);

    for (int i = 0; i < 4; i++) {
        zobristCastleConstants[i] = dis(gen);
    }

    for (int i = 0; i < 8; i++) {
        zobristEnPassantConstants[i] = dis(gen);
    }

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

    initializeBetweenLookup();
    initializeRayAttacks();
}

void initializeBetweenLookup() {
    for (int from = 0; from < 64; from++) {
        for (int to = 0; to < 64; to++) {
            uint64_t m1 = -1ULL;
            uint64_t a2a7 = 0x0001010101010100ULL;
            uint64_t b2g7 = 0x0040201008040200ULL;
            uint64_t h1b7 = 0x0002040810204080ULL;
            uint64_t btwn, line, rank, file;

            btwn = m1 << from ^ m1 << to;
            file = (to & 7) - (from & 7);
            rank = ((to | 7) - from) >> 3;
            line = (file & 7) - 1 & a2a7; /* a2a7 if same file */
            line += 2 * (((rank & 7) - 1) >> 58); /* b1g1 if same rank */
            line += (rank - file & 15) - 1 & b2g7; /* b2g7 if same diagonal */
            line += (rank + file & 15) - 1 & h1b7; /* h1b7 if same antidiag */
            line *= btwn & -btwn; /* mul acts like shift by smaller square */

            betweenTable[from][to] = line & btwn; /* return the bits on that line in-between */
        }
    }
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

uint64_t getRayAttack(int8_t square, Direction direction) {
    return rayAttacks[direction][square];
}

uint64_t getKingAttacks(int8_t square) {
    return kingAttacks[square];
}

uint64_t getKnightAttacks(int8_t square) {
    return knightAttacks[square];
}

uint64_t getBetweenSquares(int8_t from, int8_t to) {
    return betweenTable[from][to];
}

uint64_t getPieceZobristConstant(PieceType pieceType, int8_t square) {
    return zobristPieceConstants[pieceType][square];
}

uint64_t getMovingColorZobristConstant() {
    return zobristMovingColorConstant;
}

uint64_t getCastleZobristConstant(uint8_t index) {
    return zobristCastleConstants[index];
}

uint64_t getEnPassantZobristConstant(uint8_t file) {
    return zobristEnPassantConstants[file];
}

template <PieceColor color>
uint64_t calculatePawnAttacks(uint64_t bb) {
    return calculatePawnEastAttacks<color>(bb) | calculatePawnWestAttacks<color>(bb);
}

template uint64_t calculatePawnAttacks<WHITE>(uint64_t);
template uint64_t calculatePawnAttacks<BLACK>(uint64_t);

template <PieceColor color>
uint64_t getPawnAttacks(int8_t square) {
    return pawnAttacks[color][square];
}

template uint64_t getPawnAttacks<WHITE>(int8_t square);
template uint64_t getPawnAttacks<BLACK>(int8_t square);
} // namespace Zagreus