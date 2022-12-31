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
        if (move == "e1g1" && board.getCastlingRights() & CastlingRights::WHITE_KINGSIDE && board.getPieceOnSquare(Square::E1) == PieceType::WHITE_KING && board.getPieceOnSquare(Square::H1) == PieceType::WHITE_ROOK) {
            board.makeStrMove("h1e1");
            return true;
        }

        if (move == "e1c1" && board.getCastlingRights() & CastlingRights::WHITE_QUEENSIDE && board.getPieceOnSquare(Square::E1) == PieceType::WHITE_KING && board.getPieceOnSquare(Square::A1) == PieceType::WHITE_ROOK) {
            board.makeStrMove("a1e1");
            return true;
        }

        if (move == "e8g8" && board.getCastlingRights() & CastlingRights::BLACK_KINGSIDE && board.getPieceOnSquare(Square::E8) == PieceType::BLACK_KING && board.getPieceOnSquare(Square::H8) == PieceType::BLACK_ROOK) {
            board.makeStrMove("h8e8");
            return true;
        }

        if (move == "e8c8" && board.getCastlingRights() & CastlingRights::BLACK_QUEENSIDE && board.getPieceOnSquare(Square::E8) == PieceType::BLACK_KING && board.getPieceOnSquare(Square::A8) == PieceType::BLACK_ROOK) {
            board.makeStrMove("e8a8");
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

        board.print();
        SearchResult bestResult = searchManager.getBestMove(board, board.getMovingColor());

        if (bestResult.move.promotionPiece) {
            std::string result = Chess::Bitboard::getNotation(bestResult.move.fromSquare)
                    + Chess::Bitboard::getNotation(bestResult.move.toSquare)
                    + board.getCharacterForPieceType(bestResult.move.promotionPiece);

            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            return result;
        }

        std::cout << "Score: " << bestResult.score << std::endl;
        return Chess::Bitboard::getNotation(bestResult.move.fromSquare) + Chess::Bitboard::getNotation(bestResult.move.toSquare);
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