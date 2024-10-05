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

#include "move_gen.h"

#include "bitwise.h"
#include "board.h"
#include "types.h"

namespace Zagreus {
template <PieceColor color, GenerationType type>
void generatePawnMoves(const Board& board, MoveList& moves) {
    // TODO: Implement en passant
    // TODO: Handle promotions
    // TODO: Move attacks to table lookup
    // TODO: Implement GenerationType logic using a mask that is computed based on type
    constexpr Piece pawn = color == PieceColor::WHITE ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;
    constexpr PieceColor opponentColor = color == PieceColor::WHITE
                                             ? PieceColor::BLACK
                                             : PieceColor::WHITE;
    constexpr Piece opponentKing = opponentColor == PieceColor::WHITE
                                       ? Piece::WHITE_KING
                                       : Piece::BLACK_KING;

    const uint64_t pawnBB = board.getBitboard<pawn>();
    const uint64_t emptyBB = board.getEmptyBitboard();
    const uint64_t opponentKingBB = board.getBitboard<opponentKing>();
    const uint64_t opponentPieces = board.getColorBitboard<opponentColor>();
    uint64_t pawnSinglePushes;
    uint64_t pawnDoublePushes;
    uint64_t pawnWestAttacks;
    uint64_t pawnEastAttacks;

    if constexpr (color == PieceColor::WHITE) {
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

    pawnWestAttacks &= (opponentPieces & ~opponentKingBB);
    pawnEastAttacks &= (opponentPieces & ~opponentKingBB);

    constexpr Direction fromPushDirection = color == PieceColor::WHITE
                                                ? Direction::SOUTH
                                                : Direction::NORTH;
    constexpr Direction fromSqWestAttackDirection = color == PieceColor::WHITE
                                                        ? Direction::SOUTH_WEST
                                                        : Direction::NORTH_WEST;
    constexpr Direction fromSqEastAttackDirection = color == PieceColor::WHITE
                                                        ? Direction::SOUTH_EAST
                                                        : Direction::NORTH_EAST;

    while (pawnSinglePushes) {
        const uint8_t squareTo = popLsb(pawnSinglePushes);
        const uint8_t squareFrom = shift<fromPushDirection>(squareTo);
        const Move move = encodeMove(squareFrom, squareTo);

        moves.list[moves.size] = move;
        moves.size++;
    }

    while (pawnDoublePushes) {
        const uint8_t squareTo = popLsb(pawnDoublePushes);
        const uint8_t squareFrom = shift<fromPushDirection>(squareTo);
        const Move move = encodeMove(squareFrom, squareTo);

        moves.list[moves.size] = move;
        moves.size++;
    }

    while (pawnWestAttacks) {
        const uint8_t squareTo = popLsb(pawnWestAttacks);
        const uint8_t squareFrom = shift<fromSqWestAttackDirection>(squareTo);
        const Move move = encodeMove(squareFrom, squareTo);

        moves.list[moves.size] = move;
        moves.size++;
    }

    while (pawnEastAttacks) {
        const uint8_t squareTo = popLsb(pawnEastAttacks);
        const uint8_t squareFrom = shift<fromSqEastAttackDirection>(squareTo);
        const Move move = encodeMove(squareFrom, squareTo);

        moves.list[moves.size] = move;
        moves.size++;
    }
}

template <PieceColor color, GenerationType type>
void generateKnightMoves(const Board& board, MoveList& moves) {
    // TODO: Move to table calculations
    constexpr Piece knight = color == PieceColor::WHITE ? Piece::WHITE_KNIGHT : Piece::BLACK_KNIGHT;
    constexpr Piece opponentKing = color == PieceColor::WHITE ? Piece::WHITE_KING : Piece::BLACK_KING;
    const uint64_t ownPieces = board.getColorBitboard<color>();
    const uint64_t opponentKingBB = board.getBitboard<opponentKing>();
    uint64_t knightBB = board.getBitboard<knight>();

    while (knightBB) {
        const uint8_t fromSquare = popLsb(knightBB);
        uint64_t genBB = knightAttacks(fromSquare) & ~ownPieces & ~opponentKingBB;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.list[moves.size] = move;
            moves.size++;
        }
    }
}

template <PieceColor color, GenerationType type>
void generateBishopMoves(const Board& board, MoveList& moves) {
    using enum Piece;
    constexpr Piece bishop = color == PieceColor::WHITE ? WHITE_BISHOP : BLACK_BISHOP;
    constexpr Piece opponentKing = color == PieceColor::WHITE
                                       ? WHITE_KING
                                       : BLACK_KING;
    const uint64_t ownPieces = board.getColorBitboard<color>();
    const uint64_t opponentKingBB = board.getBitboard<opponentKing>();
    uint64_t bishopBB = board.getBitboard<bishop>();
    uint64_t occupied = board.getOccupiedBitboard();

    while (bishopBB) {
        const uint8_t fromSquare = popLsb(bishopBB);
        uint64_t genBB = bishopAttacks(fromSquare, occupied) & ~ownPieces & ~opponentKingBB;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.list[moves.size] = move;
            moves.size++;
        }
    }
}
} // namespace Zagreus
