//
// Created by Danny on 14-6-2022.
//

#pragma once

#include <vector>

#include "bitboard.h"

namespace Zagreus {
    std::vector<Move> generateLegalMoves(Bitboard &bitboard, PieceColor color);

    std::vector<Move> generateQuiescenceMoves(Bitboard &bitboard, PieceColor color);

    uint64_t generateMobilityMoves(Bitboard &bitboard, PieceColor color);

    void generateAttackMapMoves(std::vector<Move> &moves, Bitboard &bitboard, PieceColor color, uint64_t mask);

    void generatePawnMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce = false, uint64_t mask = ~0);

    void generateKnightMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateBishopMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateRookMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateQueenMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                            PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    void generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color, PieceType pieceType, bool quiesce = false, uint64_t mask = ~0ULL);

    bool sortMoves(Move &a, Move &b);
}

