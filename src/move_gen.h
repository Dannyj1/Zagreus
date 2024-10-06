
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

#include <stdint.h>

#include "board.h"
#include "move.h"
#include "types.h"

enum class PieceColor : uint8_t;

namespace Zagreus {
class Board;
struct MoveList;

enum class GenerationType : uint8_t {
    All,
    Quiet,
    Capture,
    Evasions
};

template <PieceColor color, GenerationType type>
void generateMoves(const Board& board, MoveList& moves);

template <PieceColor color, GenerationType type>
void generatePawnMoves(const Board& board, MoveList& moves, uint64_t genMask);

template <PieceColor color, GenerationType type>
void generateKnightMoves(const Board& board, MoveList& moves, uint64_t genMask);

template <PieceColor color, GenerationType type>
void generateBishopMoves(const Board& board, MoveList& moves, uint64_t genMask);

template <PieceColor color, GenerationType type>
void generateRookMoves(const Board& board, MoveList& moves, uint64_t genMask);

template <PieceColor color, GenerationType type>
void generateQueenMoves(const Board& board, MoveList& moves, uint64_t genMask);

template <PieceColor color, GenerationType type>
void generateKingMoves(const Board& board, MoveList& moves, uint64_t genMask);
} // namespace Zagreus
