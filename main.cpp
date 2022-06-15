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
uint64_t promotions = 0;

uint64_t perft(Chess::Board* board, Chess::PieceColor color, int depth, int startingDepth) {
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

/*        if (isEp && depth == 1) {
            board->print();
        }*/

        board->makeMove(move.tile->getX(), move.tile->getY(), piece, move.promotionPiece);

/*        if (isEp && depth == 1) {
            board->print();
        }*/

        if (!board->isKingChecked(move.piece->getColor())) {
            if (move.promotionPiece) {
                promotions++;
            }

            uint64_t nodeAmount = perft(board, Chess::getOppositeColor(color), depth - 1, startingDepth);
            nodes += nodeAmount;

            if (depth == startingDepth) {
                std::string notation = board->getTile(fromLoc.x, fromLoc.y)->getNotation() + board->getTile(move.tile->getX(), move.tile->getY())->getNotation();
                std::cout << notation << ": " << nodeAmount << std::endl;
            }

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
/*        if (isEp && depth == 1) {
            board->print();
            std::cout << "==============================" << std::endl;
        }*/
    }

    return nodes;
}

int main() {
    for (int i = 2; i < 3; i++) {
        std::cout << "Running perft for depth " << i << "..." << std::endl;
        Chess::Board board;
        // Position 2
        board.setFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
        checks = 0;
        ep = 0;
        captures = 0;
        checkMates = 0;
        castles = 0;

        auto start = std::chrono::system_clock::now();
        uint64_t nodes = perft(&board, Chess::PieceColor::WHITE, i, i);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "Depth " << i << " Nodes: " << nodes << ", Captures: " << captures << ", Checks: " << checks
                  << ", Checkmates: " << checkMates << ", E.p: " << ep << ", Castles: " << castles
                  << ", Promotions: " << promotions << ", Took: " << elapsed_seconds.count() << "s" << std::endl;

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