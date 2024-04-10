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

// TODO: clearly need more of these
TEST_CASE("Evaluation is symmetric (on mirrored positions)") {
    Zagreus::ZagreusEngine engine{};
    Zagreus::Bitboard bb{};

    SECTION("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1") {
        bb.setFromFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("r1bqk2r/pp1pppbp/1nn3p1/4P3/5B2/2PQ1N2/PPB2PPP/RN2K2R b KQkq - 2 10") {
        bb.setFromFen("r1bqk2r/pp1pppbp/1nn3p1/4P3/5B2/2PQ1N2/PPB2PPP/RN2K2R b KQkq - 2 10");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("rn2k2r/ppb2ppp/2pq1n2/5b2/4p3/1NN3P1/PP1PPPBP/R1BQK2R w kqKQ - 2 10");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("8/1kp5/8/p1pq4/4p1p1/1P2P3/P1PP2R1/5K1R w - - 3 39") {
        bb.setFromFen("8/1kp5/8/p1pq4/4p1p1/1P2P3/P1PP2R1/5K1R w - - 3 39");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("5k1r/p1pp2r1/1p2p3/4P1P1/P1PQ4/8/1KP5/8 b - - 3 39");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("2rrb1k1/ppq1bppp/5n2/2p1NR2/3P4/1QN1P3/PP4PP/2R2BK1 b - - 1 18") {
        bb.setFromFen("2rrb1k1/ppq1bppp/5n2/2p1NR2/3P4/1QN1P3/PP4PP/2R2BK1 b - - 1 18");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("2r2bk1/pp4pp/1qn1p3/3p4/2P1nr2/5N2/PPQ1BPPP/2RRB1K1 w - - 1 18");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("2n2b1r/pkpq2p1/5pn1/1P1Pp2p/P3P3/3P2Pb/1B2NP2/R2QK2R w KQ - 0 21") {
        bb.setFromFen("2n2b1r/pkpq2p1/5pn1/1P1Pp2p/P3P3/3P2Pb/1B2NP2/R2QK2R w KQ - 0 21");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("r2qk2r/1b2np2/3p2pB/p3p3/1p1pP2P/5PN1/PKPQ2P1/2N2B1R b kq - 0 21");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }
}
