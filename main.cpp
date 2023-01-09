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
#include <random>

#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "bitboard.h"
#include "senjo/Output.h"
#include "move_gen.h"
#include "types.h"
#include "search_mgr.h"
#include "magics.h"
#include "pst.h"

uint64_t perft(Zagreus::Bitboard &perftBoard, Zagreus::PieceColor color, int depth, int startingDepth) {
    uint64_t nodes = 0ULL;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Zagreus::Move> moves = generateLegalMoves(perftBoard, color);

    for (Zagreus::Move &move : moves) {
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

int main() {
    // Custom test pos 1: r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8
    // Custom test pos 2: r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14
    // Custom test pos 3: nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1
    // Custom test pos 4: r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13
    // Custom test pos 5: r3k2r/1bp2pbp/1p1p1n2/1p1qp3/2nPP3/2NBBNP1/PPP1QPP1/2KR3R w kq - 0 1
    // Custom test pos 6: r1bqk2r/pppp1ppp/2n5/2b1p3/2B1P1n1/5N2/PPPP1PPP/RNBQ1RK1 b kq - 9 6
    // eval test: 8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33
    // Default pos: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // More test positions: https://github.com/elcabesa/vajolet/blob/master/tests/perft.txt

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

    senjo::Output(senjo::Output::InfoPrefix) << "Zagreus chess engine by Danny Jelsma (https://github.com/Dannyj1)";
    senjo::Output(senjo::Output::InfoPrefix) << "";

    // TODO: calculate pieceweight on makemove? Same for updating material balance
/*    Zagreus::Bitboard bb;
    bb.setFromFEN("r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13");
    bb.setWhiteTimeMsec(999999999);
    Zagreus::searchManager.getBestMove(bb, Zagreus::PieceColor::WHITE);*/

    try {
        Zagreus::Engine engine;
        senjo::UCIAdapter adapter(engine);

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