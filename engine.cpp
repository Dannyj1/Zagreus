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

#include <algorithm>

#include "senjo/ChessEngine.h"
#include "senjo/Output.h"
#include "engine.h"
#include "search_mgr.h"
#include "move_gen.h"

namespace Zagreus {
    uint64_t captures = 0ULL;
    uint64_t checks = 0ULL;
    uint64_t ep = 0ULL;
    uint64_t castles = 0ULL;
    uint64_t promotions = 0ULL;

    uint64_t Engine::doPerft(Zagreus::Bitboard &perftBoard, Zagreus::PieceColor color, int depth, int startingDepth) {
        uint64_t nodes = 0ULL;

        if (depth == 0) {
            return 1ULL;
        }

        std::vector<Move> moves = generateLegalMoves(perftBoard, color);

        for (Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            /*bool isCapture = false;
            bool isEp = false;
            bool isCastle = false;
            Zagreus::PieceType fromPiece = perftBoard.getPieceOnSquare(move.fromSquare);
            Zagreus::PieceType toPiece = perftBoard.getPieceOnSquare(move.toSquare);

            if (toPiece != Zagreus::PieceType::EMPTY) {
                isCapture = true;
            }

            if (fromPiece == Zagreus::PieceType::WHITE_PAWN || fromPiece == Zagreus::PieceType::BLACK_PAWN) {
                if (toPiece == Zagreus::PieceType::EMPTY && std::abs((int) move.fromSquare - (int) move.toSquare) != 8
                    && std::abs((int) move.fromSquare - (int) move.toSquare) != 16) {
                    isEp = true;
                }
            }

            if (toPiece == Zagreus::PieceType::WHITE_KING || toPiece == Zagreus::PieceType::BLACK_KING) {
                isCastle = true;
            }*/

            perftBoard.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (perftBoard.isKingInCheck(color)) {
                perftBoard.unmakeMove();
                continue;
            }

/*            if (depth == 1) {
                if (move.promotionPiece != Zagreus::PieceType::EMPTY) {
                    promotions++;
                }

                if (perftBoard.isKingInCheck(Zagreus::Bitboard::getOppositeColor(color))) {
                    checks++;
                }

                if (isCapture) {
                    captures++;
                }

                if (isEp) {
                    ep++;
                }

                if (isCastle) {
                    castles++;
                }
            }*/

            uint64_t nodeAmount = doPerft(perftBoard, Zagreus::Bitboard::getOppositeColor(color), depth - 1,
                                          startingDepth);
            nodes += nodeAmount;

            if (depth == startingDepth && nodeAmount > 0LL) {
                std::string notation =
                        Zagreus::Bitboard::getNotation(move.fromSquare) +
                        Zagreus::Bitboard::getNotation(move.toSquare);
                senjo::Output(senjo::Output::InfoPrefix) << notation << ": " << nodeAmount;
            }

            perftBoard.unmakeMove();
        }

        return nodes;
    }

    std::string Engine::getEngineName() {
        return "Zagreus";
    }

    std::string Engine::getEngineVersion() {
        return "v0.5";
    }

    std::string Engine::getAuthorName() {
        return "Danny Jelsma";
    }

    std::string Engine::getEmailAddress() {
        return "";
    }

    std::string Engine::getCountryName() {
        return "The Netherlands";
    }

    std::list<senjo::EngineOption> Engine::getOptions() {
        return {};
    }

    bool Engine::setEngineOption(const std::string &optionName, const std::string &optionValue) {
        return false;
    }

    void Engine::initialize() {
        board = Bitboard{};
        engineColor = PieceColor::NONE;
        isEngineInitialized = true;
    }

    bool Engine::isInitialized() {
        return isEngineInitialized;
    }

    bool Engine::setPosition(const std::string &fen, std::string* remain) {
        board = {};
        engineColor = PieceColor::NONE;
        return board.setFromFEN(fen);
    }

    bool Engine::makeMove(const std::string &move) {
        if (move == "e1c1" && board.getCastlingRights() & CastlingRights::WHITE_QUEENSIDE &&
            board.getPieceOnSquare(Square::E1) == PieceType::WHITE_KING &&
            board.getPieceOnSquare(Square::A1) == PieceType::WHITE_ROOK) {
            board.makeStrMove("a1e1");
            return true;
        }

        if (move == "e1g1" && board.getCastlingRights() & CastlingRights::WHITE_KINGSIDE &&
            board.getPieceOnSquare(Square::E1) == PieceType::WHITE_KING &&
            board.getPieceOnSquare(Square::H1) == PieceType::WHITE_ROOK) {
            board.makeStrMove("h1e1");
            return true;
        }

        if (move == "e8c8" && board.getCastlingRights() & CastlingRights::BLACK_QUEENSIDE &&
            board.getPieceOnSquare(Square::E8) == PieceType::BLACK_KING &&
            board.getPieceOnSquare(Square::A8) == PieceType::BLACK_ROOK) {
            board.makeStrMove("a8e8");
            return true;
        }

        if (move == "e8g8" && board.getCastlingRights() & CastlingRights::BLACK_KINGSIDE &&
            board.getPieceOnSquare(Square::E8) == PieceType::BLACK_KING &&
            board.getPieceOnSquare(Square::H8) == PieceType::BLACK_ROOK) {
            board.makeStrMove("h8e8");
            return true;
        }

        board.makeStrMove(move);
        return true;
    }

