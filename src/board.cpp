/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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

#include <ctype.h>

#include <iostream>
#include <string_view>

#include "bitwise.h"
#include "eval.h"
#include "pcg_random.hpp"

namespace Zagreus {
static uint64_t zobristConstants[781]{};

/**
 * \brief Initializes the Zobrist constants.
 */
void initZobristConstants() {
    pcg64_oneseq_once_insecure rng;

    rng.seed(0x661778f67199663dULL);

    for (uint64_t& zobrist : zobristConstants) {
        zobrist = rng();

        while (zobrist == 0) {
            zobrist = rng();
        }
    }
}

/**
 * \brief Gets the Zobrist constant for a given index.
 */
uint64_t getZobristConstant(const int index) {
    assert(index >= 0 && index < 781);
    assert(zobristConstants[index] != 0);
    return zobristConstants[index];
}

bool Board::isMoveLegal(const Move move) const {
    const Square toSquare = getToSquare(move);
    const Piece pieceOnSquare = getPieceOnSquare(getFromSquare(move));
    const PieceType movedPiece = getPieceType(pieceOnSquare);
    const PieceColor movedColor = getPieceColor(pieceOnSquare);
    const MoveType moveType = getMoveType(move);

    const Square kingSquare = movedColor == WHITE ? getKingSquare<WHITE>() : getKingSquare<BLACK>();

    if (moveType == EN_PASSANT) {
        const Square fromSquare = getFromSquare(move);
        const Square capturedPawnSquare = static_cast<Square>(toSquare + (movedColor == WHITE ? SOUTH : NORTH));
        const uint64_t occupancy = (occupied ^ squareToBitboard(fromSquare) ^ squareToBitboard(capturedPawnSquare)) |
                                   squareToBitboard(toSquare);

        if (movedColor == WHITE) {
            return !isSquareAttackedBySlider<BLACK>(kingSquare, occupancy);
        } else {
            return !isSquareAttackedBySlider<WHITE>(kingSquare, occupancy);
        }
    }

    const Square fromSquare = getFromSquare(move);

    if (moveType == CASTLING) {
        uint64_t castlingPath = squareToBitboard(fromSquare) | squareToBitboard(toSquare);

        if (toSquare == G1) {
            castlingPath |= WHITE_KINGSIDE_CASTLE_PATH;
        } else if (toSquare == C1) {
            castlingPath |= WHITE_QUEENSIDE_CASTLE_PATH;
        } else if (toSquare == G8) {
            castlingPath |= BLACK_KINGSIDE_CASTLE_PATH;
        } else if (toSquare == C8) {
            castlingPath |= BLACK_QUEENSIDE_CASTLE_PATH;
        }

        while (castlingPath) {
            const Square square = static_cast<Square>(popLsb(castlingPath));
            const uint64_t attackers = movedColor == WHITE ? getSquareAttackersByColor<BLACK>(square, occupied)
                                                           : getSquareAttackersByColor<WHITE>(square, occupied);

            if (attackers) {
                return false;
            }
        }

        return true;
    }

    // King moves need to check if there are ANY attackers on the toSquare.
    if (movedPiece == KING) {
        uint64_t occupancy = occupied;
        occupancy &= ~squareToBitboard(fromSquare);
        bool result = movedColor == WHITE ? getSquareAttackersByColor<BLACK>(toSquare, occupancy) == 0
                                          : getSquareAttackersByColor<WHITE>(toSquare, occupancy) == 0;
        return result;
    }

    const uint64_t pinnedPieces = movedColor == WHITE ? this->pinnedPieces[WHITE] : this->pinnedPieces[BLACK];
    const uint64_t kingBB = squareToBitboard(kingSquare);

    return !(pinnedPieces & squareToBitboard(fromSquare)) || (getLine(fromSquare, toSquare) & kingBB);
}

/**
 * \brief Checks if castling is possible for the given side. It checks every rule, except for attacks on the castling
 * path or if the king is in check
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
uint64_t Board::getSquareAttackers(const Square square) const {
    assert(square < SQUARES);
    const uint64_t knights = getPieceBoard<WHITE_KNIGHT>() | getPieceBoard<BLACK_KNIGHT>();
    const uint64_t kings = getPieceBoard<WHITE_KING>() | getPieceBoard<BLACK_KING>();
    uint64_t bishopsQueens = getPieceBoard<WHITE_QUEEN>() | getPieceBoard<BLACK_QUEEN>();
    uint64_t rooksQueens = getPieceBoard<WHITE_QUEEN>() | getPieceBoard<BLACK_QUEEN>();
    rooksQueens |= getPieceBoard<WHITE_ROOK>() | getPieceBoard<BLACK_ROOK>();
    bishopsQueens |= getPieceBoard<WHITE_BISHOP>() | getPieceBoard<BLACK_BISHOP>();

    return (getPawnAttacks<WHITE>(square) & getPieceBoard<BLACK_PAWN>()) |
           (getPawnAttacks<BLACK>(square) & getPieceBoard<WHITE_PAWN>()) | (getKnightAttacks(square) & knights) |
           (getKingAttacks(square) & kings) | (getBishopAttacks(square, occupied) & bishopsQueens) |
           (getRookAttacks(square, occupied) & rooksQueens);
}

/**
 * \brief Retrieves the attackers of a given square by color.
 * \tparam color The color of the attackers.
 * \param square The square index (0-63).
 * \return A bitboard representing the attackers of the given color.
 */
template <PieceColor color>
uint64_t Board::getSquareAttackersByColor(const Square square, const uint64_t occupancy) const {
    assert(square < SQUARES);
    constexpr PieceColor opponentColor = !color;

    const uint64_t knights = getPieceBoard<color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT>();
    const uint64_t kings = getPieceBoard<color == WHITE ? WHITE_KING : BLACK_KING>();
    uint64_t bishopsQueens = getPieceBoard<color == WHITE ? WHITE_QUEEN : BLACK_QUEEN>();
    uint64_t rooksQueens = getPieceBoard<color == WHITE ? WHITE_QUEEN : BLACK_QUEEN>();
    rooksQueens |= getPieceBoard<color == WHITE ? WHITE_ROOK : BLACK_ROOK>();
    bishopsQueens |= getPieceBoard<color == WHITE ? WHITE_BISHOP : BLACK_BISHOP>();

    return (getPawnAttacks<opponentColor>(square) & getPieceBoard<color == WHITE ? WHITE_PAWN : BLACK_PAWN>()) |
           (getKnightAttacks(square) & knights) | (getKingAttacks(square) & kings) |
           (getBishopAttacks(square, occupancy) & bishopsQueens) | (getRookAttacks(square, occupancy) & rooksQueens);
}

template uint64_t Board::getSquareAttackersByColor<WHITE>(Square square, uint64_t occupancy) const;
template uint64_t Board::getSquareAttackersByColor<BLACK>(Square square, uint64_t occupancy) const;

template <PieceColor AttackerColor>
bool Board::isSquareAttackedBySlider(const Square square, const uint64_t occupancy) const {
    const uint64_t opponentBishops = getPieceBoard<AttackerColor == WHITE ? WHITE_BISHOP : BLACK_BISHOP>();
    const uint64_t opponentRooks = getPieceBoard<AttackerColor == WHITE ? WHITE_ROOK : BLACK_ROOK>();
    const uint64_t opponentQueens = getPieceBoard<AttackerColor == WHITE ? WHITE_QUEEN : BLACK_QUEEN>();

    return (getBishopAttacks(square, occupancy) & (opponentBishops | opponentQueens)) ||
           (getRookAttacks(square, occupancy) & (opponentRooks | opponentQueens));
}

template bool Board::isSquareAttackedBySlider<WHITE>(Square square, uint64_t occupancy) const;
template bool Board::isSquareAttackedBySlider<BLACK>(Square square, uint64_t occupancy) const;

/**
 * \brief Resets the board to the initial state.
 */
void Board::reset() {
    this->board = {};
    this->bitboards = {};
    this->colorBoards = {};
    this->history = {};
    this->sideToMove = WHITE;
    this->occupied = 0;
    this->zobristHash = 0;
    this->ply = 0;
    this->rootPly = 0;
    this->fullmoveClock = 1;
    this->halfMoveClock = 0;
    this->castlingRights = 0;
    this->enPassantSquare = NO_EN_PASSANT;
    this->previousMove = NO_MOVE;

    std::ranges::fill(board, EMPTY);
    std::ranges::fill(bitboards, 0);
    std::ranges::fill(colorBoards, 0);
    std::ranges::fill(history, BoardState{});
    std::ranges::fill(pinnedPieces, 0);
    std::ranges::fill(checkBlockers, 0);
    std::ranges::fill(pinners, 0);
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

bool Board::isDraw() const {
    const uint64_t occupied = getOccupiedBitboard();

    if (halfMoveClock >= 100) {
        return true;
    }

    const uint64_t zobristHash = getZobristHash();

    // 2-fold repetition after the root, 3-fold repetition at or before the root
    int repetitions = 0;

    for (int i = ply - 2; i >= 0 && i >= (ply - halfMoveClock); i -= 2) {
        if (history[i].zobristHash == zobristHash && (i > rootPly || ++repetitions == 2)) {
            return true;
        }
    }

    const uint64_t sufficientMaterial = getPieceBoard<WHITE_QUEEN>() | getPieceBoard<BLACK_QUEEN>() |
                                        getPieceBoard<WHITE_ROOK>() | getPieceBoard<BLACK_ROOK>() |
                                        getPieceBoard<WHITE_PAWN>() | getPieceBoard<BLACK_PAWN>();

    if (sufficientMaterial) {
        // Can never be a draw with queens, rooks or pawns
        return false;
    }

    // Only kings left
    if (popcnt(occupied) == 2) {
        return true;
    }

    // Check for KBvK, KNvK
    if (popcnt(occupied) == 3) {
        if (getPieceBoard<WHITE_BISHOP>() || getPieceBoard<BLACK_BISHOP>() || getPieceBoard<WHITE_KNIGHT>() ||
            getPieceBoard<BLACK_KNIGHT>()) {
            return true;
        }
    }

    // Check for KBvKB where bishops are on the same color, and KNvKN
    if (popcnt(occupied) == 4) {
        if (getPieceBoard<WHITE_BISHOP>() && getPieceBoard<BLACK_BISHOP>()) {
            if (((getPieceBoard<WHITE_BISHOP>() & DARK_SQUARES) && (getPieceBoard<BLACK_BISHOP>() & DARK_SQUARES)) ||
                ((getPieceBoard<WHITE_BISHOP>() & LIGHT_SQUARES) && (getPieceBoard<BLACK_BISHOP>() & LIGHT_SQUARES))) {
                return true;
            }
        }
        if (getPieceBoard<WHITE_KNIGHT>() && getPieceBoard<BLACK_KNIGHT>()) {
            return true;
        }
    }

    return false;
}

uint64_t Board::getZobristHash() const { return this->zobristHash; }

/**
 * \brief gets the square of the attacker with the lowest value of a given square
 * \tparam color The color of the attacker.
 * \param square The square to evaluate.
 *
 * \return The square of the attacker with the lowest value.
 */
template <PieceColor color>
Square Board::getSmallestAttacker(Square square) const {
    const uint64_t attackers = getSquareAttackersByColor<color>(square);

    if (!attackers) {
        return NONE;
    }

    constexpr Piece attackerOrder[6] = {
        color == WHITE ? WHITE_PAWN : BLACK_PAWN,     color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT,
        color == WHITE ? WHITE_BISHOP : BLACK_BISHOP, color == WHITE ? WHITE_ROOK : BLACK_ROOK,
        color == WHITE ? WHITE_QUEEN : BLACK_QUEEN,   color == WHITE ? WHITE_KING : BLACK_KING};

    for (const Piece piece : attackerOrder) {
        if (const uint64_t candidate = attackers & bitboards[piece]) {
            return static_cast<Square>(bitscanForward(candidate));
        }
    }

    return NONE;
}

template <PieceColor color>
void Board::updatePinnedPieces() {
    constexpr PieceColor opponentColor = !color;
    const Square kingSquare = getKingSquare<color>();
    const uint64_t opponentBishops = getPieceBoard<opponentColor == WHITE ? WHITE_BISHOP : BLACK_BISHOP>();
    const uint64_t opponentRooks = getPieceBoard<opponentColor == WHITE ? WHITE_ROOK : BLACK_ROOK>();
    const uint64_t opponentQueens = getPieceBoard<opponentColor == WHITE ? WHITE_QUEEN : BLACK_QUEEN>();
    const uint64_t opponentSliders = opponentBishops | opponentRooks | opponentQueens;

    this->pinnedPieces[color] = 0;
    this->checkBlockers[color] = 0;
    this->pinners[opponentColor] = 0;

    uint64_t candidatePinners = (getRookAttacks(kingSquare, opponentSliders) & (opponentRooks | opponentQueens)) |
                                (getBishopAttacks(kingSquare, opponentSliders) & (opponentBishops | opponentQueens));

    while (candidatePinners) {
        const Square pinnerSquare = static_cast<Square>(popLsb(candidatePinners));
        const uint64_t betweenSquares = getSquaresBetween(pinnerSquare, kingSquare);
        const uint64_t piecesBetween =
            betweenSquares & occupied & ~squareToBitboard(pinnerSquare) & ~squareToBitboard(kingSquare);

        if (piecesBetween && popcnt(piecesBetween) == 1) {
            this->checkBlockers[color] |= piecesBetween;

            if (piecesBetween & getColorBitboard<color>()) {
                this->pinnedPieces[color] |= piecesBetween;
                this->pinners[opponentColor] |= squareToBitboard(pinnerSquare);
            }
        }
    }
}

template void Board::updatePinnedPieces<WHITE>();
template void Board::updatePinnedPieces<BLACK>();

int estimateMoveValue(const Board& board, const Move move) {
    const MoveType moveType = getMoveType(move);
    Piece capturedPiece = board.getPieceOnSquare(getToSquare(move));

    if (moveType == EN_PASSANT) {
        capturedPiece = board.getSideToMove() == WHITE ? BLACK_PAWN : WHITE_PAWN;
    }

    int value = getPieceValue(capturedPiece);

    if (moveType == PROMOTION) {
        value += getPieceValue(getPieceFromPromotionPiece(getPromotionPiece(move), board.getSideToMove())) -
                 getPieceValue(WHITE_PAWN);
    } else if (moveType == CASTLING) {
        value = 0;
    }

    return value;
}

/**
 * \brief Calculates the Static Exchange Evaluation (SEE) score for a capture move. The implementation is mostly based
 * on Ethereal's SEE implementation.
 * \param move The move to evaluate.
 *
 * \return The static exchange evaluation score. Negative scores indicate a loss of material, while positive scores
 * indicate a gain of material.
 */
bool Board::see(const Move move, int threshold) {
    const Square fromSquare = getFromSquare(move);
    const Square toSquare = getToSquare(move);
    const MoveType moveType = getMoveType(move);
    const Piece nextVictim = moveType == PROMOTION ? getPieceFromPromotionPiece(getPromotionPiece(move), sideToMove)
                                                   : getPieceOnSquare(fromSquare);
    PieceType nextVictimType = getPieceType(nextVictim);
    const int moveValue = estimateMoveValue(*this, move);
    int balance = moveValue - threshold;

    if (balance < 0) {
        return false;
    }

    balance -= getPieceValue(nextVictim);

    if (balance >= 0) {
        return true;
    }

    const uint64_t bishops = getPieceBoard<WHITE_BISHOP>() | getPieceBoard<BLACK_BISHOP>() |
                             getPieceBoard<WHITE_QUEEN>() | getPieceBoard<BLACK_QUEEN>();
    const uint64_t rooks = getPieceBoard<WHITE_ROOK>() | getPieceBoard<BLACK_ROOK>() | getPieceBoard<WHITE_QUEEN>() |
                           getPieceBoard<BLACK_QUEEN>();
    const uint64_t oldOccupied = getOccupiedBitboard();
    const PieceColor oldSideToMove = sideToMove;
    occupied = (occupied ^ squareToBitboard(fromSquare)) | squareToBitboard(toSquare);

    if (moveType == EN_PASSANT) {
        const Square capturedPawnSquare =
            sideToMove == WHITE ? static_cast<Square>(toSquare + SOUTH) : static_cast<Square>(toSquare + NORTH);
        occupied ^= squareToBitboard(capturedPawnSquare);
    }

    uint64_t attackers = getSquareAttackers(toSquare) & occupied;
    sideToMove = !sideToMove;

    while (true) {
        const uint64_t ownAttackers = attackers & getColorBitboard(sideToMove);

        if (!ownAttackers) {
            break;
        }

        for (nextVictimType = PAWN; nextVictimType <= QUEEN;
             nextVictimType = static_cast<PieceType>(nextVictimType + 1)) {
            if (ownAttackers & getPieceBoard(getPieceFromType(nextVictimType, sideToMove))) {
                break;
            }
        }

        occupied ^= squareToBitboard(
            bitscanForward(ownAttackers & getPieceBoard(getPieceFromType(nextVictimType, sideToMove))));

        if (nextVictimType == PAWN || nextVictimType == BISHOP || nextVictimType == QUEEN) {
            attackers |= getBishopAttacks(toSquare, occupied) & bishops;
        }

        if (nextVictimType == ROOK || nextVictimType == QUEEN) {
            attackers |= getRookAttacks(toSquare, occupied) & rooks;
        }

        attackers &= occupied;
        balance = -balance - 1 - getPieceValue(getPieceFromType(nextVictimType, sideToMove));
        sideToMove = !sideToMove;

        if (balance >= 0) {
            if (nextVictimType == KING && (attackers & getColorBitboard(sideToMove))) {
                sideToMove = !sideToMove;
            }

            break;
        }
    }

    const bool result = sideToMove != oldSideToMove;
    occupied = oldOccupied;
    sideToMove = oldSideToMove;
    return result;
}

uint64_t Board::openFiles() const {
    const uint64_t whitePawns = getPieceBoard<WHITE_PAWN>();
    const uint64_t blackPawns = getPieceBoard<BLACK_PAWN>();

    return ~fillFile(whitePawns) & ~fillFile(blackPawns);
}

/**
 * \brief Makes a move on the board.
 * \param move The move to make.
 */
void Board::makeMove(const Move move) {
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);
    const MoveType moveType = getMoveType(move);
    const Piece movedPiece = getPieceOnSquare(fromSquare);
    const PieceType movedPieceType = getPieceType(movedPiece);
    Piece capturedPiece = getPieceOnSquare(toSquare);

    if (moveType == EN_PASSANT) {
        capturedPiece = sideToMove == WHITE ? BLACK_PAWN : WHITE_PAWN;
    }

    history[ply].move = move;
    history[ply].previousMove = previousMove;
    history[ply].capturedPiece = capturedPiece;
    history[ply].enPassantSquare = enPassantSquare;
    history[ply].castlingRights = castlingRights;
    history[ply].zobristHash = zobristHash;
    history[ply].halfMoveClock = halfMoveClock;
    history[ply].checkBlockers = checkBlockers;
    history[ply].pinnedPieces = pinnedPieces;
    history[ply].pinners = pinners;

    if (enPassantSquare != NO_EN_PASSANT) {
        zobristHash ^= getZobristConstant(ZOBRIST_EN_PASSANT_START_INDEX + (enPassantSquare % 8));
    }

    enPassantSquare = NO_EN_PASSANT;

    halfMoveClock += 1;

    if (capturedPiece != EMPTY && moveType != EN_PASSANT) {
        removePiece(capturedPiece, toSquare);
        halfMoveClock = 0;

        if (capturedPiece == WHITE_ROOK) {
            if (toSquare == A1) {
                if (castlingRights & WHITE_QUEENSIDE) {
                    castlingRights &= ~WHITE_QUEENSIDE;
                    zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_QUEENSIDE_INDEX);
                }
            } else if (toSquare == H1) {
                if (castlingRights & WHITE_KINGSIDE) {
                    castlingRights &= ~WHITE_KINGSIDE;
                    zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_KINGSIDE_INDEX);
                }
            }
        } else if (capturedPiece == BLACK_ROOK) {
            if (toSquare == A8) {
                if (castlingRights & BLACK_QUEENSIDE) {
                    castlingRights &= ~BLACK_QUEENSIDE;
                    zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_QUEENSIDE_INDEX);
                }
            } else if (toSquare == H8) {
                if (castlingRights & BLACK_KINGSIDE) {
                    castlingRights &= ~BLACK_KINGSIDE;
                    zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_KINGSIDE_INDEX);
                }
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
            if (castlingRights & WHITE_KINGSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_KINGSIDE_INDEX);
            }

            if (castlingRights & WHITE_QUEENSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_QUEENSIDE_INDEX);
            }

            castlingRights &= ~WHITE_CASTLING;
        } else {
            if (castlingRights & BLACK_KINGSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_KINGSIDE_INDEX);
            }

            if (castlingRights & BLACK_QUEENSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_QUEENSIDE_INDEX);
            }

            castlingRights &= ~BLACK_CASTLING;
        }
    }

    if (movedPiece == WHITE_KING) {
        if (castlingRights & WHITE_KINGSIDE) {
            zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_KINGSIDE_INDEX);
        }

        if (castlingRights & WHITE_QUEENSIDE) {
            zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_QUEENSIDE_INDEX);
        }

        castlingRights &= ~WHITE_CASTLING;
    } else if (movedPiece == BLACK_KING) {
        if (castlingRights & BLACK_KINGSIDE) {
            zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_KINGSIDE_INDEX);
        }

        if (castlingRights & BLACK_QUEENSIDE) {
            zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_QUEENSIDE_INDEX);
        }

        castlingRights &= ~BLACK_CASTLING;
    } else if (movedPiece == WHITE_ROOK) {
        if (fromSquare == A1) {
            if (castlingRights & WHITE_QUEENSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_QUEENSIDE_INDEX);
                castlingRights &= ~WHITE_QUEENSIDE;
            }
        } else if (fromSquare == H1) {
            if (castlingRights & WHITE_KINGSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_KINGSIDE_INDEX);
                castlingRights &= ~WHITE_KINGSIDE;
            }
        }
    } else if (movedPiece == BLACK_ROOK) {
        if (fromSquare == A8) {
            if (castlingRights & BLACK_QUEENSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_QUEENSIDE_INDEX);
                castlingRights &= ~BLACK_QUEENSIDE;
            }
        } else if (fromSquare == H8) {
            if (castlingRights & BLACK_KINGSIDE) {
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_KINGSIDE_INDEX);
                castlingRights &= ~BLACK_KINGSIDE;
            }
        }
    }

    if (movedPieceType == PAWN) {
        halfMoveClock = 0;

        if ((fromSquare ^ toSquare) == 16) {
            const uint64_t toSquareBB = squareToBitboard(toSquare);
            const uint64_t adjacentSquares = shiftEast(toSquareBB) | shiftWest(toSquareBB);
            const uint64_t opponentPawns = getPieceBoard(sideToMove == WHITE ? BLACK_PAWN : WHITE_PAWN);

            if (adjacentSquares & opponentPawns) {
                enPassantSquare = toSquare + (sideToMove == WHITE ? SOUTH : NORTH);
                zobristHash ^= getZobristConstant(ZOBRIST_EN_PASSANT_START_INDEX + (enPassantSquare % 8));
            }
        }
    }

    if (sideToMove == BLACK) {
        fullmoveClock += 1;
    }

    previousMove = move;
    sideToMove = !sideToMove;
    zobristHash ^= getZobristConstant(ZOBRIST_SIDE_TO_MOVE_INDEX);

    // Need to do this after flipping the side to move
    updatePinnedPieces<WHITE>();
    updatePinnedPieces<BLACK>();

    assert(ply < MAX_PLIES);
    ply++;
    assert(ply < MAX_PLIES);
    assert(enPassantSquare == NO_EN_PASSANT || (enPassantSquare / 8 == 2 || enPassantSquare / 8 == 5));
}

