//
// Created by Danny on 14-6-2022.
//

#include <iostream>

#include "bitboard.h"
#include "move_gen.h"

namespace Chess {
    std::vector<Move> generatePseudoLegalMoves(Bitboard bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(100);
        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);

        generatePawnMoves(moves, bitboard, ownPiecesBB, color, color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN);
        generateKnightMoves(moves, bitboard, ownPiecesBB, color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT);
        generateBishopMoves(moves, bitboard, ownPiecesBB, color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP);
        generateRookMoves(moves, bitboard, ownPiecesBB, color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK);
        generateQueenMoves(moves, bitboard, ownPiecesBB, color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);
        generateKingMoves(moves, bitboard, ownPiecesBB, color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING);

        return moves;
    }

    // TODO: generate en passant
    void generatePawnMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType) {
        uint64_t pawnBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        while (pawnBB) {
            uint64_t index = Chess::bitscanForward(pawnBB);
            uint64_t genBB;

            if (pieceType == PieceType::WHITE_PAWN) {
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
            if (pieceType == PieceType::WHITE_PAWN) {
                attackBB = bitboard.getWhitePawnAttacks(1ULL << index);

                if (bitboard.getBlackEnPassantSquare() >= 0) {
                    attackBB &= opponentPiecesBB | (1ULL << bitboard.getBlackEnPassantSquare());
                } else {
                    attackBB &= opponentPiecesBB;
                }
            } else {
                attackBB = bitboard.getBlackPawnAttacks(1ULL << index);

                if (bitboard.getWhiteEnPassantSquare() >= 0) {
                    attackBB &= opponentPiecesBB | (1ULL << bitboard.getWhiteEnPassantSquare());
                } else {
                    attackBB &= opponentPiecesBB;
                }
            }

            attackBB &= ~ownPiecesBB;

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