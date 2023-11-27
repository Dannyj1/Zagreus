/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "evaluate.h"
#include "features.h"

namespace Zagreus {
    Evaluation::Evaluation(Bitboard& bitboard) : bitboard(bitboard) {
        uint64_t whitePawns = bitboard.getPieceBoard(WHITE_PAWN);
        while (whitePawns) {
            uint8_t square = popLsb(whitePawns);
            uint64_t attacks = bitboard.getPawnAttacks<WHITE>(square);

            attacksByPiece[WHITE_PAWN] |= attacks;
            attacksByColor[WHITE] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t whiteKnights = bitboard.getPieceBoard(WHITE_KNIGHT);
        while (whiteKnights) {
            uint8_t square = popLsb(whiteKnights);
            uint64_t attacks = bitboard.getKnightAttacks(square);

            attacksByPiece[WHITE_KNIGHT] |= attacks;
            attacksByColor[WHITE] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t whiteBishops = bitboard.getPieceBoard(WHITE_BISHOP);
        while (whiteBishops) {
            uint8_t square = popLsb(whiteBishops);
            uint64_t attacks = bitboard.getBishopAttacks(square);

            attacksByPiece[WHITE_BISHOP] |= attacks;
            attacksByColor[WHITE] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t whiteRooks = bitboard.getPieceBoard(WHITE_ROOK);
        while (whiteRooks) {
            uint8_t square = popLsb(whiteRooks);
            uint64_t attacks = bitboard.getRookAttacks(square);

            attacksByPiece[WHITE_ROOK] |= attacks;
            attacksByColor[WHITE] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t whiteQueens = bitboard.getPieceBoard(WHITE_QUEEN);
        while (whiteQueens) {
            uint8_t square = popLsb(whiteQueens);
            uint64_t attacks = bitboard.getQueenAttacks(square);

            attacksByPiece[WHITE_QUEEN] |= attacks;
            attacksByColor[WHITE] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t whiteKing = bitboard.getPieceBoard(WHITE_KING);
        while (whiteKing) {
            uint8_t square = popLsb(whiteKing);
            uint64_t attacks = bitboard.getKingAttacks(square);

            attacksByPiece[WHITE_KING] |= attacks;
            attacksByColor[WHITE] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t blackPawns = bitboard.getPieceBoard(BLACK_PAWN);
        while (blackPawns) {
            uint8_t square = popLsb(blackPawns);
            uint64_t attacks = bitboard.getPawnAttacks<BLACK>(square);

            attacksByPiece[BLACK_PAWN] |= attacks;
            attacksByColor[BLACK] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t blackKnights = bitboard.getPieceBoard(BLACK_KNIGHT);
        while (blackKnights) {
            uint8_t square = popLsb(blackKnights);
            uint64_t attacks = bitboard.getKnightAttacks(square);

            attacksByPiece[BLACK_KNIGHT] |= attacks;
            attacksByColor[BLACK] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t blackBishops = bitboard.getPieceBoard(BLACK_BISHOP);
        while (blackBishops) {
            uint8_t square = popLsb(blackBishops);
            uint64_t attacks = bitboard.getBishopAttacks(square);

            attacksByPiece[BLACK_BISHOP] |= attacks;
            attacksByColor[BLACK] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t blackRooks = bitboard.getPieceBoard(BLACK_ROOK);
        while (blackRooks) {
            uint8_t square = popLsb(blackRooks);
            uint64_t attacks = bitboard.getRookAttacks(square);

            attacksByPiece[BLACK_ROOK] |= attacks;
            attacksByColor[BLACK] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t blackQueens = bitboard.getPieceBoard(BLACK_QUEEN);
        while (blackQueens) {
            uint8_t square = popLsb(blackQueens);
            uint64_t attacks = bitboard.getQueenAttacks(square);

            attacksByPiece[BLACK_QUEEN] |= attacks;
            attacksByColor[BLACK] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }

        uint64_t blackKing = bitboard.getPieceBoard(BLACK_KING);
        while (blackKing) {
            uint8_t square = popLsb(blackKing);
            uint64_t attacks = bitboard.getKingAttacks(square);

            attacksByPiece[BLACK_KING] |= attacks;
            attacksByColor[BLACK] |= attacks;
            combinedAttacks |= attacks;
            attacksFrom[square] |= attacks;
        }
    }

    constexpr int knightPhase = 1;
    constexpr int bishopPhase = 1;
    constexpr int rookPhase = 2;
    constexpr int queenPhase = 4;
    constexpr int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

    int Evaluation::getPhase() {
        int phase = totalPhase;

        phase -= popcnt(bitboard.getPieceBoard(WHITE_KNIGHT) | bitboard.getPieceBoard(BLACK_KNIGHT)) * knightPhase;
        phase -= popcnt(bitboard.getPieceBoard(WHITE_BISHOP) | bitboard.getPieceBoard(BLACK_BISHOP)) * bishopPhase;
        phase -= popcnt(bitboard.getPieceBoard(WHITE_ROOK) | bitboard.getPieceBoard(BLACK_ROOK)) * rookPhase;
        phase -= popcnt(bitboard.getPieceBoard(WHITE_QUEEN) | bitboard.getPieceBoard(BLACK_QUEEN)) * queenPhase;

        return (phase * 256 + (totalPhase / 2)) / totalPhase;
    }

    inline bool isNotPawnOrKing(PieceType pieceType) {
        return pieceType != WHITE_PAWN && pieceType != BLACK_PAWN && pieceType != WHITE_KING && pieceType != BLACK_KING;
    }

    inline bool isPawn(PieceType pieceType) {
        return pieceType == WHITE_PAWN || pieceType == BLACK_PAWN;
    }

    inline bool isKnight(PieceType pieceType) {
        return pieceType == WHITE_KNIGHT || pieceType == BLACK_KNIGHT;
    }

    inline bool isBishop(PieceType pieceType) {
        return pieceType == WHITE_BISHOP || pieceType == BLACK_BISHOP;
    }

    inline bool isRook(PieceType pieceType) {
        return pieceType == WHITE_ROOK || pieceType == BLACK_ROOK;
    }

    inline bool isQueen(PieceType pieceType) {
        return pieceType == WHITE_QUEEN || pieceType == BLACK_QUEEN;
    }

    inline bool isKing(PieceType pieceType) {
        return pieceType == WHITE_KING || pieceType == BLACK_KING;
    }

    void Evaluation::addMobilityScoreForPiece(PieceType pieceType, int mobility) {
        switch (pieceType) {
        case WHITE_KNIGHT:
            whiteMidgameScore += mobility * getEvalValue(MIDGAME_KNIGHT_MOBILITY);
            whiteEndgameScore += mobility * getEvalValue(ENDGAME_KNIGHT_MOBILITY);
            break;
        case BLACK_KNIGHT:
            blackMidgameScore += mobility * getEvalValue(MIDGAME_KNIGHT_MOBILITY);
            blackEndgameScore += mobility * getEvalValue(ENDGAME_KNIGHT_MOBILITY);
            break;
        case WHITE_BISHOP:
            whiteMidgameScore += mobility * getEvalValue(MIDGAME_BISHOP_MOBILITY);
            whiteEndgameScore += mobility * getEvalValue(ENDGAME_BISHOP_MOBILITY);
            break;
        case BLACK_BISHOP:
            blackMidgameScore += mobility * getEvalValue(MIDGAME_BISHOP_MOBILITY);
            blackEndgameScore += mobility * getEvalValue(ENDGAME_BISHOP_MOBILITY);
            break;
        case WHITE_ROOK:
            whiteMidgameScore += mobility * getEvalValue(MIDGAME_ROOK_MOBILITY);
            whiteEndgameScore += mobility * getEvalValue(ENDGAME_ROOK_MOBILITY);
            break;
        case BLACK_ROOK:
            blackMidgameScore += mobility * getEvalValue(MIDGAME_ROOK_MOBILITY);
            blackEndgameScore += mobility * getEvalValue(ENDGAME_ROOK_MOBILITY);
            break;
        case WHITE_QUEEN:
            whiteMidgameScore += mobility * getEvalValue(MIDGAME_QUEEN_MOBILITY);
            whiteEndgameScore += mobility * getEvalValue(ENDGAME_QUEEN_MOBILITY);
            break;
        case BLACK_QUEEN:
            blackMidgameScore += mobility * getEvalValue(MIDGAME_QUEEN_MOBILITY);
            blackEndgameScore += mobility * getEvalValue(ENDGAME_QUEEN_MOBILITY);
            break;
        }
    }

    void Evaluation::addKingAttackScore(PieceType pieceType, int attackCount) {
        switch (pieceType) {
        case WHITE_PAWN:
            blackMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_PAWN_PENALTY);
            blackEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_PAWN_PENALTY);
            break;
        case BLACK_PAWN:
            whiteMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_PAWN_PENALTY);
            whiteEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_PAWN_PENALTY);
            break;
        case WHITE_KNIGHT:
            blackMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_KNIGHT_PENALTY);
            blackEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_KNIGHT_PENALTY);
            break;
        case BLACK_KNIGHT:
            whiteMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_KNIGHT_PENALTY);
            whiteEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_KNIGHT_PENALTY);
            break;
        case WHITE_BISHOP:
            blackMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_BISHOP_PENALTY);
            blackEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_BISHOP_PENALTY);
            break;
        case BLACK_BISHOP:
            whiteMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_BISHOP_PENALTY);
            whiteEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_BISHOP_PENALTY);
            break;
        case WHITE_ROOK:
            blackMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_ROOK_PENALTY);
            blackEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_ROOK_PENALTY);
            break;
        case BLACK_ROOK:
            whiteMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_ROOK_PENALTY);
            whiteEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_ROOK_PENALTY);
            break;
        case WHITE_QUEEN:
            blackMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_QUEEN_PENALTY);
            blackEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_QUEEN_PENALTY);
            break;
        case BLACK_QUEEN:
            whiteMidgameScore += attackCount * getEvalValue(MIDGAME_KING_ATTACK_QUEEN_PENALTY);
            whiteEndgameScore += attackCount * getEvalValue(ENDGAME_KING_ATTACK_QUEEN_PENALTY);
            break;
        }
    }

    template <PieceColor color>
    void Evaluation::evaluatePieces() {
        uint64_t colorBoard = bitboard.getColorBoard<color>();
        int8_t blackKingSquare = bitscanForward(bitboard.getPieceBoard(BLACK_KING));
        int8_t whiteKingSquare = bitscanForward(bitboard.getPieceBoard(WHITE_KING));
        uint64_t blackKingAttacks = attacksFrom[blackKingSquare];
        uint64_t whiteKingAttacks = attacksFrom[whiteKingSquare];


        while (colorBoard) {
            uint8_t index = popLsb(colorBoard);
            PieceType pieceType = bitboard.getPieceOnSquare(index);

            // Mobility
            if (isNotPawnOrKing(pieceType)) {
                uint64_t mobilitySquares = attacksFrom[index];

                if (color == WHITE) {
                    // Exclude own pieces and attacks by opponent pawns
                    mobilitySquares &= ~(bitboard.getColorBoard<WHITE>() | attacksByPiece[BLACK_PAWN]);

                    // If pieceType == queen, exclude tiles attacked by opponent bishop, knight and rook
                    if (pieceType == WHITE_QUEEN) {
                        mobilitySquares &= ~(attacksByPiece[BLACK_BISHOP] | attacksByPiece[BLACK_KNIGHT] |
                            attacksByPiece[
                                BLACK_ROOK]);
                    }

                    // If pieceType == rook, exclude tiles attacked by opponent bishop and knight
                    if (pieceType == WHITE_ROOK) {
                        mobilitySquares &= ~(attacksByPiece[BLACK_BISHOP] | attacksByPiece[BLACK_KNIGHT]);
                    }
                } else {
                    // Exclude own pieces and attacks by opponent pawns
                    mobilitySquares &= ~(bitboard.getColorBoard<BLACK>() | attacksByPiece[WHITE_PAWN]);

                    // If pieceType == queen, exclude tiles attacked by opponent bishop, knight and rook
                    if (pieceType == BLACK_QUEEN) {
                        mobilitySquares &= ~(attacksByPiece[WHITE_BISHOP] | attacksByPiece[WHITE_KNIGHT] |
                            attacksByPiece[
                                WHITE_ROOK]);
                    }

                    // If pieceType == rook, exclude tiles attacked by opponent bishop and knight
                    if (pieceType == BLACK_ROOK) {
                        mobilitySquares &= ~(attacksByPiece[WHITE_BISHOP] | attacksByPiece[WHITE_KNIGHT]);
                    }
                }

                uint8_t mobility = popcnt(mobilitySquares);
                addMobilityScoreForPiece(pieceType, mobility);
            }

            // King safety -  Attacks around king
            if (!isKing(pieceType)) {
                uint64_t attackSquares = attacksFrom[index];
                uint64_t opponentKingAttacks = color == WHITE ? blackKingAttacks : whiteKingAttacks;
                uint64_t attacksAroundKing = attackSquares & opponentKingAttacks;
                int attackCount = popcnt(attacksAroundKing);

                addKingAttackScore(pieceType, attackCount);
            }

            // Other King safety
            if (isKing(pieceType)) {
                if (color == WHITE) {
                    // Pawn Shield
                    uint64_t kingBB = bitboard.getPieceBoard(WHITE_KING);
                    uint64_t pawnBB = bitboard.getPieceBoard(WHITE_PAWN);
                    uint64_t pawnShieldMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
                    pawnShieldMask |= nortOne(pawnShieldMask);
                    uint64_t pawnShield = pawnBB & pawnShieldMask;
                    uint8_t pawnShieldCount = std::min(popcnt(pawnShield), 3ULL);

                    whiteMidgameScore += getEvalValue(MIDGAME_PAWN_SHIELD) * pawnShieldCount;
                    whiteEndgameScore += getEvalValue(ENDGAME_PAWN_SHIELD) * pawnShieldCount;

                    // Virtual mobility - Get queen attacks from king position, with only occupied squares by own pieces. We also ignore the squares around the king.
                    uint64_t virtualMobilitySquares = bitboard.getQueenAttacks(index, bitboard.getColorBoard<WHITE>()) &
                        ~(attacksFrom[index] | bitboard.getColorBoard<
                            WHITE>());
                    whiteMidgameScore += popcnt(virtualMobilitySquares) * getEvalValue(
                        MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
                    whiteEndgameScore += popcnt(virtualMobilitySquares) * getEvalValue(
                        ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
                } else {
                    // Pawn Shield
                    uint64_t kingBB = bitboard.getPieceBoard(BLACK_KING);
                    uint64_t pawnBB = bitboard.getPieceBoard(BLACK_PAWN);
                    uint64_t pawnShieldMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
                    pawnShieldMask |= soutOne(pawnShieldMask);
                    uint64_t pawnShield = pawnBB & pawnShieldMask;
                    uint8_t pawnShieldCount = std::min(popcnt(pawnShield), 3ULL);

                    blackMidgameScore += getEvalValue(MIDGAME_PAWN_SHIELD) * pawnShieldCount;
                    blackEndgameScore += getEvalValue(ENDGAME_PAWN_SHIELD) * pawnShieldCount;

                    // Virtual mobility - Get queen attacks from king position, with only occupied squares by own pieces. We also ignore the squares around the king.
                    uint64_t virtualMobilitySquares = bitboard.getQueenAttacks(index, bitboard.getColorBoard<BLACK>()) &
                        ~(attacksFrom[index] | bitboard.getColorBoard<
                            BLACK>());
                    blackMidgameScore += popcnt(virtualMobilitySquares) * getEvalValue(
                        MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
                    blackEndgameScore += popcnt(virtualMobilitySquares) * getEvalValue(
                        ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
                }
            }

            // Per piece evaluation
            if (isPawn(pieceType)) {
                // Doubled pawn
                uint64_t pawnBB = bitboard.getPieceBoard(pieceType);
                Direction direction = color == WHITE ? NORTH : SOUTH;
                uint64_t doubledPawnsMask = bitboard.getRayAttack(index, direction);
                uint64_t doubledPawns = pawnBB & doubledPawnsMask;

                if (doubledPawns) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_DOUBLED_PAWN_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_DOUBLED_PAWN_PENALTY);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_DOUBLED_PAWN_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_DOUBLED_PAWN_PENALTY);
                    }
                }

                // Passed pawn
                if (bitboard.isPassedPawn<color>(index)) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN);
                        whiteEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN);
                        blackEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN);
                    }
                }

                // Isolated pawn
                if (bitboard.isIsolatedPawn<color>(index)) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_PAWN_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_PAWN_PENALTY);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_ISOLATED_PAWN_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_ISOLATED_PAWN_PENALTY);
                    }

                    if (bitboard.isSemiOpenFile<color>(index)) {
                        if (color == WHITE) {
                            whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                            whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                        } else {
                            blackMidgameScore += getEvalValue(MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                            blackEndgameScore += getEvalValue(ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                        }
                    }

                    if (index & DE_FILE) {
                        if (color == WHITE) {
                            whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY);
                            whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY);
                        } else {
                            blackMidgameScore += getEvalValue(MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY);
                            blackEndgameScore += getEvalValue(ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY);
                        }
                    }
                }
            }
        }
    }

    int Evaluation::evaluate() {
        int phase = getPhase();
        int modifier = bitboard.getMovingColor() == WHITE ? 1 : -1;

        evaluateMaterial<WHITE>();
        evaluateMaterial<BLACK>();

        evaluatePst<WHITE>();
        evaluatePst<BLACK>();

        evaluatePieces<WHITE>();
        evaluatePieces<BLACK>();

        int whiteScore = ((whiteMidgameScore * (256 - phase)) + (whiteEndgameScore * phase)) / 256;
        int blackScore = ((blackMidgameScore * (256 - phase)) + (blackEndgameScore * phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    template <PieceColor color>
    void Evaluation::evaluateMaterial() {
        if (color == WHITE) {
            whiteMidgameScore += bitboard.getMaterialCount<WHITE_PAWN>() * getEvalValue(MIDGAME_PAWN_MATERIAL);
            whiteEndgameScore += bitboard.getMaterialCount<WHITE_PAWN>() * getEvalValue(ENDGAME_PAWN_MATERIAL);

            whiteMidgameScore += bitboard.getMaterialCount<WHITE_KNIGHT>() * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
            whiteEndgameScore += bitboard.getMaterialCount<WHITE_KNIGHT>() * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

            whiteMidgameScore += bitboard.getMaterialCount<WHITE_BISHOP>() * getEvalValue(MIDGAME_BISHOP_MATERIAL);
            whiteEndgameScore += bitboard.getMaterialCount<WHITE_BISHOP>() * getEvalValue(ENDGAME_BISHOP_MATERIAL);

            whiteMidgameScore += bitboard.getMaterialCount<WHITE_ROOK>() * getEvalValue(MIDGAME_ROOK_MATERIAL);
            whiteEndgameScore += bitboard.getMaterialCount<WHITE_ROOK>() * getEvalValue(ENDGAME_ROOK_MATERIAL);

            whiteMidgameScore += bitboard.getMaterialCount<WHITE_QUEEN>() * getEvalValue(MIDGAME_QUEEN_MATERIAL);
            whiteEndgameScore += bitboard.getMaterialCount<WHITE_QUEEN>() * getEvalValue(ENDGAME_QUEEN_MATERIAL);
        } else {
            blackMidgameScore += bitboard.getMaterialCount<BLACK_PAWN>() * getEvalValue(MIDGAME_PAWN_MATERIAL);
            blackEndgameScore += bitboard.getMaterialCount<BLACK_PAWN>() * getEvalValue(ENDGAME_PAWN_MATERIAL);

            blackMidgameScore += bitboard.getMaterialCount<BLACK_KNIGHT>() * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
            blackEndgameScore += bitboard.getMaterialCount<BLACK_KNIGHT>() * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

            blackMidgameScore += bitboard.getMaterialCount<BLACK_BISHOP>() * getEvalValue(MIDGAME_BISHOP_MATERIAL);
            blackEndgameScore += bitboard.getMaterialCount<BLACK_BISHOP>() * getEvalValue(ENDGAME_BISHOP_MATERIAL);

            blackMidgameScore += bitboard.getMaterialCount<BLACK_ROOK>() * getEvalValue(MIDGAME_ROOK_MATERIAL);
            blackEndgameScore += bitboard.getMaterialCount<BLACK_ROOK>() * getEvalValue(ENDGAME_ROOK_MATERIAL);

            blackMidgameScore += bitboard.getMaterialCount<BLACK_QUEEN>() * getEvalValue(MIDGAME_QUEEN_MATERIAL);
            blackEndgameScore += bitboard.getMaterialCount<BLACK_QUEEN>() * getEvalValue(ENDGAME_QUEEN_MATERIAL);
        }
    }

    template <PieceColor color>
    void Evaluation::evaluatePst() {
        if (color == WHITE) {
            whiteMidgameScore += bitboard.getWhiteMidgamePst();
            whiteEndgameScore += bitboard.getWhiteEndgamePst();
        } else {
            blackMidgameScore += bitboard.getBlackMidgamePst();
            blackEndgameScore += bitboard.getBlackEndgamePst();
        }
    }
}
