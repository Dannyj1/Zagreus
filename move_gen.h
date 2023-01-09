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

#include <vector>

#include "bitboard.h"

namespace Zagreus {
    std::vector<Move> generateLegalMoves(Bitboard &bitboard, PieceColor color);

    std::vector<Move> generateQuiescenceMoves(Bitboard &bitboard, PieceColor color);

    void
    generatePawnMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color,
                      PieceType pieceType, bool quiesce = false);

    void
    generateKnightMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                        PieceColor color, PieceType pieceType, bool quiesce = false);

    void
    generateBishopMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                        PieceColor color, PieceType pieceType, bool quiesce = false);

    void
    generateRookMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color,
                      PieceType pieceType, bool quiesce = false);

    void
    generateQueenMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                       PieceColor color, PieceType pieceType, bool quiesce = false);

    void
    generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color, PieceType pieceType, bool quiesce = false);

    bool sortMoves(Bitboard &bitboard, Move &a, Move &b);
}

