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

#include "board.h"

#include <string_view>

#include "bitwise.h"

namespace Zagreus {
template <PieceColor movedColor>
bool Board::isPositionLegal() const {
    constexpr PieceColor opponentColor = !movedColor;
    constexpr Piece king = movedColor == PieceColor::WHITE ? Piece::WHITE_KING : Piece::BLACK_KING;
    const uint64_t kingBB = getBitboard<king>();
    const uint8_t kingSquare = bitscanForward(kingBB);

    return !getSquareAttackersByColor<opponentColor>(kingSquare);
}

template bool Board::isPositionLegal<PieceColor::WHITE>() const;
template bool Board::isPositionLegal<PieceColor::BLACK>() const;

uint64_t Board::getSquareAttackers(const uint8_t square) const {
    using enum Piece;

    const uint64_t knights = getBitboard<WHITE_KNIGHT>() | getBitboard<BLACK_KNIGHT>();
    const uint64_t kings = getBitboard<WHITE_KING>() | getBitboard<BLACK_KING>();
    uint64_t bishopsQueens = getBitboard<WHITE_QUEEN>() | getBitboard<BLACK_QUEEN>();
    uint64_t rooksQueens = getBitboard<WHITE_QUEEN>() | getBitboard<BLACK_QUEEN>();
    rooksQueens |= getBitboard<WHITE_ROOK>() | getBitboard<BLACK_ROOK>();
    bishopsQueens |= getBitboard<WHITE_BISHOP>() | getBitboard<BLACK_BISHOP>();

    return (pawnAttacks<PieceColor::WHITE>(square) & getBitboard<BLACK_PAWN>())
           | (pawnAttacks<PieceColor::BLACK>(square) & getBitboard<WHITE_PAWN>())
           | (knightAttacks(square) & knights)
           | (kingAttacks(square) & kings)
           | (bishopAttacks(square, occupied) & bishopsQueens)
           | (rookAttacks(square, occupied) & rooksQueens);
}

void Board::reset() {
    this->board = {};
    this->bitboards = {};
    this->occupied = 0;
    this->colorBoards = {};
    this->sideToMove = PieceColor::EMPTY;
    this->history = {};
    this->ply = 0;

    std::ranges::fill(board, Piece::EMPTY);
    std::ranges::fill(bitboards, 0);
    std::ranges::fill(colorBoards, 0);
    std::ranges::fill(history, BoardState{});
}

void Board::makeMove(const Move& move) {
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const Piece movedPiece = getPieceOnSquare(fromSquare);
    const Piece capturedPiece = getPieceOnSquare(toSquare);

    if (capturedPiece != Piece::EMPTY) {
        removePiece(capturedPiece, toSquare);
    }

    removePiece(movedPiece, fromSquare);
    setPiece(movedPiece, toSquare);

    sideToMove = !sideToMove;
    history[ply].move = move;
    history[ply].capturedPiece = capturedPiece;
    ply++;
}

void Board::unmakeMove() {
    ply--;
    const auto [move, capturedPiece] = history[ply];
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const Piece movedPiece = getPieceOnSquare(toSquare);

    removePiece(movedPiece, toSquare);
    setPiece(movedPiece, fromSquare);

    if (capturedPiece != Piece::EMPTY) {
        setPiece(capturedPiece, toSquare);
    }

    sideToMove = !sideToMove;
}


void Board::setPieceFromFENChar(const char character, const uint8_t index) {
    using enum Piece;

    // Uppercase char = white, lowercase = black
    switch (character) {
        case 'P':
            setPiece(WHITE_PAWN, index);
            break;
        case 'p':
            setPiece(BLACK_PAWN, index);
            break;
        case 'N':
            setPiece(WHITE_KNIGHT, index);
            break;
        case 'n':
            setPiece(BLACK_KNIGHT, index);
            break;
        case 'B':
            setPiece(WHITE_BISHOP, index);
            break;
        case 'b':
            setPiece(BLACK_BISHOP, index);
            break;
        case 'R':
            setPiece(WHITE_ROOK, index);
            break;
        case 'r':
            setPiece(BLACK_ROOK, index);
            break;
        case 'Q':
            setPiece(WHITE_QUEEN, index);
            break;
        case 'q':
            setPiece(BLACK_QUEEN, index);
            break;
        case 'K':
            setPiece(WHITE_KING, index);
            break;
        case 'k':
            setPiece(BLACK_KING, index);
            break;
        default:
            break;
    }
}

bool Board::setFromFEN(const std::string_view fen) {
    // TODO: Update zobrist hash once that is implemented
    uint8_t index = IDX(Square::A8);
    int spaces = 0;

    reset();

    for (const char character : fen) {
        if (character == ' ') {
            spaces++;
            continue;
        }

        if (character == ',') {
            break;
        }

        if (spaces == 0) {
            if (character == '/') {
                index -= 16;
                continue;
            }

            if (character >= '1' && character <= '8') {
                index += character - '0';
                continue;
            }

            if (character >= 'A' && character <= 'z') {
                setPieceFromFENChar(character, index);
                index++;
            } else {
                return false;
            }
        }

        if (spaces == 1) {
            if (tolower(character) == 'w') {
                sideToMove = PieceColor::WHITE;
            } else if (tolower(character) == 'b') {
                sideToMove = PieceColor::BLACK;
            } else {
                return false;
            }
        }

        // TODO: Implement and add zobrist logic once castling is implemented
        /*if (spaces == 2) {
            if (character == '-') {
                continue;
            } else if (character == 'K') {
                castlingRights |= WHITE_KINGSIDE;
                continue;
            } else if (character == 'Q') {
                castlingRights |= WHITE_QUEENSIDE;
                continue;
            } else if (character == 'k') {
                castlingRights |= BLACK_KINGSIDE;
                continue;
            } else if (character == 'q') {
                castlingRights |= BLACK_QUEENSIDE;
                continue;
            }

            return false;
        }*/

        if (spaces == 3) {
            if (character == '-') {
                continue;
            }

            if (tolower(character) < 'a' || tolower(character) > 'h') {
                continue;
            }

            const int8_t file = tolower(character) - 'a';
            // const int8_t rank = (!sideToMove) == PieceColor::WHITE ? 2 : 5;

            if (file < 0 || file > 7) {
                return false;
            }

            // TODO: Uncomment and add zobrist once en-passant is implemented
            // enPassantSquare = rank * 8 + file;
            index += 2;
        }

        // TODO: Implement halfmove and fullmove clock
        /*if (spaces == 4) {
            halfMoveClock = character - '0';
        }

        if (spaces == 5) {
            fullmoveClock = character - '0';
        }*/
    }

    return true;
}
} // namespace Zagreus
