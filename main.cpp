#include <iostream>
#include <random>
#include <unordered_map>

#include "board.h"
#include "engine.h"
#include "senjo/UCIAdapter.h"

uint64_t captures = 0;
uint64_t checks = 0;
uint64_t checkMates = 0;
uint64_t ep = 0;
uint64_t castles = 0;
std::unordered_map<std::string, uint64_t> moveCounts{};

uint64_t perft(Chess::Board* board, Chess::PieceColor color, int depth) {
    uint64_t nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Chess::Move> moves = board->getPseudoLegalMoves(color);

    for (const Chess::Move &move : moves) {
        std::shared_ptr<Chess::Piece> piece = move.piece;
        Chess::TileLocation fromLoc = board->getPiecePosition(piece->getId());
        bool isCapture = false;
        bool isEp = false;
        bool isCastle = false;

        if (depth == 1) {
            if (move.tile->getPiece() && move.tile->getPiece()->getColor() != piece->getColor()) {
                isCapture = true;
            }

            if (piece->getPieceType() == Chess::PieceType::PAWN && fromLoc.x != move.tile->getX() &&
                move.tile->getEnPassantTarget() != nullptr) {
                isEp = true;
            }

            if (move.tile->getPiece() && piece->getPieceType() == Chess::PieceType::KING
                && move.tile->getPiece()->getPieceType() == Chess::PieceType::ROOK &&
                move.tile->getPiece()->getColor() == move.piece->getColor()) {
                isCastle = true;
            }
        }

        board->makeMove(move.tile->getX(), move.tile->getY(), piece);

        if (!board->isKingChecked(move.piece->getColor())) {
            if (moveCounts.contains(board->getTile(fromLoc.x, fromLoc.y)->getNotation())) {
                moveCounts[board->getTile(fromLoc.x, fromLoc.y)->getNotation()]++;
            } else {
                moveCounts[board->getTile(fromLoc.x, fromLoc.y)->getNotation()] = 1;
            }

            nodes += perft(board, Chess::getOppositeColor(color), depth - 1);

            if (depth == 1) {
                if (board->isKingChecked(getOppositeColor(move.piece->getColor()))) {
                    checks++;
                }

                if (board->getWinner() != Chess::PieceColor::NONE) {
                    checkMates++;
                }

                if (isCapture) {
                    captures++;
                }

                if (isEp) {
                    ep++;
                    captures++;
                }

                if (isCastle) {
                    castles++;
                }
            }
        }

        board->unmakeMove();
    }

    return nodes;
}

int main() {
    for (int i = 6; i < 16; i++) {
        std::cout << "Running perft for depth " << i << "..." << std::endl;
        Chess::Board board;
        // Position 1
        board.setFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        checks = 0;
        ep = 0;
        captures = 0;
        checkMates = 0;
        castles = 0;
        moveCounts.clear();

        auto start = std::chrono::system_clock::now();
        uint64_t nodes = perft(&board, Chess::PieceColor::WHITE, i);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "Depth " << i << " Nodes: " << nodes << ", Captures: " << captures << ", Checks: " << checks
                  << ", Checkmates: " << checkMates << ", E.p: " << ep << ", Castles: " << castles << ", Took: "
                  << elapsed_seconds.count() << "s" << std::endl;

        std::cout << std::endl;
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