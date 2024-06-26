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

#include "movelist_pool.h"

#include "types.h"

namespace Zagreus {
MoveListPool::MoveListPool() {
    for (int i = 0; i < INITIAL_POOL_SIZE; ++i) {
        MoveList* moveList = createMoveList();
        pool.push_back(moveList);
    }
}

MoveListPool::~MoveListPool() {
    for (MoveList* moveList : pool) {
        destroyMoveList(moveList);
    }
}

MoveListPool* MoveListPool::getInstance() {
    static MoveListPool instance{};
    return &instance;
}

MoveList* MoveListPool::getMoveList() {
    if (pool.empty()) {
        MoveList* moveList = createMoveList();
        return moveList;
    }

    MoveList* moveList = pool.back();
    pool.pop_back();
    moveList->size = 0;
    return moveList;
}

void MoveListPool::releaseMoveList(MoveList* moveList) {
    moveList->size = 0;
    pool.push_back(moveList);
}

MoveList* MoveListPool::createMoveList() {
    auto* moveList = new MoveList();

    for (auto& move : moveList->moves) {
        move = Move();
    }

    return moveList;
}

void MoveListPool::destroyMoveList(MoveList* moveList) { delete moveList; }
} // namespace Zagreus