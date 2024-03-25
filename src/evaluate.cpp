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

#include <algorithm>

#include "features.h"

namespace Zagreus {
void Evaluation::initEvalContext(Bitboard& bitboard) {
    uint64_t whitePawns = bitboard.getPieceBoard(WHITE_PAWN);
    while (whitePawns) {
        uint8_t square = popLsb(whitePawns);
        uint64_t attacks = getPawnAttacks<WHITE>(square);

        attacksByPiece[WHITE_PAWN] |= attacks;
        attackedBy2[WHITE] |= (attacks & attacksByColor[WHITE]);
        attacksByColor[WHITE] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t whiteKnights = bitboard.getPieceBoard(WHITE_KNIGHT);
    while (whiteKnights) {
        uint8_t square = popLsb(whiteKnights);
        uint64_t attacks = getKnightAttacks(square);

        attacksByPiece[WHITE_KNIGHT] |= attacks;
        attackedBy2[WHITE] |= (attacks & attacksByColor[WHITE]);
        attacksByColor[WHITE] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t whiteBishops = bitboard.getPieceBoard(WHITE_BISHOP);
    while (whiteBishops) {
        uint8_t square = popLsb(whiteBishops);
        uint64_t attacks = bitboard.getBishopAttacks(square);

        attacksByPiece[WHITE_BISHOP] |= attacks;
        attackedBy2[WHITE] |= (attacks & attacksByColor[WHITE]);
        attacksByColor[WHITE] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t whiteRooks = bitboard.getPieceBoard(WHITE_ROOK);
    while (whiteRooks) {
        uint8_t square = popLsb(whiteRooks);
        uint64_t attacks = bitboard.getRookAttacks(square);

        attacksByPiece[WHITE_ROOK] |= attacks;
        attackedBy2[WHITE] |= (attacks & attacksByColor[WHITE]);
        attacksByColor[WHITE] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t whiteQueens = bitboard.getPieceBoard(WHITE_QUEEN);
    while (whiteQueens) {
        uint8_t square = popLsb(whiteQueens);
        uint64_t attacks = bitboard.getQueenAttacks(square);

        attacksByPiece[WHITE_QUEEN] |= attacks;
        attackedBy2[WHITE] |= (attacks & attacksByColor[WHITE]);
        attacksByColor[WHITE] |= attacks;
        attacksFrom[square] |= attacks;
    }

    {
        uint64_t whiteKing = bitboard.getPieceBoard(WHITE_KING);
        uint8_t square = popLsb(whiteKing);
        uint64_t attacks = getKingAttacks(square);

        attacksByPiece[WHITE_KING] |= attacks;
        attacksByColor[WHITE] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t blackPawns = bitboard.getPieceBoard(BLACK_PAWN);
    while (blackPawns) {
        uint8_t square = popLsb(blackPawns);
        uint64_t attacks = getPawnAttacks<BLACK>(square);

        attacksByPiece[BLACK_PAWN] |= attacks;
        attackedBy2[BLACK] |= (attacks & attacksByColor[BLACK]);
        attacksByColor[BLACK] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t blackKnights = bitboard.getPieceBoard(BLACK_KNIGHT);
    while (blackKnights) {
        uint8_t square = popLsb(blackKnights);
        uint64_t attacks = getKnightAttacks(square);

        attacksByPiece[BLACK_KNIGHT] |= attacks;
        attackedBy2[BLACK] |= (attacks & attacksByColor[BLACK]);
        attacksByColor[BLACK] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t blackBishops = bitboard.getPieceBoard(BLACK_BISHOP);
    while (blackBishops) {
        uint8_t square = popLsb(blackBishops);
        uint64_t attacks = bitboard.getBishopAttacks(square);

        attacksByPiece[BLACK_BISHOP] |= attacks;
        attackedBy2[BLACK] |= (attacks & attacksByColor[BLACK]);
        attacksByColor[BLACK] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t blackRooks = bitboard.getPieceBoard(BLACK_ROOK);
    while (blackRooks) {
        uint8_t square = popLsb(blackRooks);
        uint64_t attacks = bitboard.getRookAttacks(square);

        attacksByPiece[BLACK_ROOK] |= attacks;
        attackedBy2[BLACK] |= (attacks & attacksByColor[BLACK]);
        attacksByColor[BLACK] |= attacks;
        attacksFrom[square] |= attacks;
    }

    uint64_t blackQueens = bitboard.getPieceBoard(BLACK_QUEEN);
    while (blackQueens) {
        uint8_t square = popLsb(blackQueens);
        uint64_t attacks = bitboard.getQueenAttacks(square);

        attacksByPiece[BLACK_QUEEN] |= attacks;
        attackedBy2[BLACK] |= (attacks & attacksByColor[BLACK]);
        attacksByColor[BLACK] |= attacks;
        attacksFrom[square] |= attacks;
    }

    {
        uint64_t blackKing = bitboard.getPieceBoard(BLACK_KING);
        uint8_t square = popLsb(blackKing);
        uint64_t attacks = getKingAttacks(square);

        attacksByPiece[BLACK_KING] |= attacks;
        attacksByColor[BLACK] |= attacks;
        attacksFrom[square] |= attacks;
    }

    // A weak square is either:
    // 1. Attacked by the opponent and not by us
    weakSquares[WHITE] = attacksByColor[BLACK] & ~attacksByColor[WHITE];
    weakSquares[BLACK] = attacksByColor[WHITE] & ~attacksByColor[BLACK];

    // 2. Attacked twice by the opponent and once/not by us.
    weakSquares[WHITE] |= attackedBy2[BLACK] & ~attackedBy2[WHITE];
    weakSquares[BLACK] |= attackedBy2[WHITE] & ~attackedBy2[BLACK];

    // 3. Attacked by both colors, but our attack is not a pawn and the opponent's is.
    weakSquares[WHITE] |= (attacksByColor[WHITE] & attacksByColor[BLACK]
                           & ~attackedBy2[WHITE] & ~attackedBy2[BLACK]) & ~attacksByPiece[
            WHITE_PAWN]
        & attacksByPiece[BLACK_PAWN];
    weakSquares[BLACK] |= (attacksByColor[WHITE] & attacksByColor[BLACK]
                           & ~attackedBy2[WHITE] & ~attackedBy2[BLACK]) & ~attacksByPiece[
            BLACK_PAWN]
        & attacksByPiece[WHITE_PAWN];

    // 4. Attacked twice by both colors, but our attack has no pawns and the opponent's has at least one.
    weakSquares[WHITE] |= (attackedBy2[WHITE] & attackedBy2[BLACK])
        & ~attacksByPiece[WHITE_PAWN]
        & attacksByPiece[BLACK_PAWN];
    weakSquares[BLACK] |= (attackedBy2[WHITE] & attackedBy2[BLACK])
        & ~attacksByPiece[BLACK_PAWN]
        & attacksByPiece[WHITE_PAWN];

    // A strong square is either:
    // 1. Attacked by us and not by the opponent
    strongSquares[WHITE] = attacksByColor[WHITE] & ~attacksByColor[BLACK];
    strongSquares[BLACK] = attacksByColor[BLACK] & ~attacksByColor[WHITE];

    // 2. Attacked twice by us and once/not by the opponent.
    strongSquares[WHITE] |= attackedBy2[WHITE] & ~attackedBy2[BLACK];
    strongSquares[BLACK] |= attackedBy2[BLACK] & ~attackedBy2[WHITE];

    // 3. Attacked by both colors, but the opponent's attack is not a pawn and ours is.
    strongSquares[WHITE] |= (attacksByColor[WHITE] & attacksByColor[BLACK]
                             & ~attackedBy2[WHITE] & ~attackedBy2[BLACK])
        & attacksByPiece[WHITE_PAWN]
        & ~attacksByPiece[BLACK_PAWN];
    strongSquares[BLACK] |= (attacksByColor[WHITE] & attacksByColor[BLACK]
                             & ~attackedBy2[WHITE] & ~attackedBy2[BLACK])
        & attacksByPiece[BLACK_PAWN]
        & ~attacksByPiece[WHITE_PAWN];

    // 4. Attacked twice by both colors, but the opponent's attack has no pawns and ours has at least one.
    strongSquares[WHITE] |= (attackedBy2[WHITE] & attackedBy2[BLACK])
        & attacksByPiece[WHITE_PAWN]
        & ~attacksByPiece[BLACK_PAWN];
    strongSquares[BLACK] |= (attackedBy2[WHITE] & attackedBy2[BLACK])
        & attacksByPiece[BLACK_PAWN]
        & ~attacksByPiece[WHITE_PAWN];
}

constexpr int knightPhase = 1;
constexpr int bishopPhase = 1;
constexpr int rookPhase = 2;
constexpr int queenPhase = 4;
constexpr int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

int Evaluation::getPhase() {
    int phase = totalPhase;

    phase -= popcnt(bitboard.getPieceBoard(WHITE_KNIGHT) | bitboard.getPieceBoard(BLACK_KNIGHT)) *
        knightPhase;
    phase -= popcnt(bitboard.getPieceBoard(WHITE_BISHOP) | bitboard.getPieceBoard(BLACK_BISHOP)) *
        bishopPhase;
    phase -=
        popcnt(bitboard.getPieceBoard(WHITE_ROOK) | bitboard.getPieceBoard(BLACK_ROOK)) * rookPhase;
    phase -= popcnt(bitboard.getPieceBoard(WHITE_QUEEN) | bitboard.getPieceBoard(BLACK_QUEEN)) *
        queenPhase;

    return (phase * 256 + (totalPhase / 2)) / totalPhase;
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
    constexpr PieceColor opponentColor = color == WHITE ? BLACK : WHITE;
    uint64_t colorBoard = bitboard.getColorBoard<color>();
    int8_t blackKingSquare = bitscanForward(bitboard.getPieceBoard(BLACK_KING));
    int8_t whiteKingSquare = bitscanForward(bitboard.getPieceBoard(WHITE_KING));
    uint64_t blackKingAttacks = attacksFrom[blackKingSquare];
    uint64_t whiteKingAttacks = attacksFrom[whiteKingSquare];

    // Backward pawn
    // A backward pawn is a pawn no longer defensible by own pawns and whose stop square lacks pawn protection but is controlled by a sentry. Thus, don't considering piece tactics, the backward pawn is not able to push forward without being lost, even establishing an opponent passer. If two opposing pawns on adjacent files in knight distance are mutually backward, the more advanced is not considered backward.
    uint64_t pawnBB = bitboard.getPieceBoard(color == WHITE ? WHITE_PAWN : BLACK_PAWN);
    uint64_t stopSquares = color == WHITE ? whiteStopSquares(pawnBB) : blackStopSquares(pawnBB);
    uint64_t ownFrontSpans = color == WHITE ? whiteAttackSpans(pawnBB) : blackAttackSpans(pawnBB);
    uint64_t unprotectableStopSquares = stopSquares & ~ownFrontSpans;
    uint64_t attackedStopSquares = unprotectableStopSquares & attacksByPiece[color == WHITE
                                           ? BLACK_PAWN
                                           : WHITE_PAWN];
    int backwardPawnCount = popcnt(attackedStopSquares);

    if (color == WHITE) {
        whiteMidgameScore += backwardPawnCount * getEvalValue(MIDGAME_BACKWARD_PAWN_PENALTY);
        whiteEndgameScore += backwardPawnCount * getEvalValue(ENDGAME_BACKWARD_PAWN_PENALTY);
    } else {
        blackMidgameScore += backwardPawnCount * getEvalValue(MIDGAME_BACKWARD_PAWN_PENALTY);
        blackEndgameScore += backwardPawnCount * getEvalValue(ENDGAME_BACKWARD_PAWN_PENALTY);
    }

    while (colorBoard) {
        uint8_t squareIndex = popLsb(colorBoard);
        uint64_t square = 1ULL << squareIndex;
        PieceType pieceType = bitboard.getPieceOnSquare(squareIndex);

        // Mobility
        if (isNotPawnOrKing(pieceType)) {
            uint64_t mobilitySquares = attacksFrom[squareIndex];

            if (color == WHITE) {
                // Exclude own pieces and attacks by opponent pawns
                mobilitySquares &= ~(bitboard.getColorBoard<WHITE>() | attacksByPiece[BLACK_PAWN]);

                // If pieceType == queen, exclude tiles attacked by opponent bishop, knight and rook
                if (pieceType == WHITE_QUEEN) {
                    mobilitySquares &= ~(
                        (attacksByPiece[BLACK_BISHOP] | attacksByPiece[BLACK_KNIGHT] |
                         attacksByPiece[BLACK_ROOK]) & ~strongSquares[color]);
                }

                // If pieceType == rook, exclude tiles attacked by opponent bishop and knight
                if (pieceType == WHITE_ROOK) {
                    mobilitySquares &= ~(
                        (attacksByPiece[BLACK_BISHOP] | attacksByPiece[BLACK_KNIGHT]) & ~
                        strongSquares[color]);
                }

                // Squares that are attacked by the current piece and no other pieces of the same color, while also being attacked by the opponent are also considered weak.
                uint64_t badMobilitySquares =
                    weakSquares[color] | (
                        attacksFrom[squareIndex] & attacksByColor[BLACK] & ~attackedBy2[
                            WHITE]);
                mobilitySquares &= ~badMobilitySquares;
            } else {
                // Exclude own pieces and attacks by opponent pawns
                mobilitySquares &= ~(bitboard.getColorBoard<BLACK>() | attacksByPiece[WHITE_PAWN]);

                // If pieceType == queen, exclude tiles attacked by opponent bishop, knight and rook
                if (pieceType == BLACK_QUEEN) {
                    mobilitySquares &= ~((attacksByPiece[WHITE_BISHOP] | attacksByPiece[
                                              WHITE_KNIGHT] |
                                          attacksByPiece[WHITE_ROOK]) & ~strongSquares[color]);
                }

                // If pieceType == rook, exclude tiles attacked by opponent bishop and knight
                if (pieceType == BLACK_ROOK) {
                    mobilitySquares &= ~(
                        (attacksByPiece[WHITE_BISHOP] | attacksByPiece[WHITE_KNIGHT]) & ~
                        strongSquares[color]);
                }

                // Squares that are attacked by the current piece and no other pieces of the same color, while also being attacked by the opponent are also considered weak.
                uint64_t badMobilitySquares =
                    weakSquares[color] | (
                        attacksFrom[squareIndex] & attacksByColor[WHITE] & ~attackedBy2[
                            BLACK]);
                mobilitySquares &= ~badMobilitySquares;
            }

            uint8_t mobility = popcnt(mobilitySquares);
            addMobilityScoreForPiece(pieceType, mobility);
        }

        // King safety - Attacks around king
        if (!isKing(pieceType)) {
            uint64_t attackSquares = attacksFrom[squareIndex];
            uint64_t opponentKingAttacks = color == WHITE ? blackKingAttacks : whiteKingAttacks;
            uint64_t attacksAroundKing = attackSquares & opponentKingAttacks;
            uint8_t attackCount = popcnt(attacksAroundKing);

            addKingAttackScore(pieceType, attackCount);
        }

        // Other King safety
        if (isKing(pieceType)) {
            if (color == WHITE) {
                uint64_t kingBB = bitboard.getPieceBoard(WHITE_KING);

                // Pawn Shield
                uint64_t pawnBB = bitboard.getPieceBoard(WHITE_PAWN);

                if (!(kingBB & DE_FILE)) {
                    uint64_t pawnShieldMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
                    pawnShieldMask |= nortOne(pawnShieldMask);
                    uint64_t pawnShield = pawnBB & pawnShieldMask;
                    uint8_t pawnShieldCount = std::min<uint64_t>(popcnt(pawnShield), 3ULL);
                    uint64_t kingFile = bitboard.getFile(squareIndex);

                    whiteMidgameScore += getEvalValue(MIDGAME_PAWN_SHIELD) * pawnShieldCount;
                    whiteEndgameScore += getEvalValue(ENDGAME_PAWN_SHIELD) * pawnShieldCount;

                    // If there is no pawn in front of the king in the pawn shield, apply penalty
                    if (!(pawnBB & kingFile & pawnShield)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_PAWN_SHIELD_NO_KING_PAWN);
                        whiteEndgameScore += getEvalValue(ENDGAME_PAWN_SHIELD_NO_KING_PAWN);
                    }
                }

                // If the king is also on a semi-open or open file, apply penalty
                if (bitboard.isSemiOpenOrOpenFile<WHITE>(squareIndex)) {
                    whiteMidgameScore += getEvalValue(MIDGAME_KING_ON_SEMI_OPEN_FILE);
                    whiteEndgameScore += getEvalValue(ENDGAME_KING_ON_SEMI_OPEN_FILE);
                }

                // Penalty for king being next to a (semi-)open file
                if (squareIndex % 8 != 0) {
                    if (bitboard.isSemiOpenOrOpenFile<WHITE>(squareIndex - 1)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                    }
                }

                if (squareIndex % 8 != 7) {
                    if (bitboard.isSemiOpenOrOpenFile<WHITE>(squareIndex + 1)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                    }
                }

                // Pawn Storm
                /*uint64_t pawnStormMask = bitboard.getFile(squareIndex);

                if (squareIndex % 8 != 0) {
                    pawnStormMask |= bitboard.getFile(squareIndex - 1);
                }

                if (squareIndex % 8 != 7) {
                    pawnStormMask |= bitboard.getFile(squareIndex + 1);
                }

                uint64_t opponentPawnStormBB = bitboard.getPieceBoard(BLACK_PAWN) & pawnStormMask;
                int pawnsOnRank5Count = popcnt(opponentPawnStormBB & RANK_5);
                int pawnsOnRank4Count = popcnt(opponentPawnStormBB & RANK_4);
                int pawnsOnRank3Count = popcnt(opponentPawnStormBB & RANK_3);

                whiteMidgameScore += getEvalValue(MIDGAME_PAWN_STORM_DISTANCE_3) *
                    pawnsOnRank5Count;
                whiteEndgameScore += getEvalValue(ENDGAME_PAWN_STORM_DISTANCE_3) *
                    pawnsOnRank5Count;
                whiteMidgameScore += getEvalValue(MIDGAME_PAWN_STORM_DISTANCE_2) *
                    pawnsOnRank4Count;
                whiteEndgameScore += getEvalValue(ENDGAME_PAWN_STORM_DISTANCE_2) *
                    pawnsOnRank4Count;
                whiteMidgameScore += getEvalValue(MIDGAME_PAWN_STORM_DISTANCE_1) *
                    pawnsOnRank3Count;
                whiteEndgameScore += getEvalValue(ENDGAME_PAWN_STORM_DISTANCE_1) *
                    pawnsOnRank3Count;*/

                // Virtual mobility - Get queen attacks from king position, with only occupied squares by
                // own pieces. We also ignore the squares around the king.
                uint64_t virtualMobilitySquares =
                    bitboard.getQueenAttacks(squareIndex, bitboard.getColorBoard<WHITE>()) &
                    ~(attacksFrom[squareIndex] | bitboard.getColorBoard<WHITE>());
                whiteMidgameScore +=
                    popcnt(virtualMobilitySquares) * getEvalValue(
                        MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
                whiteEndgameScore +=
                    popcnt(virtualMobilitySquares) * getEvalValue(
                        ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
            } else {
                uint64_t kingBB = bitboard.getPieceBoard(BLACK_KING);

                // Pawn Shield
                uint64_t pawnBB = bitboard.getPieceBoard(BLACK_PAWN);

                if (!(kingBB & DE_FILE)) {
                    uint64_t pawnShieldMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
                    pawnShieldMask |= soutOne(pawnShieldMask);
                    uint64_t pawnShield = pawnBB & pawnShieldMask;
                    uint8_t pawnShieldCount = std::min<uint64_t>(popcnt(pawnShield), 3ULL);
                    uint64_t kingFile = bitboard.getFile(squareIndex);

                    blackMidgameScore += getEvalValue(MIDGAME_PAWN_SHIELD) * pawnShieldCount;
                    blackEndgameScore += getEvalValue(ENDGAME_PAWN_SHIELD) * pawnShieldCount;

                    // If there is no pawn in front of the king in the pawn shield, apply penalty
                    if (!(pawnBB & kingFile & pawnShield)) {
                        blackMidgameScore += getEvalValue(MIDGAME_PAWN_SHIELD_NO_KING_PAWN);
                        blackEndgameScore += getEvalValue(ENDGAME_PAWN_SHIELD_NO_KING_PAWN);
                    }
                }

                // If the king is also on a semi-open or open file, apply penalty
                if (bitboard.isSemiOpenOrOpenFile<BLACK>(squareIndex)) {
                    blackMidgameScore += getEvalValue(MIDGAME_KING_ON_SEMI_OPEN_FILE);
                    blackEndgameScore += getEvalValue(ENDGAME_KING_ON_SEMI_OPEN_FILE);
                }

                // Penalty for king being next to a (semi-)open file
                if (squareIndex % 8 != 0) {
                    if (bitboard.isSemiOpenOrOpenFile<BLACK>(squareIndex - 1)) {
                        blackMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                    }
                }

                if (squareIndex % 8 != 7) {
                    if (bitboard.isSemiOpenOrOpenFile<BLACK>(squareIndex + 1)) {
                        blackMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE_PENALTY);
                    }
                }

                // Pawn Storm
                /*uint64_t pawnStormMask = bitboard.getFile(squareIndex);

                if (squareIndex % 8 != 0) {
                    pawnStormMask |= bitboard.getFile(squareIndex - 1);
                }

                if (squareIndex % 8 != 7) {
                    pawnStormMask |= bitboard.getFile(squareIndex + 1);
                }

                uint64_t opponentPawnStormBB =
                    bitboard.getPieceBoard(WHITE_PAWN) & pawnStormMask;
                int pawnsOnRank4Count = popcnt(opponentPawnStormBB & RANK_4);
                int pawnsOnRank5Count = popcnt(opponentPawnStormBB & RANK_5);
                int pawnsOnRank6Count = popcnt(opponentPawnStormBB & RANK_6);

                blackMidgameScore += getEvalValue(MIDGAME_PAWN_STORM_DISTANCE_3) *
                    pawnsOnRank4Count;
                blackEndgameScore += getEvalValue(ENDGAME_PAWN_STORM_DISTANCE_3) *
                    pawnsOnRank4Count;
                blackMidgameScore += getEvalValue(MIDGAME_PAWN_STORM_DISTANCE_2) *
                    pawnsOnRank5Count;
                blackEndgameScore += getEvalValue(ENDGAME_PAWN_STORM_DISTANCE_2) *
                    pawnsOnRank5Count;
                blackMidgameScore += getEvalValue(MIDGAME_PAWN_STORM_DISTANCE_1) *
                    pawnsOnRank6Count;
                blackEndgameScore += getEvalValue(ENDGAME_PAWN_STORM_DISTANCE_1) *
                    pawnsOnRank6Count;*/

                // Virtual mobility - Get queen attacks from king position, with only occupied squares by
                // own pieces. We also ignore the squares around the king.
                uint64_t virtualMobilitySquares =
                    bitboard.getQueenAttacks(squareIndex, bitboard.getColorBoard<BLACK>()) &
                    ~(attacksFrom[squareIndex] | bitboard.getColorBoard<BLACK>());
                blackMidgameScore +=
                    popcnt(virtualMobilitySquares) * getEvalValue(
                        MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
                blackEndgameScore +=
                    popcnt(virtualMobilitySquares) * getEvalValue(
                        ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY);
            }
        }

        // Per piece evaluation
        if (isPawn(pieceType)) {
            // Doubled pawn
            uint64_t pawnBB = bitboard.getPieceBoard(pieceType);
            Direction direction = color == WHITE ? NORTH : SOUTH;
            Direction oppositeDirection = color == WHITE ? SOUTH : NORTH;
            uint64_t frontMask = getRayAttack(squareIndex, direction);
            uint64_t behindMask = getRayAttack(squareIndex, oppositeDirection);
            uint64_t doubledPawns = pawnBB & frontMask;

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
            if (bitboard.isPassedPawn<color>(squareIndex)) {
                if (color == WHITE) {
                    whiteMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN);
                    whiteEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN);
                } else {
                    blackMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN);
                    blackEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN);
                }

                // Bonus for close to promotion
                constexpr uint64_t OPPONENT_HALF = color == WHITE
                                                       ? RANK_5 | RANK_6 | RANK_7
                                                       : RANK_2 | RANK_3 | RANK_4;
                uint64_t closeToPromotion = square & OPPONENT_HALF;

                if (closeToPromotion) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN_CLOSE_TO_PROMOTION);
                        whiteEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN_CLOSE_TO_PROMOTION);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN_CLOSE_TO_PROMOTION);
                        blackEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN_CLOSE_TO_PROMOTION);
                    }
                }

                // Bonus for defended passed pawn
                uint64_t defended = square & attacksByPiece[
                                        color == WHITE ? WHITE_PAWN : BLACK_PAWN];

                if (defended) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN_DEFENDED);
                        whiteEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN_DEFENDED);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN_DEFENDED);
                        blackEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN_DEFENDED);
                    }
                }

                // Penalty for weak pawns

                // Tarrasch rule
                if (color == WHITE) {
                    // Rook in front of own passed pawn penalty
                    if (frontMask & bitboard.getPieceBoard(WHITE_ROOK)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_TARRASCH_OWN_ROOK_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_TARRASCH_OWN_ROOK_PENALTY);
                    }

                    // Rook behind own passed pawn bonus
                    if (behindMask & bitboard.getPieceBoard(WHITE_ROOK)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_TARRASCH_OWN_ROOK_DEFEND);
                        whiteEndgameScore += getEvalValue(ENDGAME_TARRASCH_OWN_ROOK_DEFEND);
                    }

                    // Opponent rook behind own passed pawn penalty
                    if (behindMask & bitboard.getPieceBoard(BLACK_ROOK)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY);
                    }
                } else {
                    // Rook in front of own passed pawn penalty
                    if (frontMask & bitboard.getPieceBoard(BLACK_ROOK)) {
                        blackMidgameScore += getEvalValue(MIDGAME_TARRASCH_OWN_ROOK_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_TARRASCH_OWN_ROOK_PENALTY);
                    }

                    // Rook behind own passed pawn bonus
                    if (behindMask & bitboard.getPieceBoard(BLACK_ROOK)) {
                        blackMidgameScore += getEvalValue(MIDGAME_TARRASCH_OWN_ROOK_DEFEND);
                        blackEndgameScore += getEvalValue(ENDGAME_TARRASCH_OWN_ROOK_DEFEND);
                    }

                    // Opponent rook behind own passed pawn penalty
                    if (behindMask & bitboard.getPieceBoard(WHITE_ROOK)) {
                        blackMidgameScore += getEvalValue(MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY);
                    }
                }
            }

            // Connected pawns
            int midgameScore = 0;
            int endgameScore = 0;

            uint64_t possibleDefenders = color == WHITE
                                             ? soEaOne(square) | soWeOne(square)
                                             : noEaOne(square) | noWeOne(square);
            uint64_t connected = pawnBB & possibleDefenders;
            int defenderCount = popcnt(connected);

            if (defenderCount) {
                midgameScore += defenderCount * getEvalValue(MIDGAME_PAWN_CONNECTED);
                endgameScore += defenderCount * getEvalValue(ENDGAME_PAWN_CONNECTED);
            }

            if (color == WHITE) {
                whiteMidgameScore += midgameScore;
                whiteEndgameScore += endgameScore;
            } else {
                blackMidgameScore += midgameScore;
                blackEndgameScore += endgameScore;
            }

            // Isolated pawn
            if (bitboard.isIsolatedPawn<color>(squareIndex)) {
                if (color == WHITE) {
                    whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_PAWN_PENALTY);
                    whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_PAWN_PENALTY);
                } else {
                    blackMidgameScore += getEvalValue(MIDGAME_ISOLATED_PAWN_PENALTY);
                    blackEndgameScore += getEvalValue(ENDGAME_ISOLATED_PAWN_PENALTY);
                }

                if (bitboard.isSemiOpenFile<color>(squareIndex)) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY);
                    }
                }

                if (square & DE_FILE) {
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

        // Knight eval
        if (isKnight(pieceType)) {
            // Penalize the knight's value for each missing pawn
            uint64_t pawnBB = bitboard.getPieceBoard(color == WHITE ? WHITE_PAWN : BLACK_PAWN);
            uint8_t pawnCount = popcnt(pawnBB);

            if (color == WHITE) {
                whiteMidgameScore += getEvalValue(MIDGAME_KNIGHT_MISSING_PAWN_PENALTY) * (
                    8 - pawnCount);
                whiteEndgameScore += getEvalValue(ENDGAME_KNIGHT_MISSING_PAWN_PENALTY) * (
                    8 - pawnCount);
            } else {
                blackMidgameScore += getEvalValue(MIDGAME_KNIGHT_MISSING_PAWN_PENALTY) * (
                    8 - pawnCount);
                blackEndgameScore += getEvalValue(ENDGAME_KNIGHT_MISSING_PAWN_PENALTY) * (
                    8 - pawnCount);
            }

            // Slight bonus for knights defended by a pawn
            uint64_t pawnAttacks = attacksByPiece[color == WHITE ? WHITE_PAWN : BLACK_PAWN];

            if (square & pawnAttacks) {
                if (color == WHITE) {
                    whiteMidgameScore += getEvalValue(MIDGAME_KNIGHT_DEFENDED_BY_PAWN);
                    whiteEndgameScore += getEvalValue(ENDGAME_KNIGHT_DEFENDED_BY_PAWN);
                } else {
                    blackMidgameScore += getEvalValue(MIDGAME_KNIGHT_DEFENDED_BY_PAWN);
                    blackEndgameScore += getEvalValue(ENDGAME_KNIGHT_DEFENDED_BY_PAWN);
                }
            }
        }

        // Bishop eval
        if (isBishop(pieceType)) {
            // Bad bishop
            uint64_t bishopAttacks = attacksFrom[squareIndex];
            uint64_t pawnBB = bitboard.getPieceBoard(color == WHITE ? WHITE_PAWN : BLACK_PAWN);
            uint64_t forwardMobility;

            if (color == WHITE) {
                forwardMobility = getRayAttack(squareIndex, NORTH_WEST) | getRayAttack(
                                      squareIndex, NORTH_EAST);
            } else {
                forwardMobility = getRayAttack(squareIndex, SOUTH_WEST) | getRayAttack(
                                      squareIndex, SOUTH_EAST);
            }

            bishopAttacks &= forwardMobility;

            // If one of our own pawns is on the same diagonal as the bishop and the bishop has <= 3
            // squares of mobility, it's a bad bishop
            if (bishopAttacks & pawnBB) {
                uint64_t bishopAttacksWithoutPawns = bishopAttacks & ~pawnBB;
                uint64_t attackCount = popcnt(bishopAttacksWithoutPawns);

                if (attackCount <= 3) {
                    if (color == WHITE) {
                        whiteMidgameScore += getEvalValue(MIDGAME_BAD_BISHOP_PENALTY);
                        whiteEndgameScore += getEvalValue(ENDGAME_BAD_BISHOP_PENALTY);
                    } else {
                        blackMidgameScore += getEvalValue(MIDGAME_BAD_BISHOP_PENALTY);
                        blackEndgameScore += getEvalValue(ENDGAME_BAD_BISHOP_PENALTY);
                    }
                }
            }

            // Only one bishop (no bishop pair)
            if (color == WHITE) {
                if (bitboard.getMaterialCount<WHITE_BISHOP>() == 1) {
                    whiteMidgameScore += getEvalValue(MIDGAME_MISSING_BISHOP_PAIR_PENALTY);
                    whiteEndgameScore += getEvalValue(ENDGAME_MISSING_BISHOP_PAIR_PENALTY);
                }
            } else {
                if (bitboard.getMaterialCount<BLACK_BISHOP>() == 1) {
                    blackMidgameScore += getEvalValue(MIDGAME_MISSING_BISHOP_PAIR_PENALTY);
                    blackEndgameScore += getEvalValue(ENDGAME_MISSING_BISHOP_PAIR_PENALTY);
                }
            }

            // Fianchetto
            if (color == WHITE) {
                if (squareIndex == G2 || squareIndex == B2) {
                    uint64_t fianchettoPattern =
                        nortOne(square) | westOne(square) | eastOne(square);
                    uint64_t antiPattern = noWeOne(square) | noEaOne(square);

                    if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                        whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_FIANCHETTO);
                        whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_FIANCHETTO);
                    }
                }
            } else {
                if (squareIndex == G7 || squareIndex == B7) {
                    uint64_t fianchettoPattern =
                        soutOne(square) | westOne(square) | eastOne(square);
                    uint64_t antiPattern = soWeOne(square) | soEaOne(square);

                    if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                        blackMidgameScore += getEvalValue(MIDGAME_BISHOP_FIANCHETTO);
                        blackEndgameScore += getEvalValue(ENDGAME_BISHOP_FIANCHETTO);
                    }
                }
            }
        }

        // Rook eval
        if (isRook(pieceType)) {
            // Increase in value as pawns disappear
            uint64_t pawnBB = bitboard.getPieceBoard(color == WHITE ? WHITE_PAWN : BLACK_PAWN);
            uint8_t pawnCount = popcnt(pawnBB);

            if (color == WHITE) {
                whiteMidgameScore += getEvalValue(MIDGAME_ROOK_PAWN_COUNT) * (8 - pawnCount);
                whiteEndgameScore += getEvalValue(ENDGAME_ROOK_PAWN_COUNT) * (8 - pawnCount);
            } else {
                blackMidgameScore += getEvalValue(MIDGAME_ROOK_PAWN_COUNT) * (8 - pawnCount);
                blackEndgameScore += getEvalValue(ENDGAME_ROOK_PAWN_COUNT) * (8 - pawnCount);
            }

            // Rook on open file
            if (bitboard.isOpenFile(squareIndex)) {
                if (color == WHITE) {
                    whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_OPEN_FILE);
                    whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_OPEN_FILE);
                } else {
                    blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_OPEN_FILE);
                    blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_OPEN_FILE);
                }
            } else if (bitboard.isSemiOpenFile<color>(squareIndex)) {
                if (color == WHITE) {
                    whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_SEMI_OPEN_FILE);
                    whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_SEMI_OPEN_FILE);
                } else {
                    blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_SEMI_OPEN_FILE);
                    blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_SEMI_OPEN_FILE);
                }
            }

            // Rook on 7th or 8th rank (or 2nd or 1st rank for black)
            if (color == WHITE) {
                if (square & (RANK_8 | RANK_7)) {
                    whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_7TH_RANK);
                    whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_7TH_RANK);
                }
            } else {
                if (square & (RANK_1 | RANK_2)) {
                    blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_7TH_RANK);
                    blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_7TH_RANK);
                }
            }

            uint64_t file = bitboard.getFile(squareIndex);
            uint64_t opponentQueens = bitboard.getPieceBoard(
                color == WHITE ? BLACK_QUEEN : WHITE_QUEEN);

            // Bonus for rook with enemy queen on same file
            if (file & opponentQueens) {
                if (color == WHITE) {
                    whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_QUEEN_FILE);
                    whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_QUEEN_FILE);
                } else {
                    blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_QUEEN_FILE);
                    blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_QUEEN_FILE);
                }
            }
        }

        int midgameScore = 0;
        int endgameScore = 0;
        if (square & weakSquares[color]) {
            if (isKnight(pieceType)) {
                midgameScore += getEvalValue(MIDGAME_KNIGHT_WEAK_SQUARE_PENALTY);
                endgameScore += getEvalValue(ENDGAME_KNIGHT_WEAK_SQUARE_PENALTY);
            } else if (isBishop(pieceType)) {
                midgameScore += getEvalValue(MIDGAME_BISHOP_WEAK_SQUARE_PENALTY);
                endgameScore += getEvalValue(ENDGAME_BISHOP_WEAK_SQUARE_PENALTY);
            } else if (isRook(pieceType)) {
                midgameScore += getEvalValue(MIDGAME_ROOK_WEAK_SQUARE_PENALTY);
                endgameScore += getEvalValue(ENDGAME_ROOK_WEAK_SQUARE_PENALTY);
            } else if (isQueen(pieceType)) {
                midgameScore += getEvalValue(MIDGAME_QUEEN_WEAK_SQUARE_PENALTY);
                endgameScore += getEvalValue(ENDGAME_QUEEN_WEAK_SQUARE_PENALTY);
            }
        }

        if (color == WHITE) {
            whiteMidgameScore += midgameScore;
            whiteEndgameScore += endgameScore;
        } else {
            blackMidgameScore += midgameScore;
            blackEndgameScore += endgameScore;
        }
    }
}

