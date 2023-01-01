#include <iostream>
#include <unordered_map>

#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "search_mgr.h"
#include "bitboard.h"
#include "senjo/Output.h"
#include "bitboard.h"
#include "move_gen.h"

int main() {
    // Custom test pos 1: r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8
    // Custom test pos 2: r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14
    // Custom test pos 3: nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1
    // Custom test pos 4: r1bq1rk1/pp2p1b1/2pp1nnp/3P2p1/2P2p1P/2N1PNB1/PP2BPP1/R2QK2R w KQ - 0 13
    // Custom test pos 5: r3k2r/1bp2pbp/1p1p1n2/1p1qp3/2nPP3/2NBBNP1/PPP1QPP1/2KR3R w kq - 0 1
    // eval test: 8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33
    // Default pos: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // More test positions: https://github.com/elcabesa/vajolet/blob/master/tests/perft.txt

    std::cout << " ______ " << std::endl;
    std::cout << " |___  / " << std::endl;
    std::cout << "    / /  __ _   __ _  _ __  ___  _   _  ___ " << std::endl;
    std::cout << "   / /  / _` | / _` || '__|/ _ \\| | | |/ __|" << std::endl;
    std::cout << "  / /__| (_| || (_| || |  |  __/| |_| |\\__ \\" << std::endl;
    std::cout << R"( /_____|\__,_| \__, ||_|   \___| \__,_||___/)" << std::endl;
    std::cout << "                __/ | " << std::endl;
    std::cout << "               |___/ " << std::endl;
    std::cout << std::endl;

    std::cout << "Zagreus chess engine by Dannyj1 (https://github.com/Dannyj1)" << std::endl;
    std::cout << std::endl;

    try {
        Chess::Engine engine;
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
        std::cout << "ERROR: " << e.what();
        return 1;
    }
}