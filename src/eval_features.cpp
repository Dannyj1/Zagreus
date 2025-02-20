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
// Base material values
int evalMaterialValues[GAME_PHASES][PIECE_TYPES] = {
    {100, 350, 350, 525, 1000, 0}, // Midgame
    {100, 350, 350, 525, 1000, 0}  // Endgame
};

// Base mobility values
int evalMobility[GAME_PHASES][PIECE_TYPES] = {
    {0, 4, 6, 2, 4, 0}, // Midgame
    {0, 2, 3, 5, 6, 0}  // Endgame
};
} // namespace Zagreus
