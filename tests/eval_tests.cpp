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

TEST_CASE("Evaluation is symmetric (on mirrored positions)", "[eval_symmetry]") {
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

    SECTION("r2q2k1/1pb2rp1/p1n1R2p/5p2/8/B1P4P/P1B1QPP1/R5K1 w - - 5 23") {
        bb.setFromFen("r2q2k1/1pb2rp1/p1n1R2p/5p2/8/B1P4P/P1B1QPP1/R5K1 w - - 5 23");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("r5k1/p1b1qpp1/b1p4p/8/5P2/P1N1r2P/1PB2RP1/R2Q2K1 b - - 5 23");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("r4rk1/2B2ppp/4p3/1qb5/1p2B3/p4N2/2Q2PPP/5RK1 w - - 0 22") {
        bb.setFromFen("r4rk1/2B2ppp/4p3/1qb5/1p2B3/p4N2/2Q2PPP/5RK1 w - - 0 22");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("5rk1/2q2ppp/P4n2/1P2b3/1QB5/4P3/2b2PPP/R4RK1 b - - 0 22");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("2b2rk1/rp3pbp/1N2p3/1BPp4/Pq6/4Q3/5PKP/3R2R1 b - - 1 25") {
        bb.setFromFen("2b2rk1/rp3pbp/1N2p3/1BPp4/Pq6/4Q3/5PKP/3R2R1 b - - 1 25");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("3r2r1/5pkp/4q3/pQ6/1bpP4/1n2P3/RP3PBP/2B2RK1 w - - 1 25");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("1KR5/4bkp1/1p3r2/1P6/8/8/8/8 b - - 7 59") {
        bb.setFromFen("1KR5/4bkp1/1p3r2/1P6/8/8/8/8 b - - 7 59");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("8/8/8/8/1p6/1P3R2/4BKP1/1kr5 w - - 7 59");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("3r1rk1/1ppq1pbp/p3p1p1/3bP1N1/P1BP1P1P/1n2B3/4Q1P1/2RR2K1 w - - 0 21") {
        bb.setFromFen("3r1rk1/1ppq1pbp/p3p1p1/3bP1N1/P1BP1P1P/1n2B3/4Q1P1/2RR2K1 w - - 0 21");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("2rr2k1/4q1p1/1N2b3/p1bp1p1p/3Bp1n1/P3P1P1/1PPQ1PBP/3R1RK1 b - - 0 21");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("8/6pk/P4p2/4b2p/2R5/3Q2P1/q4PKP/8 b - - 2 47") {
        bb.setFromFen("8/6pk/P4p2/4b2p/2R5/3Q2P1/q4PKP/8 b - - 2 47");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("8/Q4pkp/3q2p1/2r5/4B2P/p4P2/6PK/8 w - - 2 47");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("3r1q1k/pb4p1/1pp2b1p/2P5/3P1r2/1P4PB/PB2QP1P/4RRK1 b - - 0 27") {
        bb.setFromFen("3r1q1k/pb4p1/1pp2b1p/2P5/3P1r2/1P4PB/PB2QP1P/4RRK1 b - - 0 27");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("4rrk1/pb2qp1p/1p4pb/3p1R2/2p5/1PP2B1P/PB4P1/3R1Q1K w - - 0 27");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("4k2r/8/8/8/8/8/8/4K3 w k - 0 1") {
        bb.setFromFen("4k2r/8/8/8/8/8/8/4K3 w k - 0 1");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("4k3/8/8/8/8/8/8/4K2R b K - 0 1");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("8/7p/8/3k1b2/pP3P2/P3K3/7P/8 b - - 3 47") {
        bb.setFromFen("8/7p/8/3k1b2/pP3P2/P3K3/7P/8 b - - 3 47");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("8/7p/p3k3/Pp3p2/3K1B2/8/7P/8 w - - 3 47");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("r4rk1/5pp1/P1pp1q1p/2b1p3/4P1b1/1PNP1N2/1P3PPP/R2Q1RK1 w - - 0 15") {
        bb.setFromFen("r4rk1/5pp1/P1pp1q1p/2b1p3/4P1b1/1PNP1N2/1P3PPP/R2Q1RK1 w - - 0 15");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("r2q1rk1/1p3ppp/1pnp1n2/4p1B1/2B1P3/p1PP1Q1P/5PP1/R4RK1 b - - 0 15");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }

    SECTION("4k3/1R2b1Bp/1p6/3p1p2/P2Pp3/1P2P2P/5PP1/2n3K1 b - - 2 29") {
        bb.setFromFen("4k3/1R2b1Bp/1p6/3p1p2/P2Pp3/1P2P2P/5PP1/2n3K1 b - - 2 29");
        int eval1 = Zagreus::Evaluation(bb).evaluate();
        bb.setFromFen("2N3k1/5pp1/1p2p2p/p2pP3/3P1P2/1P6/1r2B1bP/4K3 w - - 2 29");
        int eval2 = Zagreus::Evaluation(bb).evaluate();

        REQUIRE(eval1 == eval2);
    }
}
