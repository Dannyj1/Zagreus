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

#pragma once

#include <vector>

#include "types.h"

namespace Zagreus {
static constexpr int INITIAL_POOL_SIZE = 100;

class MoveListPool {
public:
    static MoveListPool* getInstance();

    MoveList* getMoveList();

    void releaseMoveList(MoveList* moveList);

    ~MoveListPool();

private:
    std::vector<MoveList*> pool{};

    MoveListPool();

    static MoveList* createMoveList();

    static void destroyMoveList(MoveList* moveList);
};
} // namespace Zagreus