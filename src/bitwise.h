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

#pragma once

#include <cstdint>

#include "types.h"

namespace Zagreus {
uint64_t soutOne(uint64_t b);

uint64_t nortOne(uint64_t b);

uint64_t eastOne(uint64_t b);

uint64_t noEaOne(uint64_t b);

uint64_t soEaOne(uint64_t b);

uint64_t westOne(uint64_t b);

uint64_t soWeOne(uint64_t b);

uint64_t noWeOne(uint64_t b);

uint64_t noNoEa(uint64_t b);

uint64_t noEaEa(uint64_t b);

uint64_t soEaEa(uint64_t b);

uint64_t soSoEa(uint64_t b);

uint64_t noNoWe(uint64_t b);

uint64_t noWeWe(uint64_t b);

uint64_t soWeWe(uint64_t b);

uint64_t soSoWe(uint64_t b);

uint64_t soutOccl(uint64_t pieceBB, uint64_t empty);

uint64_t nortOccl(uint64_t pieceBB, uint64_t empty);

uint64_t eastOccl(uint64_t pieceBB, uint64_t empty);

uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty);

uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty);

uint64_t westOccl(uint64_t rooks, uint64_t empty);

uint64_t soWeOccl(uint64_t bishops, uint64_t empty);

uint64_t noWeOccl(uint64_t bishops, uint64_t empty);

uint64_t nortFill(uint64_t gen);

uint64_t soutFill(uint64_t gen);

uint64_t whiteFrontSpans(uint64_t pawns);

uint64_t whiteRearSpans(uint64_t pawns);

uint64_t blackRearSpans(uint64_t pawns);

uint64_t blackFrontSpans(uint64_t pawns);

template <PieceColor color>
uint64_t calculatePawnEastAttacks(uint64_t pawns);

template <PieceColor color>
uint64_t calculatePawnWestAttacks(uint64_t pawns);

uint64_t calculateKnightAttacks(uint64_t knights);

uint64_t calculateKingAttacks(uint64_t kingSet);

void initializeBetweenLookup();

void initializeRayAttacks();

void initializeBitboardConstants();

uint64_t getKingAttacks(int8_t square);

uint64_t getKnightAttacks(int8_t square);

uint64_t getRayAttack(int8_t square, Direction direction);

uint64_t getBetweenSquares(int8_t from, int8_t to);

uint64_t getPieceZobristConstant(PieceType pieceType, int8_t square);

uint64_t getMovingColorZobristConstant();

uint64_t getCastleZobristConstant(uint8_t index);

uint64_t getEnPassantZobristConstant(uint8_t file);

template <PieceColor color>
uint64_t getPawnAttacks(int8_t square);

template <PieceColor color>
uint64_t calculatePawnAttacks(uint64_t bb);
} // namespace Zagreus