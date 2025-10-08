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

#include "search.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

#include "board.h"
#include "constants.h"
#include "eval.h"
#include "move.h"
#include "move_gen.h"
#include "move_picker.h"
#include "timeman.h"
#include "tt.h"
#include "types.h"
#include "uci.h"

namespace Zagreus {
static TranspositionTable* tt = TranspositionTable::getTT();
static int lmrTable[MAX_PLIES][MAX_MOVES]{};

void initializeSearch() {
    for (int depth = 1; depth < MAX_PLIES; ++depth) {
        for (int moveCount = 1; moveCount < MAX_MOVES; ++moveCount) {
            lmrTable[depth][moveCount] = static_cast<int>(0.5 + 0.5 * std::log(depth) * std::log(moveCount));
        }
    }
}

template <PieceColor color>
Move search(Engine& engine, Board& board, SearchParams& params, SearchStats& stats) {
    int depth = 1;
    const int currentPly = board.getPly();
    int searchTime;
    auto endTime = std::chrono::steady_clock::time_point{};

    if (params.infinite) {
        searchTime = 0;
    } else {
        searchTime = calculateSearchTime<color>(params);
        endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(searchTime);
    }
    const auto startTime = std::chrono::steady_clock::now();
    PvLine bestPvLine = PvLine{board.getPly()};

    engine.setSearchStopped(false);

    while (!engine.isSearchStopped() && (currentPly + depth) < MAX_PLIES) {
        if (!params.infinite && (params.blackTime > 0 || params.whiteTime > 0)) {
            // Don't start the next iteration if we are 10% away from the end time
            if (std::chrono::steady_clock::now() + std::chrono::milliseconds(searchTime / 10) > endTime) {
                engine.setSearchStopped(true);
                break;
            }
        }

        if (params.depth > 0 && depth > params.depth) {
            engine.setSearchStopped(true);
            break;
        }

        if (params.max_nodes > 0 && stats.nodesSearched + stats.qNodesSearched >= params.max_nodes) {
            engine.setSearchStopped(true);
            break;
        }

        PvLine pvLine = PvLine{board.getPly()};

        const int score =
            pvSearch<color, ROOT>(engine, board, INITIAL_ALPHA, INITIAL_BETA, depth, stats, endTime, pvLine);
        assert(score != INITIAL_ALPHA && score != INITIAL_BETA);
        assert(depth > 0);

        if (endTime.time_since_epoch().count() != 0 && std::chrono::steady_clock::now() > endTime) {
            engine.setSearchStopped(true);
            break;
        }

        if (engine.isSearchStopped()) {
            break;
        }

        bestPvLine = pvLine;
        board.setPreviousPvLine(bestPvLine);

        stats.score = score;
        stats.timeSpentMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
        stats.depth = depth;

        // Make sure we don't divide by zero
        if (stats.timeSpentMs == 0) {
            stats.timeSpentMs = 1;
        }

        const uint64_t totalNodesSearch = stats.nodesSearched + stats.qNodesSearched;
        const uint64_t nps = static_cast<double>(totalNodesSearch) / (static_cast<double>(stats.timeSpentMs) / 1000.0);
        std::string pvString = parsePvLine(bestPvLine);
        engine.sendInfoMessage("depth " + std::to_string(stats.depth) + " score cp " + std::to_string(stats.score) +
                               " nodes " + std::to_string(totalNodesSearch) + " time " +
                               std::to_string(stats.timeSpentMs) + " nps " + std::to_string(nps) + " pv " + pvString);
        depth += 1;
    }

    if (bestPvLine.moves[0] == NO_MOVE) {
        // Find the first legal move and play that
        MoveList moves = MoveList{};
        generateMoves<color, ALL>(board, moves);
        MovePicker movePicker{moves};
        Move move;
        Move bestMove = NO_MOVE;

        while (movePicker.next(move)) {
            board.makeMove(move);

            if (!board.isPositionLegal<color>()) {
                board.unmakeMove();
                continue;
            }

            bestMove = move;
            board.unmakeMove();
            break;
        }

        assert(bestMove != NO_MOVE);
        return bestMove;
    }

    assert(bestPvLine.moves[0] != NO_MOVE);
    board.setPreviousPvLine(bestPvLine);
    return bestPvLine.moves[0];
}

template Move search<WHITE>(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);
template Move search<BLACK>(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);

template <PieceColor color, NodeType nodeType>
int pvSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
             const std::chrono::time_point<std::chrono::steady_clock>& endTime, PvLine& pvLine) {
    constexpr bool isPV = nodeType == PV || nodeType == ROOT;
    constexpr bool isRoot = nodeType == ROOT;
    constexpr PieceColor opponentColor = !color;

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if (!isRoot && (stats.nodesSearched + stats.qNodesSearched) % 4096 == 0) {
        if (endTime.time_since_epoch().count() != 0 && std::chrono::steady_clock::now() > endTime) {
            engine.setSearchStopped(true);
            return beta;
        }
    }

    bool isInCheck = board.isKingInCheck<color>();

    if (isInCheck) {
        depth += 1;
#ifdef TRACE_SEARCH
        stats.checkExtensions++;
#endif
    }

    if (depth <= 0) {
        assert(!isRoot);
        pvLine.moveCount = 0;
        return qSearch<color, nodeType>(engine, board, alpha, beta, depth, stats, endTime);
    }

    stats.nodesSearched += 1;

    if (!isPV) {
        const int16_t score = tt->probePosition(board.getZobristHash(), depth, alpha, beta, board.getPly());

#ifdef TRACE_SEARCH
        stats.ttProbes++;
#endif

        if (score != NO_TT_SCORE) {
#ifdef TRACE_SEARCH
            stats.ttHits++;
#endif
            return score;
        }

        // Null Move Pruning
        if (depth >= 3 && !isInCheck && board.hasNonPawnMaterial<color>() && board.getPreviousMove() != NO_MOVE) {
#ifdef TRACE_SEARCH
            stats.nmpTries++;
#endif
            board.makeNullMove();
            const int R = 2 + depth / 3;
            PvLine nmpPvLine = PvLine{board.getPly()};
            const int nullMoveScore = -pvSearch<opponentColor, REGULAR>(engine, board, -beta, -beta + 1, depth - R,
                                                                        stats, endTime, nmpPvLine);
            board.unmakeNullMove();

            if (nullMoveScore >= beta) {
#ifdef TRACE_SEARCH
                stats.nmpPrunes++;
#endif
                return nullMoveScore;
            }
        }

        // Futility pruning
        if (depth <= 2 && board.hasNonPawnMaterial<color>() && !isInCheck) {
            int futilityMargin = 350 * depth;
            const int staticEval = Evaluation(board).evaluate();

            if (staticEval + futilityMargin < alpha) {
#ifdef TRACE_SEARCH
                stats.futilityPrunes++;
#endif
                return qSearch<color, nodeType>(engine, board, alpha, beta, 0, stats, endTime);
            }
        }
    }

    bool firstMove = true;
    int legalMoves = 0;

    Move move;
    MoveList moves = MoveList{};

    if (isInCheck) {
        generateMoves<color, EVASIONS>(board, moves);
    } else {
        generateMoves<color, ALL>(board, moves);
    }

    MoveList searchedQuietMoves{};
    MoveList searchedCaptures{};
    MovePicker movePicker{moves};
    movePicker.score(board);

    PvLine nodePvLine = PvLine{board.getPly()};
    Move bestMove = NO_MOVE;
    int bestScore = INT32_MIN;
    int movesSearched = 0;

    while (movePicker.next(move)) {
        const MoveType moveType = getMoveType(move);
        const Square toSquare = getToSquare(move);
        Piece capturedPiece = board.getPieceOnSquare(toSquare);

        if (moveType == EN_PASSANT) {
            capturedPiece = color == WHITE ? BLACK_PAWN : WHITE_PAWN;
        }

        board.makeMove(move);

        if (!board.isPositionLegal<color>()) {
            board.unmakeMove();
            continue;
        }

        legalMoves += 1;

        if (capturedPiece == EMPTY) {
            searchedQuietMoves.moves[searchedQuietMoves.size++] = move;
        } else {
            searchedCaptures.moves[searchedCaptures.size++] = move;
        }

        bool doFullSearch = true;
        int score = INT32_MIN;

        // Late Move Reduction
        if (movesSearched > 3 && depth >= 3 && capturedPiece == EMPTY && getMoveType(move) != PROMOTION) {
            doFullSearch = false;
            int R = 0;

            R = lmrTable[depth][movesSearched];
            R -= isPV;
            R -= isInCheck;
            R -= board.isKingInCheck<opponentColor>();

            // Make sure depth - 1 - R is at least 1
            if (depth - 1 - R <= 0) {
                R = depth - 2;
            }

            R = std::max(0, R);

#ifdef TRACE_SEARCH
            stats.lmrSearches++;
#endif
            score = -pvSearch<opponentColor, REGULAR>(engine, board, -alpha - 1, -alpha, depth - 1 - R, stats, endTime,
                                                      nodePvLine);

            if (score > alpha) {
#ifdef TRACE_SEARCH
                stats.lmrResearches++;
#endif
                doFullSearch = true;
            }
        }

        if (doFullSearch) {
            if (firstMove) {
                if (isRoot) {
                    score = -pvSearch<opponentColor, PV>(engine, board, -beta, -alpha, depth - 1, stats, endTime,
                                                         nodePvLine);
                } else {
                    score = -pvSearch<opponentColor, nodeType>(engine, board, -beta, -alpha, depth - 1, stats, endTime,
                                                               nodePvLine);
                }

                firstMove = false;
            } else {
                score = -pvSearch<opponentColor, REGULAR>(engine, board, -alpha - 1, -alpha, depth - 1, stats, endTime,
                                                          nodePvLine);

                if (isPV && score > alpha && (isRoot || score < beta)) {
                    score = -pvSearch<opponentColor, PV>(engine, board, -beta, -alpha, depth - 1, stats, endTime,
                                                         nodePvLine);
                }
            }
        }

        board.unmakeMove();

        if (score >= beta) {
#ifdef TRACE_SEARCH
            if (movesSearched == 0) {
                stats.firstMoveCutoffs++;
            } else if (movesSearched == 1) {
                stats.secondMoveCutoffs++;
            }
            stats.totalMoveCutoffNumber += (movesSearched + 1);
            stats.totalCutoffs++;
#endif
            if (!engine.isSearchStopped()) {
                if (capturedPiece == EMPTY && getMoveType(move) != PROMOTION) {
                    const int historyValue = 300 * depth - 250;

                    tt->updateHistory<color>(move, historyValue);

                    for (int i = 0; i < searchedQuietMoves.size; ++i) {
                        const Move quietMove = searchedQuietMoves.moves[i];

                        if (quietMove != move) {
                            tt->updateHistory<color>(quietMove, -historyValue);
                        }
                    }
                }

                if (capturedPiece != EMPTY) {
                    const int historyValue = 300 * depth - 250;
                    const Piece movingPiece = board.getPieceOnSquare(getFromSquare(move));

                    tt->updateCaptureHistory(move, movingPiece, capturedPiece, historyValue);

                    for (int i = 0; i < searchedCaptures.size; ++i) {
                        const Move captureMove = searchedCaptures.moves[i];

                        if (captureMove != move) {
                            const Piece otherMovingPiece = board.getPieceOnSquare(getFromSquare(captureMove));
                            const MoveType otherMoveType = getMoveType(captureMove);
                            Piece otherCapturedPiece = board.getPieceOnSquare(getToSquare(captureMove));

                            if (otherMoveType == EN_PASSANT) {
                                otherCapturedPiece = color == WHITE ? BLACK_PAWN : WHITE_PAWN;
                            }

                            tt->updateCaptureHistory(captureMove, otherMovingPiece, otherCapturedPiece, -historyValue);
                        }
                    }
                }

                if (!isRoot) {
#ifdef TRACE_SEARCH
                    stats.ttWrites++;
#endif
                    tt->savePosition(board.getZobristHash(), depth, board.getPly(), score, move, BETA);
                }
            }

            return score;
        }

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;
                alpha = score;
                pvLine.moves[0] = move;
                std::memcpy(pvLine.moves + 1, nodePvLine.moves, nodePvLine.moveCount * sizeof(Move));
                pvLine.moveCount = nodePvLine.moveCount + 1;
            }
        }

        movesSearched += 1;
    }

    if (!legalMoves) {
        if (isInCheck) {
            alpha = -MATE_SCORE + board.getPly();
            bestScore = alpha;
        } else {
            alpha = DRAW_SCORE;
            bestScore = DRAW_SCORE;
        }
    }

    if (!isRoot) {
        const TTNodeType ttNodeType = (isPV && bestMove != NO_MOVE) ? EXACT : ALPHA;

        if (!engine.isSearchStopped()) {
#ifdef TRACE_SEARCH
            stats.ttWrites++;
#endif
            tt->savePosition(board.getZobristHash(), depth, board.getPly(), bestScore, bestMove, ttNodeType);
        }
    }

    assert(bestScore != INITIAL_ALPHA);
    return bestScore;
}

