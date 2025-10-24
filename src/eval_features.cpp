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
// Material values
int evalMaterialValues[GAME_PHASES][PIECE_TYPES]{
    {90, 459, 496, 655, 1284, 0},  // Midgame
    {108, 257, 263, 475, 874, 0}   // Endgame
};

// Mobility values
int evalMobility[GAME_PHASES][PIECE_TYPES]{
    {0, 4, 5, 2, 4, 0},  // Midgame
    {0, 2, 3, 5, 6, 0}   // Endgame
};

// Pawn structure
int evalDoubledPawnPenalty[GAME_PHASES] = {-10, -21};

// Passed Pawns
/*int evalPassedPawnBonus[GAME_PHASES][RANKS] = {
    {0, 5, 10, 20, 35, 60, 100, 0},   // Midgame
    {0, 10, 20, 40, 70, 120, 200, 0}  // Endgame
};*/

}  // namespace Zagreus
