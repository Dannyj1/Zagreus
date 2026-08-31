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

#include "search.h"

#include <cmath>
#include <iostream>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstring>

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
static int aspirationSearch(Engine& engine, Board& board, SearchParams& params, SearchStats& stats, int depth,
                            int previousScore, const std::chrono::time_point<std::chrono::steady_clock>& endTime,
                            PvLine& pvLine) {
    int alpha, beta, score;
    int delta = 50;

    if (depth >= 4) {
        alpha = std::max(-MATE_SCORE, previousScore - delta);
        beta = std::min(MATE_SCORE, previousScore + delta);
    } else {
        alpha = -MATE_SCORE;
        beta = MATE_SCORE;
    }

    SearchStack searchStack{};
    pvLine.moveCount = 0;

    while (true) {
        bool isInCheck = board.isKingInCheck<color>();
        score =
            pvSearch<color, ROOT>(engine, board, alpha, beta, depth, stats, endTime, pvLine, searchStack, isInCheck);
        assert(score >= -MATE_SCORE && score <= MATE_SCORE);

        if (engine.isSearchStopped()) {
            break;
        }

        if (score <= alpha) {
            beta = (alpha + beta) / 2;
            alpha = std::max(-MATE_SCORE, score - delta);
            delta *= 2;
            continue;
        } else if (score >= beta) {
            beta = std::min(MATE_SCORE, score + delta);
            delta *= 2;
            continue;
        }

        break;
    }

    return score;
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

    int previousScore = 0;

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

        PvLine currentPvLine = PvLine{board.getPly()};
        const int score =
            aspirationSearch<color>(engine, board, params, stats, depth, previousScore, endTime, currentPvLine);

        if (engine.isSearchStopped()) {
            break;
        }

        previousScore = score;
        bestPvLine = currentPvLine;

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
            if (!board.isMoveLegal(move)) {
                continue;
            }

            board.makeMove(move);

            bestMove = move;
            board.unmakeMove();
            break;
        }

        assert(bestMove != NO_MOVE);
        return bestMove;
    }

    assert(bestPvLine.moves[0] != NO_MOVE);
    return bestPvLine.moves[0];
}

template Move search<WHITE>(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);
template Move search<BLACK>(Engine& engine, Board& board, SearchParams& params, SearchStats& stats);

