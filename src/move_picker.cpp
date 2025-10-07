/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

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

#include "move_picker.h"

#include <array>
#include <iostream>

#include "constants.h"
#include "eval.h"
#include "tt.h"

namespace Zagreus {
/**
 * \brief Checks if there is a next move and retrieves it.
 * \param[out] move The next move if available.
 * \return True if there is a next move, false otherwise.
 */
bool MovePicker::next(Move& move) {
    if (currentIndex >= moveList.size) {
        return false;
    }

    int bestIndex = currentIndex;
    int bestScore = scores[currentIndex];

    for (int i = currentIndex + 1; i < moveList.size; ++i) {
        if (scores[i] > bestScore) {
            bestIndex = i;
            bestScore = scores[i];
        }
    }

    std::swap(scores[currentIndex], scores[bestIndex]);
    std::swap(moveList.moves[currentIndex], moveList.moves[bestIndex]);

    move = moveList.moves[currentIndex];

    if (move == NO_MOVE) {
        return false;
    }

    currentIndex += 1;
    return true;
}

/**
 *\brief Resets the move picker to the beginning of the move list.
 */
void MovePicker::reset() { currentIndex = 0; }

static TranspositionTable* tt = TranspositionTable::getTT();

void MovePicker::score(Board& board) {
    const PvLine& pvLine = board.getPreviousPvLine();
    const int currentPly = board.getPly();
    const int pvMoveIndex = currentPly - pvLine.startPly;
    Move pvMove = NO_MOVE;
    Move ttMove = NO_MOVE;

    if (pvLine.moveCount >= pvMoveIndex) {
        pvMove = pvLine.moves[pvMoveIndex];
    }

    const uint64_t zobristHash = board.getZobristHash();
    const TTEntry* entry = tt->getEntry(zobristHash);

    if (entry != nullptr) {
        ttMove = entry->bestMove;

        if (ttMove == pvMove) {
            ttMove = NO_MOVE;
        }
    }

    const Move killerMove1 = tt->getKillerMove(currentPly, 0);
    const Move killerMove2 = tt->getKillerMove(currentPly, 1);

    for (int i = 0; i < moveList.size; ++i) {
        const Move move = moveList.moves[i];
        const MoveType moveType = getMoveType(move);
        const Square toSquare = getToSquare(move);
        Piece capturedPiece = board.getPieceOnSquare(toSquare);

        if (moveType == EN_PASSANT) {
            capturedPiece = board.getSideToMove() == WHITE ? BLACK_PAWN : WHITE_PAWN;
        }

        if (move == pvMove) {
            scores[i] = 5000000;
        } else if (move == ttMove) {
            scores[i] = 2500000;
        } else if (capturedPiece != EMPTY) {
            // SEE for identifying good and bad captures, then further ordering by capture history
            const Piece movingPiece = board.getPieceOnSquare(getFromSquare(move));
            const int captureHistoryValue = tt->getCaptureHistoryValue(move, movingPiece, capturedPiece);
            const int captureScore = (getPieceValue(capturedPiece) * 10) + captureHistoryValue;
            const bool isGoodCapture = board.see(move, 0);
            const int seeScore = isGoodCapture ? 1500000 + captureScore : -1500000 + captureScore;

            scores[i] = seeScore;
        } else if (moveType == PROMOTION) {
            const Piece promotedPiece = getPieceFromPromotionPiece(getPromotionPiece(move), board.getSideToMove());
            const int promotionScore = 1500000 + getPieceValue(promotedPiece);

            scores[i] = promotionScore;
        } else if (move == killerMove1) {
            scores[i] = 900000;
        } else if (move == killerMove2) {
            scores[i] = 800000;
        } else {
            // Ordering of quiet moves
            const int historyValue = tt->getHistoryValue(board.getSideToMove(), move);

            scores[i] = historyValue;
        }
    }
}
}  // namespace Zagreus