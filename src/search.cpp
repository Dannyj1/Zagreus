/*
 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <chrono>

#include "search.h"
#include "timemanager.h"
#include "movegen.h"
#include "../senjo/Output.h"
#include "movepicker.h"
#include "pst.h"
#include "tt.h"
#include "features.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {
    float SearchManager::calculateCertainty(Bitboard &board, int depth, int bestMoveChanges, int scoreChange) {
        float certainty = 0.0f;

        certainty += (float) depth * 0.1f;

        if (bestMoveChanges > 0) {
            certainty -= (float) bestMoveChanges * (0.25f - (((float) bestMoveChanges - 1.0f) * 0.03f));
        }

        // Less certainty in the opening
        if (board.getPly() / 2 < 20) {
            certainty -= 0.15f;
        }

        certainty += ((float) scoreChange / 1000.0f);

        // If moving color in check add uncertainty
        if (board.getMovingColor() == PieceColor::WHITE) {
            if (board.isKingInCheck<PieceColor::WHITE>()) {
                certainty -= 0.2f;
            }
        } else {
            if (board.isKingInCheck<PieceColor::BLACK>()) {
                certainty -= 0.2f;
            }
        }

        // Max certainty [-1.5, 1.5]
        return std::min(std::max(certainty, -1.5f), 1.5f);
    }

    Move SearchManager::getBestMove(senjo::GoParams &params, ZagreusEngine &engine, Bitboard &board) {
        searchStats = {};
        isSearching = true;
        int bestScore = -1000000;
        Move bestMove = {};
        int iterationScore = -1000000;
        Move iterationMove = {};
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
        std::chrono::time_point<std::chrono::high_resolution_clock> initialEndTime = getEndTime(params, board, engine, board.getMovingColor());
        std::chrono::time_point<std::chrono::high_resolution_clock> endTime = initialEndTime;
        int depth = 0;
        int bestMoveChanges = 0;
        int scoreChange = 0;
        float certainty = 0.0f;

        TranspositionTable::getTT()->ageHistoryTable();

        Line iterationPvLine = {};
        while (!engine.stopRequested()) {
            endTime = initialEndTime;
            depth += 1;

            if (params.depth > 0 && depth > params.depth) {
                return bestMove;
            }

            // Update certainty and endtime
            if (params.depth == 0) {
                certainty = calculateCertainty(board, depth, bestMoveChanges, scoreChange);

                // Based on certainty, adjust the initial end time. Negative certainty means we are less certain, so we should search longer
                float timeChange = std::chrono::duration_cast<std::chrono::milliseconds>(startTime - initialEndTime).count() * certainty;
                endTime = initialEndTime + std::chrono::milliseconds((int) timeChange);

                // Lower with 0.075 every iteration, start at 1.0 and end at 0.5;
                float earlyCutoff = std::max(0.5f, 1.1f - (depth * 0.075f));

                if (std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * earlyCutoff) {
                    break;
                }
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

                int score = search(board, depth, -9999999, 9999999, move, previousMove, endTime, pvLine, engine, true);
                score *= -1;

                board.unmakeMove(move);

                if (iterationScore == -1000000 || (score > iterationScore && std::chrono::high_resolution_clock::now() < endTime)) {
                    assert(move.piece != PieceType::EMPTY);

                    if (iterationScore > -900000) {
                        bestMoveChanges++;
                        scoreChange = score - iterationScore;
                    }

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

                // Update certainty and endtime
                if (params.depth == 0) {
                    certainty = calculateCertainty(board, depth, bestMoveChanges, scoreChange);

                    // Based on certainty, adjust the initial end time. Negative certainty means we are less certain, so we should search longer
                    float timeChange = std::chrono::duration_cast<std::chrono::milliseconds>(startTime - initialEndTime).count() * certainty;
                    endTime = initialEndTime + std::chrono::milliseconds((int) timeChange);

                    // Lower with 0.075 every iteration, start at 1.0 and end at 0.5;
                    float earlyCutoff = std::max(0.5f, 1.0f - (depth * 0.075f));

                    if (std::chrono::high_resolution_clock::now() - startTime > (endTime - startTime) * earlyCutoff) {
                        break;
                    }
                }
            }

            if (depth == 1 || bestScore == -1000000 || std::chrono::high_resolution_clock::now() < endTime) {
                assert(iterationMove.piece != PieceType::EMPTY);

                if (bestScore > -900000) {
                    scoreChange = iterationScore - bestScore;
                }

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

    // TODO: use template
    int SearchManager::search(Bitboard &board, int depth, int alpha, int beta, Move &rootMove,
                                       Move &previousMove,
                                       std::chrono::time_point<std::chrono::high_resolution_clock> &endTime, Line &pvLine, ZagreusEngine &engine, bool isPv) {
        searchStats.nodes += 1;

        if (searchStats.nodes % 2048 == 0 &&
            (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return beta;
        }

        bool isOwnKingInCheck = false;
        bool depthExtended = false;

        if (board.getMovingColor() == PieceColor::WHITE) {
            isOwnKingInCheck = board.isKingInCheck<PieceColor::WHITE>();
        } else {
            isOwnKingInCheck = board.isKingInCheck<PieceColor::BLACK>();
        }

        if (isOwnKingInCheck) {
            depth += 1;
            depthExtended = true;
        }

        if (depth == 0 || board.isWinner<PieceColor::WHITE>() || board.isWinner<PieceColor::BLACK>() ||
            board.isDraw()) {
            pvLine.moveCount = 0;
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        Line line{};
        Move bestMove = {};
        int bestScore = -1000000;
        MoveList moveList;
        NodeType nodeType = NodeType::FAIL_LOW_NODE;

        if (board.getMovingColor() == PieceColor::WHITE) {
            moveList = generateMoves<PieceColor::WHITE>(board);
        } else {
            moveList = generateMoves<PieceColor::BLACK>(board);
        }

        MovePicker moves = MovePicker(moveList);
        bool searchedFirstLegalMove = false;

        while (isPv && !searchedFirstLegalMove && moves.hasNext()) {
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

            int score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                               engine, true);
            score *= -1;

            if (score > bestScore) {
                bestMove = move;
            }

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(
                                move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta,
                                                             NodeType::FAIL_HIGH_NODE,
                                                             encodeMove(bestMove));
                    return score;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
                nodeType = NodeType::PV_NODE;
            }

            searchedFirstLegalMove = true;
        }

        if (isPv && !searchedFirstLegalMove) {
            return quiesce(board, alpha, beta, rootMove, previousMove, endTime, engine);
        }

        int ttScore = TranspositionTable::getTT()->getScore(board.getZobristHash(), depth, alpha, beta);

        if (!isPv && ttScore != INT32_MIN) {
            return ttScore;
        }

        while (moves.hasNext()) {
            if (searchStats.nodes % 2048 == 0 &&
                (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
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

            int depthReduction = 0;
            bool isOpponentKingInCheck;

            if (board.getMovingColor() == PieceColor::WHITE) {
                isOpponentKingInCheck = board.isKingInCheck<PieceColor::WHITE>();
            } else {
                isOpponentKingInCheck = board.isKingInCheck<PieceColor::BLACK>();
            }

            if (!depthExtended) {
                if (depth >= 3 && moves.movesSearched() > 4 && move.captureScore != -1 &&
                    move.promotionPiece == PieceType::EMPTY && !isOwnKingInCheck && !isOpponentKingInCheck) {
                    depthReduction = depth / 2;
                }
            }

            int score;
            score = search(board, depth - 1 - depthReduction, -alpha - 1, -alpha, rootMove,
                           previousMove, endTime, line, engine, false);
            score *= -1;

            if (score > alpha && score < beta) {
                score = search(board, depth - 1, -beta, -alpha, rootMove, previousMove, endTime, line,
                               engine, false);
                score *= -1;
            }

            board.unmakeMove(move);

            if (score > alpha) {
                if (score >= beta) {
                    if (move.captureScore == NO_CAPTURE_SCORE) {
                        TranspositionTable::getTT()->killerMoves[2][board.getPly()] = TranspositionTable::getTT()->killerMoves[1][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[1][board.getPly()] = TranspositionTable::getTT()->killerMoves[0][board.getPly()];
                        TranspositionTable::getTT()->killerMoves[0][board.getPly()] = encodeMove(move);
                        TranspositionTable::getTT()->counterMoves[previousMove.from][previousMove.to] = encodeMove(
                                move);
                        TranspositionTable::getTT()->historyMoves[move.piece][move.to] += depth * depth;
                    }

                    TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, beta,
                                                             NodeType::FAIL_HIGH_NODE, encodeMove(bestMove));
                    return beta;
                }

                pvLine.moves[0] = move;
                pvLine.moveCount = 1;
                memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
                pvLine.moveCount = line.moveCount + 1;
                alpha = score;
                nodeType = NodeType::PV_NODE;
            }
        }

        TranspositionTable::getTT()->addPosition(board.getZobristHash(), depth, alpha, nodeType, encodeMove(bestMove));
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

            if (move.captureScore <= NO_CAPTURE_SCORE) {
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
        if (!engine.isTuning() && (searchStats.nodes + searchStats.qnodes) % 2048 == 0 && (engine.stopRequested() || std::chrono::high_resolution_clock::now() > endTime)) {
            return 0;
        }

        int modifier = board.getMovingColor() == PieceColor::WHITE ? 1 : -1;

        if (!engine.isTuning()) {
            if (board.isWinner<PieceColor::WHITE>()) {
                return (15000 - board.getPly()) * modifier;
            } else if (board.isWinner<PieceColor::BLACK>()) {
                return (-15000 + board.getPly()) * modifier;
            }

            if (board.isDraw()) {
                // Thanks to Stockfish for the "3-fold blindness avoidance" idea
                return 0 - 1 + ((int) searchStats.nodes & 0x2);
            }
        }

        EvalContext evalContext = createEvalContext(board);

        getWhiteMaterialScore(evalContext, board);
        getBlackMaterialScore(evalContext, board);

        getWhitePositionalScore(evalContext, board);
        getBlackPositionalScore(evalContext, board);

        getWhiteMobilityScore(evalContext, board);
        getBlackMobilityScore(evalContext, board);

        getWhiteKingScore(evalContext, board);
        getBlackKingScore(evalContext, board);

        getWhiteConnectivityScore(evalContext, board);
        getBlackConnectivityScore(evalContext, board);

        getWhiteRookScore(evalContext, board);
        getBlackRookScore(evalContext, board);

        getWhiteBishopScore(evalContext, board);
        getBlackBishopScore(evalContext, board);

        getPawnScore<PieceColor::WHITE>(evalContext, board);
        getPawnScore<PieceColor::BLACK>(evalContext, board);

        int whiteScore = ((evalContext.whiteMidgameScore * (256 - evalContext.phase)) + (evalContext.whiteEndgameScore * evalContext.phase)) / 256;
        int blackScore = ((evalContext.blackMidgameScore * (256 - evalContext.phase)) + (evalContext.blackEndgameScore * evalContext.phase)) / 256;

        return (whiteScore - blackScore) * modifier;
    }

    void getBlackMaterialScore(EvalContext &evalContext, Bitboard &board) {
        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_PAWN>()) * getEvalValue(MIDGAME_PAWN_MATERIAL);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_PAWN>()) * getEvalValue(ENDGAME_PAWN_MATERIAL);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_KNIGHT>()) * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_KNIGHT>()) * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_BISHOP>()) * getEvalValue(MIDGAME_BISHOP_MATERIAL);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_BISHOP>()) * getEvalValue(ENDGAME_BISHOP_MATERIAL);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_ROOK>()) * getEvalValue(MIDGAME_ROOK_MATERIAL);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_ROOK>()) * getEvalValue(ENDGAME_ROOK_MATERIAL);

        evalContext.blackMidgameScore += popcnt(board.getPieceBoard<BLACK_QUEEN>()) * getEvalValue(MIDGAME_QUEEN_MATERIAL);
        evalContext.blackEndgameScore += popcnt(board.getPieceBoard<BLACK_QUEEN>()) * getEvalValue(ENDGAME_QUEEN_MATERIAL);
    }

    void getWhiteMaterialScore(EvalContext &evalContext, Bitboard &board) {
        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_PAWN>()) * getEvalValue(MIDGAME_PAWN_MATERIAL);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_PAWN>()) * getEvalValue(ENDGAME_PAWN_MATERIAL);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_KNIGHT>()) * getEvalValue(MIDGAME_KNIGHT_MATERIAL);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_KNIGHT>()) * getEvalValue(ENDGAME_KNIGHT_MATERIAL);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_BISHOP>()) * getEvalValue(MIDGAME_BISHOP_MATERIAL);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_BISHOP>()) * getEvalValue(ENDGAME_BISHOP_MATERIAL);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_ROOK>()) * getEvalValue(MIDGAME_ROOK_MATERIAL);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_ROOK>()) * getEvalValue(ENDGAME_ROOK_MATERIAL);

        evalContext.whiteMidgameScore += popcnt(board.getPieceBoard<WHITE_QUEEN>()) * getEvalValue(MIDGAME_QUEEN_MATERIAL);
        evalContext.whiteEndgameScore += popcnt(board.getPieceBoard<WHITE_QUEEN>()) * getEvalValue(ENDGAME_QUEEN_MATERIAL);
    }

    void getWhitePositionalScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t colorBB = bitboard.getColorBoard<PieceColor::WHITE>();

        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            evalContext.whiteMidgameScore += getMidgamePstValue(pieceOnSquare, index);
            evalContext.whiteEndgameScore += getEndgamePstValue(pieceOnSquare, index);

            colorBB &= ~(1ULL << index);
        }
    }

    void getBlackPositionalScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t colorBB = bitboard.getColorBoard<PieceColor::BLACK>();

        while (colorBB) {
            uint64_t index = bitscanForward(colorBB);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);

            evalContext.blackMidgameScore += getMidgamePstValue(pieceOnSquare, index);
            evalContext.blackEndgameScore += getEndgamePstValue(pieceOnSquare, index);

            colorBB &= ~(1ULL << index);
        }
    }

    void getWhiteMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getColorBoard<PieceColor::WHITE>();
        uint64_t ownPiecesBBLoop = bitboard.getColorBoard<PieceColor::WHITE>()  & ~(evalContext.blackPawnAttacks);

        // Slight bonus for squares defended by own pawn
        evalContext.whiteMidgameScore += popcnt((evalContext.whiteCombinedAttacks) & evalContext.whitePawnAttacks) * getEvalValue(MIDGAME_SQUARE_DEFENDED_BY_PAWN);
        evalContext.whiteEndgameScore += popcnt((evalContext.whiteCombinedAttacks) & evalContext.whitePawnAttacks) * getEvalValue(ENDGAME_SQUARE_DEFENDED_BY_PAWN);

        while (ownPiecesBBLoop) {
            int index = bitscanForward(ownPiecesBBLoop);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = evalContext.attacksFrom[index];

            switch (pieceOnSquare) {
                case WHITE_KNIGHT:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_KNIGHT_MOBILITY);
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_KNIGHT_MOBILITY);
                    break;
                case WHITE_BISHOP:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_BISHOP_MOBILITY);
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_BISHOP_MOBILITY);
                    break;
                case WHITE_ROOK:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_ROOK_MOBILITY);
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_ROOK_MOBILITY);
                    break;
                case WHITE_QUEEN:
                    evalContext.whiteMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_QUEEN_MOBILITY);
                    evalContext.whiteEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_QUEEN_MOBILITY);
                    break;
            }

            ownPiecesBBLoop &= ~(1ULL << index);
        }
    }

    void getBlackMobilityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t ownPiecesBB = bitboard.getColorBoard<PieceColor::BLACK>();
        uint64_t ownPiecesBBLoop = bitboard.getColorBoard<PieceColor::BLACK>()  & ~(evalContext.whitePawnAttacks);

        // Slight bonus for squares defended by own pawn
        evalContext.blackMidgameScore += popcnt((evalContext.blackCombinedAttacks) & evalContext.blackPawnAttacks) * getEvalValue(MIDGAME_SQUARE_DEFENDED_BY_PAWN);
        evalContext.blackEndgameScore += popcnt((evalContext.blackCombinedAttacks) & evalContext.blackPawnAttacks) * getEvalValue(ENDGAME_SQUARE_DEFENDED_BY_PAWN);

        while (ownPiecesBBLoop) {
            int index = bitscanForward(ownPiecesBBLoop);
            PieceType pieceOnSquare = bitboard.getPieceOnSquare(index);
            uint64_t attacks = evalContext.attacksFrom[index];

            switch (pieceOnSquare) {
                case BLACK_KNIGHT:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_KNIGHT_MOBILITY);
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_KNIGHT_MOBILITY);
                    break;
                case BLACK_BISHOP:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_BISHOP_MOBILITY);
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_BISHOP_MOBILITY);
                    break;
                case BLACK_ROOK:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_ROOK_MOBILITY);
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_ROOK_MOBILITY);
                    break;
                case BLACK_QUEEN:
                    evalContext.blackMidgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(MIDGAME_QUEEN_MOBILITY);
                    evalContext.blackEndgameScore += popcnt(attacks & ~ownPiecesBB) * getEvalValue(ENDGAME_QUEEN_MOBILITY);
                    break;
            }

            ownPiecesBBLoop &= ~(1ULL << index);
        }
    }

    uint64_t whiteQueenCastlingAttackPattern = 0x70;
    uint64_t whiteKingCastlingAttackPattern = 0x6;
    void getWhiteKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::WHITE_KING>();
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();
        uint64_t safetyMask = nortOne(kingBB) | noEaOne(kingBB) | noWeOne(kingBB);
        safetyMask |= nortOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.whiteMidgameScore += std::min(getEvalValue(MIDGAME_PAWN_SHIELD) * 3, (int) (popcnt(pawnShield) * getEvalValue(MIDGAME_PAWN_SHIELD)));
        evalContext.whiteEndgameScore += std::min(getEvalValue(ENDGAME_PAWN_SHIELD) * 3, (int) (popcnt(pawnShield) * getEvalValue(ENDGAME_PAWN_SHIELD)));

        if (bitboard.isSemiOpenFileLenient<PieceColor::WHITE>(kingLocation)) {
            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KING_ON_OPEN_FILE);
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KING_ON_OPEN_FILE);
        }

        if (kingLocation % 8 != 0) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::WHITE>(kingLocation - 1)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        if (kingLocation % 8 != 7) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::WHITE>(kingLocation + 1)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        uint8_t castlingRights = bitboard.getCastlingRights();

        if (!(castlingRights & CastlingRights::WHITE_QUEENSIDE) && !(castlingRights & CastlingRights::WHITE_KINGSIDE)) {
            if (!(bitboard.getCastlingRights() & CastlingRights::HAS_WHITE_CASTLED)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_NO_CASTLING_RIGHTS);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_NO_CASTLING_RIGHTS);
            }
        } else {
            if ((castlingRights & CastlingRights::WHITE_QUEENSIDE) && (evalContext.blackCombinedAttacks & whiteQueenCastlingAttackPattern)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_QUEENSIDE_CASTLING_PREVENTED);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_QUEENSIDE_CASTLING_PREVENTED);
            }

            if ((castlingRights & CastlingRights::WHITE_KINGSIDE) && (evalContext.blackCombinedAttacks & whiteKingCastlingAttackPattern)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KINGSIDE_CASTLING_PREVENTED);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KINGSIDE_CASTLING_PREVENTED);
            }
        }

        evalContext.whiteMidgameScore += popcnt(evalContext.blackBishopAttacks & kingAttacks) * getEvalValue(MIDGAME_BISHOP_ATTACK_NEAR_KING);
        evalContext.whiteEndgameScore += popcnt(evalContext.blackBishopAttacks & kingAttacks) * getEvalValue(ENDGAME_BISHOP_ATTACK_NEAR_KING);
        evalContext.whiteMidgameScore += popcnt(evalContext.blackRookAttacks & kingAttacks) * getEvalValue(MIDGAME_ROOK_ATTACK_NEAR_KING);
        evalContext.whiteEndgameScore += popcnt(evalContext.blackRookAttacks & kingAttacks) * getEvalValue(ENDGAME_ROOK_ATTACK_NEAR_KING);
        evalContext.whiteMidgameScore += popcnt(evalContext.blackQueenAttacks & kingAttacks) * getEvalValue(MIDGAME_QUEEN_ATTACK_NEAR_KING);
        evalContext.whiteEndgameScore += popcnt(evalContext.blackQueenAttacks & kingAttacks) * getEvalValue(ENDGAME_QUEEN_ATTACK_NEAR_KING);
    }

    uint64_t blackQueenCastlingAttackPattern = 0x7000000000000000;
    uint64_t blackKingCastlingAttackPattern = 0x600000000000000;
    void getBlackKingScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::BLACK_KING>();
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t kingAttacks = bitboard.getKingAttacks(kingLocation);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();
        uint64_t safetyMask = soutOne(kingBB) | soEaOne(kingBB) | soWeOne(kingBB);
        safetyMask |= soutOne(safetyMask);
        uint64_t pawnShield = pawnBB & safetyMask;
        evalContext.blackMidgameScore += std::min(getEvalValue(MIDGAME_PAWN_SHIELD) * 3, (int) (popcnt(pawnShield) * getEvalValue(MIDGAME_PAWN_SHIELD)));
        evalContext.blackEndgameScore += std::min(getEvalValue(ENDGAME_PAWN_SHIELD) * 3, (int) (popcnt(pawnShield) * getEvalValue(ENDGAME_PAWN_SHIELD)));

        if (bitboard.isSemiOpenFileLenient<PieceColor::BLACK>(kingLocation)) {
            evalContext.blackMidgameScore += getEvalValue(MIDGAME_KING_ON_OPEN_FILE);
            evalContext.blackEndgameScore += getEvalValue(ENDGAME_KING_ON_OPEN_FILE);
        }

        if (kingLocation % 8 != 0) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::BLACK>(kingLocation - 1)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }

        if (kingLocation % 8 != 7) {
            if (bitboard.isSemiOpenFileLenient<PieceColor::BLACK>(kingLocation + 1)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_KING_NEXT_TO_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_KING_NEXT_TO_OPEN_FILE);
            }
        }
        
        uint8_t castlingRights = bitboard.getCastlingRights();
        if (!(castlingRights & CastlingRights::BLACK_QUEENSIDE) && !(castlingRights & CastlingRights::BLACK_KINGSIDE)) {
            if (!(bitboard.getCastlingRights() & CastlingRights::HAS_BLACK_CASTLED)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_NO_CASTLING_RIGHTS);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_NO_CASTLING_RIGHTS);
            }
        } else {
            if ((castlingRights & CastlingRights::BLACK_QUEENSIDE) && (evalContext.whiteCombinedAttacks & blackQueenCastlingAttackPattern)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_QUEENSIDE_CASTLING_PREVENTED);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_QUEENSIDE_CASTLING_PREVENTED);
            }

            if ((castlingRights & CastlingRights::BLACK_KINGSIDE) && (evalContext.whiteCombinedAttacks & blackKingCastlingAttackPattern)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_KINGSIDE_CASTLING_PREVENTED);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_KINGSIDE_CASTLING_PREVENTED);
            }
        }

        evalContext.blackMidgameScore += popcnt(evalContext.whiteBishopAttacks & kingAttacks) * getEvalValue(MIDGAME_BISHOP_ATTACK_NEAR_KING);
        evalContext.blackEndgameScore += popcnt(evalContext.whiteBishopAttacks & kingAttacks) * getEvalValue(ENDGAME_BISHOP_ATTACK_NEAR_KING);
        evalContext.blackMidgameScore += popcnt(evalContext.whiteRookAttacks & kingAttacks) * getEvalValue(MIDGAME_ROOK_ATTACK_NEAR_KING);
        evalContext.blackEndgameScore += popcnt(evalContext.whiteRookAttacks & kingAttacks) * getEvalValue(ENDGAME_ROOK_ATTACK_NEAR_KING);
        evalContext.blackMidgameScore += popcnt(evalContext.whiteQueenAttacks & kingAttacks) * getEvalValue(MIDGAME_QUEEN_ATTACK_NEAR_KING);
        evalContext.blackEndgameScore += popcnt(evalContext.whiteQueenAttacks & kingAttacks) * getEvalValue(ENDGAME_QUEEN_ATTACK_NEAR_KING);
    }

    void getWhiteConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::WHITE_KING>();
        uint64_t whitePieces = bitboard.getColorBoard<PieceColor::WHITE>() & ~kingBB;
        uint64_t protectedPieces = whitePieces & evalContext.whiteCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);

            switch (pieceType) {
                case WHITE_PAWN:
                case BLACK_PAWN:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PAWN_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PAWN_CONNECTIVITY);
                    break;
                case WHITE_KNIGHT:
                case BLACK_KNIGHT:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_KNIGHT_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_KNIGHT_CONNECTIVITY);
                    break;
                case WHITE_BISHOP:
                case BLACK_BISHOP:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_CONNECTIVITY);
                    break;
                case WHITE_ROOK:
                case BLACK_ROOK:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_CONNECTIVITY);
                    break;
                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_QUEEN_CONNECTIVITY);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_QUEEN_CONNECTIVITY);
                    break;
            }
            
            protectedPieces &= ~(1ULL << index);
        }
    }

    void getBlackConnectivityScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t kingBB = bitboard.getPieceBoard<PieceType::BLACK_KING>();
        uint64_t blackPieces = bitboard.getColorBoard<PieceColor::BLACK>() & ~kingBB;
        uint64_t protectedPieces = blackPieces & evalContext.blackCombinedAttacks;

        while (protectedPieces) {
            uint64_t index = bitscanForward(protectedPieces);
            PieceType pieceType = bitboard.getPieceOnSquare(index);
            
            switch (pieceType) {
                case WHITE_PAWN:
                case BLACK_PAWN:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_PAWN_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_PAWN_CONNECTIVITY);
                    break;
                case WHITE_KNIGHT:
                case BLACK_KNIGHT:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_KNIGHT_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_KNIGHT_CONNECTIVITY);
                    break;
                case WHITE_BISHOP:
                case BLACK_BISHOP:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_BISHOP_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_BISHOP_CONNECTIVITY);
                    break;
                case WHITE_ROOK:
                case BLACK_ROOK:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_CONNECTIVITY);
                    break;
                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_QUEEN_CONNECTIVITY);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_QUEEN_CONNECTIVITY);
                    break;
            }
            
            protectedPieces &= ~(1ULL << index);
        }
    }

    void getWhiteRookScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard<PieceType::WHITE_ROOK>();
        int rookAmount = popcnt(rookBB);

        while (rookBB) {
            int8_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_OPEN_FILE);
            } else if (bitboard.isSemiOpenFile<PieceColor::WHITE>(index)) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_SEMI_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_SEMI_OPEN_FILE);
            }

            if (index >= Square::A7)  {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_7TH_RANK);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_7TH_RANK);
            }

            uint64_t enemyQueenOnFile = bitboard.getPieceBoard<PieceType::BLACK_QUEEN>() & bitboard.getFile(index);

            if (enemyQueenOnFile) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ROOK_ON_QUEEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ROOK_ON_QUEEN_FILE);
            }

            rookBB &= ~(1ULL << index);
        }

        evalContext.whiteMidgameScore += ((8 - popcnt(bitboard.getPieceBoard<PieceType::WHITE_PAWN>())) * getEvalValue(MIDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
        evalContext.whiteEndgameScore += ((8 - popcnt(bitboard.getPieceBoard<PieceType::WHITE_PAWN>())) * getEvalValue(ENDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
    }

    void getBlackRookScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t rookBB = bitboard.getPieceBoard<PieceType::BLACK_ROOK>();
        int rookAmount = popcnt(rookBB);

        while (rookBB) {
            uint64_t index = bitscanForward(rookBB);

            if (bitboard.isOpenFile(index)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_OPEN_FILE);
            } else if (bitboard.isSemiOpenFile<PieceColor::BLACK>(index)) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_SEMI_OPEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_SEMI_OPEN_FILE);
            }

            if (index <= Square::H2)  {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_7TH_RANK);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_7TH_RANK);
            }

            uint64_t enemyQueenOnFile = bitboard.getPieceBoard<PieceType::WHITE_QUEEN>() & bitboard.getFile(index);

            if (enemyQueenOnFile) {
                evalContext.blackMidgameScore += getEvalValue(MIDGAME_ROOK_ON_QUEEN_FILE);
                evalContext.blackEndgameScore += getEvalValue(ENDGAME_ROOK_ON_QUEEN_FILE);
            }

            rookBB &= ~(1ULL << index);
        }

        evalContext.blackMidgameScore += ((8 - popcnt(bitboard.getPieceBoard<PieceType::BLACK_PAWN>())) * getEvalValue(MIDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
        evalContext.blackEndgameScore += ((8 - popcnt(bitboard.getPieceBoard<PieceType::BLACK_PAWN>())) * getEvalValue(ENDGAME_ROOK_LESS_PAWNS_BONUS)) * rookAmount;
    }

    void getWhiteBishopScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t bishopBB = bitboard.getPieceBoard<PieceType::WHITE_BISHOP>();
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::WHITE_PAWN>();

        if (bishopAmount == 1) {
            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_NO_BISHOP_PAIR);
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_NO_BISHOP_PAIR);
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G2 || index == Square::B2) {
                uint64_t fianchettoPattern = nortOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = noWeOne(1ULL << index) | noEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_BISHOP_FIANCHETTO);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_BISHOP_FIANCHETTO);
                }
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    void getBlackBishopScore(EvalContext &evalContext, Bitboard &bitboard) {
        uint64_t bishopBB = bitboard.getPieceBoard<PieceType::BLACK_BISHOP>();
        int bishopAmount = popcnt(bishopBB);
        uint64_t pawnBB = bitboard.getPieceBoard<PieceType::BLACK_PAWN>();

        if (bishopAmount == 1) {
            evalContext.blackMidgameScore += getEvalValue(MIDGAME_NO_BISHOP_PAIR);
            evalContext.blackEndgameScore += getEvalValue(ENDGAME_NO_BISHOP_PAIR);
        }

        while (bishopBB) {
            uint64_t index = bitscanForward(bishopBB);

            if (index == Square::G7 || index == Square::B7) {
                uint64_t fianchettoPattern = soutOne(1ULL << index) | westOne(1ULL << index) | eastOne(1ULL << index);
                uint64_t antiPattern = soWeOne(1ULL << index) | soEaOne(1ULL << index);

                if (popcnt(pawnBB & fianchettoPattern) == 3 && !(pawnBB & antiPattern)) {
                    evalContext.blackMidgameScore += getEvalValue(MIDGAME_BISHOP_FIANCHETTO);
                    evalContext.blackEndgameScore += getEvalValue(ENDGAME_BISHOP_FIANCHETTO);
                }
            }

            bishopBB &= ~(1ULL << index);
        }
    }

    template<PieceColor color>
    void getPawnScore(EvalContext &evalContext, Bitboard &bitboard) {
        for (int i = 0; i < 8; i++) {
            uint64_t pawnsOnFile = bitboard.getPawnsOnSameFile<color>(i);
            bool isPassed = bitboard.isPassedPawn<color>(i);
            bool isIsolated = bitboard.isIsolatedPawn<color>(i);
            bool isSemiOpen = bitboard.isSemiOpenFile<color>(i);
            int amountOfPawns = popcnt(pawnsOnFile);

            evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PAWN_ON_SAME_FILE) * (amountOfPawns - 1);
            evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PAWN_ON_SAME_FILE) * (amountOfPawns - 1);

            if (isPassed) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PASSED_PAWN);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PASSED_PAWN);
            } else if (isIsolated) {
                if (isSemiOpen) {
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_SEMI_OPEN_PAWN);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_SEMI_OPEN_PAWN);
                } else {
                    evalContext.whiteMidgameScore += getEvalValue(MIDGAME_ISOLATED_PAWN);
                    evalContext.whiteEndgameScore += getEvalValue(ENDGAME_ISOLATED_PAWN);
                }
            } else if (isSemiOpen) {
                evalContext.whiteMidgameScore += getEvalValue(MIDGAME_PAWN_SEMI_OPEN_FILE);
                evalContext.whiteEndgameScore += getEvalValue(ENDGAME_PAWN_SEMI_OPEN_FILE);
            }
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