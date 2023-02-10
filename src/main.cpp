/*
 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "bitboard.h"
#include "engine.h"
#include "../senjo/UCIAdapter.h"
#include "../senjo/Output.h"
#include "magics.h"
#include "movegen.h"
#include "search.h"
#include "tt.h"
#include "pst.h"
#include "tuner.h"
#include "features.h"

using namespace Zagreus;

void benchmark();

void findZobristSeed();

const std::vector<std::string> benchmarkPositions = {"8/8/1n3k2/8/3P3P/5K2/8/1N4Q1 w - -",
                                               "1rb1kbnr/p1q1pppp/np4B1/2pp4/P1PP4/2N1P3/1P3PPP/R1BQK1NR w Kk",
                                               "1rbk1bnr/pp1p1ppp/n1pq4/4pP1Q/P1B1P3/2P4P/1P1P2P1/RNB2KNR b - -",
                                               "r1bn1r2/2k3p1/6p1/pP3p2/4P2P/1P1P3R/2P1NB2/Q3K1n1 b - -",
                                               "1rb1kb1r/p3p1pp/n4p2/1pppNn2/4P3/2PP3P/PB4P1/RN1QKB1R b KQk -",
                                               "8/3P4/p4k2/P2Q4/N7/7B/4p3/4K3 w - -",
                                               "1B1N3k/4r3/5b1P/2n1P3/3K2p1/6P1/4b3/4R3 b - ",
                                               "r1n1k1r1/1pb2p2/5P2/pPPp2p1/5p1R/P1N1N3/2P1P3/B1R1KB2 w - -",
                                               "8/8/1qB1k3/1p6/3p4/1p6/8/4K3 b - -",
                                               "6nr/1pq1b1k1/1N6/5Ppp/pp2B1P1/B1P4P/P2pK3/3R2NR b - -",
                                               "rnbqk2r/1p1p1pbp/4p2n/p1p3p1/2PPP1P1/5P1N/PP1K2BP/RNBQR3 w kq -",
                                               "2bq1b2/4k3/1p3ppr/2pp1Q1N/3p3P/2P5/PpN2PP1/1RB1KBR1 w - -",
                                               "8/2k5/4p1B1/4P2p/4P2P/1K6/7N/1q6 w - -",
                                               "1N1k4/P5b1/4p1p1/2PP3p/R4BrP/8/2nKb2R/1r6 w - -",
                                               "2kN1r2/2n5/2Q5/7p/7P/1K1RB3/8/8 w - -",
                                               "8/7b/p1P1Pn2/P1k3N1/7p/K1p5/4B1P1/3N2R1 w - -",
                                               "rnbqkbnr/1p2pp1p/3p4/6p1/P1p2PP1/1p1P4/2P1P2P/RNBQKBNR w Kkq -",
                                               "1n6/8/2k3K1/2p5/2PpP1nb/3P4/8/1b6 w - -",
                                               "8/2R5/8/k7/N5PP/2K5/8/5b2 w - -",
                                               "r3k2r/1b1nb2p/p1p3pn/3Np3/1PPp1B2/7B/P2KPq2/RQ4NR w kq -",
                                               "1n6/2B2nK1/2k5/2p5/2Pp4/8/8/5b2 b - -", "3Kb3/8/8/P1R5/8/8/8/7k w - -",
                                               "1B1k2n1/6b1/r4npq/5p1r/pP2PP1P/2P5/R2NR3/3K4 b - -",
                                               "rnb4r/4b2k/1R1pP3/4N2P/pP2nP1P/p1N5/3B4/1K5R b - -",
                                               "8/1pp5/3R2P1/1pkn1p1p/5P2/r6b/8/1N2K1N1 b - -",
                                               "rn3bnr/pBp1pk1p/5p2/1p4p1/6b1/N3Q3/PP1P1PPP/R1B1K1NR b - -",
                                               "8/8/P7/2k4n/1pR4n/1B5P/3R4/3K2N1 b - -",
                                               "q7/p1rbn1p1/P1ppR3/1PP2P1k/1B1PN2p/1Q5P/1R2K3/8 b - -",
                                               "6r1/3k2N1/2n5/3pbP2/8/8/8/3K1n2 b - -",
                                               "bnk4r/7p/R3q3/p1Pp4/P7/3p4/3P2Kn/1NB5 w - -",
                                               "r7/n3k1b1/p2p4/P1pP4/2K5/P6N/3BB2R/6Q1 b - -",
                                               "3k4/8/8/1NP5/6B1/8/8/K7 b - -",
                                               "3nb3/5k2/4p3/4P2p/Qp2P2P/8/3K2B1/2N5 w - -",
                                               "6b1/k7/2KbR3/8/7P/8/8/8 b - -",
                                               "rnb5/4b1kr/1R1pP3/4N2P/pP2nP1P/p1N5/8/1KB4R w - -"};

int main(int argc , char *argv[]) {
    // Custom test pos 1: r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8
    // Custom test pos 2: r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14
    // Custom test pos 3: nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1
    // Custom test pos 4: r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13
    // Custom test pos 5: r3k2r/1bp2pbp/1p1p1n2/1p1qp3/2nPP3/2NBBNP1/PPP1QPP1/2KR3R w kq - 0 1
    // Custom test pos 6: r1bqk2r/pppp1ppp/2n5/2b1p3/2B1P1n1/5N2/PPPP1PPP/RNBQ1RK1 b kq - 9 6
    // eval test: 8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33
    // Default pos: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // More test benchmarkPositions: https://github.com/elcabesa/vajolet/blob/master/tests/perft.txt

    initializeMagicBitboards();
    initializePst();

    senjo::Output(senjo::Output::NoPrefix) << "Zagreus  Copyright (C) 2023  Danny Jelsma";
    senjo::Output(senjo::Output::NoPrefix) << "";
    senjo::Output(senjo::Output::NoPrefix) << "This program comes with ABSOLUTELY NO WARRANTY.";
    senjo::Output(senjo::Output::NoPrefix) << "This is free software, and you are welcome to redistribute it";
    senjo::Output(senjo::Output::NoPrefix) << "under the conditions of the GNU Affero General Public License v3.0 or later.";
    senjo::Output(senjo::Output::NoPrefix) << "You should have received a copy of the GNU Affero General Public License";
    senjo::Output(senjo::Output::NoPrefix) << "along with this program. If not, see <https://www.gnu.org/licenses/>.";
    senjo::Output(senjo::Output::NoPrefix) << "";

    senjo::Output(senjo::Output::NoPrefix) << " ______ ";
    senjo::Output(senjo::Output::NoPrefix) << " |___  / ";
    senjo::Output(senjo::Output::NoPrefix) << "    / /  __ _   __ _  _ __  ___  _   _  ___ ";
    senjo::Output(senjo::Output::NoPrefix) << "   / /  / _` | / _` || '__|/ _ \\| | | |/ __|";
    senjo::Output(senjo::Output::NoPrefix) << "  / /__| (_| || (_| || |  |  __/| |_| |\\__ \\";
    senjo::Output(senjo::Output::NoPrefix) << R"( /_____|\__,_| \__, ||_|   \___| \__,_||___/)";
    senjo::Output(senjo::Output::NoPrefix) << "                __/ | ";
    senjo::Output(senjo::Output::NoPrefix) << "               |___/ ";
    senjo::Output(senjo::Output::NoPrefix) << "";

    senjo::Output(senjo::Output::NoPrefix) << "Zagreus UCI chess engine by Danny Jelsma (https://github.com/Dannyj1/Zagreus)";

    if (argc >= 2) {
        if (strcmp(argv[1], "bench") == 0) {
            senjo::Output(senjo::Output::NoPrefix) << "Starting benchmark...";

            benchmark();
            return 0;
        } else if (strcmp(argv[1], "findzobristseed") == 0) {
            findZobristSeed();
            return 0;
        } else if (strcmp(argv[1], "tune") == 0) {
            startTuning(argv[2]);
            return 0;
        } else if (strcmp(argv[1], "printeval") == 0) {
            printEvalValues();
            return 0;
        }

        senjo::Output(senjo::Output::NoPrefix) << "Unknown argument!";
        return 0;
    }

    try {
        ZagreusEngine engine;
        senjo::UCIAdapter adapter(engine);

        std::string line;
        line.reserve(16384);

        while (std::getline(std::cin, line)) {
            try {
                if (!adapter.doCommand(line)) {
                    break;
                }
            } catch (const std::exception &e) {
                senjo::Output(senjo::Output::NoPrefix) << "ERROR: " << e.what();
                return -1;
            }
        }

        return 0;
    } catch (const std::exception &e) {
        senjo::Output(senjo::Output::NoPrefix) << "ERROR: " << e.what();
        return 1;
    }
}

void benchmark() {
    ZagreusEngine engine;
    senjo::UCIAdapter adapter(engine);
    uint64_t nodes = 0;
    uint64_t totalMs = 0;

    for (const std::string &position : benchmarkPositions) {
        for (int i = 0; i < 2; i++) {
            Bitboard bb;
            PieceColor color = i == 0 ? PieceColor::WHITE : PieceColor::BLACK;

            bb.setFromFen(position);
            bb.setMovingColor(color);

            auto start = std::chrono::high_resolution_clock::now();
            senjo::GoParams params{};

            params.depth = 3;
            searchManager.getBestMove(params, engine, bb);

            auto end = std::chrono::high_resolution_clock::now();
            nodes += searchManager.getSearchStats().nodes + searchManager.getSearchStats().qnodes;
            totalMs += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
    }

    if (nodes == 0 || totalMs == 0) {
        senjo::Output(senjo::Output::NoPrefix) << "0 nodes 0 nps";
        return;
    }

    uint64_t nps = nodes / (totalMs / 1000);
    senjo::Output(senjo::Output::NoPrefix) << nodes << " nodes " << nps << " nps";
}

void addHashes(Bitboard &board, int depth, std::map<uint64_t, uint64_t> &collisionMap) {
    uint64_t zobristHash = board.getZobristHash() & 1398100ULL;

    if (zobristHash == 0) {
        return;
    }

    if (collisionMap.contains(zobristHash)) {
        collisionMap[zobristHash] += 1;
    } else {
        collisionMap[zobristHash] = 1;
    }

    if (depth == 0) {
        return;
    }

    MoveList moves;

    if (board.getMovingColor() == PieceColor::WHITE) {
        moves = generateMoves<PieceColor::WHITE>(board);
    } else {
        moves = generateMoves<PieceColor::BLACK>(board);
    }

    for (Move &move : moves.moves) {
        board.makeMove(move);
        addHashes(board, depth - 1, collisionMap);
        board.unmakeMove(move);
    }
}

void findZobristSeed() {
    senjo::Output(senjo::Output::NoPrefix) << "Loading positions...";
    std::ifstream fin("perft.txt");
    std::vector<std::string> seedFindPositions{};

    std::string line;
    while (std::getline(fin, line)) {
        seedFindPositions.emplace_back(line);
    }

    senjo::Output(senjo::Output::NoPrefix) << "Removing invalid positions...";
    for (const std::string &position : seedFindPositions) {
        Bitboard bb;

        if (!bb.setFromFen(position) || position.empty()) {
            std::erase(seedFindPositions, position);
        }
    }

    senjo::Output(senjo::Output::NoPrefix) << "Finding the best Zobrist seed with the least collisions...";

    uint64_t bestSeed = 0x6C7CCC580A348E7B;
    uint64_t leastCollisions = 444143880;
    std::random_device seedRd;
    std::mt19937_64 seedGen(seedRd());
    std::uniform_int_distribution<uint64_t> seedDis;

    while (true) {
        std::map<uint64_t, uint64_t> collisionMap{};
        uint64_t seed = seedDis(seedGen);

        for (std::string& position : seedFindPositions) {
            Bitboard board;
            std::random_device rd;
            std::mt19937_64 gen(rd());
            gen.seed(seed);
            std::uniform_int_distribution<uint64_t> dis;

            for (uint64_t &zobristConstant : board.zobristConstants) {
                zobristConstant = dis(gen);
            }

            board.setFromFen(position);
            addHashes(board, 2, collisionMap);
        }

        uint64_t collisions = 0;
        for (const auto& [key, value] : collisionMap) {
            if (value > 1) {
                collisions += value;
            }
        }

        if (collisions < leastCollisions) {
            leastCollisions = collisions;
            bestSeed = seed;
            std::cout << "Found new best seed (" << leastCollisions << " collisions): 0x" << std::hex << std::uppercase << bestSeed << std::dec << std::nouppercase << std::endl;
        }
    }
}