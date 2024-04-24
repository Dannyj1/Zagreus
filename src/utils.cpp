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

#include "utils.h"

#include <x86intrin.h>

#include <sstream>

namespace Zagreus {
std::string getNotation(int8_t square) {
    std::string notation = "";

    notation += static_cast<char>(square % 8 + 'a');
    notation += static_cast<char>(square / 8 + '1');

    return notation;
}

int8_t getSquareFromString(std::string move) {
    int file = move[0] - 'a';
    int rank = move[1] - '1';

    return file + rank * 8;
}

char getCharacterForPieceType(PieceType pieceType) {
    switch (pieceType) {
        case WHITE_PAWN:
            return 'P';
        case BLACK_PAWN:
            return 'p';
        case WHITE_KNIGHT:
            return 'N';
        case BLACK_KNIGHT:
            return 'n';
        case WHITE_BISHOP:
            return 'B';
        case BLACK_BISHOP:
            return 'b';
        case WHITE_ROOK:
            return 'R';
        case BLACK_ROOK:
            return 'r';
        case WHITE_QUEEN:
            return 'Q';
        case BLACK_QUEEN:
            return 'q';
        case WHITE_KING:
            return 'K';
        case BLACK_KING:
            return 'k';
        case EMPTY:
            return ' ';
    }
}

std::string mirrorFen(std::string& fen) {
    // Split the input FEN string into main parts
    std::istringstream iss(fen);
    std::vector<std::string> parts;
    std::string part;
    while (getline(iss, part, ' ')) {
        parts.push_back(part);
    }

    // Split the board configuration into rows
    std::vector<std::string> rows;
    std::istringstream rowStream(parts[0]);
    std::string row;
    while (getline(rowStream, row, '/')) {
        rows.push_back(row);
    }

    // Reverse the order of rows and swap the case of each character
    std::reverse(rows.begin(), rows.end());
    for (auto& r : rows) {
        for (char& ch : r) {
            if (std::islower(ch)) {
                ch = std::toupper(ch);
            } else if (std::isupper(ch)) {
                ch = std::tolower(ch);
            }
        }
    }

    // Join the reversed rows back into a single string
    std::ostringstream mirroredStream;
    for (size_t i = 0; i < rows.size(); i++) {
        mirroredStream << rows[i];
        if (i != rows.size() - 1) {
            mirroredStream << '/';
        }
    }
    std::string mirrored = mirroredStream.str();

    // Swap the moving color
    if (parts[1] == "b") {
        parts[1] = "w";
    } else {
        parts[1] = "b";
    }

    // Swap the case of the castling rights
    for (char& ch : parts[2]) {
        if (std::islower(ch)) {
            ch = std::toupper(ch);
        } else if (std::isupper(ch)) {
            ch = std::tolower(ch);
        }
    }

    // Combine the modified parts back into the final FEN string
    std::ostringstream resultStream;
    resultStream << mirrored << ' ' << parts[1] << ' ' << parts[2];
    for (size_t i = 3; i < parts.size(); i++) {
        resultStream << ' ' << parts[i];
    }
    return resultStream.str();
}
} // namespace Zagreus