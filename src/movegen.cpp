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

#include "movegen.h"

#include <x86intrin.h>

#include <cassert>
#include <iostream>

#include "bitboard.h"
#include "tt.h"
#include "utils.h"

namespace Zagreus {
void addMoveToList(MoveList* moveList, int8_t from, int8_t to = 0, PieceType piece = EMPTY,
                   int captureScore = 0, PieceType promotionPiece = EMPTY, int score = 0) {
    moveList->moves[moveList->size].from = from;
    moveList->moves[moveList->size].to = to;
    moveList->moves[moveList->size].piece = piece;
    moveList->moves[moveList->size].captureScore = captureScore;
    moveList->moves[moveList->size].score = score;
    moveList->moves[moveList->size].promotionPiece = promotionPiece;
    moveList->size++;
}

int scoreMove(int ply, uint32_t pvMoveCode, Move* move,
              Move& previousMove, uint32_t moveCode, uint32_t bestMoveCode,
              TranspositionTable* tt) {
    if (moveCode == pvMoveCode) {
        return 500000;
    }

    if (moveCode == bestMoveCode) {
        return 250000;
    }

    if (move->captureScore >= 0) {
        return 100000 + move->captureScore;
    }

    if (tt->killerMoves[0][ply] == moveCode) {
        return 50000;
    }

    if (tt->killerMoves[1][ply] == moveCode) {
        return 40000;
    }

    if (tt->killerMoves[2][ply] == moveCode) {
        return 30000;
    }

    if (previousMove.piece != EMPTY && previousMove.to != NO_SQUARE
        && tt->counterMoves[previousMove.piece][previousMove.to] == moveCode) {
        return 20000;
    }

    // No capture score is -1, so we need to use < -1
    if (move->captureScore < -1) {
        return move->captureScore - 5000;
    }

    return tt->historyMoves[move->piece][move->to];
}

template <PieceColor color, GenerationType type>
void generateMoves(Bitboard& bitboard, MoveList* moveList) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t evasionSquaresBB = 0;

    if (type == EVASIONS) {
        int8_t kingSquare = bitscanForward(bitboard.getPieceBoard(color == WHITE
            ? WHITE_KING
            : BLACK_KING));
        uint64_t kingAttackers = bitboard.getSquareAttackersByColor<OPPOSITE_COLOR>(kingSquare);

        // Only generate king moves if there is more than one attacker
        if (popcnt(kingAttackers) == 1) {
            int8_t attackerSquare = popLsb(kingAttackers);
            PieceType attackerPiece = bitboard.getPieceOnSquare(attackerSquare);

            if (isSlidingPiece(attackerPiece)) {
                evasionSquaresBB |= bitboard.getBetweenSquares(attackerSquare, kingSquare);
            }

            evasionSquaresBB |= 1ULL << attackerSquare;
        }
    }

    generatePawnMoves<color, type>(bitboard, moveList, evasionSquaresBB);
    generateKnightMoves<color, type>(bitboard, moveList, evasionSquaresBB);
    generateBishopMoves<color, type>(bitboard, moveList, evasionSquaresBB);
    generateRookMoves<color, type>(bitboard, moveList, evasionSquaresBB);
    generateQueenMoves<color, type>(bitboard, moveList, evasionSquaresBB);
    generateKingMoves<color, type>(bitboard, moveList);

    TranspositionTable* tt = TranspositionTable::getTT();
    Line previousPv = bitboard.getPvLine();
    uint32_t bestMoveCode = 0;
    TTEntry* ttEntry = tt->getEntry(bitboard.getZobristHash());

    if (ttEntry->zobristHash == bitboard.getZobristHash()) {
        bestMoveCode = ttEntry->bestMoveCode;
    }

    int ply = bitboard.getPly();
    int pvfrom = ply - previousPv.startPly;
    Move pvMove = previousPv.moves[pvfrom];
    uint32_t pvMoveCode = encodeMove(&pvMove);
    Move previousMove = bitboard.getPreviousMove();

    for (int i = 0; i < moveList->size; i++) {
        Move* move = &moveList->moves[i];
        move->score = scoreMove(ply, pvMoveCode, move, previousMove, encodeMove(move),
                                bestMoveCode, tt);
    }
}

