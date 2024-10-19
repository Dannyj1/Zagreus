/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

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

#include "eval.h"

#include "eval_features.h"

namespace Zagreus {
/**
 * \brief Evaluates the current board position.
 *
 * This function calculates the evaluation score of the current board position
 * based on material and other features. It considers the phase of the game
 * (midgame or endgame) and adjusts the scores accordingly (tapered eval).
 *
 * \return The evaluation score of the current board position.
 */
int Evaluation::evaluate() {
    const int phase = calculatePhase();
    const int modifier = board.getSideToMove() == WHITE ? 1 : -1;

    evaluateMaterial();

    const int whiteScore = ((whiteMidgameScore * (256 - phase)) + (whiteEndgameScore * phase)) / 256;
    const int blackScore = ((blackMidgameScore * (256 - phase)) + (blackEndgameScore * phase)) / 256;

    return (whiteScore - blackScore) * modifier;
}

constexpr int knightPhase = 1;
constexpr int bishopPhase = 1;
constexpr int rookPhase = 2;
constexpr int queenPhase = 4;
constexpr int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

/**
 * \brief Calculates the phase of the game.
 *
 * This function calculates the phase of the game (midgame or endgame) based on
 * the remaining pieces on the board. The phase is used to adjust the evaluation
 * scores accordingly.
 *
 * \return The phase of the game as an integer.
 */
int Evaluation::calculatePhase() const {
    int phase = totalPhase;

    phase -= (board.getPieceCount<WHITE_KNIGHT>() + board.getPieceCount<BLACK_KNIGHT>()) * knightPhase;
    phase -= (board.getPieceCount<WHITE_BISHOP>() + board.getPieceCount<BLACK_BISHOP>()) * bishopPhase;
    phase -= (board.getPieceCount<WHITE_ROOK>() + board.getPieceCount<BLACK_ROOK>()) * rookPhase;
    phase -= (board.getPieceCount<WHITE_QUEEN>() + board.getPieceCount<BLACK_QUEEN>()) * queenPhase;

    return (phase * 256 + (totalPhase / 2)) / totalPhase;
}

/**
 * \brief Evaluates the material on the board.
 *
 * This function evaluates the material on the board for both sides (white and black)
 * and updates the midgame and endgame scores accordingly.
 */
void Evaluation::evaluateMaterial() {
    for (int pieceIndex = 0; pieceIndex < PIECES; pieceIndex++) {
        const Piece piece = static_cast<Piece>(pieceIndex);
        const int pieceCount = board.getPieceCount(piece);

        switch (piece) {
            case WHITE_PAWN:
                whiteMidgameScore += pieceCount * MATERIAL_MIDGAME_PAWN_VALUE;
                whiteEndgameScore += pieceCount * MATERIAL_ENDGAME_PAWN_VALUE;
                break;
            case BLACK_PAWN:
                blackMidgameScore += pieceCount * MATERIAL_MIDGAME_PAWN_VALUE;
                blackEndgameScore += pieceCount * MATERIAL_ENDGAME_PAWN_VALUE;
                break;
            case WHITE_KNIGHT:
                whiteMidgameScore += pieceCount * MATERIAL_MIDGAME_KNIGHT_VALUE;
                whiteEndgameScore += pieceCount * MATERIAL_ENDGAME_KNIGHT_VALUE;
                break;
            case BLACK_KNIGHT:
                blackMidgameScore += pieceCount * MATERIAL_MIDGAME_KNIGHT_VALUE;
                blackEndgameScore += pieceCount * MATERIAL_ENDGAME_KNIGHT_VALUE;
                break;
            case WHITE_BISHOP:
                whiteMidgameScore += pieceCount * MATERIAL_MIDGAME_BISHOP_VALUE;
                whiteEndgameScore += pieceCount * MATERIAL_ENDGAME_BISHOP_VALUE;
                break;
            case BLACK_BISHOP:
                blackMidgameScore += pieceCount * MATERIAL_MIDGAME_BISHOP_VALUE;
                blackEndgameScore += pieceCount * MATERIAL_ENDGAME_BISHOP_VALUE;
                break;
            case WHITE_ROOK:
                whiteMidgameScore += pieceCount * MATERIAL_MIDGAME_ROOK_VALUE;
                whiteEndgameScore += pieceCount * MATERIAL_ENDGAME_ROOK_VALUE;
                break;
            case BLACK_ROOK:
                blackMidgameScore += pieceCount * MATERIAL_MIDGAME_ROOK_VALUE;
                blackEndgameScore += pieceCount * MATERIAL_ENDGAME_ROOK_VALUE;
                break;
            case WHITE_QUEEN:
                whiteMidgameScore += pieceCount * MATERIAL_MIDGAME_QUEEN_VALUE;
                whiteEndgameScore += pieceCount * MATERIAL_ENDGAME_QUEEN_VALUE;
                break;
            case BLACK_QUEEN:
                blackMidgameScore += pieceCount * MATERIAL_MIDGAME_QUEEN_VALUE;
                blackEndgameScore += pieceCount * MATERIAL_ENDGAME_QUEEN_VALUE;
                break;
            default:
                break;
        }
    }
}
} // namespace Zagreus
