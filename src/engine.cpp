/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "engine.h"

#include <algorithm>
#include <iomanip>

#include "../senjo/ChessEngine.h"
#include "../senjo/Output.h"
#include "../senjo/GoParams.h"
#include "../senjo/SearchStats.h"
#include "bitboard.h"
#include "evaluate.h"
#include "movegen.h"
#include "search.h"
#include "tt.h"
#include "types.h"
#include "utils.h"

namespace Zagreus {
uint64_t ZagreusEngine::doPerft(Bitboard& perftBoard, PieceColor color, int16_t depth,
                                int startingDepth) {
    uint64_t nodes = 0ULL;

    if (depth == 0) {
        return 1ULL;
    }

    MoveListPool* moveListPool = MoveListPool::getInstance();
    MoveList* moves = moveListPool->getMoveList();

    if (color == WHITE) {
        generateMoves<WHITE, NORMAL>(perftBoard, moves);
    } else if (color == BLACK) {
        generateMoves<BLACK, NORMAL>(perftBoard, moves);
    } else {
        moveListPool->releaseMoveList(moves);
        return 0;
    }

    for (int i = 0; i < moves->size; i++) {
        Move move = moves->moves[i];

        perftBoard.makeMove(move);

        if (color == WHITE) {
            if (perftBoard.isKingInCheck<WHITE>()) {
                perftBoard.unmakeMove(move);
                continue;
            }
        } else {
            if (perftBoard.isKingInCheck<BLACK>()) {
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

    moveListPool->releaseMoveList(moves);
    return nodes;
}

std::string ZagreusEngine::getEngineName() { return "Zagreus"; }

std::string majorVersion = ZAGREUS_VERSION_MAJOR;
std::string minorVersion = ZAGREUS_VERSION_MINOR;

std::string ZagreusEngine::getEngineVersion() {
    if (majorVersion != "dev") {
        return "v" + majorVersion + "." + minorVersion;
    }
    return majorVersion + "-" + minorVersion;
}

std::string ZagreusEngine::getAuthorName() { return "Danny Jelsma"; }

std::string ZagreusEngine::getEmailAddress() { return ""; }

std::string ZagreusEngine::getCountryName() { return "The Netherlands"; }

std::list<senjo::EngineOption> ZagreusEngine::getOptions() { return options; }

senjo::EngineOption ZagreusEngine::getOption(const std::string& optionName) {
    for (senjo::EngineOption& option : options) {
        if (option.getName() == optionName) {
            return option;
        }
    }

    return senjo::EngineOption("Invalid Option");
}

bool ZagreusEngine::setEngineOption(const std::string& optionName, const std::string& optionValue) {
    for (senjo::EngineOption& option : options) {
        if (option.getName() == optionName) {
            option.setValue(optionValue);

            if (option.getName() == "Hash") {
                TranspositionTable::getTT()->setTableSize(option.getIntValue());
            }

            return true;
        }
    }

    return false;
}

void ZagreusEngine::initialize() {
    stoppingSearch = false;
    board = Bitboard{};
    TranspositionTable::getTT()->setTableSize(getOption("Hash").getIntValue());
    isEngineInitialized = true;
}

bool ZagreusEngine::isInitialized() { return isEngineInitialized; }

bool ZagreusEngine::setPosition(const std::string& fen, std::string* remain) {
    stoppingSearch = false;
    board = {};
    return board.setFromFen(fen);
}

bool ZagreusEngine::makeMove(const std::string& move) {
    stoppingSearch = false;
    return board.makeStrMove(move);
}

std::string ZagreusEngine::getFEN() {
    // TODO: implement
    return "";
}

void ZagreusEngine::printBoard() { board.print(); }

bool ZagreusEngine::whiteToMove() { return board.getMovingColor() == WHITE; }

void ZagreusEngine::clearSearchData() {
}

void ZagreusEngine::ponderHit() {
}

bool ZagreusEngine::isRegistered() { return true; }

void ZagreusEngine::registerLater() {
}

bool ZagreusEngine::doRegistration(const std::string& name, const std::string& code) {
    return true;
}

bool ZagreusEngine::isCopyProtected() { return false; }

bool ZagreusEngine::copyIsOK() { return true; }

void ZagreusEngine::setDebug(const bool flag) {
}

bool ZagreusEngine::isDebugOn() { return false; }

bool ZagreusEngine::isSearching() {
    return searching;
}

void ZagreusEngine::stopSearching() { stoppingSearch = true; }

bool ZagreusEngine::stopRequested() { return stoppingSearch; }

void ZagreusEngine::waitForSearchFinish() {
    while (isSearching()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

uint64_t ZagreusEngine::perft(const int16_t depth) {
    stoppingSearch = false;
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = doPerft(board, board.getMovingColor(), depth, depth);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = end - start;

    senjo::Output(senjo::Output::InfoPrefix)
        << "Depth " << depth << " Nodes: " << nodes
        << ", Took: " << elapsedSeconds.count() << "s";
    return nodes;
}

std::string ZagreusEngine::go(senjo::GoParams& params, std::string* ponder) {
    stoppingSearch = false;
    searching = true;
    searchStats = {};
    Move bestMove;

    if (board.getMovingColor() == WHITE) {
        bestMove = getBestMove<WHITE>(params, *this, board, searchStats);
    } else {
        bestMove = getBestMove<BLACK>(params, *this, board, searchStats);
    }

    if (bestMove.promotionPiece != EMPTY) {
        std::string result = getNotation(bestMove.from) + getNotation(bestMove.to) +
                             getCharacterForPieceType(bestMove.promotionPiece);

        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        searching = false;
        return result;
    }

    std::string result = getNotation(bestMove.from) + getNotation(bestMove.to);

    searching = false;
    searching = true;
    return getNotation(bestMove.from) + getNotation(bestMove.to);
}

senjo::SearchStats ZagreusEngine::getSearchStats() { return searchStats; }

void ZagreusEngine::resetEngineStats() {
}

void ZagreusEngine::showEngineStats() {
}

bool ZagreusEngine::isTuning() const { return tuning; }

void ZagreusEngine::setTuning(bool tuning) { ZagreusEngine::tuning = tuning; }

std::string padNumber(int number) {
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill(' ') << number;
    return oss.str();
}

// clang-format off
void ZagreusEngine::printTraceEval() {
    Evaluation eval = Evaluation(board);
    int score = eval.evaluate<true>();
    std::map<TraceMetric, int> midgameTrace = eval.getMidgameTraceMetrics();
    std::map<TraceMetric, int> endgameTrace = eval.getEndgameTraceMetrics();
    std::map<TraceMetric, int> taperedTrace = eval.getTaperedTraceMetrics();

    std::cout << "==============================================================" << std::endl;
    std::cout << "| Evaluation Score: " << padNumber(score) <<
        "        po                             |" << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "| Metric                    | Midgame  | Endgame  | Tapered  |" << std::endl;
    std::cout << "==============================================================" << std::endl;
    std::cout << "| WHITE_MATERIAL            |   " << padNumber(midgameTrace[WHITE_MATERIAL]) <<
        "   |   " << padNumber(endgameTrace[WHITE_MATERIAL]) << "   |   " << padNumber(
            taperedTrace[WHITE_MATERIAL]) << "   |" << std::endl;
    std::cout << "| BLACK_MATERIAL            |   " << padNumber(midgameTrace[BLACK_MATERIAL]) <<
        "   |   " << padNumber(endgameTrace[BLACK_MATERIAL]) << "   |   " << padNumber(
            taperedTrace[BLACK_MATERIAL]) << "   |" << std::endl;
    std::cout << "| WHITE_PST                 |   " << padNumber(midgameTrace[WHITE_PST]) <<
        "   |   " << padNumber(endgameTrace[WHITE_PST]) << "   |   " << padNumber(
            taperedTrace[WHITE_PST]) << "   |" << std::endl;
    std::cout << "| BLACK_PST                 |   " << padNumber(midgameTrace[BLACK_PST]) <<
        "   |   " << padNumber(endgameTrace[BLACK_PST]) << "   |   " << padNumber(
            taperedTrace[BLACK_PST]) << "   |" << std::endl;
    std::cout << "| WHITE_MOBILITY            |   " << padNumber(midgameTrace[WHITE_MOBILITY]) <<
        "   |   " << padNumber(endgameTrace[WHITE_MOBILITY]) << "   |   " << padNumber(
            taperedTrace[WHITE_MOBILITY]) << "   |" << std::endl;
    std::cout << "| BLACK_MOBILITY            |   " << padNumber(midgameTrace[BLACK_MOBILITY]) <<
        "   |   " << padNumber(endgameTrace[BLACK_MOBILITY]) << "   |   " << padNumber(
            taperedTrace[BLACK_MOBILITY]) << "   |" << std::endl;
    std::cout << "| WHITE_KING_SAFETY_ATTACKS |   " <<
        padNumber(midgameTrace[WHITE_KING_SAFETY_ATTACKS]) << "   |   " <<
        padNumber(endgameTrace[WHITE_KING_SAFETY_ATTACKS]) << "   |   " << padNumber(
            taperedTrace[WHITE_KING_SAFETY_ATTACKS]) << "   |" << std::endl;
    std::cout << "| BLACK_KING_SAFETY_ATTACKS |   " <<
        padNumber(midgameTrace[BLACK_KING_SAFETY_ATTACKS]) << "   |   " <<
        padNumber(endgameTrace[BLACK_KING_SAFETY_ATTACKS]) << "   |   " << padNumber(
            taperedTrace[BLACK_KING_SAFETY_ATTACKS]) << "   |" << std::endl;
    std::cout << "| WHITE_KING_SAFETY         |   " << padNumber(midgameTrace[WHITE_KING_SAFETY]) <<
        "   |   " << padNumber(endgameTrace[WHITE_KING_SAFETY]) << "   |   " << padNumber(
            taperedTrace[WHITE_KING_SAFETY]) << "   |" << std::endl;
    std::cout << "| BLACK_KING_SAFETY         |   " << padNumber(midgameTrace[BLACK_KING_SAFETY]) <<
        "   |   " << padNumber(endgameTrace[BLACK_KING_SAFETY]) << "   |   " << padNumber(
            taperedTrace[BLACK_KING_SAFETY]) << "   |" << std::endl;
    std::cout << "| WHITE_PAWN_EVAL           |   " << padNumber(midgameTrace[WHITE_PAWN_EVAL]) <<
        "   |   " << padNumber(endgameTrace[WHITE_PAWN_EVAL]) << "   |   " << padNumber(
            taperedTrace[WHITE_PAWN_EVAL]) << "   |" << std::endl;
    std::cout << "| BLACK_PAWN_EVAL           |   " << padNumber(midgameTrace[BLACK_PAWN_EVAL]) <<
        "   |   " << padNumber(endgameTrace[BLACK_PAWN_EVAL]) << "   |   " << padNumber(
            taperedTrace[BLACK_PAWN_EVAL]) << "   |" << std::endl;
    std::cout << "| WHITE_KNIGHT_EVAL         |   " << padNumber(midgameTrace[WHITE_KNIGHT_EVAL]) <<
        "   |   " << padNumber(endgameTrace[WHITE_KNIGHT_EVAL]) << "   |   " << padNumber(
            taperedTrace[WHITE_KNIGHT_EVAL]) << "   |" << std::endl;
    std::cout << "| BLACK_KNIGHT_EVAL         |   " << padNumber(midgameTrace[BLACK_KNIGHT_EVAL]) <<
        "   |   " << padNumber(endgameTrace[BLACK_KNIGHT_EVAL]) << "   |   " << padNumber(
            taperedTrace[BLACK_KNIGHT_EVAL]) << "   |" << std::endl;
    std::cout << "| WHITE_BISHOP_EVAL         |   " << padNumber(midgameTrace[WHITE_BISHOP_EVAL]) <<
        "   |   " << padNumber(endgameTrace[WHITE_BISHOP_EVAL]) << "   |   " << padNumber(
            taperedTrace[WHITE_BISHOP_EVAL]) << "   |" << std::endl;
    std::cout << "| BLACK_BISHOP_EVAL         |   " << padNumber(midgameTrace[BLACK_BISHOP_EVAL]) <<
        "   |   " << padNumber(endgameTrace[BLACK_BISHOP_EVAL]) << "   |   " << padNumber(
            taperedTrace[BLACK_BISHOP_EVAL]) << "   |" << std::endl;
    std::cout << "| WHITE_ROOK_EVAL           |   " << padNumber(midgameTrace[WHITE_ROOK_EVAL]) <<
        "   |   " << padNumber(endgameTrace[WHITE_ROOK_EVAL]) << "   |   " << padNumber(
            taperedTrace[WHITE_ROOK_EVAL]) << "   |" << std::endl;
    std::cout << "| BLACK_ROOK_EVAL           |   " << padNumber(midgameTrace[BLACK_ROOK_EVAL]) <<
        "   |   " << padNumber(endgameTrace[BLACK_ROOK_EVAL]) << "   |   " << padNumber(
            taperedTrace[BLACK_ROOK_EVAL]) << "   |" << std::endl;
    std::cout << "| WHITE_QUEEN_EVAL          |   " << padNumber(midgameTrace[WHITE_QUEEN_EVAL]) <<
        "   |   " << padNumber(endgameTrace[WHITE_QUEEN_EVAL]) << "   |   " << padNumber(
            taperedTrace[WHITE_QUEEN_EVAL]) << "   |" << std::endl;
    std::cout << "| BLACK_QUEEN_EVAL          |   " << padNumber(midgameTrace[BLACK_QUEEN_EVAL]) <<
        "   |   " << padNumber(endgameTrace[BLACK_QUEEN_EVAL]) << "   |   " << padNumber(
            taperedTrace[BLACK_QUEEN_EVAL]) << "   |" << std::endl;
    std::cout << "| WHITE_UNDEFENDED_PIECES   |   " <<
        padNumber(midgameTrace[WHITE_UNDEFENDED_PIECES]) << "   |   " <<
        padNumber(endgameTrace[WHITE_UNDEFENDED_PIECES]) << "   |   " << padNumber(
            taperedTrace[WHITE_UNDEFENDED_PIECES]) << "   |" << std::endl;
    std::cout << "| BLACK_UNDEFENDED_PIECES   |   " <<
        padNumber(midgameTrace[BLACK_UNDEFENDED_PIECES]) << "   |   " <<
        padNumber(endgameTrace[BLACK_UNDEFENDED_PIECES]) << "   |   " << padNumber(
            taperedTrace[BLACK_UNDEFENDED_PIECES]) << "   |" << std::endl;
    std::cout << "==============================================================" << std::endl;
}

// clang-format on
} // namespace Zagreus