#include <iostream>
#include <unordered_map>

#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "bitboard.h"
#include "senjo/Output.h"
#include "move_gen.h"
#include "types.h"
#include "search_mgr.h"

uint64_t perft(Zagreus::Bitboard &perftBoard, Zagreus::PieceColor color, int depth, int startingDepth) {
    uint64_t nodes = 0ULL;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Zagreus::Move> moves = generateLegalMoves(perftBoard, color);

    for (const Zagreus::Move &move : moves) {
        assert(move.fromSquare != move.toSquare);

        perftBoard.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);
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

    senjo::Output(senjo::Output::InfoPrefix) << " ______ ";
    senjo::Output(senjo::Output::InfoPrefix) << " |___  / ";
    senjo::Output(senjo::Output::InfoPrefix) << "    / /  __ _   __ _  _ __  ___  _   _  ___ ";
    senjo::Output(senjo::Output::InfoPrefix) << "   / /  / _` | / _` || '__|/ _ \\| | | |/ __|";
    senjo::Output(senjo::Output::InfoPrefix) << "  / /__| (_| || (_| || |  |  __/| |_| |\\__ \\";
    senjo::Output(senjo::Output::InfoPrefix) << R"( /_____|\__,_| \__, ||_|   \___| \__,_||___/)";
    senjo::Output(senjo::Output::InfoPrefix) << "                __/ | ";
    senjo::Output(senjo::Output::InfoPrefix) << "               |___/ ";

    senjo::Output(senjo::Output::InfoPrefix) << "Zagreus chess engine by Dannyj1 (https://github.com/Dannyj1)";


    // TODO: position startpos moves e2e4 c7c5 g1f3 d7d6 b1c3 g8f6 d2d4 c5d4 f3d4 a7a6 c1e3 e7e5 d4b3 f8e7 f2f3 c8e6 f1d3 d6d5 e4d5 f6d5 c3d5 d8d5 e1g1 g7g6 f3f4 b7b5 f4e5
/*    Zagreus::Bitboard bb;
    bb.setFromFEN("r1bq1rk1/1pp1npp1/pb1p1n1p/3Pp3/2B1P3/2P2N1P/PP3PP1/RNBQR1K1 w - - 1 11");
    bb.setWhiteTimeMsec(999999999);
    Zagreus::searchManager.getBestMove(bb, bb.getMovingColor());*/

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