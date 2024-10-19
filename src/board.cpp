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
/**
 * \brief Checks if the current position is legal for the given color.
 * \tparam movedColor The color of the player who just moved.
 * \return True if the position is legal, false otherwise.
 */
template <PieceColor movedColor>
bool Board::isPositionLegal() const {
    constexpr PieceColor opponentColor = !movedColor;
    constexpr Piece king = movedColor == WHITE ? WHITE_KING : BLACK_KING;
    const uint64_t kingBB = getBitboard<king>();
    const Square kingSquare = bitboardToSquare(kingBB);
    const Move lastMove = getLastMove();
    const MoveType lastMoveType = getMoveType(lastMove);

    if (lastMoveType == CASTLING) {
        const uint8_t fromSquare = getFromSquare(lastMove);
        const uint8_t toSquare = getToSquare(lastMove);
        const uint64_t fromSquareAttacks = getSquareAttackersByColor<opponentColor>(fromSquare);

        if (fromSquareAttacks) {
            // King was in check before castling
            return false;
        }

        uint64_t castlingPath = 0;

        if (toSquare == G1) {
            castlingPath = WHITE_KINGSIDE_CASTLE_PATH;
        } else if (toSquare == C1) {
            castlingPath = WHITE_QUEENSIDE_CASTLE_PATH;
        } else if (toSquare == G8) {
            castlingPath = BLACK_KINGSIDE_CASTLE_PATH;
        } else if (toSquare == C8) {
            castlingPath = BLACK_QUEENSIDE_CASTLE_PATH;
        }

        while (castlingPath) {
            const uint8_t square = popLsb(castlingPath);
            const uint64_t attackers = getSquareAttackersByColor<opponentColor>(square);

            if (attackers) {
                return false;
            }
        }
    }

    return !getSquareAttackersByColor<opponentColor>(kingSquare);
}

template bool Board::isPositionLegal<WHITE>() const;
template bool Board::isPositionLegal<BLACK>() const;

/**
 * \brief Checks if castling is possible for the given side. It checks every rule, except for attacks on the castling path or if the king is in check
 * \tparam side The side to check for castling (WHITE_KINGSIDE, WHITE_QUEENSIDE, BLACK_KINGSIDE, BLACK_QUEENSIDE).
 * \return True if castling is possible, false otherwise.
 */
template <CastlingRights side>
bool Board::canCastle() const {
    assert(side != WHITE_CASTLING && side != BLACK_CASTLING);

    if (!(castlingRights & side)) {
        return false;
    }

    uint64_t castlingPath = 0;

    if constexpr (side == WHITE_KINGSIDE) {
        castlingPath = WHITE_KINGSIDE_CASTLE_UNOCCUPIED;
    } else if constexpr (side == WHITE_QUEENSIDE) {
        castlingPath = WHITE_QUEENSIDE_CASTLE_UNOCCUPIED;
    } else if constexpr (side == BLACK_KINGSIDE) {
        castlingPath = BLACK_KINGSIDE_CASTLE_UNOCCUPIED;
    } else if constexpr (side == BLACK_QUEENSIDE) {
        castlingPath = BLACK_QUEENSIDE_CASTLE_UNOCCUPIED;
    }

    if (occupied & castlingPath) {
        return false;
    }

    return true;
}

template bool Board::canCastle<WHITE_KINGSIDE>() const;
template bool Board::canCastle<WHITE_QUEENSIDE>() const;
template bool Board::canCastle<BLACK_KINGSIDE>() const;
template bool Board::canCastle<BLACK_QUEENSIDE>() const;

/**
 * \brief Retrieves the attackers of a given square.
 * \param square The square index (0-63).
 * \return A bitboard representing the attackers.
 */
