/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

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

#include <catch2/catch_test_macros.hpp>
#include <string>

#include "../src/board.h"
#include "../src/magics.h"
#include "../src/move.h"
#include "../src/move_gen.h"

namespace Zagreus {

// Helper function to check if a specific castling move is legal in a given position.
// This function adheres to the engine's architecture of generating pseudo-legal moves
// and then verifying full legality.
bool isCastleMoveLegal(const std::string& fen, Square from, Square to) {
    Board board;
    board.setFromFEN(fen);

    MoveList moveList;
    const PieceColor movingColor = board.getSideToMove();

    // Check if the king is currently in check to determine the move generation type,
    // matching the logic in the search function.
    bool inCheck = false;
    if (movingColor == WHITE) {
        inCheck = board.isKingInCheck<WHITE>();
    } else {
        inCheck = board.isKingInCheck<BLACK>();
    }

    // Generate moves using the appropriate generation type.
    if (movingColor == WHITE) {
        if (inCheck) {
            generateMoves<WHITE, EVASIONS>(board, moveList);
        } else {
            generateMoves<WHITE, ALL>(board, moveList);
        }
    } else {  // BLACK
        if (inCheck) {
            generateMoves<BLACK, EVASIONS>(board, moveList);
        } else {
            generateMoves<BLACK, ALL>(board, moveList);
        }
    }

    // Iterate through the generated moves to find and validate the specific castling move.
    for (int i = 0; i < moveList.size; ++i) {
        const Move move = moveList.moves[i];

        // Skip moves that are not the castling move we're looking for.
        if (getMoveType(move) != MoveType::CASTLING || getFromSquare(move) != from || getToSquare(move) != to) {
            continue;
        }

        // To verify the pseudo-legal castling move is fully legal, we use isMoveLegal()
        // before making the move, matching the engine's search logic.
        if (board.isMoveLegal(move)) {
            return true;  // Found the legal castling move.
        }
    }

    return false;  // The specified castling move was not found in the list of legal moves.
}

TEST_CASE("CastlingRuleTests", "[castling]") {
    initZobristConstants();
    initializeMagicBitboards();
    initializeAttackLookupTables();
    initializeBetweenLookupTable();
    initializeLineLookupTable();

    SECTION("White Kingside Castling (O-O) Not Allowed") {
        // Pieces are between the king and the rook.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq - 0 1", E1, G1));
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK1NR w KQkq - 0 1", E1, G1));

        // King has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w Qkq - 0 1", E1, G1));

        // Rook has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w Qkq - 0 1", E1, G1));

        // King is in check.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppp1ppp/8/8/4r3/8/PPPP1PPP/RNBQK2R w KQkq - 0 1", E1, G1));

        // A square the king moves over is attacked.
        REQUIRE_FALSE(
            isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/5r2/8/PPPPP1PP/RNBQK2R w KQkq - 0 1", E1, G1));  // f1 attacked
        REQUIRE_FALSE(
            isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/6r1/8/PPPPPP1P/RNBQK2R w KQkq - 0 1", E1, G1));  // g1 attacked
    }

    SECTION("White Queenside Castling (O-O-O) Not Allowed") {
        // Pieces are between the king and the rook.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RN2KBNR w KQkq - 0 1", E1, C1));
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1B1KBNR w KQkq - 0 1", E1, C1));
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R2QKBNR w KQkq - 0 1", E1, C1));

        // King has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w Kkq - 0 1", E1, C1));

        // Rook has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w Kkq - 0 1", E1, C1));

        // King is in check.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/4r3/8/PPPP1PPP/R3KBNR w KQkq - 0 1", E1, C1));

        // A square the king moves over is attacked.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/2r5/8/PP1PPPPP/R3KBNR w KQkq - 0 1", E1, C1));
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/3r4/8/PPP1PPPP/R3KBNR w KQkq - 0 1", E1, C1));
    }

    SECTION("Black Kingside Castling (o-o) Not Allowed") {
        // Pieces are between the king and the rook.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqkb1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, G8));
        REQUIRE_FALSE(isCastleMoveLegal("rnbqk1nr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, G8));

        // King has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQ - 0 1", E8, G8));

        // Rook has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQq - 0 1", E8, G8));

        // King is in check.
        REQUIRE_FALSE(isCastleMoveLegal("rnbqk2r/pppp1ppp/8/4R3/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, G8));

        // A square the king moves over is attacked.
        REQUIRE_FALSE(
            isCastleMoveLegal("rnbqk2r/ppppp1pp/8/5R2/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, G8));  // f8 attacked
        REQUIRE_FALSE(
            isCastleMoveLegal("rnbqk2r/pppppp1p/8/6R1/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, G8));  // g8 attacked
    }

    SECTION("Black Queenside Castling (o-o-o) Not Allowed") {
        // Pieces are between the king and the rook.
        REQUIRE_FALSE(isCastleMoveLegal("rn2kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));
        REQUIRE_FALSE(isCastleMoveLegal("r1b1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));
        REQUIRE_FALSE(isCastleMoveLegal("r2qkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));

        // King has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQ - 0 1", E8, C8));

        // Rook has moved (castling rights removed).
        REQUIRE_FALSE(isCastleMoveLegal("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQk - 0 1", E8, C8));

        // King is in check.
        REQUIRE_FALSE(isCastleMoveLegal("r3kbnr/ppp1pppp/8/3R4/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));

        // A square the king moves over is attacked.
        REQUIRE_FALSE(isCastleMoveLegal("r3kbnr/pp1ppppp/8/2R5/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));
        REQUIRE_FALSE(isCastleMoveLegal("r3kbnr/ppp1pppp/8/3R4/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));
    }

    SECTION("White Kingside Castling (O-O) Allowed") {
        REQUIRE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1", E1, G1));
    }

    SECTION("White Queenside Castling (O-O-O) Allowed") {
        REQUIRE(isCastleMoveLegal("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR w KQkq - 0 1", E1, C1));
    }

    SECTION("Black Kingside Castling (o-o) Allowed") {
        REQUIRE(isCastleMoveLegal("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, G8));
    }

    SECTION("Black Queenside Castling (o-o-o) Allowed") {
        REQUIRE(isCastleMoveLegal("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", E8, C8));
    }
}

}  // namespace Zagreus
