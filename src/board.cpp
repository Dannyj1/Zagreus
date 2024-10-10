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

#include <iostream>
#include <ostream>
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
    assert(square < SQUARES);
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

void Board::print() const {
    std::cout << "  ---------------------------------";

    for (int index = 0; index < 64; index++) {
        if (index % 8 == 0) {
            std::cout << std::endl << index / 8 + 1 << " | ";
        }

        std::cout << getCharacterForPieceType(board[index]) << " | ";
    }

    std::cout << std::endl << "  ---------------------------------" << std::endl;
    std::cout << "    a   b   c   d   e   f   g   h  " << std::endl;
}

void Board::makeMove(const Move& move) {
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const MoveType moveType = getMoveType(move);
    const Piece movedPiece = getPieceOnSquare(fromSquare);
    const PieceType movedPieceType = pieceType(movedPiece);
    const Piece capturedPiece = getPieceOnSquare(toSquare);

    if (capturedPiece != Piece::EMPTY) {
        removePiece(capturedPiece, toSquare);
    }

    removePiece(movedPiece, fromSquare);
    setPiece(movedPiece, toSquare);

    if (movedPieceType == PieceType::PAWN) {
        if (moveType == MoveType::EN_PASSANT) {
            if (sideToMove == PieceColor::WHITE) {
                removePiece(Piece::BLACK_PAWN, toSquare + TO_INT(Direction::SOUTH));
            } else {
                removePiece(Piece::WHITE_PAWN, toSquare + TO_INT(Direction::NORTH));
            }
        }

        if ((fromSquare ^ toSquare) == 16) {
            if (sideToMove == PieceColor::WHITE) {
                enPassantSquare = toSquare + TO_INT(Direction::SOUTH);
            } else {
                enPassantSquare = toSquare + TO_INT(Direction::NORTH);
            }
        } else {
            enPassantSquare = 0;
        }
    } else {
        enPassantSquare = 0;
    }

    sideToMove = !sideToMove;
    assert(ply >= 0 && ply < MAX_PLY);
    history[ply].move = move;
    history[ply].capturedPiece = capturedPiece;
    history[ply].enPassantSquare = enPassantSquare;
    ply++;
    assert(ply >= 0 && ply < MAX_PLY);
}

void Board::unmakeMove() {
    ply--;
    assert(ply >= 0 && ply < MAX_PLY);
    const auto& [move, capturedPiece, enPassantSquare] = history[ply];
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const MoveType moveType = getMoveType(move);
    const Piece movedPiece = getPieceOnSquare(toSquare);

    removePiece(movedPiece, toSquare);
    setPiece(movedPiece, fromSquare);

    if (capturedPiece != Piece::EMPTY) {
        setPiece(capturedPiece, toSquare);
    }

    if (moveType == MoveType::EN_PASSANT) {
        const PieceColor movedPieceColor = pieceColor(movedPiece);

        if (movedPieceColor == PieceColor::WHITE) {
            setPiece(Piece::BLACK_PAWN, toSquare + TO_INT(Direction::SOUTH));
        } else {
            setPiece(Piece::WHITE_PAWN, toSquare + TO_INT(Direction::NORTH));
        }
    }

    this->sideToMove = !sideToMove;
    this->enPassantSquare = enPassantSquare;
}

void Board::setPieceFromFENChar(const char character, const uint8_t square) {
    assert(square < SQUARES);
    using enum Piece;

    // Uppercase char = white, lowercase = black
    switch (character) {
        case 'P':
            setPiece(WHITE_PAWN, square);
            break;
        case 'p':
            setPiece(BLACK_PAWN, square);
            break;
        case 'N':
            setPiece(WHITE_KNIGHT, square);
            break;
        case 'n':
            setPiece(BLACK_KNIGHT, square);
            break;
        case 'B':
            setPiece(WHITE_BISHOP, square);
            break;
        case 'b':
            setPiece(BLACK_BISHOP, square);
            break;
        case 'R':
            setPiece(WHITE_ROOK, square);
            break;
        case 'r':
            setPiece(BLACK_ROOK, square);
            break;
        case 'Q':
            setPiece(WHITE_QUEEN, square);
            break;
        case 'q':
            setPiece(BLACK_QUEEN, square);
            break;
        case 'K':
            setPiece(WHITE_KING, square);
            break;
        case 'k':
            setPiece(BLACK_KING, square);
            break;
        default:
            break;
    }
}

bool Board::setFromFEN(const std::string_view fen) {
    // TODO: Update zobrist hash once that is implemented
    uint8_t index = TO_INT(Square::A8);
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
            const int8_t rank = (!sideToMove) == PieceColor::WHITE ? 2 : 5;

            if (file < 0 || file > 7) {
                return false;
            }

            // TODO: Add enPassant zobrist
            enPassantSquare = rank * 8 + file;
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
