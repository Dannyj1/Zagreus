//
// Created by Danny on 14-6-2022.
//

#include <iostream>

#include "bitboard.h"
#include "move_gen.h"

namespace Chess {
    std::vector<Move> generateMoves(Bitboard bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(100);
        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);

        generatePawnMoves(moves, bitboard, ownPiecesBB, color, color == PieceColor::White ? PieceType::WhitePawn : PieceType::BlackPawn);
        generateKnightMoves(moves, bitboard, ownPiecesBB, color == PieceColor::White ? PieceType::WhiteKnight : PieceType::BlackKnight);
        generateBishopMoves(moves, bitboard, ownPiecesBB, color == PieceColor::White ? PieceType::WhiteBishop : PieceType::BlackBishop);
        generateRookMoves(moves, bitboard, ownPiecesBB, color == PieceColor::White ? PieceType::WhiteRook : PieceType::BlackRook);
        generateQueenMoves(moves, bitboard, ownPiecesBB, color == PieceColor::White ? PieceType::WhiteQueen : PieceType::BlackQueen);
        generateKingMoves(moves, bitboard, ownPiecesBB, color == PieceColor::White ? PieceType::WhiteKing : PieceType::BlackKing);

        return moves;
    }

    // TODO: generate attacks and en passant
    void generatePawnMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType) {
        uint64_t pawnBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        while (pawnBB) {
            uint64_t index = Chess::bitscanForward(pawnBB);
            uint64_t genBB;

            if (pieceType == PieceType::WhitePawn) {
                genBB = bitboard.getWhitePawnDoublePush(1ULL << index);
            } else {
                genBB = bitboard.getBlackPawnDoublePush(1ULL << index);
            }

            genBB &= ~ownPiecesBB;

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            uint64_t attackBB;
            if (pieceType == PieceType::WhitePawn) {
                attackBB = bitboard.getWhitePawnAttacks(1ULL << index);
            } else {
                attackBB = bitboard.getBlackPawnAttacks(1ULL << index);
            }

            attackBB &= ~ownPiecesBB;
            attackBB &= opponentPiecesBB;

            while (attackBB > 0) {
                uint64_t attackIndex = Chess::bitscanForward(attackBB);

                result.push_back({index, attackIndex, pieceType});
                attackBB &= ~(1ULL << attackIndex);
            }

            pawnBB &= ~(1ULL << index);
        }
    }

    void generateKnightMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType) {
        uint64_t knightBB = bitboard.getPieceBoard(pieceType);

        while (knightBB) {
            uint64_t index = Chess::bitscanForward(knightBB);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            genBB &= ~ownPiecesBB;

            while (genBB) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            knightBB &= ~(1ULL << index);
        }
    }

    void generateBishopMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType) {
        uint64_t bishopBB = bitboard.getPieceBoard(pieceType);

        while (bishopBB) {
            uint64_t index = Chess::bitscanForward(bishopBB);
            uint64_t genBB = bitboard.getBishopAttacks(1ULL << index);

            genBB &= ~ownPiecesBB;

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void generateRookMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType) {
        uint64_t rookBB = bitboard.getPieceBoard(pieceType);

        while (rookBB) {
            uint64_t index = Chess::bitscanForward(rookBB);
            uint64_t genBB = bitboard.getRookAttacks(1ULL << index);

            genBB &= ~ownPiecesBB;

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            rookBB &= ~(1ULL << index);
        }
    }

    void generateQueenMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType) {
        uint64_t queenBB = bitboard.getPieceBoard(pieceType);

        while (queenBB) {
            uint64_t index = Chess::bitscanForward(queenBB);
            uint64_t genBB = bitboard.getQueenAttacks(1ULL << index);

            genBB &= ~ownPiecesBB;

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            queenBB &= ~(1ULL << index);
        }
    }

    void generateKingMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceType pieceType) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType);
        uint64_t index = Chess::bitscanForward(kingBB);
        uint64_t genBB = bitboard.getKingAttacks(index);

        genBB &= ~ownPiecesBB;

        while (genBB > 0) {
            uint64_t genIndex = Chess::bitscanForward(genBB);

            result.push_back({index, genIndex, pieceType});
            genBB &= ~(1ULL << genIndex);
        }
    }
}