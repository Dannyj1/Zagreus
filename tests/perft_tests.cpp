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

TEST_CASE("'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' at depth 6 == 119060324") {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 6, 6);
    REQUIRE(perft == 119060324);
}

TEST_CASE(
    "'r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -' at depth 5 == 193690690")
 {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 5, 5);
    REQUIRE(perft == 193690690);
}

TEST_CASE("'8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -' at depth 7 == 178633661")
 {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 7, 7);
    REQUIRE(perft == 178633661);
}

TEST_CASE(
    "'r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1' at depth 5 == 15833292")
 {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 5, 5);
    REQUIRE(perft == 15833292);
}

TEST_CASE(
    "'r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1' at depth 5 == 15833292")
 {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 5, 5);
    REQUIRE(perft == 15833292);
}

TEST_CASE("'rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8' at depth 4 == 2103487")
 {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 4, 4);
    REQUIRE(perft == 2103487);
}

TEST_CASE(
    "'r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10' at depth 4 == 3894594")
 {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    bb.setFromFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    uint64_t perft = engine.doPerft(bb, bb.getMovingColor(), 4, 4);
    REQUIRE(perft == 3894594);
}
