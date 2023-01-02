//
// Created by Danny on 14-6-2022.
//

#pragma once

#include <vector>

#include "bitboard.h"

namespace Zagreus {
    void generateLegalMoves(MoveList &moves, Bitboard &bitboard, PieceColor color);

    void generateQuiescenceMoves(MoveList &moves, Bitboard &bitboard, PieceColor color);

    void generateMobilityMoves(MoveList &moves, Bitboard &bitboard, PieceColor color);

    void generateAttackMapMoves(MoveList &moves, Bitboard &bitboard, PieceColor color, uint64_t mask);

    void generatePawnMoves(MoveList &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce = false, uint64_t mask = ~0);

    void generateKnightMoves(MoveList &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateBishopMoves(MoveList &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateRookMoves(MoveList &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateQueenMoves(MoveList &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                            PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateKingMoves(MoveList &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    bool sortLegalMoves(const Move &a, const Move &b);
}

