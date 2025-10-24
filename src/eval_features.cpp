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
    {99, 349, 454, 611, 1155, -1},  // Midgame
    {106, 349, 349, 524, 999, 40}   // Endgame
};

// Mobility values
int evalMobility[GAME_PHASES][PIECE_TYPES]{
    {0, 3, 5, 1, 3, 0},  // Midgame
    {0, 1, 2, 4, 5, 0}   // Endgame
};

// Pawn structure
int evalDoubledPawnPenalty[GAME_PHASES] = {-12, -34};

// Passed Pawns
/*int evalPassedPawnBonus[GAME_PHASES][RANKS] = {
    {0, 5, 10, 20, 35, 60, 100, 0},   // Midgame
    {0, 10, 20, 40, 70, 120, 200, 0}  // Endgame
};*/

}  // namespace Zagreus
