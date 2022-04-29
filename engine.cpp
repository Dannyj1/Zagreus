#include "senjo/ChessEngine.h"
#include "senjo/Output.h"
#include "engine.h"
#include "board.h"
#include "search_mgr.h"

namespace Chess {
    std::string Engine::getEngineName() {
        return "Chess";
    }

    std::string Engine::getEngineVersion() {
        return "0.1";
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
        board = Board{};
        isEngineInitialized = true;
    }

    bool Engine::isInitialized() {
        return isEngineInitialized;
    }

    bool Engine::setPosition(const std::string &fen, std::string* remain) {
        return board.setFromFEN(fen);
    }

    bool Engine::makeMove(const std::string &move) {
        if (move.size() < 4 || move.size() > 5) {
            return false;
        }

        return board.makeStrMove(move);
    }

    std::string Engine::getFEN() {
        return board.getFEN();
    }

    void Engine::printBoard() {
        board.print();
    }

    bool Engine::whiteToMove() {
        return board.getMovingColor() == PieceColor::WHITE;
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
        // TODO: implement
        return 0;
    }

    std::string Engine::go(const senjo::GoParams &params, std::string* ponder) {
        board.setWhiteTimeMsec(params.wtime);
        board.setBlackTimeMsec(params.btime);

        SearchResult bestResult = searchManager.getBestMove(&board);
        TileLocation fromLoc = board.getPiecePosition(bestResult.move.piece->getId());
        Tile* fromTile = board.getTile(fromLoc.x, fromLoc.y);

        return fromTile->getNotation() + bestResult.move.tile->getNotation();
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

    Engine::Engine() {
        isEngineInitialized = false;
    }
}