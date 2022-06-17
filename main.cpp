#include <iostream>
#include <unordered_map>

#include "board.h"
#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "search_mgr.h"

uint64_t captures = 0;
uint64_t checks = 0;
uint64_t checkMates = 0;
uint64_t ep = 0;
uint64_t castles = 0;
uint64_t promotions = 0;

// TODO: consider taking care of things like dangling pointer and freeing memory
uint64_t perft(Chess::Board* board, Chess::PieceColor color, int depth, int startingDepth) {
    uint64_t nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Chess::Move> moves = board->getPseudoLegalMoves(color);

    for (const Chess::Move &move : moves) {
        Chess::Piece* piece = move.piece;
        Chess::TileLocation fromLoc = board->getPiecePosition(piece->getId());
        bool isCapture = false;
        bool isEp = false;
        bool isCastle = false;

        if (fromLoc.x < 0) {
            board->print();
            std::cout << "Error: piece not found. Type: " << piece->getPieceType() << ", Color: " << piece->getColor() << ", To: " << move.tile->getNotation() << std::endl;
        }

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
                std::string notation = board->getTileUnsafe(fromLoc.x, fromLoc.y)->getNotation() + board->getTileUnsafe(move.tile->getX(), move.tile->getY())->getNotation();
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
    // Custom test pos 1: r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8
    // Custom test pos 2: r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14
    // Custom test pos 3: nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1
    // eval test: 8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33

/*    for (int i = 1; i < 16; i++) {
        std::cout << "Running perft for depth " << i << "..." << std::endl;
        Chess::Board board;
        board.setFromFEN("nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1");
        checks = 0;
        ep = 0;
        captures = 0;
        checkMates = 0;
        castles = 0;

        auto start = std::chrono::system_clock::now();
        uint64_t nodes = perft(&board, board.getMovingColor(), i, i);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "Depth " << i << " Nodes: " << nodes << ", Captures: " << captures << ", Checks: " << checks
                  << ", Checkmates: " << checkMates << ", E.p: " << ep << ", Castles: " << castles
                  << ", Promotions: " << promotions << ", Took: " << elapsed_seconds.count() << "s" << std::endl;

        std::cout << std::endl;
    }*/


/*    Chess::Board board;
    Chess::SearchManager searchManager{};

    board.setFromFEN("8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33");
    Chess::Move bestMove = searchManager.getBestMove(&board, Chess::PieceColor::WHITE).move;
    Chess::TileLocation pieceLoc = board.getPiecePosition(bestMove.piece->getId());
    Chess::Tile* tile = board.getTileUnsafe(pieceLoc.x, pieceLoc.y);

    std::cout << "Move: " << tile->getNotation() << bestMove.tile->getNotation() << std::endl;*/

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