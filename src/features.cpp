/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "features.h"
#include "pst.h"
#include <iostream>

namespace Zagreus {
    int evalValues[80] = {
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
            2, // MIDGAME_SQUARE_DEFENDED_BY_PAWN
            2, // ENDGAME_SQUARE_DEFENDED_BY_PAWN
            10, // MIDGAME_CENTER_CONTROL
            10, // ENDGAME_CENTER_CONTROL
            5, // MIDGAME_EXTENDED_CENTER_CONTROL
            5, // ENDGAME_EXTENDED_CENTER_CONTROL
            7, // MIDGAME_KNIGHT_MOBILITY
            2, // ENDGAME_KNIGHT_MOBILITY
            8, // MIDGAME_BISHOP_MOBILITY
            3, // ENDGAME_BISHOP_MOBILITY
            2, // MIDGAME_ROOK_MOBILITY
            6, // ENDGAME_ROOK_MOBILITY
            4, // MIDGAME_QUEEN_MOBILITY
            8, // ENDGAME_QUEEN_MOBILITY
            -50, // MIDGAME_KING_ON_OPEN_FILE
            0, // ENDGAME_KING_ON_OPEN_FILE
            -25, // MIDGAME_KING_NEXT_TO_OPEN_FILE
            0, // ENDGAME_KING_NEXT_TO_OPEN_FILE
            -25, // MIDGAME_NO_CASTLING_RIGHTS
            0, // ENDGAME_NO_CASTLING_RIGHTS
            -5, // MIDGAME_QUEENSIDE_CASTLING_PREVENTED
            0, // ENDGAME_QUEENSIDE_CASTLING_PREVENTED
            -10, // MIDGAME_KINGSIDE_CASTLING_PREVENTED
            0, // ENDGAME_KINGSIDE_CASTLING_PREVENTED
            1, // MIDGAME_PAWN_ATTACK_NEAR_KING
            1, // ENDGAME_PAWN_ATTACK_NEAR_KING
            3, // MIDGAME_KNIGHT_ATTACK_NEAR_KING
            3, // ENDGAME_KNIGHT_ATTACK_NEAR_KING
            3, // MIDGAME_BISHOP_ATTACK_NEAR_KING
            3, // ENDGAME_BISHOP_ATTACK_NEAR_KING
            5, // MIDGAME_ROOK_ATTACK_NEAR_KING
            5, // ENDGAME_ROOK_ATTACK_NEAR_KING
            10, // MIDGAME_QUEEN_ATTACK_NEAR_KING
            10, // ENDGAME_QUEEN_ATTACK_NEAR_KING
            10, // MIDGAME_PAWN_CONNECTIVITY
            10, // ENDGAME_PAWN_CONNECTIVITY
            7, // MIDGAME_KNIGHT_CONNECTIVITY
            7, // ENDGAME_KNIGHT_CONNECTIVITY
            7, // MIDGAME_BISHOP_CONNECTIVITY
            7, // ENDGAME_BISHOP_CONNECTIVITY
            3, // MIDGAME_ROOK_CONNECTIVITY
            3, // ENDGAME_ROOK_CONNECTIVITY
            1, // MIDGAME_QUEEN_CONNECTIVITY
            1, // ENDGAME_QUEEN_CONNECTIVITY
            20, // MIDGAME_ROOK_ON_OPEN_FILE
            20, // ENDGAME_ROOK_ON_OPEN_FILE
            15, // MIDGAME_ROOK_ON_SEMI_OPEN_FILE
            15, // ENDGAME_ROOK_ON_SEMI_OPEN_FILE
            10, // MIDGAME_ROOK_ON_7TH_RANK
            10, // ENDGAME_ROOK_ON_7TH_RANK
            5, // MIDGAME_ROOK_ON_QUEEN_FILE
            5, // ENDGAME_ROOK_ON_QUEEN_FILE
            5, // MIDGAME_ROOK_LESS_PAWNS_BONUS
            5, // ENDGAME_ROOK_LESS_PAWNS_BONUS
            -25, // MIDGAME_NO_BISHOP_PAIR
            -25, // ENDGAME_NO_BISHOP_PAIR
            10, // MIDGAME_BISHOP_FIANCHETTO
            0, // ENDGAME_BISHOP_FIANCHETTO
            -10, // MIDGAME_PAWN_ON_SAME_FILE
            -10, // ENDGAME_PAWN_ON_SAME_FILE
            10, // MIDGAME_PASSED_PAWN
            10, // ENDGAME_PASSED_PAWN
            -20, // MIDGAME_ISOLATED_SEMI_OPEN_PAWN
            -20, // ENDGAME_ISOLATED_SEMI_OPEN_PAWN
            -10, // MIDGAME_ISOLATED_PAWN
            -10, // ENDGAME_ISOLATED_PAWN
            3, // MIDGAME_PAWN_SEMI_OPEN_FILE
            3, // ENDGAME_PAWN_SEMI_OPEN_FILE
            20, // MIDGAME_PAWN_SHIELD
            0, // ENDGAME_PAWN_SHIELD
    };

