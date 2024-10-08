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

#include "board.h"

#include "bitwise.h"

namespace Zagreus {
template <PieceColor movedColor>
bool Board::isPositionLegal() const {
    constexpr PieceColor opponentColor = !movedColor;
    constexpr Piece king = movedColor == PieceColor::WHITE ? Piece::WHITE_KING : Piece::BLACK_KING;
    const uint64_t kingBB = getBitboard<king>();
    const uint8_t kingSquare = bitscanForward(kingBB);

    return !getSquareAttackersByColor<opponentColor>(kingSquare);
}

template bool Board::isPositionLegal<PieceColor::WHITE>() const;
template bool Board::isPositionLegal<PieceColor::BLACK>() const;

uint64_t Board::getSquareAttackers(const uint8_t square) const {
    using enum Piece;

    const uint64_t knights = getBitboard<WHITE_KNIGHT>() | getBitboard<BLACK_KNIGHT>();
    const uint64_t kings = getBitboard<WHITE_KING>() | getBitboard<BLACK_KING>();
    uint64_t bishopsQueens = getBitboard<WHITE_QUEEN>() | getBitboard<BLACK_QUEEN>();
    uint64_t rooksQueens = getBitboard<WHITE_QUEEN>() | getBitboard<BLACK_QUEEN>();
    rooksQueens |= getBitboard<WHITE_ROOK>() | getBitboard<BLACK_ROOK>();
    bishopsQueens |= getBitboard<WHITE_BISHOP>() | getBitboard<BLACK_BISHOP>();

    return (pawnAttacks<PieceColor::WHITE>(square) & getBitboard<BLACK_PAWN>())
           | (pawnAttacks<PieceColor::BLACK>(square) & getBitboard<WHITE_PAWN>())
           | (knightAttacks(square) & knights)
           | (kingAttacks(square) & kings)
           | (bishopAttacks(square, occupied) & bishopsQueens)
           | (rookAttacks(square, occupied) & rooksQueens);
}

void Board::makeMove(const Move& move) {}
} // namespace Zagreus
