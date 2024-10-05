/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

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

// ReSharper disable CppRedundantControlFlowJump
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include "magics.h"
#include "uci.h"

namespace Zagreus {
void Engine::doSetup() {
    // According to the UCI specification, bitboard, magic bitboards and other stuff should be done only when "isready" or "setoption" is called
    if (didSetup) {
        return;
    }

    // TODO: setup here
    initializeMagicBitboards();

    didSetup = true;
}

void Engine::printStartupMessage() {
    sendMessage(" ______ ");
    sendMessage(" |___  / ");
    sendMessage("    / /  __ _   __ _  _ __  ___  _   _  ___ ");
    sendMessage("   / /  / _` | / _` || '__|/ _ \\| | | |/ __|");
    sendMessage("  / /__| (_| || (_| || |  |  __/| |_| |\\__ \\");
    sendMessage(R"( /_____|\__,_| \__, ||_|   \___| \__,_||___/)");
    sendMessage("                __/ | ");
    sendMessage("               |___/ ");
    sendMessage("");
    sendMessage("Zagreus  Copyright (C) 2023-2024  Danny Jelsma");
    sendMessage("");
    sendMessage("This program comes with ABSOLUTELY NO WARRANTY.");
    sendMessage("This is free software, and you are welcome to redistribute it");
    sendMessage("under the conditions of the GNU Affero General Public License v3.0 or later.");
    sendMessage("You should have received a copy of the GNU Affero General Public License");
    sendMessage("along with this program. If not, see <https://www.gnu.org/licenses/>.");
    sendMessage("");

    const std::string majorVersion = ZAGREUS_VERSION_MAJOR;
    const std::string minorVersion = ZAGREUS_VERSION_MINOR;
    std::string versionString = "v" + majorVersion + "." + minorVersion;

    if (majorVersion == "dev") {
        versionString = majorVersion + "-" + minorVersion;
    }

    sendMessage("Zagreus UCI chess engine " + versionString + " by Danny Jelsma (https://github.com/Dannyj1/Zagreus)");
    sendMessage("");
}

void Engine::handleUciCommand() {
    const std::string majorVersion = ZAGREUS_VERSION_MAJOR;
    const std::string minorVersion = ZAGREUS_VERSION_MINOR;
    std::string versionString = "v" + majorVersion + "." + minorVersion;

    if (majorVersion == "dev") {
        versionString = majorVersion + "-" + minorVersion;
    }

    sendMessage("id name Zagreus " + versionString);
    sendMessage("id author Danny Jelsma");

    if (!this->options.empty()) {
        for (auto& [name, option] : this->options) {
            sendMessage(option.toString());
        }
    }

    sendMessage("uciok");
}

void Engine::handleDebugCommand(std::string_view args) {
    sendMessage("Debug mode is currently not implemented.");
}

void Engine::handleIsReadyCommand(std::string_view args) {
    if (!didSetup) {
        doSetup();
    }

    sendMessage("readyok");
}

void Engine::handleSetOptionCommand(const std::string& args) {
    if (!didSetup) {
        doSetup();
    }

    std::istringstream iss(args);
    std::string word;
    std::string section = "";
    std::string name;
    std::string value;

    while (iss >> word) {
        std::string lowercaseWord = word;
        std::ranges::transform(lowercaseWord, lowercaseWord.begin(), [](const unsigned char c) { return std::tolower(c); });

        if (lowercaseWord == "name") {
            section = word;
            continue;
        }

        if (lowercaseWord == "value") {
            section = word;
            continue;
        }

        if (section == "name") {
            if (name.empty()) {
                name = word;
            } else {
                name += " " + word;
            }
        }

        if (section == "value") {
            if (value.empty()) {
                value = word;
            } else {
                value += " " + word;
            }
        }
    }

    if (name.empty()) {
        sendMessage("ERROR: No option name provided.");
        return;
    }

    if (!hasOption(name)) {
        sendMessage("ERROR: Option " + name + " does not exist.");
        return;
    }

    UCIOption& option = this->getOption(name);

    if (value.empty()) {
        if (option.getOptionType() == Button) {
            if (option.getValue() == "true") {
                value = "false";
            } else {
                value = "true";
            }
        } else {
            sendMessage("ERROR: No option value provided.");
            return;
        }
    }

    option.setValue(value);
}

void Engine::handleUciNewGameCommand(std::string_view args) {

}

void Engine::handlePositionCommand(std::string_view args) {

}

void Engine::handleGoCommand(std::string_view args) {

}

void Engine::handleStopCommand(std::string_view args) {

}

void Engine::handlePonderHitCommand(std::string_view args) {

}

void Engine::handleQuitCommand(std::string_view args) {

}

void Engine::processCommand(const std::string_view command, const std::string& args) {
    if (command == "uci") {
        handleUciCommand();
    } else if (command == "debug") {
        handleDebugCommand(args);
    } else if (command == "isready") {
        handleIsReadyCommand(args);
    } else if (command == "setoption") {
        handleSetOptionCommand(args);
    } else if (command == "register") {
        // Not relevant for our engine
        return;
    } else if (command == "ucinewgame") {
        handleUciNewGameCommand(args);
    } else if (command == "position") {
        handlePositionCommand(args);
    } else if (command == "go") {
        handleGoCommand(args);
    } else if (command == "stop") {
        handleStopCommand(args);
    } else if (command == "ponderhit") {
        handlePonderHitCommand(args);
    } else if (command == "quit") {
        handleQuitCommand(args);
    } else {
        // If unknown, we must skip it and process the rest.
        if (args.empty() || args == " " || args == "\n") {
            std::cout << "Unknown command: " << command << std::endl;
            return;
        }

        std::string newCommand;
        std::string newArgs;

        if (args.contains(' ')) {
            newCommand = args.substr(0, args.find(' '));
            newArgs = args.substr(args.find(' ') + 1);
        } else {
            newCommand = args;
        }

        processCommand(newCommand, newArgs);
    }
}

void Engine::addOption(UCIOption& option) {
    this->options[option.getName()] = option;
}

UCIOption& Engine::getOption(const std::string& name) {
    return this->options[name];
}

bool Engine::hasOption(const std::string& name) const {
    return this->options.contains(name);
}

void Engine::startUci() {
    printStartupMessage();

    // TODO: make sure that anything that is not related to UCI input reading is done in a separate thread, as "the engine must be able to read input even when thinking"
    std::string line;

    while (std::getline(std::cin, line)) {
        line = removeRedundantSpaces(line);
        std::string command;
        std::string args;

        if (size_t space_pos = line.find(' '); space_pos != std::string::npos) {
            command = line.substr(0, space_pos);
            args = line.substr(space_pos + 1);
        } else {
            command = line;
            args = "";
        }


        if (args == "\n" || args == " ") {
            args = "";
        }

        processCommand(command, args);
    }
}

void Engine::sendInfoMessage(std::string_view message) {
    std::cout << "info " << message << std::endl;
}

void Engine::sendMessage(std::string_view message) {
    std::cout << message << std::endl;
}

std::string removeRedundantSpaces(std::string_view input) {
    std::string result;
    bool inSpace = false;  // Track if we are in a sequence of spaces/tabs

    for (size_t i = 0; i < input.length(); ++i) {
        char current = input[i];

        if (current == '\r' || current == '\t') {
            current = ' ';
        }

        if (std::isspace(current)) {
            if (current == '\n') {
                // Add newline and reset inSpace
                result += '\n';
                inSpace = false;
            } else if (!inSpace) {
                // Only add space if it's the first whitespace in a sequence
                if (!result.empty() && result.back() != '\n' && result.back() != ' ') {
                    result += ' ';
                }
                inSpace = true;
            }
        } else {
            // Add non-space character and reset inSpace
            result += current;
            inSpace = false;
        }
    }

    // Trim trailing space if any
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }

