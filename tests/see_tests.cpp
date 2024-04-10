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
#include "../src/features.h"
#include "../src/search.h"

// Positions from: https://github.com/zzzzz151/Starzix/blob/main/tests%2FSEE.txt
TEST_CASE("Test SEE function", "[see]") {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};
    std::vector<float> oldParameters = Zagreus::getEvalValues();
    std::vector<float> bestParameters = Zagreus::getBaseEvalValues();
    Zagreus::updateEvalValues(bestParameters);

    int pawnValue = Zagreus::getEvalValue(Zagreus::MIDGAME_PAWN_MATERIAL);
    int knightValue = Zagreus::getEvalValue(Zagreus::MIDGAME_KNIGHT_MATERIAL);
    int bishopValue = Zagreus::getEvalValue(Zagreus::MIDGAME_BISHOP_MATERIAL);
    int rookValue = Zagreus::getEvalValue(Zagreus::MIDGAME_ROOK_MATERIAL);
    int queenValue = Zagreus::getEvalValue(Zagreus::MIDGAME_QUEEN_MATERIAL);

    // cases
    SECTION("6k1/1pp4p/p1pb4/6q1/3P1pRr/2P4P/PP1Br1P1/5RKN w - - | f1f4 | P - R + B") {
        int expected = pawnValue - rookValue + bishopValue;
        bb.setFromFen("6k1/1pp4p/p1pb4/6q1/3P1pRr/2P4P/PP1Br1P1/5RKN w - -");
        int see = bb.seeCapture<Zagreus::WHITE>(Zagreus::F1, Zagreus::F4);

        REQUIRE(see == expected);
    }

    SECTION("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - - | g4f3 | 0") {
        int expected = 0;
        bb.setFromFen("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -");
        int see = bb.seeCapture<Zagreus::BLACK>(Zagreus::G4, Zagreus::F3);

        REQUIRE(see == expected);
    }

    SECTION("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - - | d6e5 | P") {
        int expected = pawnValue;
        bb.setFromFen("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -");
        int see = bb.seeCapture<Zagreus::BLACK>(Zagreus::D6, Zagreus::E5);

        REQUIRE(see == expected);
    }

    SECTION("3r3k/3r4/2n1n3/8/3p4/2PR4/1B1Q4/3R3K w - - | d3d4 | P - R + N - P + N - B + R - Q + R") {
        int expected = pawnValue - rookValue + knightValue - pawnValue + knightValue - bishopValue + rookValue - queenValue + rookValue;
        bb.setFromFen("3r3k/3r4/2n1n3/8/3p4/2PR4/1B1Q4/3R3K w - -");
        int see = bb.seeCapture<Zagreus::WHITE>(Zagreus::D3, Zagreus::D4);

        REQUIRE(see == expected);
    }

    SECTION("r2qkbn1/ppp1pp1p/3p1rp1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R b KQq - | g4f3 | N - B + P") {
        int expected = knightValue - bishopValue + pawnValue;
        bb.setFromFen("r2qkbn1/ppp1pp1p/3p1rp1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R b KQq -");
        int see = bb.seeCapture<Zagreus::BLACK>(Zagreus::G4, Zagreus::F3);

        REQUIRE(see == expected);
    }

    SECTION("r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - | c6d4 | P - N + N - P") {
        int expected = pawnValue - knightValue + knightValue - pawnValue;
        bb.setFromFen("r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq -");
        int see = bb.seeCapture<Zagreus::BLACK>(Zagreus::C6, Zagreus::D4);

        REQUIRE(see == expected);
    }

    SECTION("1r3r2/5p2/4p2p/2k1n1P1/2PN1nP1/1P3P2/8/2KR1B1R b - - | b8b3 | P - R") {
        int expected = pawnValue - rookValue;
        bb.setFromFen("1r3r2/5p2/4p2p/2k1n1P1/2PN1nP1/1P3P2/8/2KR1B1R b - -");
        int see = bb.seeCapture<Zagreus::BLACK>(Zagreus::B8, Zagreus::B3);

        REQUIRE(see == expected);
    }

    SECTION("1r3r1k/p4pp1/2p1p2p/qpQP3P/2P5/3R4/PP3PP1/1K1R4 b - - | a5a2 | P - Q") {
        int expected = pawnValue - queenValue;
        bb.setFromFen("1r3r1k/p4pp1/2p1p2p/qpQP3P/2P5/3R4/PP3PP1/1K1R4 b - -");
        int see = bb.seeCapture<Zagreus::BLACK>(Zagreus::A5, Zagreus::A2);

        REQUIRE(see == expected);
    }

    SECTION("8/8/1k6/8/8/2N1N3/4p1K1/3n4 w - - | c3d1 | N - (N + Q - P) + Q") {
        int expected = knightValue - (knightValue + queenValue - pawnValue) + queenValue;
        bb.setFromFen("8/8/1k6/8/8/2N1N3/4p1K1/3n4 w - -");
        int see = bb.seeCapture<Zagreus::WHITE>(Zagreus::C3, Zagreus::D1);

        REQUIRE(see == expected);
    }

    SECTION("2r1k3/pbr3pp/5p1b/2KB3n/1N2N3/3P1PB1/PPP1P1PP/R2Q3R w - - | d5c6 | -B + B - N") {
        int expected = -bishopValue + bishopValue - knightValue;
        bb.setFromFen("2r1k3/pbr3pp/5p1b/2KB3n/1N2N3/3P1PB1/PPP1P1PP/R2Q3R w - -");
        int see = bb.seeCapture<Zagreus::WHITE>(Zagreus::D5, Zagreus::C6);

        REQUIRE(see == expected);
    }

    Zagreus::updateEvalValues(oldParameters);
}
