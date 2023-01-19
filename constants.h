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

#pragma once

#include <cstdint>

static constexpr uint8_t MAX_PLY = 255;
static constexpr uint8_t MAX_MOVES = 255;

constexpr uint64_t A_FILE = 0x0101010101010101;
constexpr uint64_t H_FILE = 0x8080808080808080;
constexpr uint64_t NOT_A_FILE = 0XFEFEFEFEFEFEFEFE;
constexpr uint64_t NOT_AB_FILE = 0XFCFCFCFCFCFCFCFC;
constexpr uint64_t NOT_GH_FILE = 0X3F3F3F3F3F3F3F3F;
constexpr uint64_t NOT_H_FILE = 0X7F7F7F7F7F7F7F7F;
constexpr uint64_t RANK_1 = 0x00000000000000FF;
constexpr uint64_t RANK_4 = 0x00000000FF000000;
constexpr uint64_t RANK_5 = 0x000000FF00000000;
constexpr uint64_t RANK_8 = 0xFF00000000000000;
constexpr uint64_t A1_H8_DIAG = 0x8040201008040201;
constexpr uint64_t H1_A8_DIAG = 0x0102040810204080;
constexpr uint64_t LIGHT_SQUARES = 0x55AA55AA55AA55AA;
constexpr uint64_t DARK_SQUARES = 0xAA55AA55AA55AA55;
