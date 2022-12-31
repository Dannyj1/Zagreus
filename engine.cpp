#include "senjo/ChessEngine.h"
#include "senjo/Output.h"
#include "engine.h"
#include "search_mgr.h"

namespace Chess {
    std::string Engine::getEngineName() {
        return "Zagreus";
    }

    std::string Engine::getEngineVersion() {
        return "v0.1";
    }

    std::string Engine::getAuthorName() {
        return "Dannyj1";
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
        isEngineInitialized = true;
    }

    bool Engine::isInitialized() {
        return isEngineInitialized;
    }

    bool Engine::setPosition(const std::string &fen, std::string* remain) {
        return board.setFromFEN(fen);
    }

    bool Engine::makeMove(const std::string &move) {
        if (move == "e1g1") {
            board.makeStrMove("h1e1");
            return true;
        }

        if (move == "e1c1") {
            board.makeStrMove("a1e1");
            return true;
        }

        if (move == "e8g8") {
            board.makeStrMove("h8e8");
            return true;
        }

        if (move == "e8c8") {
            board.makeStrMove("e8a8");
            return true;
        }

        board.makeStrMove(move);
        return true;
    }

    std::string Engine::getFEN() {
        //return board.getFEN();
        return "";
    }

    void Engine::printBoard() {
        //board.print();
    }

    bool Engine::whiteToMove() {
/*        return board.getMovingColor() == PieceColor::WHITE;*/
        return true;
    }

    void Engine::clearSearchData() {
        // TODO: implement
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
        //return searchManager.isCurrentlySearching();
        return false;
    }

    void Engine::stopSearching() {
        // TODO: implement
    }

    bool Engine::stopRequested() {
        // TODO: implement
        return false;
    }

    void Engine::waitForSearchFinish() {
/*        while (searchManager.isCurrentlySearching()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }*/
    }

    uint64_t Engine::perft(const int depth) {
        // TODO: implement
        return 0;
    }

    std::string Engine::go(const senjo::GoParams &params, std::string* ponder) {
        /*if (engineColor == PieceColor::NONE) {
            engineColor = board.getMovingColor();
        }

        board.setWhiteTimeMsec(params.wtime);
        board.setBlackTimeMsec(params.btime);

        board.print();
        SearchResult bestResult = searchManager.getBestMove(&board, engineColor);
        TileLocation fromLoc = board.getPiecePosition(bestResult.move.piece->getId());
        Tile* fromTile = board.getTileUnsafe(fromLoc.x, fromLoc.y);

        if (bestResult.move.promotionPiece) {
            switch (bestResult.move.promotionPiece->getPieceType()) {
                case PieceType::QUEEN:
                    return fromTile->getNotation() + bestResult.move.tile->getNotation() + "q";
                case PAWN:
                    break;
                case KNIGHT:
                    return fromTile->getNotation() + bestResult.move.tile->getNotation() + "n";
                case BISHOP:
                    return fromTile->getNotation() + bestResult.move.tile->getNotation() + "b";
                case ROOK:
                    return fromTile->getNotation() + bestResult.move.tile->getNotation() + "r";
                case KING:
                    break;
            }
        }

        std::cout << "Score: " << bestResult.score << std::endl;
        return fromTile->getNotation() + bestResult.move.tile->getNotation();*/
        return "a1a2";
    }

    senjo::SearchStats Engine::getSearchStats() {
        // TODO: implement
        return {};
    }

    void Engine::resetEngineStats() {
        // TODO: implement
    }

    void Engine::showEngineStats() {
        // TODO: implement
    }
}