template <PieceColor color, GenerationType type>
void generatePawnMoves(Bitboard& bitboard, MoveList* moveList, uint64_t evasionSquaresBB) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t pawnBB;

    if (color == WHITE) {
        pawnBB = bitboard.getPieceBoard(WHITE_PAWN);
    } else {
        pawnBB = bitboard.getPieceBoard(BLACK_PAWN);
    }

    while (pawnBB) {
        int8_t from = popLsb(pawnBB);
        uint64_t genBB = bitboard.getPawnDoublePush<color>(1ULL << from);
        uint64_t attackableSquares = bitboard.getColorBoard<OPPOSITE_COLOR>();

        if (bitboard.getEnPassantSquare() != NO_SQUARE) {
            attackableSquares |= 1ULL << bitboard.getEnPassantSquare();
        }

        genBB |= bitboard.getPawnAttacks<color>(from) & attackableSquares;
        genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard(WHITE_KING) |
                   bitboard.getPieceBoard(BLACK_KING));

        if (type == QSEARCH) {
            genBB &= (bitboard.getColorBoard<OPPOSITE_COLOR>() | PROMOTION_SQUARES);
        }

        if (type == EVASIONS) {
            genBB &= evasionSquaresBB;
        }

        while (genBB) {
            int8_t to = popLsb(genBB);
            PieceType capturedPiece = bitboard.getPieceOnSquare(to);
            int captureScore = NO_CAPTURE_SCORE;

            if (color == WHITE) {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(WHITE_PAWN, capturedPiece);
                }

                if (to >= A8) {
                    addMoveToList(moveList, from, to, WHITE_PAWN, captureScore, WHITE_QUEEN);

                    if (type == NORMAL) {
                        addMoveToList(moveList, from, to, WHITE_PAWN, captureScore, WHITE_ROOK);
                        addMoveToList(moveList, from, to, WHITE_PAWN, captureScore,
                                      WHITE_BISHOP);
                        addMoveToList(moveList, from, to, WHITE_PAWN, captureScore,
                                      WHITE_KNIGHT);
                    }
                } else {
                    addMoveToList(moveList, from, to, WHITE_PAWN, captureScore);
                }
            } else {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(BLACK_PAWN, capturedPiece);
                }

                if (to <= H1) {
                    addMoveToList(moveList, from, to, BLACK_PAWN, captureScore, BLACK_QUEEN);

                    if (type == NORMAL) {
                        addMoveToList(moveList, from, to, BLACK_PAWN, captureScore, BLACK_ROOK);
                        addMoveToList(moveList, from, to, BLACK_PAWN, captureScore,
                                      BLACK_BISHOP);
                        addMoveToList(moveList, from, to, BLACK_PAWN, captureScore,
                                      BLACK_KNIGHT);
                    }
                } else {
                    addMoveToList(moveList, from, to, BLACK_PAWN, captureScore);
                }
            }
        }
    }
}

template <PieceColor color, GenerationType type>
void generateKnightMoves(Bitboard& bitboard, MoveList* moveList, uint64_t evasionSquaresBB) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t knightBB;

    if (color == WHITE) {
        knightBB = bitboard.getPieceBoard(WHITE_KNIGHT);
    } else {
        knightBB = bitboard.getPieceBoard(BLACK_KNIGHT);
    }

    while (knightBB) {
        int8_t from = popLsb(knightBB);
        uint64_t genBB = bitboard.getKnightAttacks(from);

        genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard(WHITE_KING) |
                   bitboard.getPieceBoard(BLACK_KING));

        if (type == QSEARCH) {
            genBB &= (bitboard.getColorBoard<OPPOSITE_COLOR>());
        }

        if (type == EVASIONS) {
            genBB &= evasionSquaresBB;
        }

        while (genBB) {
            int8_t to = popLsb(genBB);
            PieceType capturedPiece = bitboard.getPieceOnSquare(to);
            int captureScore = NO_CAPTURE_SCORE;

            if (color == WHITE) {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(WHITE_KNIGHT, capturedPiece);
                }

                addMoveToList(moveList, from, to, WHITE_KNIGHT, captureScore);
            } else {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(BLACK_KNIGHT, capturedPiece);
                }

                addMoveToList(moveList, from, to, BLACK_KNIGHT, captureScore);
            }
        }
    }
}

template <PieceColor color, GenerationType type>
void generateBishopMoves(Bitboard& bitboard, MoveList* moveList, uint64_t evasionSquaresBB) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t bishopBB;

    if (color == WHITE) {
        bishopBB = bitboard.getPieceBoard(WHITE_BISHOP);
    } else {
        bishopBB = bitboard.getPieceBoard(BLACK_BISHOP);
    }

    while (bishopBB) {
        int8_t from = popLsb(bishopBB);
        uint64_t genBB = bitboard.getBishopAttacks(from);

        genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard(WHITE_KING) |
                   bitboard.getPieceBoard(BLACK_KING));

        if (type == QSEARCH) {
            genBB &= (bitboard.getColorBoard<OPPOSITE_COLOR>());
        }

        if (type == EVASIONS) {
            genBB &= evasionSquaresBB;
        }

        while (genBB) {
            int8_t to = popLsb(genBB);
            PieceType capturedPiece = bitboard.getPieceOnSquare(to);
            int captureScore = NO_CAPTURE_SCORE;

            if (color == WHITE) {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(WHITE_BISHOP, capturedPiece);
                }

                addMoveToList(moveList, from, to, WHITE_BISHOP, captureScore);
            } else {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(BLACK_BISHOP, capturedPiece);
                }

                addMoveToList(moveList, from, to, BLACK_BISHOP, captureScore);
            }
        }
    }
}

