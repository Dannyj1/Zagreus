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
} // namespace Zagreus