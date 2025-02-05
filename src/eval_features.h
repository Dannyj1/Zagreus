
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

#pragma once
#include <vector>

#include "pst.h"

namespace Zagreus {
enum EvalFeature {
    MATERIAL_MIDGAME_PAWN_VALUE,
    MATERIAL_MIDGAME_KNIGHT_VALUE,
    MATERIAL_MIDGAME_BISHOP_VALUE,
    MATERIAL_MIDGAME_ROOK_VALUE,
    MATERIAL_MIDGAME_QUEEN_VALUE,
    MATERIAL_ENDGAME_PAWN_VALUE,
    MATERIAL_ENDGAME_KNIGHT_VALUE,
    MATERIAL_ENDGAME_BISHOP_VALUE,
    MATERIAL_ENDGAME_ROOK_VALUE,
    MATERIAL_ENDGAME_QUEEN_VALUE,
    MOBILITY_MIDGAME_KNIGHT_VALUE,
    MOBILITY_MIDGAME_BISHOP_VALUE,
    MOBILITY_MIDGAME_ROOK_VALUE,
    MOBILITY_MIDGAME_QUEEN_VALUE,
    MOBILITY_ENDGAME_KNIGHT_VALUE,
    MOBILITY_ENDGAME_BISHOP_VALUE,
    MOBILITY_ENDGAME_ROOK_VALUE,
    MOBILITY_ENDGAME_QUEEN_VALUE,
};

void initializeEvalFeatures();

void updateFeatureValues(const std::vector<int>& values);

int getEvalFeatureValue(EvalFeature feature);

int getMidgamePstEvalFeature(Piece piece, Square square);

int getEndgamePstEvalFeature(Piece piece, Square square);
} // namespace Zagreus
