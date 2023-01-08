//
// Created by Danny on 14-6-2022.
//

#include <vector>
#include <cassert>
#include <immintrin.h>

#include "bitboard.h"
#include "move_gen.h"
#include "tt.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"

// TODO: apply _blsr_u64 everywhere
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

        std::sort(moves.begin(), moves.end(), [&bitboard](Move &a, Move &b) {
            return sortMoves(bitboard, a, b);
        });


        return moves;
    }

    bool sortMoves(Bitboard &bitboard, Move &a, Move &b) {
        // TODO: implement countermoves heuristic. Need to have previousMoveFrom available in this function.
        int aScore = tt.historyMoves[a.pieceType][a.toSquare];
        int bScore = tt.historyMoves[b.pieceType][b.toSquare];
        TTEntry* aEntry = tt.getPosition(a.zobristHash);
        TTEntry* bEntry = tt.getPosition(b.zobristHash);

        assert(a.fromSquare != a.toSquare);
        assert(b.fromSquare != b.toSquare);

        if (aEntry->zobristHash == a.zobristHash) {
            if (aEntry->isPVMove) {
                aScore += 50000 + aEntry->score;
            } else {
                aScore += 25000 + aEntry->score;
            }
        } else if (a.captureScore >= 0) {
            aScore += 10000 + a.captureScore;
        } else {
            uint32_t aMoveCode = encodeMove(a);

            if (tt.killerMoves[0][a.ply] == aMoveCode) {
                aScore += 5000;
            } else if (tt.killerMoves[1][a.ply] == aMoveCode) {
                aScore += 4000;
            } else if (tt.counterMoves[bitboard.getPreviousMoveFrom()][bitboard.getPreviousMoveTo()] == aMoveCode) {
                aScore += 3000;
            } else if (a.captureScore < -1) {
                aScore += a.captureScore - 5000;
            }
        }

        if (bEntry->zobristHash == b.zobristHash) {
            if (bEntry->isPVMove) {
                bScore += 50000 + bEntry->score;
            } else {
                bScore += 25000 + bEntry->score;
            }
        } else if (b.captureScore >= 0) {
            bScore += 10000 + b.captureScore;
        } else {
            uint32_t bMoveCode = encodeMove(b);

            if (tt.killerMoves[0][b.ply] == bMoveCode) {
                bScore += 5000;
            } else if (tt.killerMoves[1][b.ply] == bMoveCode) {
                bScore += 4000;
            } else if (tt.counterMoves[bitboard.getPreviousMoveFrom()][bitboard.getPreviousMoveTo()] == bMoveCode) {
                bScore += 3000;
            } else if (b.captureScore < -1) {
                bScore += b.captureScore - 5000;
            }
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

        std::sort(moves.begin(), moves.end(), [&bitboard](Move &a, Move &b) {
            return sortMoves(bitboard, a, b);
        });

        return moves;
    }

    void generatePawnMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce) {
        uint64_t pawnBB = bitboard.getPieceBoard(pieceType);

        while (pawnBB) {
            uint8_t index = bitscanForward(pawnBB);
            pawnBB = _blsr_u64(pawnBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB;

            if (pieceType == PieceType::WHITE_PAWN) {
                genBB = bitboard.getWhitePawnDoublePush(1ULL << index);
            } else {
                genBB = bitboard.getBlackPawnDoublePush(1ULL << index);
            }

            uint64_t attackBB = bitboard.getPawnAttacks(index, color);

            if (bitboard.getEnPassantSquare(Bitboard::getOppositeColor(color)) >= 0) {
                attackBB &= opponentPiecesBB | (1ULL << bitboard.getEnPassantSquare(Bitboard::getOppositeColor(color)));
            } else {
                attackBB &= opponentPiecesBB;
            }

            attackBB &= ~ownPiecesBB;
            genBB &= ~ownPiecesBB;
            genBB |= attackBB;

            if (quiesce) {
                genBB &= (opponentPiecesBB | RANK_8 | RANK_1);
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = 0;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.getPieceWeight(bitboard.getPieceOnSquare(genIndex)) - bitboard.getPieceWeight(pieceType);
                }

                if (genIndex >= 56 || genIndex <= 7) {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_QUEEN + color)});
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_ROOK + color)});
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_BISHOP + color)});
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                     captureScore,
                                     static_cast<PieceType>(PieceType::WHITE_KNIGHT + color)});
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                     captureScore});
                }

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void generateKnightMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t knightBB = bitboard.getPieceBoard(pieceType);

        while (knightBB) {
            uint8_t index = bitscanForward(knightBB);
            knightBB = _blsr_u64(knightBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = 0;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.getPieceWeight(bitboard.getPieceOnSquare(genIndex)) - bitboard.getPieceWeight(pieceType);
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void generateBishopMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                             PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t bishopBB = bitboard.getPieceBoard(pieceType);

        while (bishopBB) {
            uint8_t index = bitscanForward(bishopBB);
            bishopBB = _blsr_u64(bishopBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getBishopAttacks(index, bitboard.getOccupiedBoard());

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = 0;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.getPieceWeight(bitboard.getPieceOnSquare(genIndex)) - bitboard.getPieceWeight(pieceType);
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void generateRookMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color,
                           PieceType pieceType, bool quiesce) {
        uint64_t rookBB = bitboard.getPieceBoard(pieceType);
        uint8_t castlingRights = bitboard.getCastlingRights();
        uint64_t occupiedBB = bitboard.getOccupiedBoard();

        while (rookBB) {
            uint8_t index = bitscanForward(rookBB);
            rookBB = _blsr_u64(rookBB);
            assert(index >= 0 && index < 64);

            // TODO: refactor, is very ugly
            if (!quiesce && !bitboard.isKingInCheck(color)) {
                if (color == PieceColor::WHITE) {
                    if (castlingRights & CastlingRights::WHITE_KINGSIDE && index == Square::H1) {
                        uint64_t tilesBetween = (1ULL << Square::F1) | (1ULL << Square::G1);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H1, Square::E1, PieceType::WHITE_ROOK,
                                                 bitboard.getZobristHash(), 0, 0});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::WHITE_QUEENSIDE && index == Square::A1) {
                        uint64_t tilesBetween = (1ULL << Square::C1) | (1ULL << Square::D1);

                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B1)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A1, Square::E1, PieceType::WHITE_ROOK,
                                                 bitboard.getZobristHash(), 0, 0});
                            }
                        }
                    }
                } else {
                    if (castlingRights & CastlingRights::BLACK_KINGSIDE && index == Square::H8) {
                        uint64_t tilesBetween = (1ULL << Square::F8) | (1ULL << Square::G8);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H8, Square::E8, PieceType::BLACK_ROOK,
                                                 bitboard.getZobristHash(), 0, 0});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::BLACK_QUEENSIDE && index == Square::A8) {
                        uint64_t tilesBetween = (1ULL << Square::C8) | (1ULL << Square::D8);


                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B8)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A8, Square::E8, PieceType::BLACK_ROOK,
                                                 bitboard.getZobristHash(), 0, 0});
                            }
                        }
                    }
                }
            }

            uint64_t genBB = bitboard.getRookAttacks(index, bitboard.getOccupiedBoard());;

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = 0;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.getPieceWeight(bitboard.getPieceOnSquare(genIndex)) - bitboard.getPieceWeight(pieceType);
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void generateQueenMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                            PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t queenBB = bitboard.getPieceBoard(pieceType);

        while (queenBB) {
            uint8_t index = bitscanForward(queenBB);
            queenBB = _blsr_u64(queenBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getQueenAttacks(index, bitboard.getOccupiedBoard());

            genBB &= ~ownPiecesBB;

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                       bitboard.getPieceBoard(PieceType::BLACK_KING));

            while (genBB) {
                uint8_t genIndex = bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                int captureScore = 0;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.getPieceWeight(bitboard.getPieceOnSquare(genIndex)) - bitboard.getPieceWeight(pieceType);
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                                 captureScore});

                genBB &= ~(1ULL << genIndex);
            }
        }
    }

    void generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color, PieceType pieceType, bool quiesce) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType);
        uint64_t opponentKingBB = bitboard.getPieceBoard(
                color == PieceColor::WHITE ? PieceType::BLACK_KING : PieceType::WHITE_KING);
        uint8_t index = bitscanForward(kingBB);
        assert(index >= 0 && index < 64);
        uint64_t genBB = bitboard.getKingAttacks(index);
        uint64_t opponentKingAttacks = bitboard.getKingAttacks(bitscanForward(opponentKingBB));

        genBB &= ~ownPiecesBB;

        if (quiesce) {
            genBB &= opponentPiecesBB;
        }

        genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) | bitboard.getPieceBoard(PieceType::BLACK_KING) |
                   opponentKingAttacks);

        while (genBB) {
            uint8_t genIndex = bitscanForward(genBB);
            assert(genIndex >= 0 && genIndex < 64);

            int captureScore = 0;

            if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.getPieceWeight(bitboard.getPieceOnSquare(genIndex)) - bitboard.getPieceWeight(pieceType);
            }

            moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(),
                             captureScore});

            genBB &= ~(1ULL << genIndex);
        }
    }
}
#pragma clang diagnostic pop