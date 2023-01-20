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

#include <iostream>
#include <chrono>
#include <immintrin.h>

#include "search.h"
#include "timemanager.h"
#include "movegen.h"
#include "../senjo/Output.h"
#include "movepicker.h"
#include "pst.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    Move SearchManager::getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board) {
        searchStats = {};
        isSearching = true;
        int bestScore = -1000000;
        Move bestMove = {};
        int iterationScore = -1000000;
        Move iterationMove = {};
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime = getEndTime(params, board, engine, board.getMovingColor());
        int depth = 0;

        Line iterationPvLine = {};
        while (std::chrono::high_resolution_clock::now() - startTime < (endTime - startTime) * 0.7) {
            depth += 1;

            if (params.depth > 0 && depth > params.depth) {
                return bestMove;
            }

            searchStats.depth = depth;
            searchStats.seldepth = 0;

            senjo::Output(senjo::Output::InfoPrefix) << "Searching depth " << depth << "...";
            board.setPreviousPvLine(iterationPvLine);
            MoveList moveList;

            if (board.getMovingColor() == PieceColor::WHITE) {
                moveList = generateMoves<PieceColor::WHITE>(board);
            } else {
                moveList = generateMoves<PieceColor::BLACK>(board);
            }

            MovePicker moves = MovePicker(moveList);

            if (moves.size() == 1) {
                return bestMove;
            }

            while (moves.hasNext()) {
                Move move = moves.getNextMove();
                assert(move.from != move.to);
                assert(move.from >= 0 && move.from < 64);
                assert(move.to >= 0 && move.to < 64);

                if (depth > 1 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
                    return bestMove;
                }

                board.makeMove(move);

                if (board.getMovingColor() == PieceColor::WHITE) {
                    if (board.isKingInCheck<PieceColor::BLACK>()) {
                        board.unmakeMove(move);
                        continue;
                    }
                } else {
                    if (board.isKingInCheck<PieceColor::WHITE>()) {
                        board.unmakeMove(move);
                        continue;
                    }
                }

                Line pvLine = {};
                Move previousMove = {};
                int score = search(board, depth - 1, -9999999, 9999999, move, previousMove, endTime, pvLine, engine);
                score *= -1;
                board.unmakeMove(move);

                if (score > iterationScore && std::chrono::high_resolution_clock::now() < endTime) {
                    assert(move.piece != PieceType::EMPTY);
                    iterationScore = score;
                    iterationMove = move;

                    iterationPvLine.moves[0] = move;
                    memcpy(iterationPvLine.moves + 1, pvLine.moves, pvLine.moveCount * sizeof(Move));
                    iterationPvLine.moveCount = pvLine.moveCount + 1;

                    searchStats.score = iterationScore;
                }

                searchStats.pv = "";
                for (int i = 0; i < iterationPvLine.moveCount; i++) {
                    Move move = iterationPvLine.moves[i];

                    searchStats.pv += getNotation(move.from) + getNotation(move.to);

                    if (i != iterationPvLine.moveCount - 1) {
                        searchStats.pv += " ";
                    }
                }

                searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - startTime).count();
                senjo::Output(senjo::Output::NoPrefix) << searchStats;
            }

            if (depth == 1 || std::chrono::high_resolution_clock::now() < endTime) {
                assert(iterationMove.piece != PieceType::EMPTY);
                bestScore = iterationScore;
                bestMove = iterationMove;
                searchStats.score = bestScore;
            }

            iterationScore = -1000000;
            iterationMove = {};
        }

        searchStats.pv = "";
        for (int i = 0; i < iterationPvLine.moveCount; i++) {
            Move move = iterationPvLine.moves[i];

            searchStats.pv += getNotation(move.from) + getNotation(move.to);

            if (i != iterationPvLine.moveCount - 1) {
                searchStats.pv += " ";
            }
        }

        searchStats.score = bestScore;
        searchStats.msecs = duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count();
        isSearching = false;
        senjo::Output(senjo::Output::NoPrefix) << searchStats;
        return bestMove;
    }

    int SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                                       Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine, ZagreusEngine &engine) {
        searchStats.nodes += 1;

        if (searchStats.nodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return beta;
        }

        if (depth == 0 || board.isWinner<PieceColor::WHITE>() || board.isWinner<PieceColor::BLACK>() || board.isDraw()) {
            pvLine.moveCount = 0;
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        Line line{};
        MoveList moveList;

        if (board.getMovingColor() == PieceColor::WHITE) {
            moveList = generateMoves<PieceColor::WHITE>(board);
        } else {
            moveList = generateMoves<PieceColor::BLACK>(board);
        }

        MovePicker moves = MovePicker(moveList);
        bool searchedFirstLegalMove = false;

        while (!searchedFirstLegalMove && moves.hasNext()) {
            Move move = moves.getNextMove();

            board.makeMove(move);

            if (board.getMovingColor() == PieceColor::WHITE) {
                if (board.isKingInCheck<PieceColor::BLACK>()) {
                    board.unmakeMove(move);
                    continue;
                }
            } else {
                if (board.isKingInCheck<PieceColor::WHITE>()) {
                    board.unmakeMove(move);
                    continue;
                }
            }

            int score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line, engine);
            score *= -1;

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    return score;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
            }

            searchedFirstLegalMove = true;
        }

        if (!searchedFirstLegalMove) {
            return beta;
        }

        while (moves.hasNext()) {
            if (searchStats.nodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
                return beta;
            }

            Move move = moves.getNextMove();

            board.makeMove(move);

            if (board.getMovingColor() == PieceColor::WHITE) {
                if (board.isKingInCheck<PieceColor::BLACK>()) {
                    board.unmakeMove(move);
                    continue;
                }
            } else {
                if (board.isKingInCheck<PieceColor::WHITE>()) {
                    board.unmakeMove(move);
                    continue;
                }
            }

            int score;
            score = search(board, depth - 1, -alpha - 1, -alpha, rootMove, previousMove, endTime, line, engine);
            score *= -1;

            if (score > alpha && score < beta) {
                score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line, engine);
                score *= -1;
            }

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    return beta;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
            }
        }

        return alpha;
    }

    int SearchManager::quiesce(Bitboard &board, int alpha, int beta, Move &rootMove,
                                        Move &previousMove,
                                        std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine) {
        searchStats.qnodes += 1;

        if (searchStats.qnodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return beta;
        }

        int standPat = evaluate(board, endTime, engine);

        if (standPat >= beta) {
            return beta;
        }

        int delta = 1000;

        if (previousMove.promotionPiece != PieceType::EMPTY) {
            delta += getPieceWeight(previousMove.promotionPiece) - 100;
        }

        if (standPat < alpha - delta) {
            return alpha;
        }

        if (alpha < standPat) {
            alpha = standPat;
        }

        MoveList moveList;

        if (board.getMovingColor() == PieceColor::WHITE) {
            moveList = generateQuiescenceMoves<PieceColor::WHITE>(board);
        } else {
            moveList = generateQuiescenceMoves<PieceColor::BLACK>(board);
        }

        MovePicker moves = MovePicker(moveList);
        while (moves.hasNext()) {
            Move move = moves.getNextMove();
            assert(move.from != move.to);

            if (searchStats.qnodes % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
                return beta;
            }

            // -1 means it's not a capture
            if (move.captureScore <= -1) {
                continue;
            }

            board.makeMove(move);

            if (board.getMovingColor() == PieceColor::WHITE) {
                if (board.isKingInCheck<PieceColor::BLACK>()) {
                    board.unmakeMove(move);
                    continue;
                }
            } else {
                if (board.isKingInCheck<PieceColor::WHITE>()) {
                    board.unmakeMove(move);
                    continue;
                }
            }

            int score = quiesce(board, -beta, -alpha, rootMove, move, endTime, engine);
            score *= -1;
            board.unmakeMove(move);

            if (score >= beta) {
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }

        return alpha;
    }

    bool SearchManager::isCurrentlySearching() {
        return isSearching;
    }

    senjo::SearchStats SearchManager::getSearchStats() {
        return searchStats;
    }

    int SearchManager::evaluate(Bitboard &board, std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, ZagreusEngine &engine) {
        if ((searchStats.nodes + searchStats.qnodes) % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return 0;
        }

        int modifier = board.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (board.isWinner<PieceColor::WHITE>()) {
            return 10000 * modifier;
        } else if (board.isWinner<PieceColor::BLACK>()) {
            return -10000 * modifier;
        }

        if (board.isDraw()) {
            // Thanks Stockfish for the "3-fold blindness avoidance"
            return 0 - 1 + ((int) searchStats.nodes & 0x2);
        }

        EvalContext evalContext = createEvalContext(board);

        getWhiteMaterialScore(evalContext, board);
        getBlackMaterialScore(evalContext, board);

        getWhitePositionalScore(evalContext, board);
        getBlackPositionalScore(evalContext, board);

        int whiteScore = ((evalContext.whiteMidgameScore * (256 - evalContext.phase)) + (evalContext.whiteEndgameScore * evalContext.phase)) / 256;
        int blackScore = ((evalContext.blackMidgameScore * (256 - evalContext.phase)) + (evalContext.blackEndgameScore * evalContext.phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    void getBlackMaterialScore(EvalContext &evalContext, Bitboard &board) {
        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_PAWN>()) * getPieceWeight(BLACK_PAWN);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_PAWN>()) * getPieceWeight(BLACK_PAWN);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_KNIGHT>()) * getPieceWeight(BLACK_KNIGHT);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_KNIGHT>()) * getPieceWeight(BLACK_KNIGHT);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_BISHOP>()) * getPieceWeight(BLACK_BISHOP);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_BISHOP>()) * getPieceWeight(BLACK_BISHOP);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_ROOK>()) * getPieceWeight(BLACK_ROOK);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_ROOK>()) * getPieceWeight(BLACK_ROOK);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_QUEEN>()) * getPieceWeight(BLACK_QUEEN);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_QUEEN>()) * getPieceWeight(BLACK_QUEEN);
    }

    void getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board) {
        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_PAWN>()) * getPieceWeight(WHITE_PAWN);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_PAWN>()) * getPieceWeight(WHITE_PAWN);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_KNIGHT>()) * getPieceWeight(WHITE_KNIGHT);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_KNIGHT>()) * getPieceWeight(WHITE_KNIGHT);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_BISHOP>()) * getPieceWeight(WHITE_BISHOP);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_BISHOP>()) * getPieceWeight(WHITE_BISHOP);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_ROOK>()) * getPieceWeight(WHITE_ROOK);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_ROOK>()) * getPieceWeight(WHITE_ROOK);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_QUEEN>()) * getPieceWeight(WHITE_QUEEN);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_QUEEN>()) * getPieceWeight(WHITE_QUEEN);
    }

    void getWhitePositionalScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t colorBB = bitboard.getColorBoard<PieceColor::WHITE>();

        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            evalContext.whiteMidgameScore += getMidgamePstValue(pieceOnSquare, index);
            evalContext.whiteEndgameScore += getEndgamePstValue(pieceOnSquare, index);

            colorBB = _blsr_u64(colorBB);
        }
    }

    void getBlackPositionalScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t colorBB = bitboard.getColorBoard<PieceColor::BLACK>();

        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            evalContext.blackMidgameScore += getMidgamePstValue(pieceOnSquare, index);
            evalContext.blackEndgameScore += getEndgamePstValue(pieceOnSquare, index);

            colorBB = _blsr_u64(colorBB);
        }
    }

    int knightPhase = 1;
    int bishopPhase = 1;
    int rookPhase = 2;
    int queenPhase = 4;
    int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;
    int getGamePhase(Bitboard &bitboard) {
        int phase = totalPhase;

        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_KNIGHT>() | bitboard.getPieceBoard<PieceType::BLACK_KNIGHT>()) * knightPhase;
        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_BISHOP>() | bitboard.getPieceBoard<PieceType::BLACK_BISHOP>()) * bishopPhase;
        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_ROOK>() | bitboard.getPieceBoard<PieceType::BLACK_ROOK>()) * rookPhase;
        phase -= popcnt(bitboard.getPieceBoard<PieceType::WHITE_QUEEN>() | bitboard.getPieceBoard<PieceType::BLACK_QUEEN>()) * queenPhase;

        return (phase * 256 + (totalPhase / 2)) / totalPhase;
    }

    EvalContext createEvalContext(Bitboard &bitboard) {
        uint64_t whitePawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();
        uint64_t whiteKnightBB = bitboard.getPieceBoard<PieceType::WHITE_KNIGHT>();
        uint64_t whiteBishopBB = bitboard.getPieceBoard<PieceType::WHITE_BISHOP>();
        uint64_t whiteRookBB = bitboard.getPieceBoard<PieceType::WHITE_ROOK>();
        uint64_t whiteQueenBB = bitboard.getPieceBoard<PieceType::WHITE_QUEEN>();

        uint64_t blackPawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();
        uint64_t blackKnightBB = bitboard.getPieceBoard<PieceType::BLACK_KNIGHT>();
        uint64_t blackBishopBB = bitboard.getPieceBoard<PieceType::BLACK_BISHOP>();
        uint64_t blackRookBB = bitboard.getPieceBoard<PieceType::BLACK_ROOK>();
        uint64_t blackQueenBB = bitboard.getPieceBoard<PieceType::BLACK_QUEEN>();

        uint64_t whitePawnAttacks = calculatePawnAttacks<PieceColor::WHITE>(whitePawnBB);
        uint64_t whiteKnightAttacks = 0;
        uint64_t whiteBishopAttacks = 0;
        uint64_t whiteRookAttacks = 0;
        uint64_t whiteQueenAttacks = 0;

        uint64_t blackPawnAttacks = calculatePawnAttacks<PieceColor::BLACK>(blackPawnBB);
        uint64_t blackKnightAttacks = 0;
        uint64_t blackBishopAttacks = 0;
        uint64_t blackRookAttacks = 0;
        uint64_t blackQueenAttacks = 0;
        uint64_t attacksFrom[64] = { 0ULL };

        while (whiteKnightBB) {
            uint64_t index = bitscanForward(whiteKnightBB);

            uint64_t attacks = bitboard.getKnightAttacks(index);
            whiteKnightAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteKnightBB &= ~(1ULL << index);
        }

        while (blackKnightBB) {
            uint64_t index = bitscanForward(blackKnightBB);

            uint64_t attacks = bitboard.getKnightAttacks(index);
            blackKnightAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackKnightBB &= ~(1ULL << index);
        }

        while (whiteBishopBB) {
            uint64_t index = bitscanForward(whiteBishopBB);

            uint64_t attacks = bitboard.getBishopAttacks(index);
            whiteBishopAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteBishopBB &= ~(1ULL << index);
        }

        while (whiteRookBB) {
            uint64_t index = bitscanForward(whiteRookBB);

            uint64_t attacks = bitboard.getRookAttacks(index);
            whiteRookAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteRookBB &= ~(1ULL << index);
        }

        while (whiteQueenBB) {
            uint64_t index = bitscanForward(whiteQueenBB);

            uint64_t attacks = bitboard.getQueenAttacks(index);
            whiteQueenAttacks |= attacks;
            attacksFrom[index] = attacks;

            whiteQueenBB &= ~(1ULL << index);
        }

        while (blackBishopBB) {
            uint64_t index = bitscanForward(blackBishopBB);

            uint64_t attacks = bitboard.getBishopAttacks(index);
            blackBishopAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackBishopBB &= ~(1ULL << index);
        }

        while (blackRookBB) {
            uint64_t index = bitscanForward(blackRookBB);

            uint64_t attacks = bitboard.getRookAttacks(index);
            blackRookAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackRookBB &= ~(1ULL << index);
        }

        while (blackQueenBB) {
            uint64_t index = bitscanForward(blackQueenBB);

            uint64_t attacks = bitboard.getQueenAttacks(index);
            blackQueenAttacks |= attacks;
            attacksFrom[index] = attacks;

            blackQueenBB &= ~(1ULL << index);
        }

        uint64_t whiteCombinedAttacks = whiteKnightAttacks | whiteBishopAttacks | whiteRookAttacks | whiteQueenAttacks | whitePawnAttacks;
        uint64_t blackCombinedAttacks = blackKnightAttacks | blackBishopAttacks | blackRookAttacks | blackQueenAttacks | blackPawnAttacks;

        auto context =  EvalContext {
                getGamePhase(bitboard),
                0,
                0,
                0,
                0,
                whitePawnAttacks,
                whiteKnightAttacks,
                whiteBishopAttacks,
                whiteRookAttacks,
                whiteQueenAttacks,
                whiteCombinedAttacks,
                blackPawnAttacks,
                blackKnightAttacks,
                blackBishopAttacks,
                blackRookAttacks,
                blackQueenAttacks,
                blackCombinedAttacks,
                {}
        };

        for (int i = 0; i < 64; i++) {
            context.attacksFrom[i] = attacksFrom[i];
        }

        return context;
    }
}

#pragma clang diagnostic pop