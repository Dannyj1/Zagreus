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

#pragma once

#include <string>
#include "senjo/ChessEngine.h"
#include "bitboard.h"

namespace Zagreus {
    class ZagreusEngine : public senjo::ChessEngine {
    private:
        Bitboard board{};
        bool isEngineInitialized = false;
        PieceColor engineColor = PieceColor::NONE;
        senjo::SearchStats searchStats{};

        std::list<senjo::EngineOption> options{
            senjo::EngineOption("Move Overhead", "0", senjo::EngineOption::OptionType::Spin, 0, INT64_MAX),
            senjo::EngineOption("Hash", "512", senjo::EngineOption::OptionType::Spin, 1, INT64_MAX),
            senjo::EngineOption("Threads", "1", senjo::EngineOption::OptionType::Spin, 1, INT64_MAX),
        };

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

        senjo::EngineOption getOption(const std::string &optionName);
    };
}