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

#pragma once

#include <cstdint>

static constexpr uint8_t MAX_PLY = 255;
static constexpr uint8_t MAX_MOVES = 255;
static constexpr int ZOBRIST_CONSTANT_SIZE = 781;

static constexpr int ZOBRIST_PIECE_END = 768;
static constexpr int ZOBRIST_COLOR_INDEX = ZOBRIST_PIECE_END;
static constexpr int ZOBRIST_WHITE_KINGSIDE_INDEX = ZOBRIST_COLOR_INDEX + 1;
static constexpr int ZOBRIST_WHITE_QUEENSIDE_INDEX = ZOBRIST_WHITE_KINGSIDE_INDEX + 1;
static constexpr int ZOBRIST_BLACK_KINGSIDE_INDEX = ZOBRIST_WHITE_QUEENSIDE_INDEX + 1;
static constexpr int ZOBRIST_BLACK_QUEENSIDE_INDEX = ZOBRIST_BLACK_KINGSIDE_INDEX + 1;
static constexpr int ZOBRIST_EN_PASSANT_INDEX = ZOBRIST_BLACK_QUEENSIDE_INDEX + 1;
\
static constexpr uint64_t A_FILE = 0x0101010101010101ULL;
static constexpr uint64_t H_FILE = 0x8080808080808080ULL;
static constexpr uint64_t NOT_A_FILE = 0XFEFEFEFEFEFEFEFEULL;
static constexpr uint64_t NOT_AB_FILE = 0XFCFCFCFCFCFCFCFCULL;
static constexpr uint64_t NOT_GH_FILE = 0X3F3F3F3F3F3F3F3FULL;
static constexpr uint64_t NOT_H_FILE = 0X7F7F7F7F7F7F7F7FULL;
static constexpr uint64_t RANK_1 = 0x00000000000000FFULL;
static constexpr uint64_t RANK_4 = 0x00000000FF000000ULL;
static constexpr uint64_t RANK_5 = 0x000000FF00000000ULL;
static constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;
static constexpr uint64_t A1_H8_DIAG = 0x8040201008040201ULL;
static constexpr uint64_t H1_A8_DIAG = 0x0102040810204080ULL;
static constexpr uint64_t LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
static constexpr uint64_t DARK_SQUARES = 0xAA55AA55AA55AA55ULL;

static constexpr uint8_t WHITE_KING_CHECK_BIT = 1 << 0;
static constexpr uint8_t BLACK_KING_CHECK_BIT = 1 << 1;
static constexpr uint8_t WHITE_KING_CHECK_RESET_BIT = 1 << 2;
static constexpr uint8_t BLACK_KING_CHECK_RESET_BIT = 1 << 3;

static constexpr uint64_t WHITE_KING_SIDE_BETWEEN = 0x60ULL;
static constexpr uint64_t WHITE_QUEEN_SIDE_BETWEEN = 0xEULL;
static constexpr uint64_t BLACK_KING_SIDE_BETWEEN = 0x6000000000000000ULL;
static constexpr uint64_t BLACK_QUEEN_SIDE_BETWEEN = 0xE00000000000000ULL;

static constexpr int NO_CAPTURE_SCORE = -1;

static constexpr int MVVLVA_TABLE[12][12] = {
        {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
        {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
        {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
        {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
        {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
        {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

        {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
        {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
        {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
        {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
        {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
        {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600}
};