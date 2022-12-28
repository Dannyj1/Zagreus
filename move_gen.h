//
// Created by Danny on 14-6-2022.
//

#pragma once

#include <vector>

#include "bitboard.h"

namespace Chess {
    std::vector<Move> generateMoves(Bitboard bitboard, PieceColor color);

    void generatePawnMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType);

    void generateKnightMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType);

    void generateBishopMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType);

    void generateRookMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType);

    void generateQueenMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType);

    void generateKingMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType);
}