template <PieceColor color, NodeType nodeType>
int qSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
            const std::chrono::time_point<std::chrono::steady_clock>& endTime) {
    assert(nodeType != ROOT);
    constexpr bool isPV = nodeType == PV;

    if (board.isDraw()) {
        return DRAW_SCORE;
    }

    if ((stats.nodesSearched + stats.qNodesSearched) % 4096 == 0) {
        if (endTime.time_since_epoch().count() != 0 && std::chrono::steady_clock::now() > endTime) {
            engine.setSearchStopped(true);
            return beta;
        }
    }

    if (!isPV) {
        const int16_t score = tt->probePosition(board.getZobristHash(), depth, alpha, beta, board.getPly());

#ifdef TRACE_SEARCH
        stats.qTtProbes++;
#endif

        if (score != NO_TT_SCORE) {
#ifdef TRACE_SEARCH
            stats.qTtHits++;
#endif
            return score;
        }
    }

    stats.qNodesSearched += 1;

    const bool isInCheck = board.isKingInCheck<color>();
    int bestScore;

    if (isInCheck) {
        bestScore = -MATE_SCORE;
    } else {
        bestScore = Evaluation(board).evaluate();

        // Stand pat
        if (bestScore >= beta) {
            if (!engine.isSearchStopped()) {
#ifdef TRACE_SEARCH
                stats.ttWrites++;
#endif
                tt->savePosition(board.getZobristHash(), depth, board.getPly(), bestScore, NO_MOVE, BETA);
            }

            return bestScore;
        }

        if (bestScore > alpha) {
            alpha = bestScore;
        }
    }

    int legalMoves = 0;
    Move move;
    MoveList moves = MoveList{};
    Move bestMove = NO_MOVE;

    if (isInCheck) {
        generateMoves<color, EVASIONS>(board, moves);
    } else {
        generateMoves<color, QSEARCH>(board, moves);
    }

    MovePicker movePicker{moves};
    movePicker.score(board);

    while (movePicker.next(move)) {
        const MoveType moveType = getMoveType(move);
        const Square toSquare = getToSquare(move);
        Piece capturedPiece = board.getPieceOnSquare(toSquare);

        if (moveType == EN_PASSANT) {
            capturedPiece = color == WHITE ? BLACK_PAWN : WHITE_PAWN;
        }

        if (!isInCheck && capturedPiece != EMPTY) {
            const bool see = board.see(move, 0);

            if (!see) {
#ifdef TRACE_SEARCH
                stats.seePrunes++;
#endif
                continue;
            }
        }

        board.makeMove(move);

        if (!board.isPositionLegal<color>()) {
            board.unmakeMove();
            continue;
        }

        legalMoves += 1;

        const int score = -qSearch<!color, nodeType>(engine, board, -beta, -alpha, depth - 1, stats, endTime);

        board.unmakeMove();

        if (score >= beta) {
            if (!engine.isSearchStopped()) {
#ifdef TRACE_SEARCH
                stats.ttWrites++;
#endif
                tt->savePosition(board.getZobristHash(), depth, board.getPly(), score, move, BETA);
            }

            return score;
        }

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;
                alpha = score;
            }
        }
    }

    if (!legalMoves && isInCheck) {
        bestScore = -MATE_SCORE + board.getPly();
    }

    const TTNodeType ttNodeType = (isPV && bestMove != NO_MOVE) ? EXACT : ALPHA;

    if (!engine.isSearchStopped()) {
#ifdef TRACE_SEARCH
        stats.ttWrites++;
#endif
        tt->savePosition(board.getZobristHash(), depth, board.getPly(), bestScore, bestMove, ttNodeType);
    }

    assert(bestScore != INITIAL_ALPHA);
    return bestScore;
}
}  // namespace Zagreus