uint64_t Board::getSquareAttackers(const uint8_t square) const {
    assert(square < SQUARES);
    const uint64_t knights = getBitboard<WHITE_KNIGHT>() | getBitboard<BLACK_KNIGHT>();
    const uint64_t kings = getBitboard<WHITE_KING>() | getBitboard<BLACK_KING>();
    uint64_t bishopsQueens = getBitboard<WHITE_QUEEN>() | getBitboard<BLACK_QUEEN>();
    uint64_t rooksQueens = getBitboard<WHITE_QUEEN>() | getBitboard<BLACK_QUEEN>();
    rooksQueens |= getBitboard<WHITE_ROOK>() | getBitboard<BLACK_ROOK>();
    bishopsQueens |= getBitboard<WHITE_BISHOP>() | getBitboard<BLACK_BISHOP>();

    return (getPawnAttacks<WHITE>(square) & getBitboard<BLACK_PAWN>())
           | (getPawnAttacks<BLACK>(square) & getBitboard<WHITE_PAWN>())
           | (getKnightAttacks(square) & knights)
           | (getKingAttacks(square) & kings)
           | (getBishopAttacks(square, occupied) & bishopsQueens)
           | (getRookAttacks(square, occupied) & rooksQueens);
}

/**
 * \brief Resets the board to the initial state.
 */
void Board::reset() {
    this->board = {};
    this->bitboards = {};
    this->occupied = 0;
    this->colorBoards = {};
    this->sideToMove = WHITE;
    this->history = {};
    this->ply = 0;
    this->castlingRights = 0;

    std::ranges::fill(board, EMPTY);
    std::ranges::fill(bitboards, 0);
    std::ranges::fill(colorBoards, 0);
    std::ranges::fill(history, BoardState{});
}

/**
 * \brief Prints the current state of the board to the console.
 */
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

/**
 * \brief Makes a move on the board.
 * \param move The move to make.
 */
void Board::makeMove(const Move& move) {
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const MoveType moveType = getMoveType(move);
    Piece movedPiece = getPieceOnSquare(fromSquare);
    const PieceType movedPieceType = getPieceType(movedPiece);
    const Piece capturedPiece = getPieceOnSquare(toSquare);

    history[ply].move = move;
    history[ply].capturedPiece = capturedPiece;
    history[ply].enPassantSquare = enPassantSquare;
    enPassantSquare = 0;
    history[ply].castlingRights = castlingRights;

    if (capturedPiece != EMPTY) {
        removePiece(capturedPiece, toSquare);

        if (capturedPiece == WHITE_ROOK) {
            if (toSquare == A1) {
                castlingRights &= ~WHITE_QUEENSIDE;
            } else if (toSquare == H1) {
                castlingRights &= ~WHITE_KINGSIDE;
            }
        } else if (capturedPiece == BLACK_ROOK) {
            if (toSquare == A8) {
                castlingRights &= ~BLACK_QUEENSIDE;
            } else if (toSquare == H8) {
                castlingRights &= ~BLACK_KINGSIDE;
            }
        }
    }

    removePiece(movedPiece, fromSquare);

    if (moveType == PROMOTION) {
        const PieceColor color = getPieceColor(movedPiece);
        const PromotionPiece promotionPieceType = getPromotionPiece(move);
        const Piece promotionPiece = getPieceFromPromotionPiece(promotionPieceType, color);

        setPiece(promotionPiece, toSquare);
    } else {
        setPiece(movedPiece, toSquare);
    }

    if (moveType == EN_PASSANT) {
        if (sideToMove == WHITE) {
            removePiece(BLACK_PAWN, toSquare + SOUTH);
        } else {
            removePiece(WHITE_PAWN, toSquare + NORTH);
        }
    } else if (moveType == CASTLING) {
        if (toSquare == G1) {
            removePiece(WHITE_ROOK, H1);
            setPiece(WHITE_ROOK, F1);
        } else if (toSquare == C1) {
            removePiece(WHITE_ROOK, A1);
            setPiece(WHITE_ROOK, D1);
        } else if (toSquare == G8) {
            removePiece(BLACK_ROOK, H8);
            setPiece(BLACK_ROOK, F8);
        } else if (toSquare == C8) {
            removePiece(BLACK_ROOK, A8);
            setPiece(BLACK_ROOK, D8);
        }

        if (sideToMove == WHITE) {
            castlingRights &= ~WHITE_CASTLING;
        } else {
            castlingRights &= ~BLACK_CASTLING;
        }
    }

    if (movedPiece == WHITE_KING) {
        castlingRights &= ~WHITE_CASTLING;
    } else if (movedPiece == BLACK_KING) {
        castlingRights &= ~BLACK_CASTLING;
    } else if (movedPiece == WHITE_ROOK) {
        if (fromSquare == A1) {
            castlingRights &= ~WHITE_QUEENSIDE;
        } else if (fromSquare == H1) {
            castlingRights &= ~WHITE_KINGSIDE;
        }
    } else if (movedPiece == BLACK_ROOK) {
        if (fromSquare == A8) {
            castlingRights &= ~BLACK_QUEENSIDE;
        } else if (fromSquare == H8) {
            castlingRights &= ~BLACK_KINGSIDE;
        }
    }

    if (movedPieceType == PAWN) {
        if ((fromSquare ^ toSquare) == 16) {
            if (sideToMove == WHITE) {
                enPassantSquare = toSquare + SOUTH;
            } else {
                enPassantSquare = toSquare + NORTH;
            }
        }
    }

    sideToMove = !sideToMove;
    assert(ply >= 0 && ply < MAX_PLY);
    ply++;
    assert(ply >= 0 && ply < MAX_PLY);
}

