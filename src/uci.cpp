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
#include <stddef.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include "magics.h"
#include "uci.h"

#include "bitboard.h"
#include "perft.h"

namespace Zagreus {
constexpr std::string_view startPosFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void Engine::doSetup() {
    // According to the UCI specification, bitboard, magic bitboards and other stuff should be done only when "isready" or "setoption" is called
    if (didSetup) {
        return;
    }

    initializeMagicBitboards();
    initializeAttackLookupTables();

    didSetup = true;
}

std::string Engine::getVersionString() {
    const std::string majorVersion = ZAGREUS_VERSION_MAJOR;
    const std::string minorVersion = ZAGREUS_VERSION_MINOR;
    std::string versionString = "v" + majorVersion + "." + minorVersion;

    if (majorVersion == "dev") {
        versionString = majorVersion + "-" + minorVersion;
    }

    return versionString;
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

    sendMessage("Zagreus UCI chess engine " + getVersionString() + " by Danny Jelsma (https://github.com/Dannyj1/Zagreus)");
    sendMessage("");
}

void Engine::handleUciCommand() {
    sendMessage("id name Zagreus " + getVersionString());
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
    std::string arg;
    std::string section;
    std::string name;
    std::string value;

    while (iss >> arg) {
        std::string lowercaseWord = arg;
        std::ranges::transform(lowercaseWord, lowercaseWord.begin(),
                               [](const unsigned char c) { return std::tolower(c); });

        if (lowercaseWord == "name") {
            section = arg;
            continue;
        }

        if (lowercaseWord == "value") {
            section = arg;
            continue;
        }

        if (section == "name") {
            if (name.empty()) {
                name = arg;
            } else {
                name += " " + arg;
            }
        }

        if (section == "value") {
            if (value.empty()) {
                value = arg;
            } else {
                value += " " + arg;
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

void Engine::handlePositionCommand(const std::string_view args) {
    // If the first arg is not "startpos" or "fen", report invalid usage
    if (!args.starts_with("startpos") && !args.starts_with("fen")) {
        sendMessage("ERROR: Invalid usage of position command.");
        return;
    }

    // Go through each argument one by one
    std::istringstream iss((args.data()));
    std::string arg;

    // If the first argument is "startpos", set the board to the starting position
    iss >> arg;
    board = Board();

    if (arg == "startpos") {
        board.setFromFEN(startPosFEN);
    } else if (arg == "fen") {
        std::string fen;

        // Append args to the fen string until we reach the "moves" keyword or the end of the string
        while (iss >> arg && arg != "moves") {
            if (fen.empty()) {
                fen = arg;
            } else {
                fen += " " + arg;
            }
        }

        if (fen.empty()) {
            sendMessage("ERROR: No FEN string provided.");
            return;
        }

        if (!board.setFromFEN(fen)) {
            sendMessage("ERROR: Invalid FEN string provided.");
            return;
        }
    }

    while (iss >> arg) {
        if (arg == "moves") {
            continue;
        }

        if (arg.size() != 4 && arg.size() != 5) {
            sendMessage("ERROR: Invalid move notation provided.");
            return;
        }

        const Move move = fromMoveNotation(arg);
        board.makeMove(move);
    }
}

void Engine::handleGoCommand(std::string_view args) {
}

void Engine::handleStopCommand(std::string_view args) {
}

void Engine::handlePonderHitCommand(std::string_view args) {
}

void Engine::handleQuitCommand(std::string_view args) {

}

void Engine::handlePerftCommand(const std::string& args) {
    if (!didSetup) {
        doSetup();
    }

    if (args.empty() || args == " " || args == "\n") {
        sendMessage("ERROR: No depth provided.");
        return;
    }

    if (args.contains(' ')) {
        sendMessage("ERROR: Too many arguments provided.");
        return;
    }

    int depth = 0;

    try {
        depth = std::stoi(args);
    } catch (const std::invalid_argument& e) {
        sendMessage("ERROR: Depth must be an integer.");
        return;
    }

    if (depth <= 0) {
        sendMessage("ERROR: Depth must be at least 1.");
        return;
    }

    if (board.getOccupiedBitboard() == 0ULL) {
        board.setFromFEN(startPosFEN);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    const uint64_t nodes = perft(board, depth);
    const auto end = std::chrono::high_resolution_clock::now();
    const std::string tookSeconds = std::to_string(std::chrono::duration<double>(end - start).count());

    sendInfoMessage(
        "Depth: " + std::to_string(depth) + ", Nodes: " + std::to_string(nodes) + ", Time: " + tookSeconds + "s");
}

void Engine::handlePrintCommand() {
    board.print();
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
    } else if (command == "perft") {
        handlePerftCommand(args);
    } else if (command == "print") {
        handlePrintCommand();
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

void Engine::sendInfoMessage(const std::string_view message) {
    std::cout << "info " << message << std::endl;
}

void Engine::sendMessage(const std::string_view message) {
    std::cout << message << std::endl;
}

std::string removeRedundantSpaces(const std::string_view input) {
    std::string result;
    bool inSpace = false; // Track if we are in a sequence of spaces/tabs

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
