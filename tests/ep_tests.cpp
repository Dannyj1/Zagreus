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

#include "../src/bitboard.h"
#include "../src/engine.h"
#include "../src/evaluate.h"

TEST_CASE("Test en-passant", "[ep]") {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    SECTION("6k1/p2q4/1p3pp1/2b4p/4p2P/P2P2P1/3Q1PK1/5B2 w - - 0 31") {
        Zagreus::Move move{Zagreus::F2, Zagreus::F4, Zagreus::WHITE_PAWN};
        bb.makeMove(move);
        int8_t epSquare = bb.getEnPassantSquare();
        REQUIRE(epSquare == Zagreus::F3);
        Zagreus::Move epMove{Zagreus::E4, Zagreus::F3, Zagreus::BLACK_PAWN};
        bb.makeMove(epMove);
        Zagreus::PieceType pieceOnEpSquare = bb.getPieceOnSquare(Zagreus::F3);
        Zagreus::PieceType pieceOnCaptureSquare = bb.getPieceOnSquare(Zagreus::F4);
        REQUIRE(pieceOnEpSquare == Zagreus::BLACK_PAWN);
        REQUIRE(pieceOnCaptureSquare == Zagreus::EMPTY);
    }
}