int Evaluation::evaluate() {
    int phase = getPhase();
    int modifier = bitboard.getMovingColor() == WHITE ? 1 : -1;

    initEvalContext(bitboard);

    evaluateMaterial<WHITE>();
    evaluateMaterial<BLACK>();

    evaluatePst<WHITE>();
    evaluatePst<BLACK>();

    evaluatePieces<WHITE>();
    evaluatePieces<BLACK>();

    evaluateThreats<WHITE>();
    evaluateThreats<BLACK>();

    /*evaluateSpace<WHITE>();
    evaluateSpace<BLACK>();*/

    int whiteScore = ((whiteMidgameScore * (256 - phase)) + (whiteEndgameScore * phase)) / 256;
    int blackScore = ((blackMidgameScore * (256 - phase)) + (blackEndgameScore * phase)) / 256;

    return (whiteScore - blackScore) * modifier;
}

template <PieceColor color>
void Evaluation::evaluateMaterial() {
    if (color == WHITE) {
        whiteMidgameScore +=
            bitboard.getMaterialCount<WHITE_PAWN>() * getEvalValue(MIDGAME_PAWN_MATERIAL);
        whiteEndgameScore +=
            bitboard.getMaterialCount<WHITE_PAWN>() * getEvalValue(ENDGAME_PAWN_MATERIAL);

        whiteMidgameScore +=
            bitboard.getMaterialCount<WHITE_KNIGHT>() * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
        whiteEndgameScore +=
            bitboard.getMaterialCount<WHITE_KNIGHT>() * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

        whiteMidgameScore +=
            bitboard.getMaterialCount<WHITE_BISHOP>() * getEvalValue(MIDGAME_BISHOP_MATERIAL);
        whiteEndgameScore +=
            bitboard.getMaterialCount<WHITE_BISHOP>() * getEvalValue(ENDGAME_BISHOP_MATERIAL);

        whiteMidgameScore +=
            bitboard.getMaterialCount<WHITE_ROOK>() * getEvalValue(MIDGAME_ROOK_MATERIAL);
        whiteEndgameScore +=
            bitboard.getMaterialCount<WHITE_ROOK>() * getEvalValue(ENDGAME_ROOK_MATERIAL);

        whiteMidgameScore +=
            bitboard.getMaterialCount<WHITE_QUEEN>() * getEvalValue(MIDGAME_QUEEN_MATERIAL);
        whiteEndgameScore +=
            bitboard.getMaterialCount<WHITE_QUEEN>() * getEvalValue(ENDGAME_QUEEN_MATERIAL);
    } else {
        blackMidgameScore +=
            bitboard.getMaterialCount<BLACK_PAWN>() * getEvalValue(MIDGAME_PAWN_MATERIAL);
        blackEndgameScore +=
            bitboard.getMaterialCount<BLACK_PAWN>() * getEvalValue(ENDGAME_PAWN_MATERIAL);

        blackMidgameScore +=
            bitboard.getMaterialCount<BLACK_KNIGHT>() * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
        blackEndgameScore +=
            bitboard.getMaterialCount<BLACK_KNIGHT>() * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

        blackMidgameScore +=
            bitboard.getMaterialCount<BLACK_BISHOP>() * getEvalValue(MIDGAME_BISHOP_MATERIAL);
        blackEndgameScore +=
            bitboard.getMaterialCount<BLACK_BISHOP>() * getEvalValue(ENDGAME_BISHOP_MATERIAL);

        blackMidgameScore +=
            bitboard.getMaterialCount<BLACK_ROOK>() * getEvalValue(MIDGAME_ROOK_MATERIAL);
        blackEndgameScore +=
            bitboard.getMaterialCount<BLACK_ROOK>() * getEvalValue(ENDGAME_ROOK_MATERIAL);

        blackMidgameScore +=
            bitboard.getMaterialCount<BLACK_QUEEN>() * getEvalValue(MIDGAME_QUEEN_MATERIAL);
        blackEndgameScore +=
            bitboard.getMaterialCount<BLACK_QUEEN>() * getEvalValue(ENDGAME_QUEEN_MATERIAL);
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

template <PieceColor color>
void Evaluation::evaluateSpace() {
    // Will be here until I finally clean up this mess of a class/file
    int evalScores[COLORS][GAME_PHASES]{};

    constexpr PieceType ownPawnType = color == WHITE ? WHITE_PAWN : BLACK_PAWN;
    uint64_t pawnBB = bitboard.getPieceBoard(ownPawnType);
    uint64_t defendedPawns = pawnBB & attacksByPiece[ownPawnType];
    uint64_t pawnAttacks = attacksByPiece[ownPawnType];
    uint64_t pawnSpace = EXTENDED_CENTER_SQUARES & pawnAttacks;
    uint64_t strongCenter = EXTENDED_CENTER_SQUARES & strongSquares[color];
    uint64_t weakCenter = EXTENDED_CENTER_SQUARES & weakSquares[color];
    uint64_t behindPawnSpace;
    uint64_t extendedCenterFiles = C_FILE | D_FILE | E_FILE | F_FILE;

    if (color == WHITE) {
        pawnSpace |= (EXTENDED_CENTER_SQUARES | (RANK_2 & extendedCenterFiles)) &
                          whiteRearSpans(pawnBB);
    } else {
        pawnSpace |= (EXTENDED_CENTER_SQUARES | (RANK_7 & extendedCenterFiles)) &
                          blackRearSpans(pawnBB);
    }

    pawnSpace |= defendedPawns;

    evalScores[color][MIDGAME] += popcnt(pawnSpace) * getEvalValue(MIDGAME_PAWN_SPACE);
    evalScores[color][ENDGAME] += popcnt(pawnSpace) * getEvalValue(ENDGAME_PAWN_SPACE);
    evalScores[color][MIDGAME] += popcnt(strongCenter) * getEvalValue(MIDGAME_STRONG_SPACE_SQUARES);
    evalScores[color][ENDGAME] += popcnt(strongCenter) * getEvalValue(ENDGAME_STRONG_SPACE_SQUARES);
    evalScores[color][MIDGAME] += popcnt(weakCenter) * getEvalValue(
        MIDGAME_WEAK_SPACE_SQUARES_PENALTY);
    evalScores[color][ENDGAME] += popcnt(weakCenter) * getEvalValue(
        ENDGAME_WEAK_SPACE_SQUARES_PENALTY);
    // evalScores[color][MIDGAME] += popcnt(behindPawnSpace) * getEvalValue(MIDGAME_BEHIND_PAWN_SPACE);
    // evalScores[color][ENDGAME] += popcnt(behindPawnSpace) * getEvalValue(ENDGAME_BEHIND_PAWN_SPACE);

    // Will be here until I finally clean up this mess of a class/file
    if (color == WHITE) {
        whiteMidgameScore += evalScores[WHITE][MIDGAME];
        whiteEndgameScore += evalScores[WHITE][ENDGAME];
    } else {
        blackMidgameScore += evalScores[BLACK][MIDGAME];
        blackEndgameScore += evalScores[BLACK][ENDGAME];
    }
}

template <PieceColor color>
void Evaluation::evaluateThreats() {
    // Will be here until I finally clean up this mess of a class/file
    int evalScores[COLORS][GAME_PHASES]{};

    constexpr PieceColor opponentColor = color == WHITE ? BLACK : WHITE;
    constexpr PieceType opponentPawnType = color == WHITE ? BLACK_PAWN : WHITE_PAWN;
    constexpr PieceType opponentBishopType = color == WHITE ? BLACK_BISHOP : WHITE_BISHOP;
    constexpr PieceType opponentKnightType = color == WHITE ? BLACK_KNIGHT : WHITE_KNIGHT;
    constexpr PieceType opponentRookType = color == WHITE ? BLACK_ROOK : WHITE_ROOK;
    uint64_t ownPawns = bitboard.getPieceBoard(color == WHITE ? WHITE_PAWN : BLACK_PAWN);
    uint64_t opponentPawns = bitboard.getPieceBoard(opponentPawnType);
    uint64_t ownBishops = bitboard.getPieceBoard(color == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
    uint64_t ownKnights = bitboard.getPieceBoard(color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT);
    uint64_t ownRooks = bitboard.getPieceBoard(color == WHITE ? WHITE_ROOK : BLACK_ROOK);
    uint64_t ownQueens = bitboard.getPieceBoard(color == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
    uint64_t minorPieces = ownBishops | ownKnights;
    uint64_t majorPieces = ownRooks | ownQueens;

    uint64_t undefendedPawns = ownPawns & ~attacksByColor[color];
    uint64_t hangingPawns = undefendedPawns & attacksByColor[opponentColor];

    evalScores[color][MIDGAME] += popcnt(undefendedPawns) * getEvalValue(
        MIDGAME_UNDEFENDED_PAWN_PENALTY);
    evalScores[color][ENDGAME] += popcnt(undefendedPawns) * getEvalValue(
        ENDGAME_UNDEFENDED_PAWN_PENALTY);
    evalScores[color][MIDGAME] += popcnt(hangingPawns) * getEvalValue(MIDGAME_HANGING_PAWN_PENALTY);
    evalScores[color][ENDGAME] += popcnt(hangingPawns) * getEvalValue(ENDGAME_HANGING_PAWN_PENALTY);

    uint64_t opponentPawnPushes = bitboard.getPawnDoublePush<opponentColor>(opponentPawns);
    uint64_t undefendedMinorPieces = minorPieces & ~attacksByColor[color];
    uint64_t hangingMinorPieces = undefendedMinorPieces & attacksByColor[opponentColor];
    uint64_t minorPiecesAttackedByPawns = minorPieces & attacksByPiece[opponentPawnType];
    uint64_t opponentPawnPushAttacks = calculatePawnAttacks<opponentColor>(opponentPawnPushes);
    uint64_t minorPiecesThreatenedByPawn = opponentPawnPushAttacks & minorPieces;

    evalScores[color][MIDGAME] += popcnt(undefendedMinorPieces) * getEvalValue(
        MIDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY);
    evalScores[color][ENDGAME] += popcnt(undefendedMinorPieces) * getEvalValue(
        ENDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY);
    evalScores[color][MIDGAME] += popcnt(hangingMinorPieces) * getEvalValue(
        MIDGAME_HANGING_MINOR_PIECE_PENALTY);
    evalScores[color][ENDGAME] += popcnt(hangingMinorPieces) * getEvalValue(
        ENDGAME_HANGING_MINOR_PIECE_PENALTY);
    evalScores[color][MIDGAME] += popcnt(minorPiecesAttackedByPawns) * getEvalValue(
        MIDGAME_MINOR_PIECE_ATTACKED_BY_PAWN_PENALTY);
    evalScores[color][ENDGAME] += popcnt(minorPiecesAttackedByPawns) * getEvalValue(
        ENDGAME_MINOR_PIECE_ATTACKED_BY_PAWN_PENALTY);
    evalScores[color][MIDGAME] += popcnt(minorPiecesThreatenedByPawn) * getEvalValue(
        MIDGAME_MINOR_PIECE_PAWN_THREAT_PENALTY);
    evalScores[color][ENDGAME] += popcnt(minorPiecesThreatenedByPawn) * getEvalValue(
        ENDGAME_MINOR_PIECE_PAWN_THREAT_PENALTY);

    uint64_t attacksByMinorPieces = attacksByPiece[opponentBishopType] | attacksByPiece[
                                        opponentKnightType];
    uint64_t undefendedMajorPieces = majorPieces & ~attacksByColor[color];
    uint64_t hangingMajorPieces = (undefendedMajorPieces & attacksByColor[opponentColor]) | (
                                      majorPieces & attacksByMinorPieces) | (
                                      ownQueens & attacksByPiece[opponentRookType]);
    uint64_t majorPiecesAttackedByPawns = majorPieces & attacksByPiece[opponentPawnType];
    uint64_t majorPiecesThreatenedByPawns = opponentPawnPushAttacks & majorPieces;

    evalScores[color][MIDGAME] += popcnt(undefendedMajorPieces) * getEvalValue(
        MIDGAME_MAJOR_PIECE_NOT_DEFENDED_PENALTY);
    evalScores[color][ENDGAME] += popcnt(undefendedMajorPieces) * getEvalValue(
        ENDGAME_MAJOR_PIECE_NOT_DEFENDED_PENALTY);
    evalScores[color][MIDGAME] += popcnt(hangingMajorPieces) * getEvalValue(
        MIDGAME_HANGING_MAJOR_PIECE_PENALTY);
    evalScores[color][ENDGAME] += popcnt(hangingMajorPieces) * getEvalValue(
        ENDGAME_HANGING_MAJOR_PIECE_PENALTY);
    evalScores[color][MIDGAME] += popcnt(majorPiecesAttackedByPawns) * getEvalValue(
        MIDGAME_MAJOR_PIECE_ATTACKED_BY_PAWN_PENALTY);
    evalScores[color][ENDGAME] += popcnt(majorPiecesAttackedByPawns) * getEvalValue(
        ENDGAME_MAJOR_PIECE_ATTACKED_BY_PAWN_PENALTY);
    evalScores[color][MIDGAME] += popcnt(majorPiecesThreatenedByPawns) * getEvalValue(
        MIDGAME_MAJOR_PIECE_THREATED_PENALTY);
    evalScores[color][ENDGAME] += popcnt(majorPiecesThreatenedByPawns) * getEvalValue(
        ENDGAME_MAJOR_PIECE_THREATED_PENALTY);

    // Will be here until I finally clean up this mess of a class/file
    if (color == WHITE) {
        whiteMidgameScore += evalScores[WHITE][MIDGAME];
        whiteEndgameScore += evalScores[WHITE][ENDGAME];
    } else {
        blackMidgameScore += evalScores[BLACK][MIDGAME];
        blackEndgameScore += evalScores[BLACK][ENDGAME];
    }
}
} // namespace Zagreus