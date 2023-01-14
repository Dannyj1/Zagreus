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

#include <iostream>
#include <unordered_map>

#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "bitboard.h"
#include "senjo/Output.h"
#include "move_gen.h"
#include "types.h"
#include "search_mgr.h"
#include "magics.h"
#include "pst.h"
#include "tt.h"

void benchmark();

uint64_t perft(Zagreus::Bitboard &perftBoard, Zagreus::PieceColor color, int depth, int startingDepth) {
    uint64_t nodes = 0ULL;

    if (depth == 0) {
        return 1ULL;
    }

    Zagreus::MovePicker moves = generateLegalMoves(perftBoard, color);

    while (moves.hasNext()) {
        Zagreus::Move move = moves.getNextMove();
        assert(move.fromSquare != move.toSquare);

        perftBoard.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

        if (perftBoard.isKingInCheck(color)) {
            perftBoard.unmakeMove();
            continue;
        }

        uint64_t nodeAmount = perft(perftBoard, Zagreus::Bitboard::getOppositeColor(color), depth - 1, startingDepth);
        nodes += nodeAmount;

        if (depth == startingDepth && nodeAmount > 0LL) {
            std::string notation =
                    Zagreus::Bitboard::getNotation(move.fromSquare) + Zagreus::Bitboard::getNotation(move.toSquare);
            senjo::Output(senjo::Output::InfoPrefix) << notation << ": " << nodeAmount;
        }

        perftBoard.unmakeMove();
    }

    return nodes;
}

int main(int argc , char *argv[]) {
    // Custom test pos 1: r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8
    // Custom test pos 2: r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14
    // Custom test pos 3: nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1
    // Custom test pos 4: r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13
    // Custom test pos 5: r3k2r/1bp2pbp/1p1p1n2/1p1qp3/2nPP3/2NBBNP1/PPP1QPP1/2KR3R w kq - 0 1
    // Custom test pos 6: r1bqk2r/pppp1ppp/2n5/2b1p3/2B1P1n1/5N2/PPPP1PPP/RNBQ1RK1 b kq - 9 6
    // eval test: 8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33
    // Default pos: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // More test positions: https://github.com/elcabesa/vajolet/blob/master/tests/perft.txt

    Zagreus::TranspositionTable::getTT()->setTableSize(512);
    Zagreus::initializeMagicBitboards();
    Zagreus::initializePst();

    senjo::Output(senjo::Output::InfoPrefix) << "Zagreus  Copyright (C) 2023  Danny Jelsma";
    senjo::Output(senjo::Output::InfoPrefix) << "This program comes with ABSOLUTELY NO WARRANTY";
    senjo::Output(senjo::Output::InfoPrefix) << "This is free software, and you are welcome to redistribute it";
    senjo::Output(senjo::Output::InfoPrefix) << "under certain conditions; See the LICENSE file for details";
    senjo::Output(senjo::Output::InfoPrefix) << "";

    senjo::Output(senjo::Output::InfoPrefix) << " ______ ";
    senjo::Output(senjo::Output::InfoPrefix) << " |___  / ";
    senjo::Output(senjo::Output::InfoPrefix) << "    / /  __ _   __ _  _ __  ___  _   _  ___ ";
    senjo::Output(senjo::Output::InfoPrefix) << "   / /  / _` | / _` || '__|/ _ \\| | | |/ __|";
    senjo::Output(senjo::Output::InfoPrefix) << "  / /__| (_| || (_| || |  |  __/| |_| |\\__ \\";
    senjo::Output(senjo::Output::InfoPrefix) << R"( /_____|\__,_| \__, ||_|   \___| \__,_||___/)";
    senjo::Output(senjo::Output::InfoPrefix) << "                __/ | ";
    senjo::Output(senjo::Output::InfoPrefix) << "               |___/ ";
    senjo::Output(senjo::Output::InfoPrefix) << "";

    senjo::Output(senjo::Output::InfoPrefix) << "Zagreus UCI chess engine by Danny Jelsma (https://github.com/Dannyj1)";

    if (argc == 2 && strcmp(argv[1], "bench") == 0) {
        senjo::Output(senjo::Output::InfoPrefix) << "Starting benchmark...";

        benchmark();
        return 0;
    }

    // TODO: calculate pieceweight on makemove? Same for updating material balance

    try {
        Zagreus::ZagreusEngine engine;
        senjo::UCIAdapter adapter(engine);

        /*Zagreus::Bitboard bb;
        bb.setFromFEN("r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13");
        bb.makeMove(0, 1, Zagreus::PieceType::WHITE_ROOK, Zagreus::PieceType::EMPTY);
        bb.setWhiteTimeMsec(999999999);
        Zagreus::searchManager.getBestMove(engine, bb, Zagreus::PieceColor::WHITE);*/

        std::string line;
        line.reserve(16384);

        while (std::getline(std::cin, line)) {
            try {
                if (!adapter.doCommand(line)) {
                    break;
                }
            } catch (const std::exception &e) {
                senjo::Output() << "ERROR: " << e.what();
                return -1;
            }
        }

        return 0;
    } catch (const std::exception &e) {
        senjo::Output(senjo::Output::InfoPrefix) << "ERROR: " << e.what();
        return 1;
    }
}

