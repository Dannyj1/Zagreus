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

#pragma once

#include <map>

#include "bitboard.h"
#include "constants.h"
#include "features.h"

namespace Zagreus {
enum TraceMetric {
    WHITE_MATERIAL,
    BLACK_MATERIAL,
    WHITE_PST,
    BLACK_PST,
    WHITE_MOBILITY,
    BLACK_MOBILITY,
    WHITE_KING_SAFETY_ATTACKS,
    BLACK_KING_SAFETY_ATTACKS,
    WHITE_KING_SAFETY,
    BLACK_KING_SAFETY,
    WHITE_PAWN_EVAL,
    BLACK_PAWN_EVAL,
    WHITE_KNIGHT_EVAL,
    BLACK_KNIGHT_EVAL,
    WHITE_BISHOP_EVAL,
    BLACK_BISHOP_EVAL,
    WHITE_ROOK_EVAL,
    BLACK_ROOK_EVAL,
    WHITE_QUEEN_EVAL,
    BLACK_QUEEN_EVAL,
    WHITE_UNDEFENDED_PIECES,
    BLACK_UNDEFENDED_PIECES,
};

class Evaluation {
private:
    Bitboard& bitboard;
    std::map<TraceMetric, int> midgameTraceMetrics{};
    std::map<TraceMetric, int> endgameTraceMetrics{};
    std::map<TraceMetric, int> taperedTraceMetrics{};

    uint64_t attacksByPiece[PIECE_TYPES]{};
    uint64_t attacksByColor[COLORS]{};
    uint64_t attackedBy2[COLORS]{};
    uint64_t attacksFrom[SQUARES]{};

    int whiteMidgameScore = 0;
    int whiteEndgameScore = 0;
    int blackMidgameScore = 0;
    int blackEndgameScore = 0;

    int getPhase();

    template <PieceColor color, bool trace>
    void evaluateMaterial();

    template <PieceColor color, bool trace>
    void evaluatePst();

    template <PieceColor color, bool trace>
    void evaluatePieces();

    void initEvalContext(Bitboard& bitboard);

public:
    Evaluation(Bitboard& bitboard)
        : bitboard(bitboard) {
    }

    template <bool trace>
    int evaluate();

    std::map<TraceMetric, int> getMidgameTraceMetrics();

    std::map<TraceMetric, int> getEndgameTraceMetrics();

    std::map<TraceMetric, int> getTaperedTraceMetrics();
};
} // namespace Zagreus