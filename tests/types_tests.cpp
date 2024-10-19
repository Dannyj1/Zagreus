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

#include "catch2/catch_test_macros.hpp"

#include "../src/types.h"

namespace Zagreus {
// Test all pieces at once for correct identification of piece type
TEST_CASE("test_pieceType", "[types]") {
    REQUIRE(getPieceType(Piece::WHITE_PAWN) == PieceType::PAWN);
    REQUIRE(getPieceType(Piece::BLACK_PAWN) == PieceType::PAWN);
    REQUIRE(getPieceType(Piece::WHITE_KNIGHT) == PieceType::KNIGHT);
    REQUIRE(getPieceType(Piece::BLACK_KNIGHT) == PieceType::KNIGHT);
    REQUIRE(getPieceType(Piece::WHITE_BISHOP) == PieceType::BISHOP);
    REQUIRE(getPieceType(Piece::BLACK_BISHOP) == PieceType::BISHOP);
    REQUIRE(getPieceType(Piece::WHITE_ROOK) == PieceType::ROOK);
    REQUIRE(getPieceType(Piece::BLACK_ROOK) == PieceType::ROOK);
    REQUIRE(getPieceType(Piece::WHITE_QUEEN) == PieceType::QUEEN);
    REQUIRE(getPieceType(Piece::BLACK_QUEEN) == PieceType::QUEEN);
    REQUIRE(getPieceType(Piece::WHITE_KING) == PieceType::KING);
    REQUIRE(getPieceType(Piece::BLACK_KING) == PieceType::KING);
}

// Test the pieceColor function for determining the color of all pieces as white or black
TEST_CASE("test_pieceColor", "[types]") {
    // Given
    std::vector pieces = {
        WHITE_PAWN, BLACK_PAWN,
        WHITE_KNIGHT, BLACK_KNIGHT,
        WHITE_BISHOP, BLACK_BISHOP,
        WHITE_ROOK, BLACK_ROOK,
        WHITE_QUEEN, BLACK_QUEEN,
        WHITE_KING, BLACK_KING
    };

    // When & Then
    for (const auto& piece : pieces) {
        PieceColor color = getPieceColor(piece);
        if (piece == WHITE_PAWN || piece == WHITE_KNIGHT || piece == WHITE_BISHOP ||
            piece == WHITE_ROOK || piece == WHITE_QUEEN || piece == WHITE_KING) {
            REQUIRE(color == PieceColor::WHITE);
            } else {
                REQUIRE(color == PieceColor::BLACK);
            }
    }
}
} // namespace Zagreus
