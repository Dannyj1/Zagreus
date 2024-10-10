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

#include <array>

#include "move_gen.h"
#include "bitwise.h"
#include "board.h"
#include "types.h"
#include "bitboard.h"
#include "move.h"

namespace Zagreus {
template <PieceColor color, GenerationType type>
void generateMoves(const Board& board, MoveList& moves) {
    assert(moves.size == 0);

    // TODO: Implement GenerationType logic using a mask that is computed based on type
    constexpr Piece opponentKing = color == WHITE ? BLACK_KING : WHITE_KING;
    const uint64_t ownPieces = board.getColorBitboard<color>();
    const uint64_t opponentKingBB = board.getBitboard<opponentKing>();
    const uint64_t genMask = ~(ownPieces | opponentKingBB);

    generatePawnMoves<color, type>(board, moves, genMask);
    generateKnightMoves<color, type>(board, moves, genMask);
    generateBishopMoves<color, type>(board, moves, genMask);
    generateRookMoves<color, type>(board, moves, genMask);
    generateQueenMoves<color, type>(board, moves, genMask);
    generateKingMoves<color, type>(board, moves, genMask);
}

template <PieceColor color, GenerationType type>
void generatePawnMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    // TODO: Handle promotions
    constexpr Piece pawn = color == WHITE ? WHITE_PAWN : BLACK_PAWN;
    constexpr PieceColor opponentColor = !color;

    const uint64_t pawnBB = board.getBitboard<pawn>();
    const uint64_t emptyBB = board.getEmptyBitboard();
    const uint64_t opponentPieces = board.getColorBitboard<opponentColor>();
    uint64_t pawnSinglePushes;
    uint64_t pawnDoublePushes;
    uint64_t pawnWestAttacks;
    uint64_t pawnEastAttacks;

    if constexpr (color == WHITE) {
        pawnSinglePushes = whitePawnSinglePush(pawnBB, emptyBB);
        pawnDoublePushes = whitePawnDoublePush(pawnBB, emptyBB);
        pawnWestAttacks = whitePawnWestAttacks(pawnBB);
        pawnEastAttacks = whitePawnEastAttacks(pawnBB);
    } else {
        pawnSinglePushes = blackPawnSinglePush(pawnBB, emptyBB);
        pawnDoublePushes = blackPawnDoublePush(pawnBB, emptyBB);
        pawnWestAttacks = blackPawnWestAttacks(pawnBB);
        pawnEastAttacks = blackPawnEastAttacks(pawnBB);
    }

    uint64_t enPassantMask = squareToBitboard(board.getEnPassantSquare());

    if constexpr (color == WHITE) {
        enPassantMask &= RANK_6;
    } else {
        enPassantMask &= RANK_3;
    }

    pawnSinglePushes &= genMask;
    pawnDoublePushes &= genMask;
    pawnWestAttacks &= (opponentPieces | enPassantMask) & genMask;
    pawnEastAttacks &= (opponentPieces | enPassantMask) & genMask;

    constexpr Direction fromPushDirection = color == WHITE ? NORTH : SOUTH;
    constexpr Direction fromSqWestAttackDirection = color == WHITE ? NORTH_WEST : SOUTH_WEST;
    constexpr Direction fromSqEastAttackDirection = color == WHITE ? NORTH_EAST : SOUTH_EAST;

    while (pawnSinglePushes) {
        const uint8_t squareTo = popLsb(pawnSinglePushes);
        const uint8_t squareFrom = squareTo - fromPushDirection;
        const Move move = encodeMove(squareFrom, squareTo);

        moves.moves[moves.size] = move;
        moves.size++;
    }

    while (pawnDoublePushes) {
        const uint8_t squareTo = popLsb(pawnDoublePushes);
        const uint8_t squareFrom = squareTo - fromPushDirection - fromPushDirection;
        const Move move = encodeMove(squareFrom, squareTo);

        moves.moves[moves.size] = move;
        moves.size++;
    }

    while (pawnWestAttacks) {
        const uint8_t squareTo = popLsb(pawnWestAttacks);
        const uint8_t squareFrom = squareTo - fromSqWestAttackDirection;
        MoveType moveType = NORMAL;

        if (squareTo == board.getEnPassantSquare() && squareToBitboard(squareTo) & enPassantMask) {
            moveType = EN_PASSANT;
        }

        const Move move = encodeMove(squareFrom, squareTo, moveType);
        moves.moves[moves.size] = move;
        moves.size++;
    }

    while (pawnEastAttacks) {
        const uint8_t squareTo = popLsb(pawnEastAttacks);
        const uint8_t squareFrom = squareTo - fromSqEastAttackDirection;
        MoveType moveType = NORMAL;

        if (squareTo == board.getEnPassantSquare() && squareToBitboard(squareTo) & enPassantMask) {
            moveType = EN_PASSANT;
        }

        const Move move = encodeMove(squareFrom, squareTo, moveType);
        moves.moves[moves.size] = move;
        moves.size++;
    }
}

template <PieceColor color, GenerationType type>
void generateKnightMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece knight = color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
    uint64_t knightBB = board.getBitboard<knight>();

    while (knightBB) {
        const uint8_t fromSquare = popLsb(knightBB);
        const uint64_t attacks = knightAttacks(fromSquare);
        uint64_t genBB = attacks & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

template <PieceColor color, GenerationType type>
void generateBishopMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece bishop = color == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
    const uint64_t occupied = board.getOccupiedBitboard();
    uint64_t bishopBB = board.getBitboard<bishop>();

    while (bishopBB) {
        const uint8_t fromSquare = popLsb(bishopBB);
        uint64_t genBB = bishopAttacks(fromSquare, occupied) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

template <PieceColor color, GenerationType type>
void generateRookMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece rook = color == WHITE ? WHITE_ROOK : BLACK_ROOK;
    const uint64_t occupied = board.getOccupiedBitboard();
    uint64_t rookBB = board.getBitboard<rook>();

    while (rookBB) {
        const uint8_t fromSquare = popLsb(rookBB);
        uint64_t genBB = rookAttacks(fromSquare, occupied) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

template <PieceColor color, GenerationType type>
void generateQueenMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece queen = color == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
    const uint64_t occupied = board.getOccupiedBitboard();
    uint64_t queenBB = board.getBitboard<queen>();

    while (queenBB) {
        const uint8_t fromSquare = popLsb(queenBB);
        uint64_t genBB = queenAttacks(fromSquare, occupied) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

template <PieceColor color, GenerationType type>
void generateKingMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    // TODO: Implement castling
    constexpr Piece king = color == WHITE ? WHITE_KING : BLACK_KING;
    uint64_t kingBB = board.getBitboard<king>();

    while (kingBB) {
        const uint8_t fromSquare = popLsb(kingBB);
        uint64_t genBB = kingAttacks(fromSquare) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

// explicit instantiation of generateMoves
template void generateMoves<WHITE, ALL>(const Board& board, MoveList& moves);
template void generateMoves<WHITE, QUIET>(const Board& board, MoveList& moves);
template void generateMoves<WHITE, CAPTURES>(const Board& board, MoveList& moves);
template void generateMoves<WHITE, EVASIONS>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, ALL>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, QUIET>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, CAPTURES>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, EVASIONS>(const Board& board, MoveList& moves);
} // namespace Zagreus
