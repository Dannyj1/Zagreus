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
    {100, 350, 350, 525, 1000, 0},  // Midgame
    {100, 350, 350, 525, 1000, 0}   // Endgame
};

// Mobility values
int evalKnightMobility[9][GAME_PHASES] = {
    {-50, -40}, {-28, -18}, {-15, -5}, {-6, 4}, {1, 11}, {7, 17}, {12, 22}, {16, 26}, {20, 30},
};

int evalBishopMobility[14][GAME_PHASES] = {
    {-60, -50}, {-36, -26}, {-23, -13}, {-13, -3}, {-5, 5},  {1, 11},  {6, 16},
    {11, 21},   {15, 25},   {19, 29},   {22, 32},  {25, 35}, {27, 37}, {30, 40},
};

int evalRookMobility[15][GAME_PHASES] = {
    {-70, -50}, {-43, -24}, {-27, -9}, {-16, 1}, {-8, 9},  {-1, 16}, {5, 22},  {11, 27},
    {15, 31},   {19, 35},   {23, 39},  {26, 42}, {29, 45}, {32, 47}, {35, 50},
};

int evalQueenMobility[28][GAME_PHASES] = {
    {-40, -30}, {-23, -13}, {-14, -4}, {-7, 3},  {-1, 9},  {3, 13},  {7, 17},  {10, 20}, {13, 23}, {15, 25},
    {18, 28},   {20, 30},   {22, 32},  {23, 33}, {25, 35}, {27, 37}, {28, 38}, {29, 39}, {31, 41}, {32, 42},
    {33, 43},   {34, 44},   {35, 45},  {36, 46}, {37, 47}, {38, 48}, {39, 49}, {40, 50},
};

// Pawn structure
// Doubled pawns
int evalDoubledPawnPenalty[GAME_PHASES] = {-10, -20};

// Passed Pawns
// int evalPassedPawnBonus[GAME_PHASES][RANKS] = {
//     {0, 5, 10, 20, 35, 60, 100, 0},   // Midgame
//     {0, 10, 20, 40, 70, 120, 200, 0}  // Endgame
// };

}  // namespace Zagreus
