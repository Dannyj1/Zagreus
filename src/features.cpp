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

#include "features.h"

#include <iostream>

#include "pst.h"

namespace Zagreus {
int evalValues[72] = {99, 96, 353, 348, 371, 358, 531, 530, 1007, 1003, 7, 1, 6, 4, 1, 6, 2, 12, 14,
                      3, -6, 0, -2, 8, 3, 1, -6, -6, -10, -2, -9, -19, -11, -11, 0, 13, -3, -6, -8,
                      -7, -16, -12, -4, -7, 4, -3, -5, -5, -4, -15, -33, -30, 13, 6, 6, 7, 21, 18,
                      10, 4, -6, 0, 0, -5, 2, 17, 0, -10, 5, -3, -14, -7,};

int baseEvalValues[72] = {
    100, // MIDGAME_PAWN_MATERIAL
    100, // ENDGAME_PAWN_MATERIAL
    350, // MIDGAME_KNIGHT_MATERIAL
    350, // ENDGAME_KNIGHT_MATERIAL
    350, // MIDGAME_BISHOP_MATERIAL
    350, // ENDGAME_BISHOP_MATERIAL
    525, // MIDGAME_ROOK_MATERIAL
    525, // ENDGAME_ROOK_MATERIAL
    1000, // MIDGAME_QUEEN_MATERIAL
    1000, // ENDGAME_QUEEN_MATERIAL
    4, // MIDGAME_KNIGHT_MOBILITY
    2, // ENDGAME_KNIGHT_MOBILITY
    6, // MIDGAME_BISHOP_MOBILITY
    3, // ENDGAME_BISHOP_MOBILITY
    2, // MIDGAME_ROOK_MOBILITY
    5, // ENDGAME_ROOK_MOBILITY
    4, // MIDGAME_QUEEN_MOBILITY
    6, // ENDGAME_QUEEN_MOBILITY
    5, // MIDGAME_PAWN_SHIELD
    0, // ENDGAME_PAWN_SHIELD
    -3, // MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY
    0, // ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY
    -1, // MIDGAME_KING_ATTACK_PAWN_PENALTY
    -1, // ENDGAME_KING_ATTACK_PAWN_PENALTY
    -3, // MIDGAME_KING_ATTACK_KNIGHT_PENALTY
    -5, // ENDGAME_KING_ATTACK_KNIGHT_PENALTY
    -4, // MIDGAME_KING_ATTACK_BISHOP_PENALTY
    -6, // ENDGAME_KING_ATTACK_BISHOP_PENALTY
    -6, // MIDGAME_KING_ATTACK_ROOK_PENALTY
    -8, // ENDGAME_KING_ATTACK_ROOK_PENALTY
    -10, // MIDGAME_KING_ATTACK_QUEEN_PENALTY
    -13, // ENDGAME_KING_ATTACK_QUEEN_PENALTY
    -4, // MIDGAME_DOUBLED_PAWN_PENALTY
    -8, // ENDGAME_DOUBLED_PAWN_PENALTY
    5, // MIDGAME_PASSED_PAWN
    10, // ENDGAME_PASSED_PAWN
    -3, // MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY
    -6, // ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY
    -6, // MIDGAME_ISOLATED_PAWN_PENALTY
    -10, // ENDGAME_ISOLATED_PAWN_PENALTY
    -2, // MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY
    -4, // ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY
    -1, // MIDGAME_KNIGHT_MISSING_PAWN_PENALTY
    -3, // ENDGAME_KNIGHT_MISSING_PAWN_PENALTY
    2, // MIDGAME_KNIGHT_DEFENDED_BY_PAWN
    2, // ENDGAME_KNIGHT_DEFENDED_BY_PAWN
    -5, // MIDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY
    -5, // ENDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY
    -10, // MIDGAME_BAD_BISHOP_PENALTY
    -15, // ENDGAME_BAD_BISHOP_PENALTY
    -50, // MIDGAME_MISSING_BISHOP_PAIR_PENALTY
    -50, // ENDGAME_MISSING_BISHOP_PAIR_PENALTY
    5, // MIDGAME_BISHOP_FIANCHETTO
    0, // ENDGAME_BISHOP_FIANCHETTO
    1, // MIDGAME_ROOK_PAWN_COUNT
    3, // ENDGAME_ROOK_PAWN_COUNT
    5, // MIDGAME_ROOK_ON_OPEN_FILE
    10, // ENDGAME_ROOK_ON_OPEN_FILE
    2, // MIDGAME_ROOK_ON_SEMI_OPEN_FILE
    4, // ENDGAME_ROOK_ON_SEMI_OPEN_FILE
    5, // MIDGAME_ROOK_ON_7TH_RANK
    8, // MIDGAME_ROOK_ON_7TH_RANK
    0, // MIDGAME_TARRASCH_OWN_ROOK_PENALTY
    -10, // ENDGAME_TARRASCH_OWN_ROOK_PENALTY
    0, // MIDGAME_TARRASCH_OWN_ROOK_DEFEND
    10, // ENDGAME_TARRASCH_OWN_ROOK_DEFEND
    0, // MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY
    -10, // ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY
    4, // MIDGAME_ROOK_ON_QUEEN_FILE
    4, // ENDGAME_ROOK_ON_QUEEN_FILE
    -4, // MIDGAME_MINOR_PIECE_ON_WEAK_SQUARE_PENALTY
    -4, // ENDGAME_MINOR_PIECE_ON_WEAK_SQUARE_PENALTY
};

void printEvalValues() {
    for (int i = 0; i < getEvalFeatureSize(); i++) {
        std::cout << evalFeatureNames[i] << ": " << evalValues[i] << std::endl;
    }
}

int getEvalValue(EvalFeature feature) { return evalValues[feature]; }

int getEvalFeatureSize() { return sizeof(evalValues) / sizeof(evalValues[0]); }

// Some sane default values for tuning
std::vector<double> getBaseEvalValues() {
    std::vector<double> values;

    values.reserve(getEvalFeatureSize());
    for (int i = 0; i < getEvalFeatureSize(); i++) {
        values.emplace_back(baseEvalValues[i]);
    }

    for (int i : getBaseMidgameValues()) {
        values.emplace_back(i);
    }

    for (int i : getBaseEndgameValues()) {
        values.emplace_back(i);
    }

    return values;
}

std::vector<double> getEvalValues() {
    std::vector<double> values;

    values.reserve(getEvalFeatureSize());
    for (int i = 0; i < getEvalFeatureSize(); i++) {
        values.emplace_back(evalValues[i]);
    }

    for (int i : getMidgameValues()) {
        values.emplace_back(i);
    }

    for (int i : getEndgameValues()) {
        values.emplace_back(i);
    }

    return values;
}

void updateEvalValues(std::vector<double>& newValues) {
    int evalFeatureSize = getEvalFeatureSize();
    size_t pstSize = getMidgameValues().size();

    for (int i = 0; i < evalFeatureSize; i++) {
        evalValues[i] = static_cast<int>(newValues[i]);
    }

    for (int i = 0; i < 6; i++) {
        for (int8_t j = 0; j < 64; j++) {
            int pieceIndex = i * 2;

            setMidgamePstValue(static_cast<PieceType>(pieceIndex), 63 - j,
                               static_cast<int>(newValues[evalFeatureSize + i * 64 + j]));
            setMidgamePstValue(static_cast<PieceType>(pieceIndex + 1), j,
                               static_cast<int>(newValues[evalFeatureSize + i * 64 + j]));
            setEndgamePstValue(static_cast<PieceType>(pieceIndex), 63 - j,
                               static_cast<int>(newValues[evalFeatureSize + pstSize + i * 64 + j]));
            setEndgamePstValue(static_cast<PieceType>(pieceIndex + 1), j,
                               static_cast<int>(newValues[evalFeatureSize + pstSize + i * 64 + j]));
        }
    }
}
} // namespace Zagreus