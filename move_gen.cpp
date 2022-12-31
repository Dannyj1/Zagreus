//
// Created by Danny on 14-6-2022.
//

#include <iostream>

#include "bitboard.h"
#include "move_gen.h"

namespace Chess {
    std::vector<Move> generatePseudoLegalMoves(Bitboard bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(1000);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);

        generatePawnMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN, false);
        generateKnightMoves(moves, bitboard, ownPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT, false);
        generateBishopMoves(moves, bitboard, ownPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP, false);
        generateRookMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK, false);
        generateQueenMoves(moves, bitboard, ownPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN, false);
        generateKingMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING, false);

        return moves;
    }

    std::vector<Move> generateQuiescenceMoves(Bitboard bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(1000);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);

        generatePawnMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN, true);
        generateKnightMoves(moves, bitboard, ownPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT, true);
        generateBishopMoves(moves, bitboard, ownPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP, true);
        generateRookMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK, true);
        generateQueenMoves(moves, bitboard, ownPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN, true);
        generateKingMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING, true);

        return moves;
    }

    std::vector<Move> generateMobilityMoves(Bitboard bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(1000);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);

        generateKnightMoves(moves, bitboard, ownPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT, false);
        generateBishopMoves(moves, bitboard, ownPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP, false);
        generateRookMoves(moves, bitboard, ownPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK, false);
        generateQueenMoves(moves, bitboard, ownPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN, false);

        return moves;
    }

    // TODO: generate en passant
    void generatePawnMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color,
                           PieceType pieceType, bool quiesce) {
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

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                if (pieceType == PieceType::WHITE_PAWN && genIndex >= 56) {
                    result.push_back({index, genIndex, pieceType, PieceType::WHITE_QUEEN});
                    result.push_back({index, genIndex, pieceType, PieceType::WHITE_ROOK});
                    result.push_back({index, genIndex, pieceType, PieceType::WHITE_BISHOP});
                    result.push_back({index, genIndex, pieceType, PieceType::WHITE_KNIGHT});
                } else if (pieceType == PieceType::BLACK_PAWN && genIndex <= 7) {
                    result.push_back({index, genIndex, pieceType, PieceType::BLACK_QUEEN});
                    result.push_back({index, genIndex, pieceType, PieceType::BLACK_ROOK});
                    result.push_back({index, genIndex, pieceType, PieceType::BLACK_BISHOP});
                    result.push_back({index, genIndex, pieceType, PieceType::BLACK_KNIGHT});
                } else {
                    result.push_back({index, genIndex, pieceType});
                }

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

            if (quiesce) {
                attackBB &= opponentPiecesBB;
            }

            while (attackBB > 0) {
                uint64_t attackIndex = Chess::bitscanForward(attackBB);

                if (pieceType == PieceType::WHITE_PAWN && attackIndex >= 56) {
                    result.push_back({index, attackIndex, pieceType, PieceType::WHITE_QUEEN});
                    result.push_back({index, attackIndex, pieceType, PieceType::WHITE_ROOK});
                    result.push_back({index, attackIndex, pieceType, PieceType::WHITE_BISHOP});
                    result.push_back({index, attackIndex, pieceType, PieceType::WHITE_KNIGHT});
                } else if (pieceType == PieceType::BLACK_PAWN && attackIndex <= 7) {
                    result.push_back({index, attackIndex, pieceType, PieceType::BLACK_QUEEN});
                    result.push_back({index, attackIndex, pieceType, PieceType::BLACK_ROOK});
                    result.push_back({index, attackIndex, pieceType, PieceType::BLACK_BISHOP});
                    result.push_back({index, attackIndex, pieceType, PieceType::BLACK_KNIGHT});
                } else {
                    result.push_back({index, attackIndex, pieceType});
                }

                attackBB &= ~(1ULL << attackIndex);
            }

            pawnBB &= ~(1ULL << index);
        }
    }

    void generateKnightMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t knightBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        while (knightBB) {
            uint64_t index = Chess::bitscanForward(knightBB);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            knightBB &= ~(1ULL << index);
        }
    }

    void generateBishopMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t bishopBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        while (bishopBB) {
            uint64_t index = Chess::bitscanForward(bishopBB);
            uint64_t genBB = bitboard.getBishopAttacks(1ULL << index);

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void generateRookMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color,
                           PieceType pieceType, bool quiesce) {
        uint64_t rookBB = bitboard.getPieceBoard(pieceType);
        uint8_t castlingRights = bitboard.getCastlingRights();
        uint64_t occupiedBB = bitboard.getOccupiedBoard();
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        while (rookBB) {
            uint64_t index = Chess::bitscanForward(rookBB);

            if (!quiesce) {
                // TODO: refactor, is very ugly
                if (color == PieceColor::WHITE) {
                    if (castlingRights & CastlingRights::WHITE_KINGSIDE && index == Square::H1) {
                        uint64_t tilesBetween = (1ULL << Square::F1) | (1ULL << Square::G1);

                        if (!(occupiedBB & tilesBetween)) {
                            uint64_t opponentAttacks = bitboard.getAttackedTilesForColor(
                                    Bitboard::getOppositeColor(color));

                            if (!(opponentAttacks & (tilesBetween | (1ULL << Square::E1)))) {
                                result.push_back({Square::H1, Square::E1, PieceType::WHITE_ROOK});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::WHITE_QUEENSIDE && index == Square::A1) {
                        uint64_t tilesBetween = (1ULL << Square::C1) | (1ULL << Square::D1);

                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B1)))) {
                            uint64_t opponentAttacks = bitboard.getAttackedTilesForColor(
                                    Bitboard::getOppositeColor(color));

                            if (!(opponentAttacks & (tilesBetween | (1ULL << Square::E1)))) {
                                result.push_back({Square::A1, Square::E1, PieceType::WHITE_ROOK});
                            }
                        }
                    }
                } else {
                    if (castlingRights & CastlingRights::BLACK_KINGSIDE && index == Square::H8) {
                        uint64_t tilesBetween = (1ULL << Square::F8) | (1ULL << Square::G8);

                        if (!(occupiedBB & tilesBetween)) {
                            uint64_t opponentAttacks = bitboard.getAttackedTilesForColor(
                                    Bitboard::getOppositeColor(color));

                            if (!(opponentAttacks & (tilesBetween | (1ULL << Square::E8)))) {
                                result.push_back({Square::H8, Square::E8, PieceType::BLACK_ROOK});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::BLACK_QUEENSIDE && index == Square::A8) {
                        uint64_t tilesBetween = (1ULL << Square::C8) | (1ULL << Square::D8);


                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B8)))) {
                            uint64_t opponentAttacks = bitboard.getAttackedTilesForColor(
                                    Bitboard::getOppositeColor(color));

                            if (!(opponentAttacks & (tilesBetween | (1ULL << Square::E8)))) {
                                result.push_back({Square::A8, Square::E8, PieceType::BLACK_ROOK});
                            }
                        }
                    }
                }
            }

            uint64_t genBB = bitboard.getRookAttacks(1ULL << index);
            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            rookBB &= ~(1ULL << index);
        }
    }

    void generateQueenMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t queenBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        while (queenBB) {
            uint64_t index = Chess::bitscanForward(queenBB);
            uint64_t genBB = bitboard.getQueenAttacks(1ULL << index);

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB > 0) {
                uint64_t genIndex = Chess::bitscanForward(genBB);

                result.push_back({index, genIndex, pieceType});
                genBB &= ~(1ULL << genIndex);
            }

            queenBB &= ~(1ULL << index);
        }
    }

    void generateKingMoves(std::vector<Move> &result, Bitboard bitboard, uint64_t ownPiecesBB, PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType);
        uint64_t index = Chess::bitscanForward(kingBB);
        uint64_t genBB = bitboard.getKingAttacks(index);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        genBB &= ~ownPiecesBB;

        if (quiesce) {
            genBB &= opponentPiecesBB;
        }

        while (genBB > 0) {
            uint64_t genIndex = Chess::bitscanForward(genBB);

            result.push_back({index, genIndex, pieceType});
            genBB &= ~(1ULL << genIndex);
        }
    }
}