    int baseEvalValues[80] = {
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
            2, // MIDGAME_SQUARE_DEFENDED_BY_PAWN
            2, // ENDGAME_SQUARE_DEFENDED_BY_PAWN
            10, // MIDGAME_CENTER_CONTROL
            10, // ENDGAME_CENTER_CONTROL
            5, // MIDGAME_EXTENDED_CENTER_CONTROL
            5, // ENDGAME_EXTENDED_CENTER_CONTROL
            7, // MIDGAME_KNIGHT_MOBILITY
            2, // ENDGAME_KNIGHT_MOBILITY
            8, // MIDGAME_BISHOP_MOBILITY
            3, // ENDGAME_BISHOP_MOBILITY
            2, // MIDGAME_ROOK_MOBILITY
            6, // ENDGAME_ROOK_MOBILITY
            4, // MIDGAME_QUEEN_MOBILITY
            8, // ENDGAME_QUEEN_MOBILITY
            -50, // MIDGAME_KING_ON_OPEN_FILE
            0, // ENDGAME_KING_ON_OPEN_FILE
            -25, // MIDGAME_KING_NEXT_TO_OPEN_FILE
            0, // ENDGAME_KING_NEXT_TO_OPEN_FILE
            -25, // MIDGAME_NO_CASTLING_RIGHTS
            0, // ENDGAME_NO_CASTLING_RIGHTS
            -5, // MIDGAME_QUEENSIDE_CASTLING_PREVENTED
            0, // ENDGAME_QUEENSIDE_CASTLING_PREVENTED
            -10, // MIDGAME_KINGSIDE_CASTLING_PREVENTED
            0, // ENDGAME_KINGSIDE_CASTLING_PREVENTED
            1, // MIDGAME_PAWN_ATTACK_NEAR_KING
            1, // ENDGAME_PAWN_ATTACK_NEAR_KING
            3, // MIDGAME_KNIGHT_ATTACK_NEAR_KING
            3, // ENDGAME_KNIGHT_ATTACK_NEAR_KING
            3, // MIDGAME_BISHOP_ATTACK_NEAR_KING
            3, // ENDGAME_BISHOP_ATTACK_NEAR_KING
            5, // MIDGAME_ROOK_ATTACK_NEAR_KING
            5, // ENDGAME_ROOK_ATTACK_NEAR_KING
            10, // MIDGAME_QUEEN_ATTACK_NEAR_KING
            10, // ENDGAME_QUEEN_ATTACK_NEAR_KING
            10, // MIDGAME_PAWN_CONNECTIVITY
            10, // ENDGAME_PAWN_CONNECTIVITY
            7, // MIDGAME_KNIGHT_CONNECTIVITY
            7, // ENDGAME_KNIGHT_CONNECTIVITY
            7, // MIDGAME_BISHOP_CONNECTIVITY
            7, // ENDGAME_BISHOP_CONNECTIVITY
            3, // MIDGAME_ROOK_CONNECTIVITY
            3, // ENDGAME_ROOK_CONNECTIVITY
            1, // MIDGAME_QUEEN_CONNECTIVITY
            1, // ENDGAME_QUEEN_CONNECTIVITY
            20, // MIDGAME_ROOK_ON_OPEN_FILE
            20, // ENDGAME_ROOK_ON_OPEN_FILE
            15, // MIDGAME_ROOK_ON_SEMI_OPEN_FILE
            15, // ENDGAME_ROOK_ON_SEMI_OPEN_FILE
            10, // MIDGAME_ROOK_ON_7TH_RANK
            10, // ENDGAME_ROOK_ON_7TH_RANK
            5, // MIDGAME_ROOK_ON_QUEEN_FILE
            5, // ENDGAME_ROOK_ON_QUEEN_FILE
            5, // MIDGAME_ROOK_LESS_PAWNS_BONUS
            5, // ENDGAME_ROOK_LESS_PAWNS_BONUS
            -25, // MIDGAME_NO_BISHOP_PAIR
            -25, // ENDGAME_NO_BISHOP_PAIR
            10, // MIDGAME_BISHOP_FIANCHETTO
            0, // ENDGAME_BISHOP_FIANCHETTO
            -10, // MIDGAME_PAWN_ON_SAME_FILE
            -10, // ENDGAME_PAWN_ON_SAME_FILE
            10, // MIDGAME_PASSED_PAWN
            10, // ENDGAME_PASSED_PAWN
            -20, // MIDGAME_ISOLATED_SEMI_OPEN_PAWN
            -20, // ENDGAME_ISOLATED_SEMI_OPEN_PAWN
            -10, // MIDGAME_ISOLATED_PAWN
            -10, // ENDGAME_ISOLATED_PAWN
            3, // MIDGAME_PAWN_SEMI_OPEN_FILE
            3, // ENDGAME_PAWN_SEMI_OPEN_FILE
            20, // MIDGAME_PAWN_SHIELD
            0, // ENDGAME_PAWN_SHIELD
    };

