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

#include "tbprobe.h"
#include "syzygy.h"

#include "engine.h"
#include "search.h"

namespace Zagreus {
int tablebaseProbe(Bitboard board, int depth, SearchContext& context) {
    uint64_t occ = board.getOccupiedBoard();
    int occCount = std::popcount(occ);

    if (occCount > TB_LARGEST) {
        return TB_RESULT_FAILED;
    }

    int probeDepth = context.syzygyProbeDepth;
    if (probeDepth != 0 && depth < probeDepth) {
        return TB_RESULT_FAILED;
    }

    uint64_t kings = board.getPieceBoard(WHITE_KING) | board.getPieceBoard(BLACK_KING);
    uint64_t queens = board.getPieceBoard(WHITE_QUEEN) | board.getPieceBoard(BLACK_QUEEN);
    uint64_t rooks = board.getPieceBoard(WHITE_ROOK) | board.getPieceBoard(BLACK_ROOK);
    uint64_t bishops = board.getPieceBoard(WHITE_BISHOP) | board.getPieceBoard(BLACK_BISHOP);
    uint64_t knights = board.getPieceBoard(WHITE_KNIGHT) | board.getPieceBoard(BLACK_KNIGHT);
    uint64_t pawns = board.getPieceBoard(WHITE_PAWN) | board.getPieceBoard(BLACK_PAWN);
    int8_t ep = board.getEnPassantSquare();

    if (ep == NO_SQUARE) {
        ep = 0;
    }

    return tb_probe_wdl(board.getColorBoard<WHITE>(), board.getColorBoard<BLACK>(), kings, queens,
                        rooks, bishops, knights, pawns, ep, board.getMovingColor() == WHITE);
}
} // namespace Zagreus