    std::string Engine::getFEN() {
        // TODO: implement
        //return board.getFEN();
        return "";
    }

    void Engine::printBoard() {
        board.print();
    }

    bool Engine::whiteToMove() {
        return board.getMovingColor() == PieceColor::WHITE;
    }

    void Engine::clearSearchData() {
        searchManager.resetStats();
    }

    void Engine::ponderHit() {
    }

    bool Engine::isRegistered() {
        return true;
    }

    void Engine::registerLater() {

    }

    bool Engine::doRegistration(const std::string &name, const std::string &code) {
        return true;
    }

    bool Engine::isCopyProtected() {
        return false;
    }

    bool Engine::copyIsOK() {
        return true;
    }

    void Engine::setDebug(const bool flag) {

    }

    bool Engine::isDebugOn() {
        return false;
    }

    bool Engine::isSearching() {
        return searchManager.isCurrentlySearching();
    }

    void Engine::stopSearching() {
        // TODO: implement
    }

    bool Engine::stopRequested() {
        // TODO: implement
        return false;
    }

    void Engine::waitForSearchFinish() {
        while (searchManager.isCurrentlySearching()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    uint64_t Engine::perft(const int depth) {
        auto start = std::chrono::high_resolution_clock::now();
        uint64_t nodes = doPerft(board, board.getMovingColor(), depth, depth);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        senjo::Output(senjo::Output::InfoPrefix) << "Depth " << depth << " Nodes: " << nodes << ", Took: "
                                                 << elapsed_seconds.count() << "s";
        return nodes;
    }

    std::string Engine::go(const senjo::GoParams &params, std::string* ponder) {
        if (engineColor == PieceColor::NONE) {
            engineColor = board.getMovingColor();
        }

        board.setWhiteTimeMsec(params.wtime);
        board.setBlackTimeMsec(params.btime);
        board.setWhiteTimeIncrement(params.winc);
        board.setBlackTimeIncrement(params.binc);

//        board.print();
        SearchResult bestResult = searchManager.getBestMove(board, engineColor);

        if (bestResult.move.promotionPiece != PieceType::EMPTY) {
            std::string result = Zagreus::Bitboard::getNotation(bestResult.move.fromSquare)
                                 + Zagreus::Bitboard::getNotation(bestResult.move.toSquare)
                                 + Zagreus::Bitboard::getCharacterForPieceType(bestResult.move.promotionPiece);

            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c) { return std::tolower(c); });

            return result;
        }

        std::string result = Zagreus::Bitboard::getNotation(bestResult.move.fromSquare) +
                             Zagreus::Bitboard::getNotation(bestResult.move.toSquare);

        if (result == "a1e1" && board.getCastlingRights() & CastlingRights::WHITE_QUEENSIDE &&
            board.getPieceOnSquare(Square::E1) == PieceType::WHITE_KING &&
            board.getPieceOnSquare(Square::A1) == PieceType::WHITE_ROOK) {
            return "e1c1";
        }

        if (result == "h1e1" && board.getCastlingRights() & CastlingRights::WHITE_KINGSIDE &&
            board.getPieceOnSquare(Square::E1) == PieceType::WHITE_KING &&
            board.getPieceOnSquare(Square::H1) == PieceType::WHITE_ROOK) {
            return "e1g1";
        }

        if (result == "a8e8" && board.getCastlingRights() & CastlingRights::BLACK_QUEENSIDE &&
            board.getPieceOnSquare(Square::E8) == PieceType::BLACK_KING &&
            board.getPieceOnSquare(Square::A8) == PieceType::BLACK_ROOK) {
            return "e8c8";
        }

        if (result == "h8e8" && board.getCastlingRights() & CastlingRights::BLACK_KINGSIDE &&
            board.getPieceOnSquare(Square::E8) == PieceType::BLACK_KING &&
            board.getPieceOnSquare(Square::H8) == PieceType::BLACK_ROOK) {
            return "e8g8";
        }

        senjo::Output(senjo::Output::InfoPrefix) << "Score: " << bestResult.score;
        return Zagreus::Bitboard::getNotation(bestResult.move.fromSquare) +
               Zagreus::Bitboard::getNotation(bestResult.move.toSquare);
    }

    senjo::SearchStats Engine::getSearchStats() {
        return searchManager.getSearchStats();
    }

    void Engine::resetEngineStats() {

    }

    void Engine::showEngineStats() {
        // TODO: implement
    }
}