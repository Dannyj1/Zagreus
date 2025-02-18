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

#pragma once
#include "constants.h"

namespace Zagreus {
enum PieceType : uint8_t;
extern const int baseMaterialValues[GAME_PHASES][PIECE_TYPES];
extern const int baseMobility[GAME_PHASES][PIECE_TYPES];

extern const int baseMgPawnTable[64];
extern const int baseEgPawnTable[64];
extern const int baseMgKnightTable[64];
extern const int baseEgKnightTable[64];
extern const int baseMgBishopTable[64];
extern const int baseEgBishopTable[64];
extern const int baseMgRookTable[64];
extern const int baseEgRookTable[64];
extern const int baseMgQueenTable[64];
extern const int baseEgQueenTable[64];
extern const int baseMgKingTable[64];
extern const int baseEgKingTable[64];

extern int baseMidgamePstTable[PIECES][SQUARES];
extern int baseEndgamePstTable[PIECES][SQUARES];

void initializeBasePst();

const int* getBaseMidgameTable(PieceType pieceType);

const int* getBaseEndgameTable(PieceType pieceType);
} // namespace Zagreus 