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

namespace Zagreus {

    enum TraceMetric {
        WHITE_MIDGAME_MATERIAL,
        WHITE_ENDGAME_MATERIAL,
        BLACK_MIDGAME_MATERIAL,
        BLACK_ENDGAME_MATERIAL,
        WHITE_MIDGAME_PST,
        WHITE_ENDGAME_PST,
        BLACK_MIDGAME_PST,
        BLACK_ENDGAME_PST,
        WHITE_MIDGAME_MOBILITY,
        WHITE_ENDGAME_MOBILITY,
        BLACK_MIDGAME_MOBILITY,
        BLACK_ENDGAME_MOBILITY,
    };

    class Evaluation {
    public:
        Evaluation(Bitboard &bitboard);

        int evaluate();
    private:
        Bitboard &bitboard;
        std::map<TraceMetric, int> traceMetrics{};

        uint64_t attacksByPiece[PIECE_TYPES]{};
        uint64_t attacksByColor[COLORS]{};
        uint64_t attacksFrom[SQUARES]{};
        uint64_t combinedAttacks{};

        int whiteMidgameScore = 0;
        int whiteEndgameScore = 0;
        int blackMidgameScore = 0;
        int blackEndgameScore = 0;

        int getPhase();

        template<PieceColor color>
        void evaluateMaterial();

        template<PieceColor color>
        void evaluatePst();

        template<PieceColor color>
        void evaluatePieces();

        void addMobilityScoreForPiece(PieceType pieceType, int mobility);
    };
}
