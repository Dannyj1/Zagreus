/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
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

#include <x86intrin.h>
#include <lzcntintrin.h>

#include "utils.h"

namespace Zagreus {
    uint64_t popcnt(uint64_t b) {
        return __builtin_popcountll(b);
    }

    int8_t bitscanForward(uint64_t b) {
        return b ? _tzcnt_u64(b) : 0;
    }

    int8_t bitscanReverse(uint64_t b) {
        return b ? _lzcnt_u64(b) ^ 63 : 0;
    }

    uint32_t encodeMove(const Move &move) {
        return (move.promotionPiece << 20) | (move.piece << 15) |
               (move.to << 7) | move.from;
    }

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

    static constexpr uint16_t pieceWeights[12] = {
            100, 100, 350, 350, 350, 350, 525, 525, 1000, 1000, 65535, 65535
    };

    uint16_t getPieceWeight(PieceType type) {
        return pieceWeights[type];
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

    int mvvlva(PieceType attacker, PieceType victim) {
        return MVVLVA_TABLE[attacker][victim];
    }
}