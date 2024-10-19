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

#include <cassert>

#include "move.h"

namespace Zagreus {
std::string getMoveNotation(const uint8_t fromSquare, const uint8_t toSquare) {
    assert(fromSquare < SQUARES);
    assert(toSquare < SQUARES);
    std::string notation;

    notation += static_cast<char>(fromSquare % 8 + 'a');
    notation += static_cast<char>(fromSquare / 8 + '1');
    notation += static_cast<char>(toSquare % 8 + 'a');
    notation += static_cast<char>(toSquare / 8 + '1');

    return notation;
}

std::string getMoveNotation(const uint8_t fromSquare, const uint8_t toSquare, const PromotionPiece promotionPiece) {
    assert(fromSquare < SQUARES);
    assert(toSquare < SQUARES);
    assert(promotionPiece < 4);
    std::string notation;

    notation += static_cast<char>(fromSquare % 8 + 'a');
    notation += static_cast<char>(fromSquare / 8 + '1');
    notation += static_cast<char>(toSquare % 8 + 'a');
    notation += static_cast<char>(toSquare / 8 + '1');
    notation += "qrbn"[promotionPiece];

    return notation;
}

std::string getMoveNotation(const Move move) {
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);

    if (const MoveType moveType = getMoveType(move);
        moveType == PROMOTION) {
        const PromotionPiece promotionPiece = getPromotionPiece(move);

        return getMoveNotation(fromSquare, toSquare, promotionPiece);
    }

    return getMoveNotation(fromSquare, toSquare);
}

Move fromMoveNotation(const std::string_view notation) {
    const Square fromSquare = fromSquareNotation(notation.substr(0, 2));
    const Square toSquare = fromSquareNotation(notation.substr(2, 2));

    // If there is a 5th character, it is a promotion
    if (notation.size() == 5) {
        const char promotionPieceChar = notation[4];
        PromotionPiece promotionPiece;

        switch (promotionPieceChar) {
            case 'q':
                promotionPiece = QUEEN_PROMOTION;
                break;
            case 'r':
                promotionPiece = ROOK_PROMOTION;
                break;
            case 'b':
                promotionPiece = BISHOP_PROMOTION;
                break;
            case 'n':
                promotionPiece = KNIGHT_PROMOTION;
                break;
            default:
                assert(false);
                return 0;
        }

        return encodeMove(fromSquare, toSquare, promotionPiece);
    }

    return encodeMove(fromSquare, toSquare);
}

std::string getSquareNotation(const Square square) {
    assert(square < SQUARES);
    std::string notation;

    notation += static_cast<char>(square % 8 + 'a');
    notation += static_cast<char>(square / 8 + '1');

    return notation;
}

Square fromSquareNotation(const std::string_view notation) {
    const uint8_t file = notation[0] - 'a';
    const uint8_t rank = notation[1] - '1';

    return static_cast<Square>(file + rank * 8);
}
} // namespace Zagreus
