#include <iostream>
#include "board.h"
#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "senjo/Output.h"

uint64_t perft(Chess::Board* board, Chess::PieceColor color, int depth) {
    uint64_t nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Chess::Move> moves = board->getLegalMoves(color, true);

    for (const Chess::Move &move : moves) {
        board->makeMove(move.tile->getX(), move.tile->getY(), move.piece);

        if (!board->isKingChecked(move.piece->getColor())) {
            nodes += perft(board, Chess::getOppositeColor(color), depth - 1);
        }

        board->unmakeMove();
    }

    return nodes;
}

int main() {
    for (int i = 7; i < 16; i++) {
        std::cout << "Running perft for depth " << i << "..." << std::endl;
        Chess::Board board;
        auto start = std::chrono::system_clock::now();
        uint64_t nodes = perft(&board, Chess::PieceColor::WHITE, i);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "Depth " << i << " Nodes: " << nodes << ", Took: " << elapsed_seconds.count() << "s"
                  << std::endl;
    }

    std::cout << "Perft done!" << std::endl;

    try {
        Engine engine;
        senjo::UCIAdapter adapter(engine);

        std::string line;
        line.reserve(16384);

        while (std::getline(std::cin, line)) {
            senjo::Output() << "INFO: " << line;
            if (!adapter.doCommand(line)) {
                break;
            }
        }

        return 0;
    } catch (const std::exception &e) {
        senjo::Output() << "ERROR: " << e.what();
        return 1;
    }
}