//
// Created by Danny on 21-4-2022.
//

#pragma once

#include <string>
#include "senjo/ChessEngine.h"
#include "bitboard.h"

namespace Zagreus {
    class Engine : public senjo::ChessEngine {
    private:
        Bitboard board{};
        bool isEngineInitialized = false;
        PieceColor engineColor = PieceColor::NONE;
    public:
        uint64_t doPerft(Zagreus::Bitboard &board, Zagreus::PieceColor color, int depth, int startingDepth);

        std::string getEngineName() override;

        std::string getEngineVersion() override;

        std::string getAuthorName() override;

        std::string getEmailAddress() override;

        std::string getCountryName() override;

        std::list<senjo::EngineOption> getOptions() override;

        bool setEngineOption(const std::string &optionName, const std::string &optionValue) override;

        void initialize() override;

        bool isInitialized() override;

        bool setPosition(const std::string &fen, std::string* remain) override;

        bool makeMove(const std::string &move) override;

        std::string getFEN() override;

        void printBoard() override;

        bool whiteToMove() override;

        void clearSearchData() override;

        void ponderHit() override;

        bool isRegistered() override;

        void registerLater() override;

        bool doRegistration(const std::string &name, const std::string &code) override;

        bool isCopyProtected() override;

        bool copyIsOK() override;

        void setDebug(const bool flag) override;

        bool isDebugOn() override;

        bool isSearching() override;

        void stopSearching() override;

        bool stopRequested() override;

        void waitForSearchFinish() override;

        uint64_t perft(const int depth) override;

        std::string go(const senjo::GoParams &params, std::string* ponder) override;

        senjo::SearchStats getSearchStats() override;

        void resetEngineStats() override;

        void showEngineStats() override;
    };
}