void benchmark() {
    Zagreus::ZagreusEngine engine;
    senjo::UCIAdapter adapter(engine);
    std::vector<std::string> positions = {"r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8",
                                          "r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14",
                                          "nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1",
                                          "r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13",
                                          "r3k2r/1bp2pbp/1p1p1n2/1p1qp3/2nPP3/2NBBNP1/PPP1QPP1/2KR3R w kq - 0 1",
                                          "r1bqk2r/pppp1ppp/2n5/2b1p3/2B1P1n1/5N2/PPPP1PPP/RNBQ1RK1 b kq - 9 6",
                                          "8/8/1n3k2/8/3P3P/5K2/8/1N4Q1 w - -",
                                          "1rb1kbnr/p1q1pppp/np4B1/2pp4/P1PP4/2N1P3/1P3PPP/R1BQK1NR w Kk",
                                          "1rbk1bnr/pp1p1ppp/n1pq4/4pP1Q/P1B1P3/2P4P/1P1P2P1/RNB2KNR b - -",
                                          "r1bn1r2/2k3p1/6p1/pP3p2/4P2P/1P1P3R/2P1NB2/Q3K1n1 b - -",
                                          "1rb1kb1r/p3p1pp/n4p2/1pppNn2/4P3/2PP3P/PB4P1/RN1QKB1R b KQk -",
                                          "8/3P4/p4k2/P2Q4/N7/7B/4p3/4K3 w - -",
                                          "1B1N3k/4r3/5b1P/2n1P3/3K2p1/6P1/4b3/4R3 b - ",
                                          "r1n1k1r1/1pb2p2/5P2/pPPp2p1/5p1R/P1N1N3/2P1P3/B1R1KB2 w - -",
                                          "8/8/1qB1k3/1p6/3p4/1p6/8/4K3 b - -",
                                          "6nr/1pq1b1k1/1N6/5Ppp/pp2B1P1/B1P4P/P2pK3/3R2NR b - -"};
    uint64_t nodes = 0;
    uint64_t totalMs = 0;

    for (std::string &position : positions) {
        for (int i = 0; i < 2; i++) {
            Zagreus::Bitboard bb;
            Zagreus::PieceColor color = i == 0 ? Zagreus::PieceColor::WHITE : Zagreus::PieceColor::BLACK;

            bb.setFromFEN(position);
            bb.setBenchmarking(true);
            bb.setWhiteTimeMsec(999999999);
            bb.setBlackTimeMsec(999999999);

            auto start = std::chrono::high_resolution_clock::now();
            Zagreus::searchManager.getBestMove(engine, bb, color);

            auto end = std::chrono::high_resolution_clock::now();
            nodes += Zagreus::searchManager.getSearchStats().nodes + Zagreus::searchManager.getSearchStats().qnodes;
            totalMs += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
    }

    uint64_t nps = nodes / (totalMs / 1000);
    senjo::Output(senjo::Output::NoPrefix) << nodes << " nodes " << nps << " nps";
}