template <PieceColor color, GenerationType type>
void generateRookMoves(Bitboard& bitboard, MoveList* moveList, uint64_t evasionSquaresBB) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t rookBB;

    if (color == WHITE) {
        rookBB = bitboard.getPieceBoard(WHITE_ROOK);
    } else {
        rookBB = bitboard.getPieceBoard(BLACK_ROOK);
    }

    while (rookBB) {
        int8_t from = popLsb(rookBB);
        uint64_t genBB = bitboard.getRookAttacks(from);

        genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard(WHITE_KING) |
                   bitboard.getPieceBoard(BLACK_KING));

        if (type == QSEARCH) {
            genBB &= (bitboard.getColorBoard<OPPOSITE_COLOR>());
        }

        if (type == EVASIONS) {
            genBB &= evasionSquaresBB;
        }

        while (genBB) {
            int8_t to = popLsb(genBB);
            PieceType capturedPiece = bitboard.getPieceOnSquare(to);
            int captureScore = NO_CAPTURE_SCORE;

            if (color == WHITE) {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(WHITE_ROOK, capturedPiece);
                }

                addMoveToList(moveList, from, to, WHITE_ROOK, captureScore);
            } else {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(BLACK_ROOK, capturedPiece);
                }

                addMoveToList(moveList, from, to, BLACK_ROOK, captureScore);
            }
        }
    }
}

template <PieceColor color, GenerationType type>
void generateQueenMoves(Bitboard& bitboard, MoveList* moveList, uint64_t evasionSquaresBB) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t queenBB;

    if (color == WHITE) {
        queenBB = bitboard.getPieceBoard(WHITE_QUEEN);
    } else {
        queenBB = bitboard.getPieceBoard(BLACK_QUEEN);
    }

    while (queenBB) {
        int8_t from = popLsb(queenBB);
        uint64_t genBB = bitboard.getQueenAttacks(from);

        genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getPieceBoard(WHITE_KING) |
                   bitboard.getPieceBoard(BLACK_KING));

        if (type == QSEARCH) {
            genBB &= (bitboard.getColorBoard<OPPOSITE_COLOR>());
        }

        if (type == EVASIONS) {
            genBB &= evasionSquaresBB;
        }

        while (genBB) {
            int8_t to = popLsb(genBB);
            PieceType capturedPiece = bitboard.getPieceOnSquare(to);
            int captureScore = NO_CAPTURE_SCORE;

            if (color == WHITE) {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(WHITE_QUEEN, capturedPiece);
                }

                addMoveToList(moveList, from, to, WHITE_QUEEN, captureScore);
            } else {
                if (capturedPiece != EMPTY) {
                    captureScore = type == QSEARCH
                                       ? bitboard.seeCapture<color>(from, to)
                                       : mvvlva(BLACK_QUEEN, capturedPiece);
                }

                addMoveToList(moveList, from, to, BLACK_QUEEN, captureScore);
            }
        }
    }
}

