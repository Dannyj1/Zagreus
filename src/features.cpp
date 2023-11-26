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

#include "features.h"
#include "pst.h"
#include <iostream>

namespace Zagreus {
    int evalValues[32] = { 84, 99, 348, 348, 379, 354, 535, 533, 1006, 1004, 5, 0, 6, 1, 0, 9, 0, 16, 17, 3, -8, 0, 0, 6, 5, 0, -9, -4, -13, -3, -11, -20,  };

    int baseEvalValues[32] = {
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
            7, // MIDGAME_KNIGHT_MOBILITY
            2, // ENDGAME_KNIGHT_MOBILITY
            8, // MIDGAME_BISHOP_MOBILITY
            3, // ENDGAME_BISHOP_MOBILITY
            2, // MIDGAME_ROOK_MOBILITY
            6, // ENDGAME_ROOK_MOBILITY
            4, // MIDGAME_QUEEN_MOBILITY
            8, // ENDGAME_QUEEN_MOBILITY
            20, // MIDGAME_PAWN_SHIELD
            0, // ENDGAME_PAWN_SHIELD
            -5, // MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY
            0, // ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY
            -2, // MIDGAME_KING_ATTACK_PAWN_PENALTY
            0, // ENDGAME_KING_ATTACK_PAWN_PENALTY
            -5, // MIDGAME_KING_ATTACK_KNIGHT_PENALTY
            -7, // ENDGAME_KING_ATTACK_KNIGHT_PENALTY
            -7, // MIDGAME_KING_ATTACK_BISHOP_PENALTY
            -10, // ENDGAME_KING_ATTACK_BISHOP_PENALTY
            -10, // MIDGAME_KING_ATTACK_ROOK_PENALTY
            -15, // ENDGAME_KING_ATTACK_ROOK_PENALTY
            -15, // MIDGAME_KING_ATTACK_QUEEN_PENALTY
            -20 // ENDGAME_KING_ATTACK_QUEEN_PENALTY
    };

    void printEvalValues() {
        for (int i = 0; i < getEvalFeatureSize(); i++) {
            std::cout << evalFeatureNames[i] << ": " << evalValues[i] << std::endl;
        }
    }

    int getEvalValue(EvalFeature feature) {
        return evalValues[feature];
    }

    int getEvalFeatureSize() {
        return sizeof(evalValues) / sizeof(evalValues[0]);
    }

    // Some sane default values for tuning
    std::vector<double> getBaseEvalValues() {
        std::vector<double> values;

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

    void updateEvalValues(std::vector<double> &newValues) {
        int evalFeatureSize = getEvalFeatureSize();
        int pstSize = getMidgameValues().size();

        // New features are given as 10000 times the actual value to deal with the fact that gradients are floats
        for (int i = 0; i < evalFeatureSize; i++) {
            evalValues[i] = (int) newValues[i];
        }

        for (int i = 0; i < 6; i++) {
            for (int8_t j = 0; j < 64; j++) {
                int pieceIndex = i * 2;

                setMidgamePstValue((PieceType) pieceIndex, 63 - j, (int) newValues[evalFeatureSize + i * 64 + j]);
                setMidgamePstValue((PieceType) (pieceIndex + 1), j, (int) newValues[evalFeatureSize + i * 64 + j]);
                setEndgamePstValue((PieceType) pieceIndex, 63 - j, (int) newValues[evalFeatureSize + pstSize + i * 64 + j]);
                setEndgamePstValue((PieceType) (pieceIndex + 1), j, (int) newValues[evalFeatureSize + pstSize + i * 64 + j]);
            }
        }
    }
}
