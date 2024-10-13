
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

#include <cstdint>

static constexpr uint16_t MAX_PLY = 750;

static constexpr uint64_t NOT_A_FILE = 0xFEFEFEFEFEFEFEFEULL;
static constexpr uint64_t NOT_AB_FILE = 0xFCFCFCFCFCFCFCFCULL;
static constexpr uint64_t NOT_GH_FILE = 0x3F3F3F3F3F3F3F3FULL;
static constexpr uint64_t NOT_H_FILE = 0x7F7F7F7F7F7F7F7FULL;

static constexpr uint64_t RANK_1 = 0x00000000000000FFULL;
static constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
static constexpr uint64_t RANK_3 = 0x0000000000FF0000ULL;
static constexpr uint64_t RANK_4 = 0x00000000FF000000ULL;
static constexpr uint64_t RANK_5 = 0x000000FF00000000ULL;
static constexpr uint64_t RANK_6 = 0x0000FF0000000000ULL;
static constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;
static constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;

static constexpr uint64_t WHITE_KINGSIDE_CASTLE_PATH = 0x60ULL;
static constexpr uint64_t WHITE_QUEENSIDE_CASTLE_PATH = 0xCULL;
static constexpr uint64_t BLACK_KINGSIDE_CASTLE_PATH = 0x6000000000000000ULL;
static constexpr uint64_t BLACK_QUEENSIDE_CASTLE_PATH = 0xC00000000000000ULL;

static constexpr uint64_t WHITE_KINGSIDE_CASTLE_UNOCCUPIED = 0x60ULL;
static constexpr uint64_t WHITE_QUEENSIDE_CASTLE_UNOCCUPIED = 0xEULL;
static constexpr uint64_t BLACK_KINGSIDE_CASTLE_UNOCCUPIED = 0x6000000000000000ULL;
static constexpr uint64_t BLACK_QUEENSIDE_CASTLE_UNOCCUPIED = 0xE00000000000000ULL;

static constexpr uint8_t SQUARES = 64;
static constexpr uint8_t PIECES = 12;
static constexpr uint8_t COLORS = 2;

static constexpr uint8_t MAX_MOVES = 255;
