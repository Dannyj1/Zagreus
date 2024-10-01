
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

constexpr uint64_t NOT_A_FILE = 0xFEFEFEFEFEFEFEFEULL;
constexpr uint64_t NOT_H_FILE = 0x7F7F7F7F7F7F7F7FULL;

constexpr uint64_t RANK_4 = 0x00000000FF000000ULL;
constexpr uint64_t RANK_5 = 0x000000FF00000000ULL;

constexpr uint8_t SQUARES = 64;
constexpr uint8_t PIECES = 12;
constexpr uint8_t COLORS = 2;

constexpr uint8_t MAX_MOVES = 255;