template <PieceColor color, NodeType nodeType>
int pvSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
             const std::chrono::time_point<std::chrono::steady_clock>& endTime, PvLine& pvLine,
             SearchStack& searchStack, bool isInCheck) {
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

    if (isInCheck) {
        depth += 1;
#ifdef TRACE_SEARCH
        stats.checkExtensions++;
#endif
    }

    if (depth <= 0) {
        assert(!isRoot);
        pvLine.moveCount = 0;
        return qSearch<color, nodeType>(engine, board, alpha, beta, depth, stats, endTime, searchStack, isInCheck);
    }

    stats.nodesSearched += 1;

    TTEntry* ttEntry = nullptr;
    const int16_t ttScore = tt->probePosition(board.getZobristHash(), depth, alpha, beta, board.getPly(), ttEntry);
    Move ttMove = ttEntry ? ttEntry->bestMove : NO_MOVE;
    int ttDepth = ttEntry ? ttEntry->depth : -MAX_PLIES;

#ifdef TRACE_SEARCH
    stats.ttProbes++;

    if (ttScore != NO_TT_SCORE) {
        stats.ttHits++;
    }
#endif

    if (!isPV && ttScore != NO_TT_SCORE) {
        return ttScore;
    }

    const int eval = isInCheck ? 0 : Evaluation(board).evaluate();

    if (!isPV) {
        // Reverse Futility Pruning
        int rfMargin = 150 * depth;
        if (!isInCheck && depth <= 3 && eval >= beta + rfMargin) {
#ifdef TRACE_SEARCH
            stats.reverseFutilityPrunes++;
#endif
            return eval;
        }

        // Null Move Pruning
        if (depth >= 2 && !isInCheck && board.hasNonPawnMaterial<color>() && board.getPreviousMove() != NO_MOVE &&
            eval >= beta) {
#ifdef TRACE_SEARCH
            stats.nmpTries++;
#endif
            board.makeNullMove();
            const int R = 2 + depth / 3;
            PvLine nmpPvLine = PvLine{board.getPly()};
            const int nullMoveScore = -pvSearch<opponentColor, REGULAR>(
                engine, board, -beta, -beta + 1, depth - R, stats, endTime, nmpPvLine, searchStack, isInCheck);
            board.unmakeNullMove();

            if (nullMoveScore >= beta) {
#ifdef TRACE_SEARCH
                stats.nmpPrunes++;
#endif
                return nullMoveScore;
            }
        }
    } else {
        // Internal iterative reductions (IIR)
        /*if (depth >= 6 && ttMove == NO_MOVE) {
            depth -= 1;
        }*/
    }

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

    Move excludedMove = searchStack.excludedMove[depth];

    while (movePicker.next(move)) {
        if (move == excludedMove) {
            continue;
        }

        const MoveType moveType = getMoveType(move);
        const Square toSquare = getToSquare(move);
        Piece capturedPiece = board.getPieceOnSquare(toSquare);

        if (moveType == EN_PASSANT) {
            capturedPiece = color == WHITE ? BLACK_PAWN : WHITE_PAWN;
        }

        if (!board.isMoveLegal(move)) {
            continue;
        }

        board.makeMove(move);

        legalMoves += 1;
        int moveDepth = depth - 1;
        bool moveGivesCheck = board.isKingInCheck<opponentColor>();

        // Futility pruning
        if (depth <= 4 && !isInCheck && !moveGivesCheck && capturedPiece == EMPTY && getMoveType(move) != PROMOTION) {
            const int futilityMargin = 200 * depth;
            const int futilityScore = eval + futilityMargin;

            if (futilityScore <= alpha) {
#ifdef TRACE_SEARCH
                stats.futilityPrunes++;
#endif

                if (futilityScore > bestScore) {
                    bestScore = futilityScore;
                }

                board.unmakeMove();
                continue;
            }
        }

        // Singular Extensions
        /*if (!isRoot && excludedMove == NO_MOVE && depth >= 6 && ttEntry && move == ttMove &&
            ttEntry->nodeType == BETA && ttDepth >= depth - 3) {
#ifdef TRACE_SEARCH
            stats.singularAttempts++;
#endif
            // Need to unmake the move, as it was already made
            board.unmakeMove();

            int singularBeta = ttEntry->score - depth;
            int singularDepth = moveDepth / 2;

            searchStack.excludedMove[singularDepth] = move;
            PvLine singularPvLine = PvLine{board.getPly()};
            const int singularScore =
                pvSearch<color, REGULAR>(engine, board, singularBeta - 1, singularBeta, singularDepth, stats, endTime,
                                         singularPvLine, searchStack);
            searchStack.excludedMove[singularDepth] = NO_MOVE;

            if (singularScore < singularBeta) {
#ifdef TRACE_SEARCH
                stats.singularExtensions++;
#endif

                moveDepth += 1;
            }

            // Re-make move
            board.makeMove(move);
        }*/

        __builtin_prefetch(&tt->transpositionTable[board.getZobristHash() & tt->hashSize]);

        if (capturedPiece == EMPTY) {
            searchedQuietMoves.moves[searchedQuietMoves.size++] = move;
        } else {
            searchedCaptures.moves[searchedCaptures.size++] = move;
        }

        int score;

        if (isPV && movesSearched == 0) {
            score = -pvSearch<opponentColor, PV>(engine, board, -beta, -alpha, moveDepth, stats, endTime, nodePvLine,
                                                 searchStack, moveGivesCheck);
        } else {
            int lmrReduction = 0;
            bool isLmr = false;

            // Late Move Reduction
            if (movesSearched > 2 && depth >= 3 && capturedPiece == EMPTY && getMoveType(move) != PROMOTION) {
                isLmr = true;
#ifdef TRACE_SEARCH
                stats.lmrSearches++;
#endif
                lmrReduction = lmrTable[depth][movesSearched];
                lmrReduction -= isPV;
                lmrReduction -= isInCheck;
                lmrReduction -= moveGivesCheck;

                if (depth - 1 - lmrReduction <= 0) {
                    lmrReduction = depth - 2;
                }

                lmrReduction = std::max(0, lmrReduction);
                assert(moveDepth - lmrReduction > 0);
            }

            score = -pvSearch<opponentColor, REGULAR>(engine, board, -alpha - 1, -alpha, moveDepth - lmrReduction,
                                                      stats, endTime, nodePvLine, searchStack, moveGivesCheck);

            if (isLmr && score > alpha) {
#ifdef TRACE_SEARCH
                stats.lmrResearches++;
#endif

                score = -pvSearch<opponentColor, REGULAR>(engine, board, -alpha - 1, -alpha, moveDepth, stats, endTime,
                                                          nodePvLine, searchStack, moveGivesCheck);
            }

            if (isPV && score > alpha) {
                score = -pvSearch<opponentColor, PV>(engine, board, -beta, -alpha, moveDepth, stats, endTime,
                                                     nodePvLine, searchStack, moveGivesCheck);
            }
        }

        board.unmakeMove();
        movesSearched += 1;

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;
                alpha = score;
                pvLine.moves[0] = move;
                std::memcpy(pvLine.moves + 1, nodePvLine.moves, nodePvLine.moveCount * sizeof(Move));
                pvLine.moveCount = nodePvLine.moveCount + 1;

                if (score >= beta) {
#ifdef TRACE_SEARCH
                    if (movesSearched == 1) {
                        stats.firstMoveCutoffs++;
                    } else if (movesSearched == 2) {
                        stats.secondMoveCutoffs++;
                    }

                    stats.totalMoveCutoffNumber += movesSearched;
                    stats.totalCutoffs++;
#endif

                    if (!engine.isSearchStopped()) {
                        if (capturedPiece == EMPTY && getMoveType(move) != PROMOTION) {
                            // Killer move heuristic
                            tt->addKillerMove(move, board.getPly());

                            // History heuristic
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

                                    tt->updateCaptureHistory(captureMove, otherMovingPiece, otherCapturedPiece,
                                                             -historyValue);
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
            }
        }
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

    assert(bestScore >= -MATE_SCORE && bestScore <= MATE_SCORE);
    return bestScore;
}

template <PieceColor color, NodeType nodeType>
int qSearch(Engine& engine, Board& board, int alpha, int beta, int depth, SearchStats& stats,
            const std::chrono::time_point<std::chrono::steady_clock>& endTime, SearchStack& searchStack,
            bool isInCheck) {
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
        TTEntry* ttEntry = nullptr;
        const int16_t score = tt->probePosition(board.getZobristHash(), depth, alpha, beta, board.getPly(), ttEntry);

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

        if (!board.isMoveLegal(move)) {
            continue;
        }

        board.makeMove(move);

        legalMoves += 1;

        __builtin_prefetch(&tt->transpositionTable[board.getZobristHash() & tt->hashSize]);

        const bool moveGivesCheck = board.isKingInCheck<!color>();
        const int score = -qSearch<!color, nodeType>(engine, board, -beta, -alpha, depth - 1, stats, endTime,
                                                     searchStack, moveGivesCheck);

        board.unmakeMove();

        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = move;
                alpha = score;

                if (score >= beta) {
                    if (!engine.isSearchStopped()) {
#ifdef TRACE_SEARCH
                        stats.ttWrites++;
#endif
                        tt->savePosition(board.getZobristHash(), depth, board.getPly(), score, move, BETA);
                    }

                    return score;
                }
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

    assert(bestScore >= -MATE_SCORE && bestScore <= MATE_SCORE);
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
    singularExtensions = 0;
    singularAttempts = 0;
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
    ss << std::fixed << std::setprecision(2);

    auto printStatLine = [&](const std::string& label, uint64_t value) {
        ss << label << ": " << value / numPositions;
        if (numPositions > 1) {
            ss << " (Total: " << value << ")";
        }
        ss << "\n";
    };

    auto printStatLineWithRate = [&](const std::string& label, uint64_t value, double rate) {
        ss << label << ": " << value / numPositions;
        if (numPositions > 1) {
            ss << " (Total: " << value << ")";
        }
        ss << " (" << rate << "%)\n";
    };

    ss << "\n--- Search Statistics" << (numPositions > 1 ? " (Averages per position & Totals)" : "") << " ---\n"
       << "Effective Branching Factor: " << branchFactor << "\n"
       << "\n--- Transposition Table ---\n";

    printStatLine("Probes", ttProbes);
    printStatLineWithRate("Hits", ttHits, ttHitRate);
    printStatLine("Writes", ttWrites);
    printStatLine("Q-Search Probes", qTtProbes);
    printStatLineWithRate("Q-Search Hits", qTtHits, qTtHitRate);

    ss << "\n--- Move Ordering ---\n";
    printStatLineWithRate("First Move Cutoffs", firstMoveCutoffs, firstMoveCutoffRate);
    printStatLineWithRate("Second Move Cutoffs", secondMoveCutoffs, secondMoveCutoffRate);
    ss << "Average Move Cutoff: " << avgMoveCutoff << "\n";

    ss << "\n--- Pruning, Reductions, and Extensions ---\n";
    printStatLine("NMP Tries", nmpTries);
    printStatLineWithRate("NMP Prunes", nmpPrunes, nmpSuccessRate);
    printStatLine("LMR Searches", lmrSearches);
    printStatLineWithRate("LMR Researches", lmrResearches, lmrResearchRate);
    printStatLine("Futility Prunes", futilityPrunes);
    printStatLine("Reverse Futility Prunes", reverseFutilityPrunes);
    printStatLine("Singular Extensions", singularExtensions);
    printStatLine("Singular Attempts", singularAttempts);
    printStatLine("Check Extensions", checkExtensions);

    ss << "\n--- Quiescence Search ---\n";
    printStatLine("SEE Prunes", seePrunes);

    ss << "-------------------------";
    engine.sendInfoMessage(ss.str());
}

SearchStats& SearchStats::operator+=(const SearchStats& other) {
    nodesSearched += other.nodesSearched;
    qNodesSearched += other.qNodesSearched;
    ttProbes += other.ttProbes;
    ttHits += other.ttHits;
    ttWrites += other.ttWrites;
    qTtProbes += other.qTtProbes;
    qTtHits += other.qTtHits;
    firstMoveCutoffs += other.firstMoveCutoffs;
    secondMoveCutoffs += other.secondMoveCutoffs;
    totalMoveCutoffNumber += other.totalMoveCutoffNumber;
    totalCutoffs += other.totalCutoffs;
    nmpTries += other.nmpTries;
    nmpPrunes += other.nmpPrunes;
    lmrSearches += other.lmrSearches;
    lmrResearches += other.lmrResearches;
    futilityPrunes += other.futilityPrunes;
    reverseFutilityPrunes += other.reverseFutilityPrunes;
    checkExtensions += other.checkExtensions;
    singularExtensions += other.singularExtensions;
    singularAttempts += other.singularAttempts;
    seePrunes += other.seePrunes;

    return *this;
}

}  // namespace Zagreus
#endif
