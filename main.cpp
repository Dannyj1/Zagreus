#include <iostream>
#include <unordered_map>

#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "search_mgr.h"
#include "bitboard.h"
#include "senjo/Output.h"
#include "bitboard.h"
#include "move_gen.h"

uint64_t captures = 0ULL;
uint64_t checks = 0ULL;
uint64_t ep = 0ULL;
uint64_t castles = 0ULL;
uint64_t promotions = 0ULL;

uint64_t perft(Chess::Bitboard &board, Chess::PieceColor color, int depth, int startingDepth) {
    uint64_t nodes = 0ULL;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Chess::Move> moves = Chess::generatePseudoLegalMoves(board, color);

    for (const Chess::Move &move : moves) {
        bool isCapture = false;
        bool isEp = false;
        bool isCastle = false;

/*
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
*/

        if (board.getPieceOnSquare(move.toSquare) != Chess::PieceType::EMPTY) {
            isCapture = true;
        }

        board.makeMove(move.fromSquare, move.toSquare, move.pieceType);

        if (!board.isKingInCheck(color)) {
/*            if (move.promotionPiece) {
                promotions++;
            }*/

            if (depth == 1) {
                if (board.isKingInCheck(Chess::Bitboard::getOppositeColor(color))) {
                    checks++;
                }

                if (isCapture) {
                    captures++;
                }

                /*
                 if (isEp) {
                     ep++;
                     captures++;
                 }

                 if (isCastle) {
                     castles++;
                 }*/
            }

            uint64_t nodeAmount = perft(board, Chess::Bitboard::getOppositeColor(color), depth - 1, startingDepth);
            nodes += nodeAmount;

            if (depth == startingDepth && nodeAmount > 0LL) {
                std::string notation =
                        Chess::Bitboard::getNotation(move.fromSquare) + Chess::Bitboard::getNotation(move.toSquare);
                std::cout << notation << ": " << nodeAmount << std::endl;
            }
        }

        board.unmakeMove();
    }

    return nodes;
}

int main() {
    // Custom test pos 1: r3kb1r/pppbqppp/4pn2/n2p4/3P1B2/2PBPN2/PP1N1PPP/R2QK2R w KQkq - 5 8
    // Custom test pos 2: r2qr1k1/1p1bbppp/2pp1n2/2n5/p2NPB2/P1N2Q1P/BPP2PP1/R3R1K1 b - - 2 14
    // Custom test pos 3: nk5b/2rBppP1/pPpp1R2/1NP1Qpr1/3PPPp1/3RKPN1/1Pq1np1b/3B4 w - - 0 1
    // eval test: 8/6Q1/8/k1NN1R2/P3P3/1p6/P1P2KPP/R7 w - - 0 33
    // Default pos: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

/*    Chess::Bitboard bb;
    bb.setFromFEN("rnbqkbnr/pppppp1p/8/6p1/7P/4P3/PPPP1PP1/RNBQKBNR b KQkq - 0 2");
    bb.print();
    bb.makeMove(Chess::Square::G5, Chess::Square::G4, Chess::PieceType::BLACK_PAWN);
    bb.print();
    std::vector<Chess::Move> availableMoves = Chess::generatePseudoLegalMoves(bb, Chess::PieceColor::WHITE);
    bb.printAvailableMoves(availableMoves);
    bb.unmakeMove();
    bb.print();*/

    for (int i = 1; i < 16; i++) {
        std::cout << "Running perft for depth " << i << "..." << std::endl;
        Chess::Bitboard board;
        board.setFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        checks = 0LL;
        ep = 0LL;
        captures = 0LL;
        castles = 0LL;

        auto start = std::chrono::system_clock::now();
        uint64_t nodes = perft(board, Chess::PieceColor::WHITE, i, i);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "Depth " << i << " Nodes: " << nodes << ", Captures: " << captures << ", Checks: " << checks
                  << ", E.p: " << ep << ", Castles: " << castles
                  << ", Promotions: " << promotions << ", Took: " << elapsed_seconds.count() << "s" << std::endl;

        std::cout << std::endl;
    }

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