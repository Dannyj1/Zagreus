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

static constexpr uint16_t MAX_PLY = 750;
static constexpr uint8_t MAX_MOVES = 255;
static constexpr int ZOBRIST_CONSTANT_SIZE = 781;

static constexpr int ZOBRIST_WHITE_KINGSIDE_INDEX = 0;
static constexpr int ZOBRIST_WHITE_QUEENSIDE_INDEX = 1;
static constexpr int ZOBRIST_BLACK_KINGSIDE_INDEX = 2;
static constexpr int ZOBRIST_BLACK_QUEENSIDE_INDEX = 3;

static constexpr int MATE_SCORE = 20000;
static constexpr int MAX_POSITIVE = 1000000;
static constexpr int MAX_NEGATIVE = -1000000;
static constexpr int DRAW_SCORE = 0;

static constexpr uint64_t A_FILE = 0x0101010101010101ULL;
static constexpr uint64_t B_FILE = 0x0202020202020202ULL;
static constexpr uint64_t C_FILE = 0x0404040404040404ULL;
static constexpr uint64_t D_FILE = 0x0808080808080808ULL;
static constexpr uint64_t E_FILE = 0x1010101010101010ULL;
static constexpr uint64_t F_FILE = 0x2020202020202020ULL;
static constexpr uint64_t G_FILE = 0x4040404040404040ULL;
static constexpr uint64_t H_FILE = 0x8080808080808080ULL;
static constexpr uint64_t DE_FILE = 0x01818181818181818ULL;
static constexpr uint64_t NOT_A_FILE = 0XFEFEFEFEFEFEFEFEULL;
static constexpr uint64_t NOT_AB_FILE = 0XFCFCFCFCFCFCFCFCULL;
static constexpr uint64_t NOT_GH_FILE = 0X3F3F3F3F3F3F3F3FULL;
static constexpr uint64_t NOT_H_FILE = 0X7F7F7F7F7F7F7F7FULL;
static constexpr uint64_t RANK_1 = 0xFFULL;
static constexpr uint64_t RANK_2 = 0xFF00ULL;
static constexpr uint64_t RANK_3 = 0xFF0000ULL;
static constexpr uint64_t RANK_4 = 0xFF000000ULL;
static constexpr uint64_t RANK_5 = 0xFF00000000ULL;
static constexpr uint64_t RANK_6 = 0xFF0000000000ULL;
static constexpr uint64_t RANK_7 = 0xFF000000000000ULL;
static constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;
static constexpr uint64_t A1_H8_DIAG = 0x8040201008040201ULL;
static constexpr uint64_t H1_A8_DIAG = 0x0102040810204080ULL;
static constexpr uint64_t LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
static constexpr uint64_t DARK_SQUARES = 0xAA55AA55AA55AA55ULL;
static constexpr uint64_t CENTER_SQUARES = 0x0000001818000000ULL;
static constexpr uint64_t EXTENDED_CENTER_SQUARES = 0x00003C3C3C3C0000ULL;
static constexpr uint64_t PROMOTION_SQUARES = 0xFF000000000000FFULL;

static constexpr uint64_t WHITE_KING_SIDE_BETWEEN = 0x60ULL;
static constexpr uint64_t WHITE_QUEEN_SIDE_BETWEEN = 0xEULL;
static constexpr uint64_t BLACK_KING_SIDE_BETWEEN = 0x6000000000000000ULL;
static constexpr uint64_t BLACK_QUEEN_SIDE_BETWEEN = 0xE00000000000000ULL;

static constexpr int NO_CAPTURE_SCORE = -1;

static constexpr int COLORS = 2;
static constexpr int PIECE_TYPES = 12;
static constexpr int SQUARES = 64;

static constexpr int MVVLVA_TABLE[12][12] = {
    {105, 105, 205, 205, 305, 305, 405, 405, 505, 505, 605, 605},
    {104, 104, 204, 204, 304, 304, 404, 404, 504, 504, 604, 604},
    {103, 103, 203, 203, 303, 303, 403, 403, 503, 503, 603, 603},
    {102, 102, 202, 202, 302, 302, 402, 402, 502, 502, 602, 602},
    {101, 101, 201, 201, 301, 301, 401, 401, 501, 501, 601, 601},
    {100, 100, 200, 200, 300, 300, 400, 400, 500, 500, 600, 600},
    {105, 105, 205, 205, 305, 305, 405, 405, 505, 505, 605, 605},
    {104, 104, 204, 204, 304, 304, 404, 404, 504, 504, 604, 604},
    {103, 103, 203, 203, 303, 303, 403, 403, 503, 503, 603, 603},
    {102, 102, 202, 202, 302, 302, 402, 402, 502, 502, 602, 602},
    {101, 101, 201, 201, 301, 301, 401, 401, 501, 501, 601, 601},
    {100, 100, 200, 200, 300, 300, 400, 400, 500, 500, 600, 600},
};