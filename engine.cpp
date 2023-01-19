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
#include "bitboard.h"
#include "types.h"
#include "movegen.h"
#include "utils.h"

namespace Zagreus {
    uint64_t ZagreusEngine::doPerft(Bitboard &perftBoard, PieceColor color, int depth, int startingDepth) {
        uint64_t nodes = 0ULL;

        if (depth == 0) {
            return 1ULL;
        }

        MoveList moves;

        if (color == PieceColor::WHITE) {
            moves = generateMoves<PieceColor::WHITE>(perftBoard);
        } else if (color == PieceColor::BLACK) {
            moves = generateMoves<PieceColor::BLACK>(perftBoard);
        } else {
            return 0;
        }

        for (int i = 0; i < moves.count; i++) {
            Move move = moves.moves[i];

            perftBoard.makeMove(move);

            if (color == PieceColor::WHITE) {
                if (perftBoard.isKingInCheck<PieceColor::WHITE>()) {
                    perftBoard.unmakeMove(move);
                    continue;
                }
            } else {
                if (perftBoard.isKingInCheck<PieceColor::BLACK>()) {
                    perftBoard.unmakeMove(move);
                    continue;
                }
            }

            uint64_t nodeAmount = doPerft(perftBoard, getOppositeColor(color), depth - 1,
                                          startingDepth);
            nodes += nodeAmount;
            perftBoard.unmakeMove(move);

            if (depth == startingDepth && nodeAmount > 0ULL) {
                std::string notation = getNotation(move.from) + getNotation(move.to);
                senjo::Output(senjo::Output::NoPrefix) << notation << ": " << nodeAmount;
            }
        }

        return nodes;
    }

    std::string ZagreusEngine::getEngineName() {
        return "Zagreus";
    }

    std::string ZagreusEngine::getEngineVersion() {
        return "v2.0.0-dev";
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

/*                if (option.getName() == "Hash") {
                    TranspositionTable::getTT()->setTableSize(option.getIntValue());
                }*/

                return true;
            }
        }

        return false;
    }

    void ZagreusEngine::initialize() {
        board = Bitboard{};
        engineColor = PieceColor::NONE;
//        TranspositionTable::getTT()->setTableSize(getOption("Hash").getIntValue());
        isEngineInitialized = true;
    }

    bool ZagreusEngine::isInitialized() {
        return isEngineInitialized;
    }

    bool ZagreusEngine::setPosition(const std::string &fen, std::string* remain) {
        board = {};
        engineColor = PieceColor::NONE;
        return board.setFromFen(fen);
    }

    bool ZagreusEngine::makeMove(const std::string &move) {
        // board.makeStrMove(move);
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
//        searchManager.resetStats();
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
//        return searchManager.isCurrentlySearching();
        return true;
    }

    void ZagreusEngine::stopSearching() {
        // TODO: implement
    }

    bool ZagreusEngine::stopRequested() {
        // TODO: implement
        return false;
    }

    void ZagreusEngine::waitForSearchFinish() {
/*        while (searchManager.isCurrentlySearching()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }*/
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
        /*if (engineColor == PieceColor::NONE) {
            engineColor = board.getMovingColor();
        }

        searchManager.setWhiteTimeMsec(params.wtime);
        searchManager.setBlackTimeMsec(params.btime);
        searchManager.setWhiteTimeIncrement(params.winc);
        searchManager.setBlackTimeIncrement(params.binc);

//        board.print();
        SearchResult bestResult = searchManager.getBestMove(*this, board, engineColor);

        if (bestResult.move.promotionPiece != PieceType::EMPTY) {
            std::string result = Bitboard::getNotation(bestResult.move.fromSquare)
                                 + Bitboard::getNotation(bestResult.move.toSquare)
                                 + Bitboard::getCharacterForPieceType(bestResult.move.promotionPiece);

            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c) { return std::tolower(c); });

            return result;
        }

        std::string result = Bitboard::getNotation(bestResult.move.fromSquare) +
                             Bitboard::getNotation(bestResult.move.toSquare);

        senjo::Output(senjo::Output::InfoPrefix) << "Score: " << bestResult.score;
        return Bitboard::getNotation(bestResult.move.fromSquare) +
               Bitboard::getNotation(bestResult.move.toSquare);*/
        return "";
    }

    senjo::SearchStats ZagreusEngine::getSearchStats() {
        return searchStats;
    }

    void ZagreusEngine::resetEngineStats() {

    }

    void ZagreusEngine::showEngineStats() {
        // TODO: implement
    }
}