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
    int evalValues[70] = { 97, 108, 363, 353, 383, 364, 536, 529, 1017, 1008, 7, 0, 5, 4, 0, 6, 0, 19, 19, 7, -7, 0, 1, 11, 7, 0, -9, -5, -12, 0, -9, -25, -11, -13, 0, 23, 8, 3, -8, -13, -15, -14, -4, -2, 11, 12, -2, 10, -5, -5, -30, -23, 20, 10, 8, 10, 29, 22, 14, 6, 3, 12, 12, 4, 7, 21, 12, 2, 10, 2,  };

    int baseEvalValues[70] = {
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
        -2, // ENDGAME_KING_ATTACK_PAWN_PENALTY
        -5, // MIDGAME_KING_ATTACK_KNIGHT_PENALTY
        -7, // ENDGAME_KING_ATTACK_KNIGHT_PENALTY
        -7, // MIDGAME_KING_ATTACK_BISHOP_PENALTY
        -10, // ENDGAME_KING_ATTACK_BISHOP_PENALTY
        -10, // MIDGAME_KING_ATTACK_ROOK_PENALTY
        -15, // ENDGAME_KING_ATTACK_ROOK_PENALTY
        -15, // MIDGAME_KING_ATTACK_QUEEN_PENALTY
        -20, // ENDGAME_KING_ATTACK_QUEEN_PENALTY
        -10, // MIDGAME_DOUBLED_PAWN_PENALTY
        -15, // ENDGAME_DOUBLED_PAWN_PENALTY
        10, // MIDGAME_PASSED_PAWN
        20, // ENDGAME_PASSED_PAWN
        -5, // MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY
        -10, // ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY
        -10, // MIDGAME_ISOLATED_PAWN_PENALTY
        -15, // ENDGAME_ISOLATED_PAWN_PENALTY
        -3, // MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY
        -6, // ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY
        -5, // MIDGAME_KNIGHT_MISSING_PAWN_PENALTY
        -3, // ENDGAME_KNIGHT_MISSING_PAWN_PENALTY
        5, // MIDGAME_KNIGHT_DEFENDED_BY_PAWN
        3, // ENDGAME_KNIGHT_DEFENDED_BY_PAWN
        -5, // MIDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY
        -5, // ENDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY
        -15, // MIDGAME_BAD_BISHOP_PENALTY
        -10, // ENDGAME_BAD_BISHOP_PENALTY
        -50, // MIDGAME_MISSING_BISHOP_PAIR_PENALTY
        -50, // ENDGAME_MISSING_BISHOP_PAIR_PENALTY
        15, // MIDGAME_BISHOP_FIANCHETTO
        0, // ENDGAME_BISHOP_FIANCHETTO
        5, // MIDGAME_ROOK_PAWN_COUNT
        5, // ENDGAME_ROOK_PAWN_COUNT
        10, // MIDGAME_ROOK_ON_OPEN_FILE
        15, // ENDGAME_ROOK_ON_OPEN_FILE
        5, // MIDGAME_ROOK_ON_SEMI_OPEN_FILE
        7, // ENDGAME_ROOK_ON_SEMI_OPEN_FILE
        10, // MIDGAME_ROOK_ON_7TH_RANK
        15, // MIDGAME_ROOK_ON_7TH_RANK
        0, // MIDGAME_TARRASCH_OWN_ROOK_PENALTY
        -10, // ENDGAME_TARRASCH_OWN_ROOK_PENALTY
        0, // MIDGAME_TARRASCH_OWN_ROOK_DEFEND
        10, // ENDGAME_TARRASCH_OWN_ROOK_DEFEND
        0, // MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY
        -10, // ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY
        6, // MIDGAME_ROOK_ON_QUEEN_FILE
        3, // ENDGAME_ROOK_ON_QUEEN_FILE
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
