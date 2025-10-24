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
#ifdef ZAGREUS_TUNER
#include <array>
#include <string>

#include "constants.h"
#include "eval.h"
#include "types.h"

namespace Zagreus {
struct TunePosition {
    std::string fen;
    PieceColor sideToMove = WHITE;
    double result = 0.0;
    EvalTrace trace{};
};

void startTuning(std::string filePath);
}  // namespace Zagreus
#endif