template <PieceColor color, GenerationType type>
void generateKingMoves(Bitboard& bitboard, MoveList* moveList) {
    constexpr PieceColor OPPOSITE_COLOR = color == WHITE ? BLACK : WHITE;
    uint64_t kingBB;
    uint64_t opponentKingBB;

    if (color == WHITE) {
        kingBB = bitboard.getPieceBoard(WHITE_KING);
        opponentKingBB = bitboard.getPieceBoard(BLACK_KING);
    } else {
        kingBB = bitboard.getPieceBoard(BLACK_KING);
        opponentKingBB = bitboard.getPieceBoard(WHITE_KING);
    }

    int8_t from = bitscanForward(kingBB);
    uint64_t genBB = bitboard.getKingAttacks(from);
    int8_t opponentKingSquare = bitscanForward(opponentKingBB);

    genBB &= ~(bitboard.getColorBoard<color>() | bitboard.getKingAttacks(opponentKingSquare));

    if (type == QSEARCH) {
        genBB &= bitboard.getColorBoard<OPPOSITE_COLOR>();
    }

    while (genBB) {
        int8_t to = popLsb(genBB);
        PieceType capturedPiece = bitboard.getPieceOnSquare(to);
        int captureScore = NO_CAPTURE_SCORE;

        if (color == WHITE) {
            if (capturedPiece != EMPTY) {
                captureScore = type == QSEARCH
                                   ? bitboard.seeCapture<color>(from, to)
                                   : mvvlva(WHITE_KING, capturedPiece);
            }

            addMoveToList(moveList, from, to, WHITE_KING, captureScore);
        } else {
            if (capturedPiece != EMPTY) {
                captureScore = type == QSEARCH
                                   ? bitboard.seeCapture<color>(from, to)
                                   : mvvlva(BLACK_KING, capturedPiece);
            }

            addMoveToList(moveList, from, to, BLACK_KING, captureScore);
        }
    }

    if (type == QSEARCH || type == EVASIONS) {
        return;
    }

    uint64_t occupiedBB = bitboard.getOccupiedBoard();

    if (color == WHITE) {
        if (bitboard.getCastlingRights() & WHITE_KINGSIDE &&
            (occupiedBB & WHITE_KING_SIDE_BETWEEN) == 0 &&
            bitboard.getPieceOnSquare(H1) == WHITE_ROOK) {
            uint64_t tilesToCheck = WHITE_KING_SIDE_BETWEEN;
            bool canCastle = true;

            while (tilesToCheck) {
                int8_t tilefrom = popLsb(tilesToCheck);

                if (bitboard.isSquareAttackedByColor<BLACK>(tilefrom)) {
                    canCastle = false;
                    break;
                }
            }

            if (canCastle) {
                addMoveToList(moveList, from, G1, WHITE_KING, -1);
            }
        }

        if (bitboard.getCastlingRights() & WHITE_QUEENSIDE &&
            (occupiedBB & WHITE_QUEEN_SIDE_BETWEEN) == 0 &&
            bitboard.getPieceOnSquare(A1) == WHITE_ROOK) {
            uint64_t tilesToCheck = WHITE_QUEEN_SIDE_BETWEEN & ~(1ULL << B1);
            bool canCastle = true;

            while (tilesToCheck) {
                int8_t tilefrom = popLsb(tilesToCheck);

                if (bitboard.isSquareAttackedByColor<BLACK>(tilefrom)) {
                    canCastle = false;
                    break;
                }
            }

            if (canCastle) {
                addMoveToList(moveList, from, C1, WHITE_KING, -1);
            }
        }
    } else {
        if (bitboard.getCastlingRights() & BLACK_KINGSIDE &&
            (occupiedBB & BLACK_KING_SIDE_BETWEEN) == 0 &&
            bitboard.getPieceOnSquare(H8) == BLACK_ROOK) {
            uint64_t tilesToCheck = BLACK_KING_SIDE_BETWEEN;
            bool canCastle = true;

            while (tilesToCheck) {
                int8_t tilefrom = popLsb(tilesToCheck);

                if (bitboard.isSquareAttackedByColor<WHITE>(tilefrom)) {
                    canCastle = false;
                    break;
                }
            }

            if (canCastle) {
                addMoveToList(moveList, from, G8, BLACK_KING, -1);
            }
        }

        if (bitboard.getCastlingRights() & BLACK_QUEENSIDE &&
            (occupiedBB & BLACK_QUEEN_SIDE_BETWEEN) == 0 &&
            bitboard.getPieceOnSquare(A8) == BLACK_ROOK) {
            uint64_t tilesToCheck = BLACK_QUEEN_SIDE_BETWEEN & ~(1ULL << B8);
            bool canCastle = true;

            while (tilesToCheck) {
                int8_t tilefrom = popLsb(tilesToCheck);

                if (bitboard.isSquareAttackedByColor<WHITE>(tilefrom)) {
                    canCastle = false;
                    break;
                }
            }

            if (canCastle) {
                addMoveToList(moveList, from, C8, BLACK_KING, -1);
            }
        }
    }
}

template void generateMoves<WHITE, NORMAL>(Bitboard& bitboard, MoveList* moveList);
template void generateMoves<WHITE, QSEARCH>(Bitboard& bitboard, MoveList* moveList);
template void generateMoves<WHITE, EVASIONS>(Bitboard& bitboard, MoveList* moveList);
template void generateMoves<BLACK, NORMAL>(Bitboard& bitboard, MoveList* moveList);
template void generateMoves<BLACK, QSEARCH>(Bitboard& bitboard, MoveList* moveList);
template void generateMoves<BLACK, EVASIONS>(Bitboard& bitboard, MoveList* moveList);
} // namespace Zagreus
