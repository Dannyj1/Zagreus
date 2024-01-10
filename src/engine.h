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

#pragma once

#include <string>

#include "../senjo/ChessEngine.h"
#include "bitboard.h"
#include "types.h"

namespace Zagreus {
class ZagreusEngine : public senjo::ChessEngine {
private:
    Bitboard board{};
    bool isEngineInitialized = false;
    senjo::SearchStats searchStats{};
    bool stoppingSearch = false;
    bool searching = false;
    bool tuning = false;
    MoveListPool* moveListPool = MoveListPool::getInstance();

    std::list<senjo::EngineOption> options{
        senjo::EngineOption("MoveOverhead", "50", senjo::EngineOption::OptionType::Spin, 0, 5000),
        senjo::EngineOption("Hash", "512", senjo::EngineOption::OptionType::Spin, 1, 33554432),
        senjo::EngineOption("Threads", "1", senjo::EngineOption::OptionType::Spin, 1, 1),
        senjo::EngineOption("SyzygyPath", "", senjo::EngineOption::OptionType::String),
        senjo::EngineOption("SyzygyProbeLimit", "0", senjo::EngineOption::OptionType::Spin, 0, 100),
#ifdef SPSA_TUNE
        senjo::EngineOption("SPSA_NMPBaseR", "3", senjo::EngineOption::OptionType::Spin, 1, 5),
        senjo::EngineOption("SPSA_NMPMinDepth", "3", senjo::EngineOption::OptionType::Spin, 3, 6),
        senjo::EngineOption("SPSA_NMPDepthMultiplier", "0.33",
                            senjo::EngineOption::OptionType::String),
        senjo::EngineOption("SPSA_NMPMinPieces", "1", senjo::EngineOption::OptionType::Spin, 0, 10),
        senjo::EngineOption("SPSA_LMRMoveCountThreshold", "0.6",
                            senjo::EngineOption::OptionType::String),
        senjo::EngineOption("SPSA_LMRDepthMultiplier", "0.33",
                            senjo::EngineOption::OptionType::String),
        senjo::EngineOption("SPSA_LMRMinMovesSearched", "5", senjo::EngineOption::OptionType::Spin,
                            1, 10),
        senjo::EngineOption("SPSA_LMRMinDepth", "3", senjo::EngineOption::OptionType::Spin, 2, 6),
#endif
    };

public:
    std::string getEngineName() override;

    std::string getEngineVersion() override;

    std::string getAuthorName() override;

    std::string getEmailAddress() override;

    std::string getCountryName() override;

    std::list<senjo::EngineOption> getOptions() override;

    bool setEngineOption(const std::string& optionName, const std::string& optionValue) override;

    void initialize() override;

    bool isInitialized() override;

    bool setPosition(const std::string& fen, std::string* remain) override;

    bool makeMove(const std::string& move) override;

    std::string getFEN() override;

    void printBoard() override;

    bool whiteToMove() override;

    void clearSearchData() override;

    void ponderHit() override;

    bool isRegistered() override;

    void registerLater() override;

    bool doRegistration(const std::string& name, const std::string& code) override;

    bool isCopyProtected() override;

    bool copyIsOK() override;

    void setDebug(const bool flag) override;

    bool isDebugOn() override;

    bool isSearching() override;

    void stopSearching() override;

    bool stopRequested() override;

    void waitForSearchFinish() override;

    uint64_t perft(const int16_t depth) override;

    std::string go(senjo::GoParams& params, std::string* ponder) override;

    senjo::SearchStats getSearchStats() override;

    void resetEngineStats() override;

    void showEngineStats() override;

    senjo::EngineOption getOption(const std::string& optionName);

    uint64_t doPerft(Bitboard& perftBoard, PieceColor color, int16_t depth, int startingDepth);

    bool isTuning() const;

    void setTuning(bool tuning);
};
} // namespace Zagreus
