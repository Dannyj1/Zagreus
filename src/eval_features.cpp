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

#include "eval_features.h"

#include <vector>

namespace Zagreus {
static std::vector baseEvalFeatures{
    100, // MATERIAL_MIDGAME_PAWN_VALUE
    350, // MATERIAL_MIDGAME_KNIGHT_VALUE
    350, // MATERIAL_MIDGAME_BISHOP_VALUE
    525, // MATERIAL_MIDGAME_ROOK_VALUE
    1000, // MATERIAL_MIDGAME_QUEEN_VALUE
    100, // MATERIAL_ENDGAME_PAWN_VALUE
    350, // MATERIAL_ENDGAME_KNIGHT_VALUE
    350, // MATERIAL_ENDGAME_BISHOP_VALUE
    525, // MATERIAL_ENDGAME_ROOK_VALUE
    1000, // MATERIAL_ENDGAME_QUEEN_VALUE
    4, // MOBILITY_MIDGAME_KNIGHT_VALUE
    6, // MOBILITY_MIDGAME_BISHOP_VALUE
    2, // MOBILITY_MIDGAME_ROOK_VALUE
    4, // MOBILITY_MIDGAME_QUEEN_VALUE
    2, // MOBILITY_ENDGAME_KNIGHT_VALUE
    3, // MOBILITY_ENDGAME_BISHOP_VALUE
    5, // MOBILITY_ENDGAME_ROOK_VALUE
    6 // MOBILITY_ENDGAME_QUEEN_VALUE
};

static int midgamePstStartIndex = 0;
static int endgamePstStartIndex = 0;

static std::vector<int> evalFeatures{};

void initializeEvalFeatures() {
    initializePst();

    for (const int feature : baseEvalFeatures) {
        evalFeatures.push_back(feature);
    }

    midgamePstStartIndex = evalFeatures.size();

    for (Piece pieces = WHITE_PAWN; pieces <= BLACK_KING; pieces++) {
        for (Square square = A1; square <= H8; square++) {
            baseEvalFeatures.push_back(getMidgamePstBaseValue(pieces, square));
            evalFeatures.push_back(getMidgamePstBaseValue(pieces, square));
        }
    }

    endgamePstStartIndex = evalFeatures.size();

    for (Piece pieces = WHITE_PAWN; pieces <= BLACK_KING; pieces++) {
        for (Square square = A1; square <= H8; square++) {
            baseEvalFeatures.push_back(getEndgamePstBaseValue(pieces, square));
            evalFeatures.push_back(getEndgamePstBaseValue(pieces, square));
        }
    }
}

void updateFeatureValues(const std::vector<int>& values) {
    evalFeatures = values;
}

int getEvalFeatureValue(const EvalFeature feature) {
    return evalFeatures[static_cast<int>(feature)];
}

int getMidgamePstEvalFeature(const Piece piece, const Square square) {
    return evalFeatures[midgamePstStartIndex + piece * 64 + square];
}

int getEndgamePstEvalFeature(const Piece piece, const Square square) {
    return evalFeatures[endgamePstStartIndex + piece * 64 + square];
}
} // namespace Zagreus
