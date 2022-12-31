//
// Created by Danny on 14-6-2022.
//

#pragma once

#include <vector>

#include "bitboard.h"

namespace Chess {
    std::vector<Move> generatePseudoLegalMoves(Bitboard bitboard, PieceColor color);

    std::vector<Move> generateQuiescenceMoves(Bitboard bitboard, PieceColor color);

    std::vector<Move> generateMobilityMoves(Bitboard bitboard, PieceColor color);

    void generatePawnMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color,
                           PieceType pieceType, bool quiesce);

    void generateKnightMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce);

    void generateBishopMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce);

    void generateRookMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color,
                           PieceType pieceType, bool quiesce);

    void generateQueenMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce);

    void generateKingMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce);
}