#ifdef TRACE_SEARCH
#include <iomanip>
#include <sstream>

namespace Zagreus {
void SearchStats::clearTrace() {
    ttProbes = 0;
    ttHits = 0;
    ttWrites = 0;
    qTtProbes = 0;
    qTtHits = 0;
    firstMoveCutoffs = 0;
    secondMoveCutoffs = 0;
    totalMoveCutoffNumber = 0;
    totalCutoffs = 0;
    nmpTries = 0;
    nmpPrunes = 0;
    lmrSearches = 0;
    lmrResearches = 0;
    futilityPrunes = 0;
    checkExtensions = 0;
    seePrunes = 0;
}

void SearchStats::printTrace(Engine& engine, int numPositions) const {
    const uint64_t totalNodes = nodesSearched + qNodesSearched;
    const uint64_t avgTotalNodes = totalNodes / numPositions;
    const double branchFactor = avgTotalNodes > 1 && depth > 1 ? std::pow(avgTotalNodes, 1.0 / depth) : 0.0;

    const double ttHitRate = ttProbes > 0 ? static_cast<double>(ttHits) / ttProbes * 100.0 : 0.0;
    const double qTtHitRate = qTtProbes > 0 ? static_cast<double>(qTtHits) / qTtProbes * 100.0 : 0.0;
    const double nmpSuccessRate = nmpTries > 0 ? static_cast<double>(nmpPrunes) / nmpTries * 100.0 : 0.0;
    const double lmrResearchRate = lmrSearches > 0 ? static_cast<double>(lmrResearches) / lmrSearches * 100.0 : 0.0;
    const double firstMoveCutoffRate =
        totalCutoffs > 0 ? static_cast<double>(firstMoveCutoffs) / totalCutoffs * 100.0 : 0.0;
    const double secondMoveCutoffRate =
        totalCutoffs > 0 ? static_cast<double>(secondMoveCutoffs) / totalCutoffs * 100.0 : 0.0;
    const double avgMoveCutoff = totalCutoffs > 0 ? static_cast<double>(totalMoveCutoffNumber) / totalCutoffs : 0.0;

    std::stringstream ss;
    ss << "\n--- Search Statistics" << (numPositions > 1 ? " (Averages per position)" : "") << " ---\n"
       << "Effective Branching Factor: " << std::fixed << std::setprecision(2) << branchFactor << "\n"
       << "\n--- Transposition Table ---\n"
       << "Probes: " << ttProbes / numPositions << "\n"
       << "Hits: " << ttHits / numPositions << " (" << ttHitRate << "%)\n"
       << "Writes: " << ttWrites / numPositions << "\n"
       << "Q-Search Probes: " << qTtProbes / numPositions << "\n"
       << "Q-Search Hits: " << qTtHits / numPositions << " (" << qTtHitRate << "%)\n"
       << "\n--- Move Ordering ---\n"
       << "First Move Cutoffs: " << firstMoveCutoffs / numPositions << " (" << firstMoveCutoffRate << "%)\n"
       << "Second Move Cutoffs: " << secondMoveCutoffs / numPositions << " (" << secondMoveCutoffRate << "%)\n"
       << "Average Move Cutoff: " << avgMoveCutoff << "\n"
       << "\n--- Pruning, Reductions, and Extensions ---\n"
       << "NMP Tries: " << nmpTries / numPositions << "\n"
       << "NMP Prunes: " << nmpPrunes / numPositions << " (" << nmpSuccessRate << "%)\n"
       << "LMR Searches: " << lmrSearches / numPositions << "\n"
       << "LMR Researches: " << lmrResearches / numPositions << " (" << lmrResearchRate << "%)\n"
       << "Futility Prunes: " << futilityPrunes / numPositions << "\n"
       << "Check Extensions: " << checkExtensions / numPositions << "\n"
       << "\n--- Quiescence Search ---\n"
       << "SEE Prunes: " << seePrunes / numPositions << "\n"
       << "-------------------------";
    engine.sendInfoMessage(ss.str());
}
}  // namespace Zagreus
#endif
