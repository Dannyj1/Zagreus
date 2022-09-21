//
// Created by Danny on 14-6-2022.
//
#include <string>

#include "bitboard.h"
#include "move_gen.h"

namespace Chess {
    std::vector<Move> generateMoves(Bitboard bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(50);

        generatePawnMoves(moves, bitboard, color == PieceColor::White ? PieceType::WhitePawn : PieceType::BlackPawn);
        generateKnightMoves(moves, bitboard, color == PieceColor::White ? PieceType::WhiteKnight : PieceType::BlackKnight);
        generateBishopMoves(moves, bitboard, color == PieceColor::White ? PieceType::WhiteBishop : PieceType::BlackBishop);
        generateRookMoves(moves, bitboard, color == PieceColor::White ? PieceType::WhiteRook : PieceType::BlackRook);
        generateQueenMoves(moves, bitboard, color == PieceColor::White ? PieceType::WhiteQueen : PieceType::BlackQueen);
        generateKingMoves(moves, bitboard, color == PieceColor::White ? PieceType::WhiteKing : PieceType::BlackKing);

        return moves;
    }

    void generatePawnMoves(std::vector<Move> &result, Bitboard bitboard, PieceType pieceType) {
        uint64_t pawnBB = bitboard.getPieceBoard(pieceType);

        while (pawnBB) {
            uint64_t index = Chess::bitscanForward(pawnBB);
            uint64_t genBB;

            if (pieceType == PieceType::WhitePawn) {
                genBB = bitboard.getWhitePawnDoublePush(pawnBB);
            } else {
                genBB = bitboard.getBlackPawnDoublePush(pawnBB);
            }

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << index);
            }

            pawnBB &= ~(1ULL << index);
        }
    }

    void generateKnightMoves(std::vector<Move> &result, Bitboard bitboard, PieceType pieceType) {
        uint64_t knightBB = bitboard.getPieceBoard(pieceType);

        while (knightBB) {
            uint64_t index = Chess::bitscanForward(knightBB);
            uint64_t genBB;

            genBB = bitboard.getKnightAttacks(index);

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << index);
            }

            knightBB &= ~(1ULL << index);
        }
    }

    void generateBishopMoves(std::vector<Move> &result, Bitboard bitboard, PieceType pieceType) {
        uint64_t bishopBB = bitboard.getPieceBoard(pieceType);

        while (bishopBB) {
            uint64_t index = Chess::bitscanForward(bishopBB);
            uint64_t genBB;

            genBB = bitboard.getBishopAttacks(1 << index);

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << index);
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void generateRookMoves(std::vector<Move> &result, Bitboard bitboard, PieceType pieceType) {
        uint64_t rookBB = bitboard.getPieceBoard(pieceType);

        while (rookBB) {
            uint64_t index = Chess::bitscanForward(rookBB);
            uint64_t genBB;

            genBB = bitboard.getRookAttacks(1 << index);

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << index);
            }

            rookBB &= ~(1ULL << index);
        }
    }

    void generateQueenMoves(std::vector<Move> &result, Bitboard bitboard, PieceType pieceType) {
        uint64_t queenBB = bitboard.getPieceBoard(pieceType);

        while (queenBB) {
            uint64_t index = Chess::bitscanForward(queenBB);
            uint64_t genBB;

            genBB = bitboard.getQueenAttacks(1 << index);

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << index);
            }

            queenBB &= ~(1ULL << index);
        }
    }

    void generateKingMoves(std::vector<Move> &result, Bitboard bitboard, PieceType pieceType) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType);
        uint64_t index = Chess::bitscanForward(kingBB);
        uint64_t genBB;

        genBB = bitboard.getKingAttacks(index);

        while (genBB > 0) {
            uint64_t genIndex = Chess::bitscanForward(genBB);

            result.push_back({index, genIndex, pieceType});
            genBB &= ~(1ULL << index);
        }
    }
}