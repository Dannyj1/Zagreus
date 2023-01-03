//
// Created by Danny on 14-6-2022.
//

#include <iostream>
#include <vector>
#include <cassert>

#include "bitboard.h"
#include "move_gen.h"
#include "tt.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    std::vector<Move> generateLegalMoves(Bitboard &bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(50);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        generatePawnMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN);
        generateKnightMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT);
        generateBishopMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP);
        generateRookMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK);
        generateQueenMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);
        generateKingMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING);

        std::sort(std::begin(moves), std::end(moves), sortLegalMoves);

        return moves;
    }

    bool sortLegalMoves(const Move &a, const Move &b) {
        int aScore = 0;
        int bScore = 0;

        assert(a.fromSquare != a.toSquare);
        assert(b.fromSquare != b.toSquare);

        if (tt.isPVMove(a)) {
            aScore += 50000 + tt.getPositionScore(a.zobristHash);
        } else if (tt.isPositionInTable(a.zobristHash)) {
            aScore += 25000 + tt.getPositionScore(a.zobristHash);
        /*} else if (aScore > 0) {
            // Good capture
            bScore += 10000;*/
        } else if (tt.isKillerMove(a)) {
            aScore += 5000;
        }

        if (tt.isPVMove(b)) {
            bScore += 50000 + tt.getPositionScore(b.zobristHash);
        } else if (tt.isPositionInTable(a.zobristHash)) {
            bScore += 25000 + tt.getPositionScore(b.zobristHash);
            /*} else if (bScore > 0) {
                // Good capture
                bScore += 10000;*/
        } else if (tt.isKillerMove(b)) {
            bScore += 5000;
        }

        return aScore > bScore;
    }

    std::vector<Move> generateQuiescenceMoves(Bitboard &bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(50);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        generatePawnMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN, true);
        generateKnightMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT, true);
        generateBishopMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP, true);
        generateRookMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK, true);
        generateQueenMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN, true);
        generateKingMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING, true);

        return moves;
    }

    std::vector<Move> generateMobilityMoves(Bitboard &bitboard, PieceColor color) {
        std::vector<Move> moves;
        moves.reserve(50);

        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        generateKnightMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT);
        generateBishopMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP);
        generateRookMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK);
        generateQueenMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);

        return moves;
    }

    void generateAttackMapMoves(std::vector<Move> &moves, Bitboard &bitboard, PieceColor color, uint64_t mask) {
        uint64_t ownPiecesBB = bitboard.getBoardByColor(color);
        uint64_t opponentPiecesBB = bitboard.getBoardByColor(Bitboard::getOppositeColor(color));

        generatePawnMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN, false, mask);
        generateKnightMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT, false,
                            mask);
        generateBishopMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                            color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP, false,
                            mask);
        generateRookMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK, false, mask);
        generateQueenMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                           color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN, false, mask);
        generateKingMoves(moves, bitboard, ownPiecesBB, opponentPiecesBB, color,
                          color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING, false, mask);
    }

    void generatePawnMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t pawnBB = bitboard.getPieceBoard(pieceType) & mask;

        while (pawnBB) {
            uint64_t index = Zagreus::bitscanForward(pawnBB);
            uint64_t genBB;

            if (pieceType == PieceType::WHITE_PAWN) {
                genBB = bitboard.getWhitePawnDoublePush(1ULL << index);
            } else {
                genBB = bitboard.getBlackPawnDoublePush(1ULL << index);
            }

            uint64_t attackBB = bitboard.getPawnAttacks(index, color);
            if (mask == ~0ULL) {
                if (bitboard.getEnPassantSquare(Bitboard::getOppositeColor(color)) >= 0) {
                    attackBB &= opponentPiecesBB | (1ULL << bitboard.getEnPassantSquare(Bitboard::getOppositeColor(color)));
                } else {
                    attackBB &= opponentPiecesBB;
                }

                attackBB &= ~ownPiecesBB;
                genBB &= ~ownPiecesBB;
            }

            genBB |= attackBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB) {
                uint64_t genIndex = Zagreus::bitscanForward(genBB);

                if (mask == ~0ULL) {
                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    if (genIndex >= 56 || genIndex <= 7) {
                        moves.push_back({index, genIndex, pieceType, bitboard.zobristHash,
                                         static_cast<PieceType>(PieceType::WHITE_QUEEN + color)});
                        moves.push_back({index, genIndex, pieceType, bitboard.zobristHash,
                                         static_cast<PieceType>(PieceType::WHITE_ROOK + color)});
                        moves.push_back({index, genIndex, pieceType, bitboard.zobristHash,
                                         static_cast<PieceType>(PieceType::WHITE_BISHOP + color)});
                        moves.push_back({index, genIndex, pieceType, bitboard.zobristHash,
                                         static_cast<PieceType>(PieceType::WHITE_KNIGHT + color)});
                    } else {
                        moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                    }

                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                }

                genBB &= ~(1ULL << genIndex);
            }

            pawnBB &= ~(1ULL << index);
        }
    }

    void generateKnightMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t knightBB = bitboard.getPieceBoard(pieceType) & mask;

        while (knightBB) {
            uint64_t index = Zagreus::bitscanForward(knightBB);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB) {
                uint64_t genIndex = Zagreus::bitscanForward(genBB);

                if (mask == ~0ULL) {
                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                }

                genBB &= ~(1ULL << genIndex);
            }

            knightBB &= ~(1ULL << index);
        }
    }

    void generateBishopMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t bishopBB = bitboard.getPieceBoard(pieceType) & mask;

        while (bishopBB) {
            uint64_t index = Zagreus::bitscanForward(bishopBB);
            uint64_t genBB = bitboard.getBishopAttacks(1ULL << index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB) {
                uint64_t genIndex = Zagreus::bitscanForward(genBB);

                if (mask == ~0ULL) {
                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                }

                genBB &= ~(1ULL << genIndex);
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void generateRookMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t rookBB = bitboard.getPieceBoard(pieceType) & mask;
        uint8_t castlingRights = bitboard.getCastlingRights();
        uint64_t occupiedBB = bitboard.getOccupiedBoard();

        while (rookBB) {
            uint64_t index = Zagreus::bitscanForward(rookBB);

            // TODO: refactor, is very ugly
            if (!quiesce && mask == ~0ULL) {
                if (color == PieceColor::WHITE) {
                    if (castlingRights & CastlingRights::WHITE_KINGSIDE && index == Square::H1) {
                        uint64_t tilesBetween = (1ULL << Square::F1) | (1ULL << Square::G1);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H1, Square::E1, PieceType::WHITE_ROOK, bitboard.zobristHash});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::WHITE_QUEENSIDE && index == Square::A1) {
                        uint64_t tilesBetween = (1ULL << Square::C1) | (1ULL << Square::D1);

                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B1)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A1, Square::E1, PieceType::WHITE_ROOK, bitboard.zobristHash});
                            }
                        }
                    }
                } else {
                    if (castlingRights & CastlingRights::BLACK_KINGSIDE && index == Square::H8) {
                        uint64_t tilesBetween = (1ULL << Square::F8) | (1ULL << Square::G8);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H8, Square::E8, PieceType::BLACK_ROOK, bitboard.zobristHash});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::BLACK_QUEENSIDE && index == Square::A8) {
                        uint64_t tilesBetween = (1ULL << Square::C8) | (1ULL << Square::D8);


                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B8)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A8, Square::E8, PieceType::BLACK_ROOK, bitboard.zobristHash});
                            }
                        }
                    }
                }
            }

            uint64_t genBB = bitboard.getRookAttacks(1ULL << index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB) {
                uint64_t genIndex = Zagreus::bitscanForward(genBB);

                if (mask == ~0ULL) {
                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                }
                genBB &= ~(1ULL << genIndex);
            }

            rookBB &= ~(1ULL << index);
        }
    }

    void generateQueenMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                            PieceColor color, PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t queenBB = bitboard.getPieceBoard(pieceType) & mask;

        while (queenBB) {
            uint64_t index = Zagreus::bitscanForward(queenBB);
            uint64_t genBB = bitboard.getQueenAttacks(1ULL << index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            while (genBB) {
                uint64_t genIndex = Zagreus::bitscanForward(genBB);

                if (mask == ~0ULL) {
                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                }

                genBB &= ~(1ULL << genIndex);
            }

            queenBB &= ~(1ULL << index);
        }
    }

    void generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color, PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType) & mask;
        uint64_t index = Zagreus::bitscanForward(kingBB);
        uint64_t genBB = bitboard.getKingAttacks(index);

        if (mask == ~0ULL) {
            genBB &= ~ownPiecesBB;
        } else {
            genBB &= ~(1ULL << index);
        }

        if (quiesce) {
            genBB &= opponentPiecesBB;
        }

        while (genBB) {
            uint64_t genIndex = Zagreus::bitscanForward(genBB);

            if (mask == ~0ULL) {
                bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                if (bitboard.isKingInCheck(color)) {
                    bitboard.unmakeMove();
                    genBB &= ~(1ULL << genIndex);
                    continue;
                }

                moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
                bitboard.unmakeMove();
            } else {
                moves.push_back({index, genIndex, pieceType, bitboard.zobristHash});
            }

            genBB &= ~(1ULL << genIndex);
        }
    }
}
#pragma clang diagnostic pop