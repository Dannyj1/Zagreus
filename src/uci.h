/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUstd::string ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

namespace Zagreus {
class UCIOption;

class Engine {
private:
    bool didSetup = false;
    std::map<std::string, UCIOption> options;

    void handleUciCommand();
    void handleDebugCommand(const std::string& args);
    void handleIsReadyCommand(const std::string& args);
    void handleSetOptionCommand(const std::string& args);
    void handleUciNewGameCommand(const std::string& args);
    void handlePositionCommand(const std::string& args);
    void handleGoCommand(const std::string& args);
    void handleStopCommand(const std::string& args);
    void handlePonderHitCommand(const std::string& args);
    void handleQuitCommand(const std::string& args);
    void processCommand(const std::string& string, const std::string& args);
public:
    void startUci();
    void sendInfoMessage(const std::string& message);
    void sendMessage(const std::string& message);
    void doSetup();
    void printStartupMessage();
    void addOption(UCIOption& option);
    UCIOption& getOption(const std::string& name);
    bool hasOption(const std::string& name);
};

enum UCIOptionType {
    Check,
    Spin,
    Combo,
    Button,
    String
};

class UCIOption {
private:
    std::string name;
    UCIOptionType optionType;
    std::string value;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    std::vector<std::string> var;
public:
    UCIOption() = default;

    UCIOption(std::string name, UCIOptionType optionType) : name(std::move(name)), optionType(optionType) {}

    UCIOptionType getOptionType();

    std::string getName();

    std::string getValue();

    void setValue(std::string value);

    std::string getDefaultValue();

    void setDefaultValue(std::string value);

    std::string getMinValue();

    void setMinValue(std::string value);

    std::string getMaxValue();

    void setMaxValue(std::string value);

    std::string toString();

    void addVar(std::string value);

    void setVar(std::vector<std::string> values);

    void removeVar(std::string value);

    void clearVar();

    std::string getVar(int index);

    std::vector<std::string> getVar();
};

std::string removeRedundantSpaces(const std::string& input);
} // namespace Zagreus