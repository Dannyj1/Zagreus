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

#include <random>

#include "bitboard.h"
#include "bitwise.h"
#include "magics.h"

namespace Zagreus {
    Bitboard::Bitboard() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        gen.seed(24420221607);
        std::uniform_int_distribution<uint64_t> dis;

/*        for (uint64_t &zobristConstant : zobristConstants) {
            zobristConstant = dis(gen);
        }*/

        uint64_t sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            kingAttacks[sq] = calculateKingAttacks(sqBB) & ~sqBB;
        }

        sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            knightAttacks[sq] = calculateKnightAttacks(sqBB) & ~sqBB;
        }

        sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            pawnAttacks[PieceColor::WHITE][sq] = calculatePawnAttacks<PieceColor::WHITE>(sqBB) & ~sqBB;
            pawnAttacks[PieceColor::BLACK][sq] = calculatePawnAttacks<PieceColor::BLACK>(sqBB) & ~sqBB;
        }

        initializeBetweenLookup();
    }

    template<PieceType pieceType>
    uint64_t Bitboard::getPieceBoard() {
        return pieceBB[pieceType];
    }

    template<PieceColor color>
    uint64_t Bitboard::getColorBoard() {
        return colorBB[color];
    }

    uint64_t Bitboard::getOccupiedBoard() {
        return occupiedBB;
    }

    uint64_t Bitboard::getEmptyBoard() {
        return ~occupiedBB;
    }

    PieceType Bitboard::getPieceOnSquare(int8_t square) {
        return pieceSquareMapping[square];
    }

    uint64_t Bitboard::getKingAttacks(int8_t square) {
        return kingAttacks[square];
    }

    uint64_t Bitboard::getKnightAttacks(int8_t square) {
        return kingAttacks[square];
    }

    uint64_t Bitboard::getQueenAttacks(int8_t square, uint64_t occupancy) {
        return getBishopMagicAttacks(square, occupancy) | getRookMagicAttacks(square, occupancy);
    }

    uint64_t Bitboard::getBishopAttacks(int8_t square, uint64_t occupancy) {
        return getBishopMagicAttacks(square, occupancy);
    }

    uint64_t Bitboard::getRookAttacks(int8_t square, uint64_t occupancy) {
        return getRookMagicAttacks(square, occupancy);
    }

    void Bitboard::setPiece(int8_t square, PieceType piece) {

    }

    void Bitboard::removePiece(int8_t square, PieceType piece) {

    }

    void Bitboard::makeMove(Move &move) {

    }

    void Bitboard::unmakeMove(Move &move) {

    }

    void Bitboard::print() {

    }

    void Bitboard::printAvailableMoves(MoveList &moves) {

    }

    bool Bitboard::setFromFen(std::string &fen) {
        return false;
    }

    bool Bitboard::isDraw() {
        return false;
    }

    bool Bitboard::isWinner(PieceColor color) {
        return false;
    }

    void Bitboard::initializeBetweenLookup() {
        for (int from = 0; from < 64; from++) {
            for (int to = 0; to < 64; to++) {
                uint64_t m1 = -1ULL;
                uint64_t a2a7 = 0x0001010101010100ULL;
                uint64_t b2g7 = 0x0040201008040200ULL;
                uint64_t h1b7 = 0x0002040810204080ULL;
                uint64_t btwn, line, rank, file;

                btwn = (m1 << from) ^ (m1 << to);
                file = (to & 7) - (from & 7);
                rank = ((to | 7) - from) >> 3;
                line = ((file & 7) - 1) & a2a7; /* a2a7 if same file */
                line += 2 * (((rank & 7) - 1) >> 58); /* b1g1 if same rank */
                line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
                line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
                line *= btwn & -btwn; /* mul acts like shift by smaller square */

                betweenTable[from][to] = line & btwn;   /* return the bits on that line in-between */
            }
        }
    }
}