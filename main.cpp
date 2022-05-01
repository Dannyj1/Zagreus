#include <iostream>
#include <random>

#include "board.h"
#include "engine.h"
#include "senjo/UCIAdapter.h"

uint64_t perft(Chess::Board* board, Chess::PieceColor color, int depth) {
    uint64_t nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Chess::Move> moves = board->getLegalMoves(color, true);

    for (const Chess::Move &move : moves) {
        std::shared_ptr<Chess::Piece> piece = move.piece;
        board->makeMove(move.tile->getX(), move.tile->getY(), piece);

        if (!board->isKingChecked(move.piece->getColor())) {
            nodes += perft(board, Chess::getOppositeColor(color), depth - 1);
        }

        board->unmakeMove();
    }

    return nodes;
}

int main() {
    for (int i = 1; i < 16; i++) {
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
        Chess::Engine engine;
        senjo::UCIAdapter adapter(engine);

        std::string line;
        line.reserve(16384);

        while (std::getline(std::cin, line)) {
            //try {
                if (!adapter.doCommand(line)) {
                    break;
                }
/*            } catch (const std::exception &e) {
                senjo::Output() << "ERROR: " << e.what();
                return -1;
            }*/
        }

        return 0;
    } catch (const std::exception &e) {
        std::cout << "ERROR: " << e.what();
        return 1;
    }
}