    int getEvalValue(EvalFeature feature) {
        return evalValues[feature];
    }

    int getEvalFeatureSize() {
        return sizeof(evalValues) / sizeof(evalValues[0]);
    }

    // Some sane default values for tuning
    std::vector<int> getBaseEvalValues() {
        std::vector<int> values;

        for (int i = 0; i < getEvalFeatureSize(); i++) {
            values.emplace_back(baseEvalValues[i]);
        }

        for (int i : getMidgameValues()) {
            values.emplace_back(i);
        }

        for (int i : getEndgameValues()) {
            values.emplace_back(i);
        }

        return values;
    }

    std::vector<int> getEvalValues() {
        std::vector<int> values;

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

    void updateEvalValues(std::vector<int> &newValues) {
        int evalFeatureSize = getEvalFeatureSize();
        int pstSize = getMidgameValues().size();

        for (int i = 0; i < evalFeatureSize; i++) {
            evalValues[i] = newValues[i];
        }

        for (int i = 0; i < 6; i++) {
            for (int8_t j = 0; j < 64; j++) {
                int pieceIndex = i * 2;

                setMidgamePstValue((PieceType) pieceIndex, 63 - j, newValues[evalFeatureSize + i * 64 + j]);
                setMidgamePstValue((PieceType) (pieceIndex + 1), j, newValues[evalFeatureSize + i * 64 + j]);
                setEndgamePstValue((PieceType) pieceIndex, 63 - j, newValues[evalFeatureSize + pstSize + i * 64 + j]);
                setEndgamePstValue((PieceType) (pieceIndex + 1), j, newValues[evalFeatureSize + pstSize + i * 64 + j]);
            }
        }
    }
}
