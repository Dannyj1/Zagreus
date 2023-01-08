//
// Created by Danny on 14-6-2022.
//

#pragma once

#include <vector>

#include "bitboard.h"

namespace Zagreus {
    std::vector<Move> generateLegalMoves(Bitboard &bitboard, PieceColor color);

    std::vector<Move> generateQuiescenceMoves(Bitboard &bitboard, PieceColor color);

    void
    generatePawnMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color,
                      PieceType pieceType, bool quiesce = false);

    void
    generateKnightMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                        PieceColor color, PieceType pieceType, bool quiesce = false);

    void
    generateBishopMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                        PieceColor color, PieceType pieceType, bool quiesce = false);

    void
    generateRookMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color,
                      PieceType pieceType, bool quiesce = false);

    void
    generateQueenMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                       PieceColor color, PieceType pieceType, bool quiesce = false);

    void
    generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                      PieceColor color, PieceType pieceType, bool quiesce = false);

    bool sortMoves(Bitboard &bitboard, Move &a, Move &b);
}