    return result;
}

UCIOptionType UCIOption::getOptionType() {
    return this->optionType;
}

std::string UCIOption::getName() {
    return this->name;
}

std::string UCIOption::getValue() {
    return this->value;
}

void UCIOption::setValue(const std::string& value) {
    this->value = value;
}

std::string UCIOption::getDefaultValue() {
    return this->defaultValue;
}

void UCIOption::setDefaultValue(const std::string& value) {
    this->defaultValue = value;
}

std::string UCIOption::getMinValue() {
    return this->minValue;
}

void UCIOption::setMinValue(std::string value) {
    this->minValue = value;
}

std::string UCIOption::getMaxValue() {
    return this->maxValue;
}

void UCIOption::setMaxValue(std::string value) {
    this->maxValue = value;
}

std::string getUciOptionTypeAsString(const UCIOptionType type) {
    switch (type) {
        case Check:
            return "check";
        case Spin:
            return "spin";
        case Combo:
            return "combo";
        case Button:
            return "button";
        case String:
            return "string";
        default:
            return "unknown";
    }
}

std::string UCIOption::toString() {
    std::string result = "option name " + this->name + " type " + getUciOptionTypeAsString(this->optionType);

    if (!this->defaultValue.empty()) {
        result += " default " + this->defaultValue;
    }

    if (!this->minValue.empty()) {
        result += " min " + this->minValue;
    }

    if (!this->maxValue.empty()) {
        result += " max " + this->maxValue;
    }

    if (!this->var.empty()) {
        for (const auto& optionValue : this->var) {
            result += " var " + optionValue;
        }
    }

    return result;
}

void UCIOption::addVar(std::string value) {
    this->var.push_back(value);
}

void UCIOption::setVar(std::vector<std::string> values) {
    this->var = values;
}

void UCIOption::removeVar(std::string value) {
    this->var.erase(std::remove(this->var.begin(), this->var.end(), value), this->var.end());
}

void UCIOption::clearVar() {
    this->var.clear();
}

std::string UCIOption::getVar(const int index) {
    return this->var.at(index);
}

std::vector<std::string> UCIOption::getVar() {
    return this->var;
}
} // namespace Zagreus