/**
 * \brief Unmakes the last move on the board.
 */
void Board::unmakeMove() {
    ply--;
    assert(ply >= 0 && ply < MAX_PLIES);
    const BoardState& state = history[ply];
    const Square fromSquare = getFromSquare(state.move);
    const Square toSquare = getToSquare(state.move);
    const MoveType moveType = getMoveType(state.move);
    Piece movedPiece = getPieceOnSquare(toSquare);
    const PieceColor movedColor = getPieceColor(movedPiece);

    removePiece(movedPiece, toSquare);

    if (moveType == PROMOTION) {
        const PieceColor color = getPieceColor(movedPiece);
        movedPiece = static_cast<Piece>(WHITE_PAWN + color);
    }

    setPiece(movedPiece, fromSquare);

    if (state.capturedPiece != EMPTY) {
        Square capturedSquare = toSquare;

        if (moveType == EN_PASSANT) {
            capturedSquare =
                movedColor == WHITE ? static_cast<Square>(toSquare + SOUTH) : static_cast<Square>(toSquare + NORTH);
        }

        setPiece(state.capturedPiece, capturedSquare);
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

    if (movedColor == BLACK) {
        fullmoveClock -= 1;
    }

    assert(state.move != NO_MOVE);
    this->previousMove = state.previousMove;
    this->halfMoveClock = state.halfMoveClock;
    this->sideToMove = !sideToMove;
    this->enPassantSquare = state.enPassantSquare;
    this->castlingRights = state.castlingRights;
    this->zobristHash = state.zobristHash;
    this->checkBlockers = state.checkBlockers;
    this->pinnedPieces = state.pinnedPieces;
    this->pinners = state.pinners;
}

void Board::makeNullMove() {
    history[ply].move = NO_MOVE;
    history[ply].previousMove = previousMove;
    history[ply].capturedPiece = EMPTY;
    history[ply].enPassantSquare = enPassantSquare;
    history[ply].castlingRights = castlingRights;
    history[ply].zobristHash = zobristHash;
    history[ply].halfMoveClock = halfMoveClock;

    if (enPassantSquare != NO_EN_PASSANT) {
        zobristHash ^= getZobristConstant(ZOBRIST_EN_PASSANT_START_INDEX + (enPassantSquare % 8));
    }

    enPassantSquare = NO_EN_PASSANT;

    halfMoveClock += 1;

    if (sideToMove == BLACK) {
        fullmoveClock += 1;
    }

    previousMove = NO_MOVE;
    sideToMove = !sideToMove;
    zobristHash ^= getZobristConstant(ZOBRIST_SIDE_TO_MOVE_INDEX);

    assert(ply < MAX_PLIES);
    ply++;
    assert(ply < MAX_PLIES);
}

void Board::unmakeNullMove() {
    ply--;
    assert(ply >= 0 && ply < MAX_PLIES);
    const BoardState& state = history[ply];

    if (sideToMove == BLACK) {
        fullmoveClock -= 1;
    }

    assert(state.move == NO_MOVE);
    this->previousMove = state.previousMove;
    this->halfMoveClock = state.halfMoveClock;
    this->sideToMove = !sideToMove;
    this->enPassantSquare = state.enPassantSquare;
    this->castlingRights = state.castlingRights;
    this->zobristHash = state.zobristHash;
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
                zobristHash ^= getZobristConstant(ZOBRIST_SIDE_TO_MOVE_INDEX);
            } else {
                return false;
            }
        }

        if (spaces == 2) {
            if (character == '-') {
                continue;
            } else if (character == 'K') {
                castlingRights |= WHITE_KINGSIDE;
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_KINGSIDE_INDEX);
                continue;
            }

            if (character == 'Q') {
                castlingRights |= WHITE_QUEENSIDE;
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_WHITE_QUEENSIDE_INDEX);
                continue;
            }

            if (character == 'k') {
                castlingRights |= BLACK_KINGSIDE;
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_KINGSIDE_INDEX);
                continue;
            }

            if (character == 'q') {
                castlingRights |= BLACK_QUEENSIDE;
                zobristHash ^= getZobristConstant(ZOBRIST_CASTLING_BLACK_QUEENSIDE_INDEX);
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

            const uint8_t epSquare = rank * 8 + file;
            const uint64_t pushedPawn = squareToBitboard(epSquare + (sideToMove == WHITE ? SOUTH : NORTH)) &
                                        getPieceBoard(sideToMove == WHITE ? BLACK_PAWN : WHITE_PAWN);
            const uint64_t capturingSquares = shiftEast(pushedPawn) | shiftWest(pushedPawn);
            const uint64_t pawns = getPieceBoard(sideToMove == WHITE ? WHITE_PAWN : BLACK_PAWN);

            if (capturingSquares & pawns) {
                enPassantSquare = epSquare;
                zobristHash ^= getZobristConstant(ZOBRIST_EN_PASSANT_START_INDEX + (enPassantSquare % 8));
            }

            index += 2;
        }

        if (spaces == 4) {
            if (character >= '0' && character <= '9') {
                halfMoveClock = halfMoveClock * 10 + (character - '0');
            }
        }

        if (spaces == 5) {
            if (character >= '0' && character <= '9') {
                fullmoveClock = fullmoveClock * 10 + (character - '0');
            }
        }
    }

    updatePinnedPieces<WHITE>();
    updatePinnedPieces<BLACK>();
    return true;
}

}  // namespace Zagreus