/**
 * \brief Unmakes the last move on the board.
 */
void Board::unmakeMove() {
    ply--;
    assert(ply >= 0 && ply < MAX_PLY);
    const auto& [move, capturedPiece, enPassantSquare, castlingRights] = history[ply];
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const MoveType moveType = getMoveType(move);
    Piece movedPiece = getPieceOnSquare(toSquare);

    removePiece(movedPiece, toSquare);

    if (moveType == PROMOTION) {
        const PieceColor color = getPieceColor(movedPiece);
        movedPiece = static_cast<Piece>(WHITE_PAWN + color);
    }

    setPiece(movedPiece, fromSquare);

    if (capturedPiece != EMPTY) {
        setPiece(capturedPiece, toSquare);
    }

    if (moveType == EN_PASSANT) {
        const PieceColor movedPieceColor = getPieceColor(movedPiece);

        if (movedPieceColor == WHITE) {
            setPiece(BLACK_PAWN, toSquare + SOUTH);
        } else {
            setPiece(WHITE_PAWN, toSquare + NORTH);
        }
    }

    if (moveType == CASTLING) {
        if (toSquare == G1) {
            removePiece(WHITE_ROOK, F1);
            setPiece(WHITE_ROOK, H1);
        } else if (toSquare == C1) {
            removePiece(WHITE_ROOK, D1);
            setPiece(WHITE_ROOK, A1);
        } else if (toSquare == G8) {
            removePiece(BLACK_ROOK, F8);
            setPiece(BLACK_ROOK, H8);
        } else if (toSquare == C8) {
            removePiece(BLACK_ROOK, D8);
            setPiece(BLACK_ROOK, A8);
        }
    }

    this->sideToMove = !sideToMove;
    this->enPassantSquare = enPassantSquare;
    this->castlingRights = castlingRights;
}

/**
 * \brief Sets a piece on the board from a FEN character.
 * \param character The FEN character representing the piece.
 * \param square The square index (0-63).
 */
void Board::setPieceFromFENChar(const char character, const uint8_t square) {
    assert(square < SQUARES);
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

/**
 * \brief Sets the board state from a FEN string.
 * \param fen The FEN string representing the board state.
 * \return True if the FEN string was valid, false otherwise.
 */
bool Board::setFromFEN(const std::string_view fen) {
    // TODO: Update zobrist hash once that is implemented
    uint8_t index = A8;
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
                sideToMove = WHITE;
            } else if (tolower(character) == 'b') {
                sideToMove = BLACK;
            } else {
                return false;
            }
        }

        // TODO: Add zobrist hashes
        if (spaces == 2) {
            if (character == '-') {
                continue;
            } else if (character == 'K') {
                castlingRights |= WHITE_KINGSIDE;
                continue;
            }

            if (character == 'Q') {
                castlingRights |= WHITE_QUEENSIDE;
                continue;
            }

            if (character == 'k') {
                castlingRights |= BLACK_KINGSIDE;
                continue;
            }

            if (character == 'q') {
                castlingRights |= BLACK_QUEENSIDE;
                continue;
            }

            return false;
        }

        if (spaces == 3) {
            if (character == '-') {
                continue;
            }

            if (tolower(character) < 'a' || tolower(character) > 'h') {
                continue;
            }

            const int8_t file = tolower(character) - 'a';
            const int8_t rank = (!sideToMove) == WHITE ? 2 : 5;

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
