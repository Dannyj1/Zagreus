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

#include <iostream>

#include "pst.h"

namespace Zagreus {
int evalValues[70] = {
    105, 103, 371, 354, 398, 370, 543, 548, 1032, 1021, 9,   5,  6,   4,   0,   7,   0,  22,
    21,  -1,  -6,  -1,  -12, 9,   6,   -5,  -12,  0,    -20, 1,  -13, -30, -13, -6,  0,  23,
    -5,  -10, -10, -9,  -21, -9,  -5,  -9,  7,    -7,   -5,  -9, -5,  -17, -31, -17, 22, 5,
    8,   7,   36,  22,  19,  0,   -5,  0,   11,   4,    6,   16, 0,   -6,  11,  0,
};

int baseEvalValues[70] = {
    100,   // MIDGAME_PAWN_MATERIAL
    100,   // ENDGAME_PAWN_MATERIAL
    350,   // MIDGAME_KNIGHT_MATERIAL
    350,   // ENDGAME_KNIGHT_MATERIAL
    350,   // MIDGAME_BISHOP_MATERIAL
    350,   // ENDGAME_BISHOP_MATERIAL
    525,   // MIDGAME_ROOK_MATERIAL
    525,   // ENDGAME_ROOK_MATERIAL
    1000,  // MIDGAME_QUEEN_MATERIAL
    1000,  // ENDGAME_QUEEN_MATERIAL
    7,     // MIDGAME_KNIGHT_MOBILITY
    2,     // ENDGAME_KNIGHT_MOBILITY
    8,     // MIDGAME_BISHOP_MOBILITY
    3,     // ENDGAME_BISHOP_MOBILITY
    2,     // MIDGAME_ROOK_MOBILITY
    6,     // ENDGAME_ROOK_MOBILITY
    4,     // MIDGAME_QUEEN_MOBILITY
    8,     // ENDGAME_QUEEN_MOBILITY
    20,    // MIDGAME_PAWN_SHIELD
    0,     // ENDGAME_PAWN_SHIELD
    -5,    // MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY
    0,     // ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY
    -2,    // MIDGAME_KING_ATTACK_PAWN_PENALTY
    -2,    // ENDGAME_KING_ATTACK_PAWN_PENALTY
    -5,    // MIDGAME_KING_ATTACK_KNIGHT_PENALTY
    -7,    // ENDGAME_KING_ATTACK_KNIGHT_PENALTY
    -7,    // MIDGAME_KING_ATTACK_BISHOP_PENALTY
    -10,   // ENDGAME_KING_ATTACK_BISHOP_PENALTY
    -10,   // MIDGAME_KING_ATTACK_ROOK_PENALTY
    -15,   // ENDGAME_KING_ATTACK_ROOK_PENALTY
    -15,   // MIDGAME_KING_ATTACK_QUEEN_PENALTY
    -20,   // ENDGAME_KING_ATTACK_QUEEN_PENALTY
    -10,   // MIDGAME_DOUBLED_PAWN_PENALTY
    -15,   // ENDGAME_DOUBLED_PAWN_PENALTY
    10,    // MIDGAME_PASSED_PAWN
    20,    // ENDGAME_PASSED_PAWN
    -5,    // MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY
    -10,   // ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY
    -10,   // MIDGAME_ISOLATED_PAWN_PENALTY
    -15,   // ENDGAME_ISOLATED_PAWN_PENALTY
    -3,    // MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY
    -6,    // ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY
    -5,    // MIDGAME_KNIGHT_MISSING_PAWN_PENALTY
    -3,    // ENDGAME_KNIGHT_MISSING_PAWN_PENALTY
    5,     // MIDGAME_KNIGHT_DEFENDED_BY_PAWN
    3,     // ENDGAME_KNIGHT_DEFENDED_BY_PAWN
    -5,    // MIDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY
    -5,    // ENDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY
    -15,   // MIDGAME_BAD_BISHOP_PENALTY
    -10,   // ENDGAME_BAD_BISHOP_PENALTY
    -50,   // MIDGAME_MISSING_BISHOP_PAIR_PENALTY
    -50,   // ENDGAME_MISSING_BISHOP_PAIR_PENALTY
    15,    // MIDGAME_BISHOP_FIANCHETTO
    0,     // ENDGAME_BISHOP_FIANCHETTO
    5,     // MIDGAME_ROOK_PAWN_COUNT
    5,     // ENDGAME_ROOK_PAWN_COUNT
    10,    // MIDGAME_ROOK_ON_OPEN_FILE
    15,    // ENDGAME_ROOK_ON_OPEN_FILE
    5,     // MIDGAME_ROOK_ON_SEMI_OPEN_FILE
    7,     // ENDGAME_ROOK_ON_SEMI_OPEN_FILE
    10,    // MIDGAME_ROOK_ON_7TH_RANK
    15,    // MIDGAME_ROOK_ON_7TH_RANK
    0,     // MIDGAME_TARRASCH_OWN_ROOK_PENALTY
    -10,   // ENDGAME_TARRASCH_OWN_ROOK_PENALTY
    0,     // MIDGAME_TARRASCH_OWN_ROOK_DEFEND
    10,    // ENDGAME_TARRASCH_OWN_ROOK_DEFEND
    0,     // MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY
    -10,   // ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY
    6,     // MIDGAME_ROOK_ON_QUEEN_FILE
    3,     // ENDGAME_ROOK_ON_QUEEN_FILE
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
}  // namespace Zagreus
