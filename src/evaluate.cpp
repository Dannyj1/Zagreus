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
    Evaluation::Evaluation(Bitboard &bitboard) : bitboard(bitboard) {
        uint64_t whitePawnAttacks = calculatePawnAttacks<PieceColor::WHITE>(bitboard.getPieceBoard(PieceType::WHITE_PAWN));
        attacksByPiece[PieceType::WHITE_PAWN] |= whitePawnAttacks;
        attacksByColor[PieceColor::WHITE] |= whitePawnAttacks;
        combinedAttacks |= whitePawnAttacks;

        uint64_t whiteKnightAttacks = calculateKnightAttacks(bitboard.getPieceBoard(PieceType::WHITE_KNIGHT));
        attacksByPiece[PieceType::WHITE_KNIGHT] |= whiteKnightAttacks;
        attacksByColor[PieceColor::WHITE] |= whiteKnightAttacks;
        combinedAttacks |= whiteKnightAttacks;

        uint64_t whiteKingAttacks = bitboard.getKingAttacks(popcnt(bitboard.getPieceBoard(PieceType::WHITE_KING)));
        attacksByPiece[PieceType::WHITE_KING] |= whiteKingAttacks;
        attacksByColor[PieceColor::WHITE] |= whiteKingAttacks;
        combinedAttacks |= whiteKingAttacks;

        uint64_t blackPawnAttacks = calculatePawnAttacks<PieceColor::BLACK>(bitboard.getPieceBoard(PieceType::BLACK_PAWN));
        attacksByPiece[PieceType::BLACK_PAWN] |= blackPawnAttacks;
        attacksByColor[PieceColor::BLACK] |= blackPawnAttacks;
        combinedAttacks |= blackPawnAttacks;

        uint64_t blackKnightAttacks = calculateKnightAttacks(bitboard.getPieceBoard(PieceType::BLACK_KNIGHT));
        attacksByPiece[PieceType::BLACK_KNIGHT] |= blackKnightAttacks;
        attacksByColor[PieceColor::BLACK] |= blackKnightAttacks;
        combinedAttacks |= blackKnightAttacks;

        uint64_t blackKingAttacks = bitboard.getKingAttacks(popcnt(bitboard.getPieceBoard(PieceType::BLACK_KING)));
        attacksByPiece[PieceType::BLACK_KING] |= blackKingAttacks;
        attacksByColor[PieceColor::BLACK] |= blackKingAttacks;
        combinedAttacks |= blackKingAttacks;

        uint64_t whiteBishops = bitboard.getPieceBoard(PieceType::WHITE_BISHOP);
        while (whiteBishops) {
            int square = popLsb(whiteBishops);
            uint64_t attacks = bitboard.getBishopAttacks(square);

            attacksByPiece[PieceType::WHITE_BISHOP] |= attacks;
            attacksByColor[PieceColor::WHITE] |= attacks;
            combinedAttacks |= attacks;
        }

        uint64_t whiteRooks = bitboard.getPieceBoard(PieceType::WHITE_ROOK);
        while (whiteRooks) {
            int square = popLsb(whiteRooks);
            uint64_t attacks = bitboard.getRookAttacks(square);

            attacksByPiece[PieceType::WHITE_ROOK] |= attacks;
            attacksByColor[PieceColor::WHITE] |= attacks;
            combinedAttacks |= attacks;
        }

        uint64_t whiteQueens = bitboard.getPieceBoard(PieceType::WHITE_QUEEN);
        while (whiteQueens) {
            int square = popLsb(whiteQueens);
            uint64_t attacks = bitboard.getQueenAttacks(square);

            attacksByPiece[PieceType::WHITE_QUEEN] |= attacks;
            attacksByColor[PieceColor::WHITE] |= attacks;
            combinedAttacks |= attacks;
        }

        uint64_t blackBishops = bitboard.getPieceBoard(PieceType::BLACK_BISHOP);
        while (blackBishops) {
            int square = popLsb(blackBishops);
            uint64_t attacks = bitboard.getBishopAttacks(square);

            attacksByPiece[PieceType::BLACK_BISHOP] |= attacks;
            attacksByColor[PieceColor::BLACK] |= attacks;
            combinedAttacks |= attacks;
        }

        uint64_t blackRooks = bitboard.getPieceBoard(PieceType::BLACK_ROOK);
        while (blackRooks) {
            int square = popLsb(blackRooks);
            uint64_t attacks = bitboard.getRookAttacks(square);

            attacksByPiece[PieceType::BLACK_ROOK] |= attacks;
            attacksByColor[PieceColor::BLACK] |= attacks;
            combinedAttacks |= attacks;
        }

        uint64_t blackQueens = bitboard.getPieceBoard(PieceType::BLACK_QUEEN);
        while (blackQueens) {
            int square = popLsb(blackQueens);
            uint64_t attacks = bitboard.getQueenAttacks(square);

            attacksByPiece[PieceType::BLACK_QUEEN] |= attacks;
            attacksByColor[PieceColor::BLACK] |= attacks;
            combinedAttacks |= attacks;
        }
    }

    constexpr int knightPhase = 1;
    constexpr int bishopPhase = 1;
    constexpr int rookPhase = 2;
    constexpr int queenPhase = 4;
    constexpr int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

    int Evaluation::getPhase() {
        int phase = totalPhase;

        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_KNIGHT) | bitboard.getPieceBoard(PieceType::BLACK_KNIGHT)) * knightPhase;
        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_BISHOP) | bitboard.getPieceBoard(PieceType::BLACK_BISHOP)) * bishopPhase;
        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_ROOK) | bitboard.getPieceBoard(PieceType::BLACK_ROOK)) * rookPhase;
        phase -= popcnt(bitboard.getPieceBoard(PieceType::WHITE_QUEEN) | bitboard.getPieceBoard(PieceType::BLACK_QUEEN)) * queenPhase;

        return (phase * 256 + (totalPhase / 2)) / totalPhase;
    }

    template<bool trace>
    int Evaluation::evaluate() {
        int phase = getPhase();
        int modifier = bitboard.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        evaluateMaterial<PieceColor::WHITE, trace>();
        evaluateMaterial<PieceColor::BLACK, trace>();

        int whiteScore = ((whiteMidgameScore * (256 - phase)) + (whiteEndgameScore * phase)) / 256;
        int blackScore = ((blackMidgameScore * (256 - phase)) + (blackEndgameScore * phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    template int Evaluation::evaluate<true>();
    template int Evaluation::evaluate<false>();

    template<PieceColor color, bool trace>
    void Evaluation::evaluateMaterial() {
        if (color == PieceColor::WHITE) {
            int whitePawnCount = bitboard.getMaterialCount<WHITE_PAWN>();
            int whiteKnightCount = bitboard.getMaterialCount<WHITE_KNIGHT>();
            int whiteBishopCount = bitboard.getMaterialCount<WHITE_BISHOP>();
            int whiteRookCount = bitboard.getMaterialCount<WHITE_ROOK>();
            int whiteQueenCount = bitboard.getMaterialCount<WHITE_QUEEN>();
            int startMidgameScore = whiteMidgameScore;
            int startEndgameScore = whiteEndgameScore;

            whiteMidgameScore += whitePawnCount * getEvalValue(MIDGAME_PAWN_MATERIAL);
            whiteEndgameScore += whitePawnCount * getEvalValue(ENDGAME_PAWN_MATERIAL);

            whiteMidgameScore += whiteKnightCount * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
            whiteEndgameScore += whiteKnightCount * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

            whiteMidgameScore += whiteBishopCount * getEvalValue(MIDGAME_BISHOP_MATERIAL);
            whiteEndgameScore += whiteBishopCount * getEvalValue(ENDGAME_BISHOP_MATERIAL);

            whiteMidgameScore += whiteRookCount * getEvalValue(MIDGAME_ROOK_MATERIAL);
            whiteEndgameScore += whiteRookCount * getEvalValue(ENDGAME_ROOK_MATERIAL);

            whiteMidgameScore += whiteQueenCount * getEvalValue(MIDGAME_QUEEN_MATERIAL);
            whiteEndgameScore += whiteQueenCount * getEvalValue(ENDGAME_QUEEN_MATERIAL);

            if (trace) {
                traceMetrics[WHITE_MIDGAME_MATERIAL] = whiteMidgameScore - startMidgameScore;
                traceMetrics[WHITE_ENDGAME_MATERIAL] = whiteEndgameScore - startEndgameScore;
            }
        } else {
            int blackPawnCount = bitboard.getMaterialCount<BLACK_PAWN>();
            int blackKnightCount = bitboard.getMaterialCount<BLACK_KNIGHT>();
            int blackBishopCount = bitboard.getMaterialCount<BLACK_BISHOP>();
            int blackRookCount = bitboard.getMaterialCount<BLACK_ROOK>();
            int blackQueenCount = bitboard.getMaterialCount<BLACK_QUEEN>();
            int startMidgameScore = blackMidgameScore;
            int startEndgameScore = blackEndgameScore;

            blackMidgameScore += blackPawnCount * getEvalValue(MIDGAME_PAWN_MATERIAL);
            blackEndgameScore += blackPawnCount * getEvalValue(ENDGAME_PAWN_MATERIAL);

            blackMidgameScore += blackKnightCount * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
            blackEndgameScore += blackKnightCount * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

            blackMidgameScore += blackBishopCount * getEvalValue(MIDGAME_BISHOP_MATERIAL);
            blackEndgameScore += blackBishopCount * getEvalValue(ENDGAME_BISHOP_MATERIAL);

            blackMidgameScore += blackRookCount * getEvalValue(MIDGAME_ROOK_MATERIAL);
            blackEndgameScore += blackRookCount * getEvalValue(ENDGAME_ROOK_MATERIAL);

            blackMidgameScore += blackQueenCount * getEvalValue(MIDGAME_QUEEN_MATERIAL);
            blackEndgameScore += blackQueenCount * getEvalValue(ENDGAME_QUEEN_MATERIAL);

            if (trace) {
                traceMetrics[BLACK_MIDGAME_MATERIAL] = blackMidgameScore - startMidgameScore;
                traceMetrics[BLACK_ENDGAME_MATERIAL] = blackEndgameScore - startEndgameScore;
            }
        }
    }
}