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
#include "tt.h"

namespace Zagreus {
    uint64_t ZagreusEngine::doPerft(Zagreus::Bitboard &perftBoard, Zagreus::PieceColor color, int depth, int startingDepth) {
        uint64_t nodes = 0ULL;

        if (depth == 0) {
            return 1ULL;
        }

        std::vector<Move> moves = generateLegalMoves(perftBoard, color);

        for (Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            perftBoard.makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (perftBoard.isKingInCheck(color)) {
                perftBoard.unmakeMove();
                continue;
            }

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

    std::string ZagreusEngine::getEngineName() {
        return "Zagreus";
    }

    std::string ZagreusEngine::getEngineVersion() {
        return "v1.0";
    }

    std::string ZagreusEngine::getAuthorName() {
        return "Danny Jelsma";
    }

    std::string ZagreusEngine::getEmailAddress() {
        return "";
    }

    std::string ZagreusEngine::getCountryName() {
        return "The Netherlands";
    }

    std::list<senjo::EngineOption> ZagreusEngine::getOptions() {
        return options;
    }

    senjo::EngineOption ZagreusEngine::getOption(const std::string &optionName) {
        for (senjo::EngineOption &option : options) {
            if (option.getName() == optionName) {
                return option;
            }
        }

        return senjo::EngineOption("Invalid Option");
    }

    bool ZagreusEngine::setEngineOption(const std::string &optionName, const std::string &optionValue) {
        for (senjo::EngineOption &option : options) {
            if (option.getName() == optionName) {
                option.setValue(optionValue);

                if (option.getName() == "Hash") {
                    tt.setTableSize(option.getIntValue());
                }

                return true;
            }
        }

        return false;
    }

    void ZagreusEngine::initialize() {
        board = Bitboard{};
        engineColor = PieceColor::NONE;
        tt.setTableSize(getOption("Hash").getIntValue());
        isEngineInitialized = true;
    }

    bool ZagreusEngine::isInitialized() {
        return isEngineInitialized;
    }

    bool ZagreusEngine::setPosition(const std::string &fen, std::string* remain) {
        board = {};
        engineColor = PieceColor::NONE;
        return board.setFromFEN(fen);
    }

    bool ZagreusEngine::makeMove(const std::string &move) {
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

    std::string ZagreusEngine::getFEN() {
        // TODO: implement
        //return board.getFEN();
        return "";
    }

    void ZagreusEngine::printBoard() {
        board.print();
    }

    bool ZagreusEngine::whiteToMove() {
        return board.getMovingColor() == PieceColor::WHITE;
    }

    void ZagreusEngine::clearSearchData() {
        searchManager.resetStats();
    }

    void ZagreusEngine::ponderHit() {
    }

    bool ZagreusEngine::isRegistered() {
        return true;
    }

    void ZagreusEngine::registerLater() {

    }

    bool ZagreusEngine::doRegistration(const std::string &name, const std::string &code) {
        return true;
    }

    bool ZagreusEngine::isCopyProtected() {
        return false;
    }

    bool ZagreusEngine::copyIsOK() {
        return true;
    }

    void ZagreusEngine::setDebug(const bool flag) {

    }

    bool ZagreusEngine::isDebugOn() {
        return false;
    }

    bool ZagreusEngine::isSearching() {
        return searchManager.isCurrentlySearching();
    }

    void ZagreusEngine::stopSearching() {
        // TODO: implement
    }

    bool ZagreusEngine::stopRequested() {
        // TODO: implement
        return false;
    }

    void ZagreusEngine::waitForSearchFinish() {
        while (searchManager.isCurrentlySearching()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    uint64_t ZagreusEngine::perft(const int depth) {
        auto start = std::chrono::high_resolution_clock::now();
        uint64_t nodes = doPerft(board, board.getMovingColor(), depth, depth);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        senjo::Output(senjo::Output::InfoPrefix) << "Depth " << depth << " Nodes: " << nodes << ", Took: "
                                                 << elapsed_seconds.count() << "s";
        return nodes;
    }

    std::string ZagreusEngine::go(const senjo::GoParams &params, std::string* ponder) {
        if (engineColor == PieceColor::NONE) {
            engineColor = board.getMovingColor();
        }

        board.setWhiteTimeMsec(params.wtime);
        board.setBlackTimeMsec(params.btime);
        board.setWhiteTimeIncrement(params.winc);
        board.setBlackTimeIncrement(params.binc);

//        board.print();
        SearchResult bestResult = searchManager.getBestMove(*this, board, engineColor);

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

    senjo::SearchStats ZagreusEngine::getSearchStats() {
        return searchManager.getSearchStats();
    }

    void ZagreusEngine::resetEngineStats() {

    }

    void ZagreusEngine::showEngineStats() {
        // TODO: implement
    }
}