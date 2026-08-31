/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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
// Material values
int evalMaterialValues[GAME_PHASES][PIECE_TYPES]{
    {96, 339, 368, 527, 999, 0},   // Midgame
    {141, 317, 335, 526, 1013, 0}  // Endgame
};

// Mobility values
int evalMobility[GAME_PHASES][PIECE_TYPES]{
    {0, 8, 9, 6, 3, 0},   // Midgame
    {0, -4, -2, 3, 7, 0}  // Endgame
};

// Pawn structure
int evalDoubledPawnPenalty[GAME_PHASES] = {-15, -32};

// Passed Pawns
int evalPassedPawnBonus[GAME_PHASES][RANKS] = {
    {0, 5, 10, 20, 35, 60, 100, 0},   // Midgame
    {0, 10, 20, 40, 70, 120, 200, 0}  // Endgame
};

}  // namespace Zagreus
