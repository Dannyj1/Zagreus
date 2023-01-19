/*
 Zagreus is a UCI Chess Engine
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef NDEBUG
#pragma once

#include <cassert>
#include "bitboard.h"
#include "movegen.h"

namespace Zagreus {
    void testPawnSinglePush() {
        Bitboard bb;
        bb.setFromFen("8/8/8/8/8/3P4/8/8 w - - 0 1");
        MoveList moves = generateMoves<PieceColor::WHITE>(bb);
        assert(moves.count == 1);

        bb.setFromFen("8/8/3p4/8/8/8/8/8 w - - 0 1");
        moves = generateMoves<PieceColor::BLACK>(bb);
        assert(moves.count == 1);
    }

    void testPawnDoublePush() {
        Bitboard bb;
        bb.setFromFen("8/8/8/8/8/8/5P2/8 w - - 0 1");
        MoveList moves = generateMoves<PieceColor::WHITE>(bb);
        assert(moves.count == 2);

        bb.setFromFen("8/2p5/8/8/8/8/8/8 w - - 0 1");
        moves = generateMoves<PieceColor::BLACK>(bb);
        assert(moves.count == 2);
    }

    void testPromotion() {
        Bitboard bb;
        bb.setFromFen("8/3P4/8/8/8/8/8/8 w - - 0 1");
        MoveList moves = generateMoves<PieceColor::WHITE>(bb);
        assert(moves.count == 4);

        bb.setFromFen("8/8/8/8/8/8/3p4/8 w - - 0 1");
        moves = generateMoves<PieceColor::BLACK>(bb);
        assert(moves.count == 4);
    }

    void runTests() {
        testPawnSinglePush();
        std::cout << "testPawnSinglePush passed" << std::endl;
        testPawnDoublePush();
        std::cout << "testPawnDoublePush passed" << std::endl;
        testPromotion();
        std::cout << "testPromotion passed" << std::endl;
    }
}
#endif