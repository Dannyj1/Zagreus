//
// Created by Danny on 14-6-2022.
//

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

        std::sort(std::begin(moves), std::end(moves), sortMoves);

        return moves;
    }

    bool sortMoves(Move &a, Move &b) {
        // TODO: implement proper killer heuristic
        int aScore = 0;
        int bScore = 0;
        uint32_t aMoveCode = encodeMove(a);
        uint32_t bMoveCode = encodeMove(b);
        TTEntry aEntry = tt.getPosition(a.zobristHash);
        TTEntry bEntry = tt.getPosition(b.zobristHash);

        assert(a.fromSquare != a.toSquare);
        assert(b.fromSquare != b.toSquare);

        if (tt.isPVMove(aMoveCode)) {
            aScore = 50000;
        } else if (aEntry.zobristHash == a.zobristHash) {
            aScore = 25000 + aEntry.score;
        } else if (a.captureScore > 0) {
            // Good capture
            aScore = 10000 + a.captureScore;
        } else if (tt.killerMoves[0][a.ply] == aMoveCode) {
            aScore = 5000;
        } else if (tt.killerMoves[1][a.ply] == aMoveCode) {
            aScore = 4000;
        } else if (tt.killerMoves[2][a.ply] == aMoveCode) {
            aScore = 3000;
        } else {
            assert(a.fromSquare >= 0);
            assert(a.toSquare >= 0);
            aScore = tt.historyMoves[a.pieceType][a.toSquare];
        }

        if (tt.isPVMove(b.zobristHash)) {
            bScore = 50000;
        } else if (bEntry.zobristHash == b.zobristHash) {
            bScore = 25000 + bEntry.score;
        } else if (b.captureScore > 0) {
            // Good capture
            bScore = 10000 + b.captureScore;
        } else if (tt.killerMoves[0][b.ply] == bMoveCode) {
            bScore = 5000;
        } else if (tt.killerMoves[1][b.ply] == bMoveCode) {
            bScore = 4000;
        } else if (tt.killerMoves[2][b.ply] == bMoveCode) {
            bScore = 3000;
        } else {
            assert(b.fromSquare >= 0);
            assert(b.toSquare >= 0);
            bScore = tt.historyMoves[b.pieceType][b.toSquare];
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

        std::sort(std::begin(moves), std::end(moves), sortMoves);
        return moves;
    }

    uint64_t generateMobilityMoves(Bitboard &bitboard, PieceColor color) {
        uint64_t results = 0ULL;
        uint64_t knightBB = bitboard.getPieceBoard(color == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT);
        uint64_t bishopBB = bitboard.getPieceBoard(color == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP);
        uint64_t rookBB = bitboard.getPieceBoard(color == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK);
        uint64_t queenBB = bitboard.getPieceBoard(color == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);

        results |= calculateKnightAttacks(knightBB);
        results |= bitboard.getBishopAttacks(bishopBB);
        results |= bitboard.getRookAttacks(rookBB);
        results |= bitboard.getQueenAttacks(queenBB);

        return results;
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
            int index = Zagreus::bitscanForward(pawnBB);
            assert(index >= 0 && index < 64);
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

            if (mask == ~0ULL) {
                genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                           bitboard.getPieceBoard(PieceType::BLACK_KING));
            }

            while (genBB) {
                int genIndex = Zagreus::bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                if (mask == ~0ULL) {
                    int captureScore = 0;

                    if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.mvvlva(genIndex, color);
                    }

                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    if (genIndex >= 56 || genIndex <= 7) {
                        moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore,
                                         static_cast<PieceType>(PieceType::WHITE_QUEEN + color)});
                        moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore,
                                         static_cast<PieceType>(PieceType::WHITE_ROOK + color)});
                        moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore,
                                         static_cast<PieceType>(PieceType::WHITE_BISHOP + color)});
                        moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore,
                                         static_cast<PieceType>(PieceType::WHITE_KNIGHT + color)});
                    } else {
                        moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore});
                    }

                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), 0});
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
            int index = Zagreus::bitscanForward(knightBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getKnightAttacks(index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            if (mask == ~0ULL) {
                genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                           bitboard.getPieceBoard(PieceType::BLACK_KING));
            }

            while (genBB) {
                int genIndex = Zagreus::bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                if (mask == ~0ULL) {
                    int captureScore = 0;

                    if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.mvvlva(genIndex, color);
                    }

                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), 0});
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
            int index = Zagreus::bitscanForward(bishopBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getBishopMoves(index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            if (mask == ~0ULL) {
                genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                           bitboard.getPieceBoard(PieceType::BLACK_KING));
            }

            while (genBB) {
                int genIndex = Zagreus::bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                if (mask == ~0ULL) {
                    int captureScore = 0;

                    if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.mvvlva(genIndex, color);
                    }

                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), 0});
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
            int index = Zagreus::bitscanForward(rookBB);
            assert(index >= 0 && index < 64);

            // TODO: refactor, is very ugly
            if (!quiesce && mask == ~0ULL) {
                if (color == PieceColor::WHITE) {
                    if (castlingRights & CastlingRights::WHITE_KINGSIDE && index == Square::H1) {
                        uint64_t tilesBetween = (1ULL << Square::F1) | (1ULL << Square::G1);

                        if (!(occupiedBB & tilesBetween)) {
                            if (!bitboard.isSquareAttackedByColor(Square::F1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::G1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::H1, Square::E1, PieceType::WHITE_ROOK, bitboard.getZobristHash(), 0});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::WHITE_QUEENSIDE && index == Square::A1) {
                        uint64_t tilesBetween = (1ULL << Square::C1) | (1ULL << Square::D1);

                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B1)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D1, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E1, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A1, Square::E1, PieceType::WHITE_ROOK, bitboard.getZobristHash(), 0});
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
                                moves.push_back({Square::H8, Square::E8, PieceType::BLACK_ROOK, bitboard.getZobristHash(), 0});
                            }
                        }
                    }

                    if (castlingRights & CastlingRights::BLACK_QUEENSIDE && index == Square::A8) {
                        uint64_t tilesBetween = (1ULL << Square::C8) | (1ULL << Square::D8);


                        if (!(occupiedBB & (tilesBetween | (1ULL << Square::B8)))) {
                            if (!bitboard.isSquareAttackedByColor(Square::C8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::D8, Bitboard::getOppositeColor(color))
                                && !bitboard.isSquareAttackedByColor(Square::E8, Bitboard::getOppositeColor(color))) {
                                moves.push_back({Square::A8, Square::E8, PieceType::BLACK_ROOK, bitboard.getZobristHash(), 0});
                            }
                        }
                    }
                }
            }

            uint64_t genBB = bitboard.getRookMoves(index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            if (mask == ~0ULL) {
                genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                           bitboard.getPieceBoard(PieceType::BLACK_KING));
            }

            while (genBB) {
                int genIndex = Zagreus::bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                if (mask == ~0ULL) {
                    int captureScore = 0;

                    if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.mvvlva(genIndex, color);
                    }

                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), 0});
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
            int index = Zagreus::bitscanForward(queenBB);
            assert(index >= 0 && index < 64);
            uint64_t genBB = bitboard.getQueenMoves(index);

            if (mask == ~0ULL) {
                genBB &= ~ownPiecesBB;
            } else {
                genBB &= ~(1ULL << index);
            }

            if (quiesce) {
                genBB &= opponentPiecesBB;
            }

            if (mask == ~0ULL) {
                genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) |
                           bitboard.getPieceBoard(PieceType::BLACK_KING));
            }

            while (genBB) {
                int genIndex = Zagreus::bitscanForward(genBB);
                assert(genIndex >= 0 && genIndex < 64);

                if (mask == ~0ULL) {
                    int captureScore = 0;

                    if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                        captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.mvvlva(genIndex, color);
                    }

                    bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                    if (bitboard.isKingInCheck(color)) {
                        bitboard.unmakeMove();
                        genBB &= ~(1ULL << genIndex);
                        continue;
                    }

                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore});
                    bitboard.unmakeMove();
                } else {
                    moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), 0});
                }

                genBB &= ~(1ULL << genIndex);
            }

            queenBB &= ~(1ULL << index);
        }
    }

    void generateKingMoves(std::vector<Move> &moves, Bitboard &bitboard, uint64_t ownPiecesBB, uint64_t opponentPiecesBB,
                           PieceColor color, PieceType pieceType, bool quiesce, uint64_t mask) {
        uint64_t kingBB = bitboard.getPieceBoard(pieceType) & mask;
        uint64_t opponentKingBB = bitboard.getPieceBoard(color == PieceColor::WHITE ? PieceType::BLACK_KING : PieceType::WHITE_KING);
        int index = Zagreus::bitscanForward(kingBB);
        assert(index >= 0 && index < 64);
        uint64_t genBB = bitboard.getKingAttacks(index);
        uint64_t opponentKingAttacks = bitboard.getKingAttacks(Zagreus::bitscanForward(opponentKingBB));

        if (mask == ~0ULL) {
            genBB &= ~ownPiecesBB;
        } else {
            genBB &= ~(1ULL << index);
        }

        if (quiesce) {
            genBB &= opponentPiecesBB;
        }

        genBB &= ~(bitboard.getPieceBoard(PieceType::WHITE_KING) | bitboard.getPieceBoard(PieceType::BLACK_KING) | opponentKingAttacks);

        while (genBB) {
            int genIndex = Zagreus::bitscanForward(genBB);
            assert(genIndex >= 0 && genIndex < 64);

            if (mask == ~0ULL) {
                int captureScore = 0;

                if (bitboard.getPieceOnSquare(genIndex) != PieceType::EMPTY) {
                    captureScore = quiesce ? bitboard.seeCapture(index, genIndex, color) : bitboard.mvvlva(genIndex, color);
                }

                bitboard.makeMove(index, genIndex, pieceType, PieceType::EMPTY);

                if (bitboard.isKingInCheck(color)) {
                    bitboard.unmakeMove();
                    genBB &= ~(1ULL << genIndex);
                    continue;
                }

                moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), captureScore});
                bitboard.unmakeMove();
            } else {
                moves.push_back({index, genIndex, pieceType, bitboard.getZobristHash(), bitboard.getPly(), 0});
            }

            genBB &= ~(1ULL << genIndex);
        }
    }
}
#pragma clang diagnostic pop