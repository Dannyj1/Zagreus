//
// Created by Danny on 8-5-2022.
//

#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <algorithm>
#include <cassert>

#include "bitboard.h"
#include "move_gen.h"
#include "types.h"
#include "senjo/Output.h"

namespace Zagreus {

    Bitboard::Bitboard() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        generatedMoves.reserve(256);

        for (uint64_t &zobristConstant : zobristConstants) {
            zobristConstant = dis(gen);
        }

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
            pawnAttacks[PieceColor::WHITE][sq] = calculatePawnAttacks(sqBB, PieceColor::WHITE) & ~sqBB;
            pawnAttacks[PieceColor::BLACK][sq] = calculatePawnAttacks(sqBB, PieceColor::BLACK) & ~sqBB;
        }

        initializeRayAttacks();
    }

    uint64_t Bitboard::getPieceBoard(int pieceType) {
        return pieceBB[pieceType];
    }

    uint64_t Bitboard::getWhiteBoard() {
        return colorBB[PieceColor::WHITE];
    }

    uint64_t Bitboard::getBlackBoard() {
        return colorBB[PieceColor::BLACK];
    }

    uint64_t Bitboard::getEmptyBoard() const {
        return ~occupiedBB;
    }

    uint64_t Bitboard::getOccupiedBoard() const {
        return occupiedBB;
    }

    uint64_t Bitboard::getKingAttacks(int square) {
        return this->kingAttacks[square];
    }

    uint64_t Bitboard::getKnightAttacks(int square) {
        return this->knightAttacks[square];
    }

    uint64_t Bitboard::getQueenMoves(int square) {
        uint64_t queenMoves = 0ULL;

        queenMoves |= getRayAttacks(getOccupiedBoard(), Direction::NORTH_WEST, square);
        queenMoves |= getRayAttacks(getOccupiedBoard(), Direction::NORTH, square);
        queenMoves |= getRayAttacks(getOccupiedBoard(), Direction::NORTH_EAST, square);
        queenMoves |= getRayAttacks(getOccupiedBoard(), Direction::EAST, square);
        queenMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::SOUTH_EAST, square);
        queenMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::SOUTH, square);
        queenMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::SOUTH_WEST, square);
        queenMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::WEST, square);

        return queenMoves;
    }

    uint64_t Bitboard::getQueenAttacks(uint64_t bb) {
        uint64_t queenAttacks = 0ULL;
        uint64_t emptyBB = getEmptyBoard();

        queenAttacks |= nortOccl(bb, emptyBB);
        queenAttacks |= noEaOccl(bb, emptyBB);
        queenAttacks |= eastOccl(bb, emptyBB);
        queenAttacks |= soEaOccl(bb, emptyBB);
        queenAttacks |= soutOccl(bb, emptyBB);
        queenAttacks |= soWeOccl(bb, emptyBB);
        queenAttacks |= westOccl(bb, emptyBB);
        queenAttacks |= noWeOccl(bb, emptyBB);

        return queenAttacks;
    }

    uint64_t Bitboard::getBishopMoves(int square) {
        uint64_t bishopMoves = 0ULL;

        bishopMoves |= getRayAttacks(getOccupiedBoard(), Direction::NORTH_WEST, square);
        bishopMoves |= getRayAttacks(getOccupiedBoard(), Direction::NORTH_EAST, square);
        bishopMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::SOUTH_EAST, square);
        bishopMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::SOUTH_WEST, square);

        return bishopMoves;
    }

    uint64_t Bitboard::getBishopAttacks(uint64_t bb) {
        uint64_t bishopAttacks = 0ULL;
        uint64_t emptyBB = getEmptyBoard();

        bishopAttacks |= noEaOccl(bb, emptyBB);
        bishopAttacks |= noWeOccl(bb, emptyBB);
        bishopAttacks |= soEaOccl(bb, emptyBB);
        bishopAttacks |= soWeOccl(bb, emptyBB);

        return bishopAttacks;
    }

    uint64_t Bitboard::getRookMoves(int square) {
        uint64_t rookMoves = 0ULL;

        rookMoves |= getRayAttacks(getOccupiedBoard(), Direction::NORTH, square);
        rookMoves |= getRayAttacks(getOccupiedBoard(), Direction::EAST, square);
        rookMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::SOUTH, square);
        rookMoves |= getNegativeRayAttacks(getOccupiedBoard(), Direction::WEST, square);

        return rookMoves;
    }

    uint64_t Bitboard::getRookAttacks(uint64_t bb) {
        uint64_t rookAttacks = 0ULL;
        uint64_t emptyBB = getEmptyBoard();

        rookAttacks |= nortOccl(bb, emptyBB);
        rookAttacks |= westOccl(bb, emptyBB);
        rookAttacks |= soutOccl(bb, emptyBB);
        rookAttacks |= eastOccl(bb, emptyBB);

        return rookAttacks;
    }

    uint64_t Bitboard::getPawnAttacks(int square, PieceColor color) {
        return this->pawnAttacks[color][square];
    }

    uint64_t Bitboard::getWhitePawnAttacks(uint64_t wPawns) {
        return getWhitePawnEastAttacks(wPawns) | getWhitePawnWestAttacks(wPawns);
    }

    uint64_t Bitboard::getBlackPawnAttacks(uint64_t bPawns) {
        return getBlackPawnEastAttacks(bPawns) | getBlackPawnWestAttacks(bPawns);
    }

    uint64_t Bitboard::getWhitePawnSinglePush(uint64_t wPawns) {
        return nortOne(wPawns) & getEmptyBoard();
    }

    uint64_t Bitboard::getWhitePawnDoublePush(uint64_t wPawns) {
        uint64_t singlePushs = getWhitePawnSinglePush(wPawns);

        return singlePushs | (nortOne(singlePushs) & getEmptyBoard() & RANK_4);
    }

    uint64_t Bitboard::getBlackPawnSinglePush(uint64_t bPawns) {
        return soutOne(bPawns) & getEmptyBoard();
    }

    uint64_t Bitboard::getBlackPawnDoublePush(uint64_t bPawns) {
        uint64_t singlePushs = getBlackPawnSinglePush(bPawns);

        return singlePushs | (soutOne(singlePushs) & getEmptyBoard() & RANK_5);
    }

    void Bitboard::setPiece(int index, PieceType pieceType) {
        assert(index < 64);
        assert(index >= 0);
        assert(pieceType != PieceType::EMPTY);
        pieceBB[pieceType] |= 1ULL << index;
        occupiedBB |= 1ULL << index;
        assert(pieceType % 2 == 0 || pieceType % 2 == 1);
        colorBB[pieceType % 2] |= 1ULL << index;
        pieceSquareMapping[index] = pieceType;
        zobristHash ^= zobristConstants[index * 12 + pieceType];
    }

    void Bitboard::removePiece(int index, PieceType pieceType) {
        assert(index < 64);
        assert(index >= 0);
        assert(pieceType != PieceType::EMPTY);
        pieceBB[pieceType] &= ~(1ULL << index);
        occupiedBB &= ~(1ULL << index);
        assert(pieceType % 2 == 0 || pieceType % 2 == 1);
        colorBB[pieceType % 2] &= ~(1ULL << index);
        pieceSquareMapping[index] = PieceType::EMPTY;
        zobristHash ^= zobristConstants[index * 12 + pieceType];
    }

    void Bitboard::clearAttacksForPieces(uint64_t pieces) {
        while (pieces) {
            int index = bitscanForward(pieces);

            while (attacksFrom[index]) {
                int attackIndex = bitscanForward(attacksFrom[index]);

                attacksTo[attackIndex] &= ~(1ULL << index);
                attacksFrom[index] &= ~(1ULL << attackIndex);
            }

            pieces &= ~(1ULL << index);
        }
    }

    void Bitboard::makeMove(int fromSquare, int toSquare, PieceType pieceType, PieceType promotionPiece) {
        PieceType capturedPiece = getPieceOnSquare(toSquare);

        assert(fromSquare >= 0);
        assert(fromSquare <= 63);
        assert(toSquare <= 63);
        assert(toSquare >= 0);
        assert(moveHistoryIndex >= 0);
        assert(moveHistoryIndex < 256);

        moveHistory[moveHistoryIndex] = zobristHash;
        moveHistoryIndex++;

        pushUndoData(pieceBB, pieceSquareMapping, colorBB, occupiedBB, enPassantSquare, castlingRights, attacksFrom, attacksTo, zobristHash, movesMade,
                           halfMoveClock, fullmoveClock);

        uint64_t updateMask = attacksTo[fromSquare] | attacksTo[toSquare] | (1ULL << fromSquare) | (1ULL << toSquare);

        clearAttacksForPieces(updateMask);
        generatedMoves.clear();

        movesMade += 1;
        halfMoveClock += 1;

        if (enPassantSquare[PieceColor::WHITE] != -1) {
            zobristHash ^= zobristConstants[(enPassantSquare[PieceColor::WHITE] % 8) + 768 + 5];
        }

        if (enPassantSquare[PieceColor::BLACK] != -1) {
            zobristHash ^= zobristConstants[(enPassantSquare[PieceColor::BLACK] % 8) + 768 + 13];
        }

        if (getMovingColor() == PieceColor::BLACK) {
            fullmoveClock += 1;
        }

        if (pieceType == PieceType::WHITE_PAWN || pieceType == PieceType::BLACK_PAWN) {
            halfMoveClock = 0;

            if (fromSquare - toSquare == 16) {
                enPassantSquare[PieceColor::WHITE] = -1;
                enPassantSquare[PieceColor::BLACK] = toSquare + 8;
                zobristHash ^= zobristConstants[(enPassantSquare[PieceColor::BLACK] % 8) + 768 + 5];
            } else if (fromSquare - toSquare == -16) {
                enPassantSquare[PieceColor::WHITE] = toSquare - 8;
                enPassantSquare[PieceColor::BLACK] = -1;
                zobristHash ^= zobristConstants[(enPassantSquare[PieceColor::WHITE] % 8) + 768 + 5];
            } else {
                if (std::abs(fromSquare - toSquare) == 7 || std::abs(fromSquare - toSquare) == 9) {
                    if (enPassantSquare[PieceColor::WHITE] == toSquare) {
                        assert(pieceSquareMapping[toSquare + 8] != PieceType::EMPTY);
                        removePiece(toSquare + 8, PieceType::WHITE_PAWN);
                    } else if (enPassantSquare[PieceColor::BLACK] == toSquare) {
                        assert(pieceSquareMapping[toSquare - 8] != PieceType::EMPTY);
                        removePiece(toSquare - 8, PieceType::BLACK_PAWN);
                    }
                }

                enPassantSquare[PieceColor::WHITE] = -1;
                enPassantSquare[PieceColor::BLACK] = -1;
            }
        } else {
            enPassantSquare[PieceColor::WHITE] = -1;
            enPassantSquare[PieceColor::BLACK] = -1;
        }

        if (pieceType == PieceType::WHITE_KING) {
            castlingRights &= ~(CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE);
            zobristHash ^= zobristConstants[768 + 1];
            zobristHash ^= zobristConstants[768 + 2];
        } else if (pieceType == PieceType::BLACK_KING) {
            castlingRights &= ~(CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
            zobristHash ^= zobristConstants[768 + 3];
            zobristHash ^= zobristConstants[768 + 4];
        }

        // TODO: refactor. Really ugly
        if (pieceType == PieceType::WHITE_ROOK && toSquare == Square::E1) {
            if (fromSquare == Square::A1 && (castlingRights & CastlingRights::WHITE_QUEENSIDE)) {
                handleCastling(Square::A1, Square::E1, PieceType::WHITE_ROOK, PieceType::WHITE_KING, updateMask);
                return;
            } else if (fromSquare == Square::H1 && (castlingRights & CastlingRights::WHITE_KINGSIDE)) {
                handleCastling(Square::H1, Square::E1, PieceType::WHITE_ROOK, PieceType::WHITE_KING, updateMask);
                return;
            }
        }

        if (pieceType == PieceType::BLACK_ROOK && toSquare == Square::E8) {
            if (fromSquare == Square::A8 && (castlingRights & CastlingRights::BLACK_QUEENSIDE)) {
                handleCastling(Square::A8, Square::E8, PieceType::BLACK_ROOK, PieceType::BLACK_KING, updateMask);
                return;
            } else if (fromSquare == Square::H8 && (castlingRights & CastlingRights::BLACK_KINGSIDE)) {
                handleCastling(Square::H8, Square::E8, PieceType::BLACK_ROOK, PieceType::BLACK_KING, updateMask);
                return;
            }
        }

        if (pieceType == PieceType::WHITE_ROOK) {
            if (fromSquare == Square::A1) {
                castlingRights &= ~CastlingRights::WHITE_QUEENSIDE;
                zobristHash ^= zobristConstants[768 + 2];
            } else if (fromSquare == Square::H1) {
                castlingRights &= ~CastlingRights::WHITE_KINGSIDE;
                zobristHash ^= zobristConstants[768 + 1];
            }
        }

        if (pieceType == PieceType::BLACK_ROOK) {
            if (fromSquare == Square::A8) {
                castlingRights &= ~CastlingRights::BLACK_QUEENSIDE;
                zobristHash ^= zobristConstants[768 + 4];
            } else if (fromSquare == Square::H8) {
                castlingRights &= ~CastlingRights::BLACK_KINGSIDE;
                zobristHash ^= zobristConstants[768 + 3];
            }
        }

        if (capturedPiece != PieceType::EMPTY) {
            assert(capturedPiece != PieceType::EMPTY);
            assert(capturedPiece != PieceType::WHITE_KING);
            assert(capturedPiece != PieceType::BLACK_KING);
            removePiece(toSquare, capturedPiece);
            halfMoveClock = 0;
        }

        assert(pieceType != PieceType::EMPTY);
        removePiece(fromSquare, pieceType);

        if (promotionPiece != PieceType::EMPTY) {
            setPiece(toSquare, promotionPiece);
        } else {
            setPiece(toSquare, pieceType);
        }

        movingColor = getOppositeColor(movingColor);
        zobristHash ^= zobristConstants[768];

        assert(generatedMoves.empty());
        generateAttackMapMoves(generatedMoves, *this, PieceColor::WHITE, updateMask);
        generateAttackMapMoves(generatedMoves, *this, PieceColor::BLACK, updateMask);

        // Add new attacks of affected pieces
        for (const Move &move : generatedMoves) {
            assert(move.fromSquare != move.toSquare);

            attacksTo[move.toSquare] |= (1ULL << move.fromSquare);
            attacksFrom[move.fromSquare] |= (1ULL << move.toSquare);
        }

        generatedMoves.clear();
    }

    void Bitboard::makeStrMove(const std::string &move) {
        int fromSquare = getSquareFromString(move.substr(0, 2));
        int toSquare = getSquareFromString(move.substr(2, 2));
        PieceType promotionPiece = PieceType::EMPTY;

        if (move.length() == 5) {
            if (move.ends_with("q")) {
                promotionPiece =
                        getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN;
            } else if (move.ends_with("r")) {
                promotionPiece = getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK;
            } else if (move.ends_with("b")) {
                promotionPiece =
                        getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP;
            } else if (move.ends_with("n")) {
                promotionPiece =
                        getMovingColor() == PieceColor::WHITE ? PieceType::WHITE_KNIGHT : PieceType::BLACK_KNIGHT;
            }
        }

        makeMove(fromSquare, toSquare, getPieceOnSquare(fromSquare), promotionPiece);
    }

    void Bitboard::pushUndoData(const uint64_t pieceBB[12], const PieceType pieceSquareMapping[64], const uint64_t colorBB[2],
                      uint64_t occupiedBB, const int enPassantSquare[2],
                      uint8_t castlingRights, const uint64_t attacksFrom[64], const uint64_t attacksTo[64],
                      uint64_t zobristHash, unsigned int movesMade, unsigned int halfMoveClock, int fullMoveClock) {
        for (int i = 0; i < 12; i++) {
            undoStack[undoStackIndex].pieceBB[i] = pieceBB[i];
        }

        for (int i = 0; i < 64; i++) {
            undoStack[undoStackIndex].attacksFrom[i] = attacksFrom[i];
            undoStack[undoStackIndex].attacksTo[i] = attacksTo[i];
            undoStack[undoStackIndex].pieceSquareMapping[i] = pieceSquareMapping[i];
        }

        undoStack[undoStackIndex].colorBB[0] = colorBB[0];
        undoStack[undoStackIndex].colorBB[1] = colorBB[1];
        undoStack[undoStackIndex].occupiedBB = occupiedBB;
        undoStack[undoStackIndex].enPassantSquare[0] = enPassantSquare[0];
        undoStack[undoStackIndex].enPassantSquare[1] = enPassantSquare[1];
        undoStack[undoStackIndex].castlingRights = castlingRights;
        undoStack[undoStackIndex].zobristHash = zobristHash;
        undoStack[undoStackIndex].movesMade = movesMade;
        undoStack[undoStackIndex].halfMoveClock = halfMoveClock;
        undoStack[undoStackIndex].fullMoveClock = fullMoveClock;

        undoStackIndex++;
        assert(undoStackIndex < 256);
    }

    void Bitboard::unmakeMove() {
        moveHistoryIndex--;
        assert(moveHistoryIndex >= 0);

        undoStackIndex--;
        UndoData undoData = undoStack[undoStackIndex];
        assert(undoStackIndex >= 0);

        for (int i = 0; i < 12; i++) {
            pieceBB[i] =  undoData.pieceBB[i];
        }

        for (int i = 0; i < 64; i++) {
            pieceSquareMapping[i] =  undoData.pieceSquareMapping[i];
            attacksTo[i] =  undoData.attacksTo[i];
            attacksFrom[i] =  undoData.attacksFrom[i];
        }

        colorBB[0] =  undoData.colorBB[0];
        colorBB[1] =  undoData.colorBB[1];
        occupiedBB =  undoData.occupiedBB;
        enPassantSquare[0] =  undoData.enPassantSquare[0];
        enPassantSquare[1] =  undoData.enPassantSquare[1];
        castlingRights =  undoData.castlingRights;
        movingColor = getOppositeColor(movingColor);
        zobristHash =  undoData.zobristHash;
        movesMade =  undoData.movesMade;
        halfMoveClock =  undoData.halfMoveClock;
        fullmoveClock =  undoData.fullMoveClock;
    }

    int Bitboard::getEnPassantSquare(PieceColor color) {
        return enPassantSquare[color];
    }

    bool Bitboard::setFromFEN(const std::string &fen) {
        int index = 63;
        int spaces = 0;

        castlingRights = 0;
        zobristHash = 0;
        movesMade = 0;
        halfMoveClock = 0;
        fullmoveClock = 1;
        enPassantSquare[0] = -1;
        enPassantSquare[1] = -1;

        for (int i = 0; i < 64; i++) {
            attacksFrom[i] = 0;
            attacksTo[i] = 0;
        }

        for (const char character : fen) {
            if (character == ' ') {
                spaces++;
                continue;
            }

            if (spaces == 0) {
                if (character == '/') {
                    continue;
                }

                if (character >= '1' && character <= '8') {
                    index -= character - '0';
                    continue;
                }

                if (character >= 'A' && character <= 'z') {
                    assert(index >= 0);
                    setPieceFromFENChar(character, index);
                    undoStackIndex = 0;
                    moveHistoryIndex = 0;
                    index--;
                    continue;
                } else {
                    senjo::Output(senjo::Output::InfoPrefix) << "Invalid character!";
                    return false;
                }
            }

            if (spaces == 1) {
                movingColor = character == 'w' ? PieceColor::WHITE : PieceColor::BLACK;

                if (movingColor == PieceColor::BLACK) {
                    zobristHash ^= zobristConstants[768];
                }

                continue;
            }

            if (spaces == 2) {
                if (character == '-') {
                    continue;
                }

                if (character == 'K') {
                    castlingRights |= CastlingRights::WHITE_KINGSIDE;
                    zobristHash ^= zobristConstants[768 + 1];
                    continue;
                }

                if (character == 'Q') {
                    castlingRights |= CastlingRights::WHITE_QUEENSIDE;
                    zobristHash ^= zobristConstants[768 + 2];
                    continue;
                }

                if (character == 'k') {
                    castlingRights |= CastlingRights::BLACK_KINGSIDE;
                    zobristHash ^= zobristConstants[768 + 3];
                    continue;
                }

                if (character == 'q') {
                    castlingRights |= CastlingRights::BLACK_QUEENSIDE;
                    zobristHash ^= zobristConstants[768 + 4];
                    continue;
                }

                continue;
            }

            // TODO: en passant square, halfmove and fullmove clock
        }

        undoStackIndex = 0;
        moveHistoryIndex = 0;
        return true;
    }

    void Bitboard::setPieceFromFENChar(const char character, int index) {
        uint64_t updateMask = attacksTo[index] | attacksFrom[index] | (1ULL << index);

        generatedMoves.clear();
        generateAttackMapMoves(generatedMoves, *this, PieceColor::WHITE, updateMask);
        generateAttackMapMoves(generatedMoves, *this, PieceColor::BLACK, updateMask);

        // Clear old attacks of affected pieces
        for (const Move &move : generatedMoves) {
            assert(move.fromSquare != move.toSquare);

            attacksTo[move.toSquare] &= ~(1ULL << move.fromSquare);
            attacksFrom[move.fromSquare] &= ~(1ULL << move.toSquare);
        }

        generatedMoves.clear();

        // Uppercase = WHITE, lowercase = black
        switch (character) {
            case 'P':
                setPiece(index, PieceType::WHITE_PAWN);
                break;
            case 'p':
                setPiece(index, PieceType::BLACK_PAWN);
                break;
            case 'N':
                setPiece(index, PieceType::WHITE_KNIGHT);
                break;
            case 'n':
                setPiece(index, PieceType::BLACK_KNIGHT);
                break;
            case 'B':
                setPiece(index, PieceType::WHITE_BISHOP);
                break;
            case 'b':
                setPiece(index, PieceType::BLACK_BISHOP);
                break;
            case 'R':
                setPiece(index, PieceType::WHITE_ROOK);
                break;
            case 'r':
                setPiece(index, PieceType::BLACK_ROOK);
                break;
            case 'Q':
                setPiece(index, PieceType::WHITE_QUEEN);
                break;
            case 'q':
                setPiece(index, PieceType::BLACK_QUEEN);
                break;
            case 'K':
                setPiece(index, PieceType::WHITE_KING);
                break;
            case 'k':
                setPiece(index, PieceType::BLACK_KING);
                break;
        }

        generateAttackMapMoves(generatedMoves, *this, PieceColor::WHITE, updateMask);
        generateAttackMapMoves(generatedMoves, *this, PieceColor::BLACK, updateMask);

        // Add new attacks of affected pieces
        for (const Move &move : generatedMoves) {
            assert(move.fromSquare != move.toSquare);

            attacksTo[move.toSquare] |= (1ULL << move.fromSquare);
            attacksFrom[move.fromSquare] |= (1ULL << move.toSquare);
        }

        generatedMoves.clear();
    }

    void Bitboard::print() {
        int bbAmount = sizeof(pieceBB) / sizeof(uint64_t);
        char boardChars[64];

        std::fill_n(boardChars, 64, ' ');

        for (int i = 0; i < bbAmount; i++) {
            uint64_t bb = pieceBB[i];
            char pieceChar = getCharacterForPieceType(static_cast<PieceType>(i));

            while (bb) {
                uint64_t index = bitscanForward(bb);
                bb &= ~(1ULL << index);

                boardChars[index] = pieceChar;
            }
        }

        std::cout << "---------------------------------" << std::endl;
        std::cout << "| ";

        for (int i = 0; i < 64; i++) {
            char boardChar = boardChars[63 - i];

            std::cout << boardChar << " | ";

            if ((i + 1) % 8 == 0 && i != 63) {
                std::cout << std::endl << "| ";
            }
        }

        std::cout << std::endl << "---------------------------------" << std::endl;
    }

    void Bitboard::printAvailableMoves(const std::vector<Move> &availableMoves) {
        int bbAmount = sizeof(pieceBB) / sizeof(uint64_t);
        char boardChars[64];

        std::fill_n(boardChars, 64, ' ');

        for (int i = 0; i < bbAmount; i++) {
            uint64_t bb = pieceBB[i];
            char pieceChar = getCharacterForPieceType(static_cast<PieceType>(i));

            while (bb) {
                uint64_t index = bitscanForward(bb);
                bb &= ~(1ULL << index);

                boardChars[index] = pieceChar;
            }
        }

        for (const Move &move : availableMoves) {
            boardChars[move.toSquare] = 'X';
        }

        std::cout << "---------------------------------" << std::endl;
        std::cout << "| ";

        for (int i = 0; i < 64; i++) {
            char boardChar = boardChars[63 - i];

            std::cout << boardChar << " | ";

            if ((i + 1) % 8 == 0 && i != 63) {
                std::cout << std::endl << "| ";
            }
        }

        std::cout << std::endl << "---------------------------------" << std::endl;
    }

    bool Bitboard::isSquareAttackedByColor(int square, PieceColor color) {
        return attacksTo[square] & getBoardByColor(color);
    }

    uint64_t Bitboard::getSquareAttacks(int square) {
        return attacksTo[square];
    }

    uint64_t Bitboard::getSquareAttacksByColor(int square, PieceColor color) {
        return attacksTo[square] & getBoardByColor(color);
    }

    PieceColor Bitboard::getMovingColor() const {
        return movingColor;
    }

    bool Bitboard::isKingInCheck(PieceColor color) {
        uint64_t kingBB = getPieceBoard(color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING);

        if (!kingBB) {
            return false;
        }

        uint64_t kingLocation = bitscanForward(kingBB);

        return isSquareAttackedByColor(kingLocation, getOppositeColor(color));
    }

    bool Bitboard::isDraw() {
        std::vector<Move> moves = generateLegalMoves(*this, movingColor);

        if (moves.empty() && !isKingInCheck(movingColor)) {
            return true;
        }

        if (halfMoveClock >= 100) {
            return true;
        }

        int repetitionCount = 0;
        for (int i = moveHistoryIndex - 1; i >= 0; i--) {
            if (moveHistory[i] == moveHistory[moveHistoryIndex]) {
                repetitionCount++;
            }

            if (repetitionCount >= 3) {
                return true;
            }
        }

        return isInsufficientMaterial();
    }

    int Bitboard::getMovesMade() const {
        return movesMade;
    }

    int Bitboard::getWhiteTimeMsec() const {
        return whiteTimeMsec;
    }

    int Bitboard::getBlackTimeMsec() const {
        return blackTimeMsec;
    }

    void Bitboard::setWhiteTimeMsec(unsigned int whiteTimeMsec) {
        Bitboard::whiteTimeMsec = whiteTimeMsec;
    }

    void Bitboard::setBlackTimeMsec(unsigned int blackTimeMsec) {
        Bitboard::blackTimeMsec = blackTimeMsec;
    }

    bool Bitboard::isInsufficientMaterial() {
        int pieceCount = popcnt(getOccupiedBoard());

        if (pieceCount > 4) {
            return false;
        }

        if (pieceCount == 2) {
            return true;
        }

        if (pieceCount == 3) {
            uint64_t bishopBB = getPieceBoard(WHITE_BISHOP) | getPieceBoard(BLACK_BISHOP);
            uint64_t knightBB = getPieceBoard(WHITE_KNIGHT) | getPieceBoard(BLACK_KNIGHT);

            if (bishopBB || knightBB) {
                return true;
            }
        }

        if (pieceCount == 4) {
            if (popcnt(getWhiteBoard()) != 2) {
                return false;
            }

            uint64_t whiteDrawPieces = getPieceBoard(WHITE_BISHOP) | getPieceBoard(WHITE_KNIGHT);
            uint64_t blackDrawPieces = getPieceBoard(BLACK_BISHOP) | getPieceBoard(BLACK_KNIGHT);

            // King not included in the above boards, so if there is only one piece left, it's a draw
            if (popcnt(whiteDrawPieces) == 1 && popcnt(blackDrawPieces) == 1) {
                return true;
            }
        }

        return false;
    }

    uint64_t Bitboard::getZobristHash() const {
        return zobristHash;
    }

    std::string Bitboard::getNotation(int index) {
        std::string notation;

        int file = index % 8;
        int rank = index / 8;

        notation += "hgfedcba"[file];
        notation += "12345678"[rank];

        return notation;
    }

    void Bitboard::printAvailableMoves(uint64_t availableMoves) {
        char boardChars[64];

        std::fill_n(boardChars, 64, ' ');

        while (availableMoves) {
            uint64_t index = bitscanForward(availableMoves);
            availableMoves &= ~(1ULL << index);

            boardChars[index] = 'X';
        }

        std::cout << "---------------------------------" << std::endl;
        std::cout << "| ";

        for (int i = 0; i < 64; i++) {
            char boardChar = boardChars[63 - i];

            std::cout << boardChar << " | ";

            if ((i + 1) % 8 == 0 && i != 63) {
                std::cout << std::endl << "| ";
            }
        }

        std::cout << std::endl << "---------------------------------" << std::endl;
    }

    uint8_t Bitboard::getCastlingRights() const {
        return castlingRights;
    }

    char Bitboard::getCharacterForPieceType(PieceType pieceType) {
        switch (pieceType) {
            case WHITE_PAWN:
                return 'P';
            case BLACK_PAWN:
                return 'p';
            case WHITE_KNIGHT:
                return 'N';
            case BLACK_KNIGHT:
                return 'n';
            case WHITE_BISHOP:
                return 'B';
            case BLACK_BISHOP:
                return 'b';
            case WHITE_ROOK:
                return 'R';
            case BLACK_ROOK:
                return 'r';
            case WHITE_QUEEN:
                return 'Q';
            case BLACK_QUEEN:
                return 'q';
            case WHITE_KING:
                return 'K';
            case BLACK_KING:
                return 'k';
            case EMPTY:
                return ' ';
        }
    }

    uint64_t Bitboard::getBoardByColor(PieceColor color) {
        return colorBB[color];
    }

    PieceType Bitboard::getPieceOnSquare(int square) {
        return pieceSquareMapping[square];
    }

    bool Bitboard::isWinner(PieceColor color) {
        if (!isKingInCheck(getOppositeColor(color))) {
            return false;
        }

        std::vector<Move> moves = generateLegalMoves(*this, getOppositeColor(color));

        for (const Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);
            bool isKingInCheck = this->isKingInCheck(getOppositeColor(color));
            unmakeMove();

            if (!isKingInCheck) {
                return false;
            }
        }

        return true;
    }

    static const std::map<Square, std::pair<Square, Square>> destinations = {
            {Square::A1, {Square::C1, Square::D1}},
            {Square::H1, {Square::G1, Square::F1}},
            {Square::A8, {Square::C8, Square::D8}},
            {Square::H8, {Square::G8, Square::F8}}
    };

    void Bitboard::handleCastling(Square rookSquare, Square kingSquare, PieceType rookType, PieceType kingType, uint64_t updateMask) {
        auto it = destinations.find(rookSquare);

        assert(it != destinations.end());
        assert(rookType != PieceType::EMPTY);
        assert(kingType != PieceType::EMPTY);
        if (it == destinations.end()) {
            generatedMoves.clear();
            return;
        }

        removePiece(rookSquare, rookType);
        removePiece(kingSquare, kingType);
        setPiece(it->second.first, kingType);
        setPiece(it->second.second, rookType);

        if (kingType == PieceType::WHITE_KING) {
            castlingRights &= ~(CastlingRights::WHITE_QUEENSIDE | CastlingRights::WHITE_KINGSIDE);
            zobristHash ^= zobristConstants[768 + 1];
            zobristHash ^= zobristConstants[768 + 2];
        } else {
            castlingRights &= ~(CastlingRights::BLACK_QUEENSIDE | CastlingRights::BLACK_KINGSIDE);
            zobristHash ^= zobristConstants[768 + 3];
            zobristHash ^= zobristConstants[768 + 4];
        }

        movingColor = getOppositeColor(movingColor);
        zobristHash ^= zobristConstants[768];

        assert(generatedMoves.empty());
        generateAttackMapMoves(generatedMoves, *this, PieceColor::WHITE, updateMask);
        generateAttackMapMoves(generatedMoves, *this, PieceColor::BLACK, updateMask);

        // Add new attacks of affected pieces
        for (const Move &move : generatedMoves) {
            assert(move.fromSquare != move.toSquare);

            attacksTo[move.toSquare] |= (1ULL << move.fromSquare);
            attacksFrom[move.fromSquare] |= (1ULL << move.toSquare);
        }

        generatedMoves.clear();
    }

    PieceColor Bitboard::getOppositeColor(PieceColor color) {
        return static_cast<PieceColor>(color ^ 1);
    }

    int Bitboard::getSquareFromString(std::string notation) {
        int file = 'h' - notation[0];
        int rank = notation[1] - '1';

        return rank * 8 + file;
    }

    uint64_t Bitboard::calculatePawnAttacks(uint64_t bb, PieceColor color) {
        if (color == PieceColor::WHITE) {
            return getWhitePawnAttacks(bb);
        } else {
            return getBlackPawnAttacks(bb);
        }
    }

    int Bitboard::getPieceWeight(PieceType type) {
        switch (type) {
            case WHITE_PAWN:
            case BLACK_PAWN:
                return 100;
            case WHITE_KNIGHT:
            case BLACK_KNIGHT:
            case WHITE_BISHOP:
            case BLACK_BISHOP:
                return 350;
            case WHITE_ROOK:
            case BLACK_ROOK:
                return 525;
            case WHITE_QUEEN:
            case BLACK_QUEEN:
                return 1000;
            case WHITE_KING:
            case BLACK_KING:
                return 100000;
            case EMPTY:
                return 0;
        }
    }

    int Bitboard::getLeastValuablePieceWeight(int attackedSquare, PieceColor color) {
        PieceType pieceOnSquare = pieceSquareMapping[attackedSquare];
        int leastValuableWeight = getPieceColor(pieceOnSquare) == color ? getPieceWeight(pieceOnSquare) : 9999999;
        uint64_t attacks = getSquareAttacksByColor(attackedSquare, color);

        while (attacks) {
            int attackerSquare = bitscanForward(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight < leastValuableWeight) {
                leastValuableWeight = weight;
            }

            attacks &= ~(1ULL << attackerSquare);
        }

        return leastValuableWeight;
    }

    int Bitboard::getSmallestAttackerSquare(int square, PieceColor attackingColor) {
        uint64_t attacks = getSquareAttacksByColor(square, attackingColor);
        int smallestAttackerSquare = -1;
        int smallestAttackerWeight = 999999999;

        while (attacks) {
            int attackerSquare = bitscanForward(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight < smallestAttackerWeight) {
                smallestAttackerWeight = weight;
                smallestAttackerSquare = attackerSquare;
            }

            attacks &= ~(1ULL << attackerSquare);
        }

        return smallestAttackerSquare;
    }

    int Bitboard::seeCapture(int fromSquare, int toSquare, PieceColor attackingColor) {
        assert(fromSquare >= 0);
        assert(fromSquare <= 63);
        assert(toSquare >= 0);
        assert(toSquare <= 63);
        int score = 0;
        PieceType movingPiece = pieceSquareMapping[fromSquare];
        PieceType capturedPieceType = pieceSquareMapping[toSquare];

        assert(movingPiece != PieceType::EMPTY);
        assert(capturedPieceType != PieceType::EMPTY);
        makeMove(fromSquare, toSquare, movingPiece, PieceType::EMPTY);
        score = getPieceWeight(capturedPieceType) - see(toSquare, getOppositeColor(attackingColor));
        unmakeMove();

        return score;
    }

    int Bitboard::see(int square, PieceColor attackingColor) {
        assert(square >= 0);
        assert(square <= 63);
        int score = 0;
        int smallestAttackerSquare = getSmallestAttackerSquare(square, attackingColor);

        if (smallestAttackerSquare >= 0) {
            PieceType movingPiece = pieceSquareMapping[smallestAttackerSquare];
            PieceType capturedPieceType = pieceSquareMapping[square];

            assert(movingPiece != PieceType::EMPTY);
            assert(capturedPieceType != PieceType::EMPTY);
            makeMove(smallestAttackerSquare, square, movingPiece, PieceType::EMPTY);
            score = std::max(0, getPieceWeight(capturedPieceType) - see(square, getOppositeColor(attackingColor)));
            unmakeMove();
        }

        return score;
    }

    int Bitboard::mvvlva(int attackedSquare, PieceColor attackingColor) {
        return getMostValuablePieceWeight(attackedSquare, attackingColor) - getLeastValuablePieceWeight(attackedSquare, getOppositeColor(attackingColor));
    }

    int Bitboard::getMostValuablePieceWeight(int attackedSquare, PieceColor color) {
        PieceType pieceOnSquare = pieceSquareMapping[attackedSquare];
        int mostValuableWeight = getPieceColor(pieceOnSquare) == color ? getPieceWeight(pieceOnSquare) : -9999999;
        uint64_t attacks = getSquareAttacksByColor(attackedSquare, color);

        while (attacks) {
            int attackerSquare = bitscanForward(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight > mostValuableWeight) {
                mostValuableWeight = weight;
            }

            attacks &= ~(1ULL << attackerSquare);
        }

        return mostValuableWeight;
    }

    uint64_t Bitboard::getRayAttacks(uint64_t occupied, Direction direction, int square) {
        uint64_t attacks = rayAttacks[direction][square];
        uint64_t blocker = attacks & occupied;
        int blockerSquare = bitscanForward(blocker | 0x8000000000000000ULL);

        return attacks ^ rayAttacks[direction][blockerSquare];
    }

    uint64_t Bitboard::getNegativeRayAttacks(uint64_t occupied, Direction direction, int square) {
        uint64_t attacks = rayAttacks[direction][square];
        uint64_t blocker =  attacks & occupied;
        int blockerSquare = bitscanReverse(blocker | 1ULL);

        return attacks ^ rayAttacks[direction][blockerSquare];
    }

    PieceColor Bitboard::getPieceColor(PieceType type) {
        return static_cast<PieceColor>(type % 2);
    }

    void Bitboard::initializeRayAttacks() {
        uint64_t sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            for (int direction = 0; direction < 8; direction++) {
                switch (static_cast<Direction>(direction)) {
                    case NORTH:
                        rayAttacks[direction][sq] = nortOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case SOUTH:
                        rayAttacks[direction][sq] = soutOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case EAST:
                        rayAttacks[direction][sq] = eastOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case WEST:
                        rayAttacks[direction][sq] = westOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case NORTH_EAST:
                        rayAttacks[direction][sq] = noEaOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case NORTH_WEST:
                        rayAttacks[direction][sq] = noWeOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case SOUTH_EAST:
                        rayAttacks[direction][sq] = soEaOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                    case SOUTH_WEST:
                        rayAttacks[direction][sq] = soWeOccl(sqBB, ~0ULL) & ~sqBB;
                        break;
                }
            }
        }
    }

    uint64_t soutOne(uint64_t b) {
        return b >> 8ULL;
    }

    uint64_t nortOne(uint64_t b) {
        return b << 8ULL;
    }

    uint64_t eastOne(uint64_t b) {
        return (b << 1ULL) & NOT_A_FILE;
    }

    uint64_t noEaOne(uint64_t b) {
        return (b << 9ULL) & NOT_A_FILE;
    }

    uint64_t soEaOne(uint64_t b) {
        return (b >> 7ULL) & NOT_A_FILE;
    }

    uint64_t westOne(uint64_t b) {
        return (b >> 1ULL) & NOT_H_FILE;
    }

    uint64_t soWeOne(uint64_t b) {
        return (b >> 9ULL) & NOT_H_FILE;
    }

    uint64_t noWeOne(uint64_t b) {
        return (b << 7ULL) & NOT_H_FILE;
    }

    uint64_t noNoEa(uint64_t b) {
        return (b << 17ULL) & NOT_A_FILE;
    }

    uint64_t noEaEa(uint64_t b) {
        return (b << 10ULL) & NOT_AB_FILE;
    }

    uint64_t soEaEa(uint64_t b) {
        return (b >> 6ULL) & NOT_AB_FILE;
    }

    uint64_t soSoEa(uint64_t b) {
        return (b >> 15ULL) & NOT_A_FILE;
    }

    uint64_t noNoWe(uint64_t b) {
        return (b << 15ULL) & NOT_H_FILE;
    }

    uint64_t noWeWe(uint64_t b) {
        return (b << 6ULL) & NOT_GH_FILE;

    }

    uint64_t soWeWe(uint64_t b) {
        return (b >> 10ULL) & NOT_GH_FILE;
    }

    uint64_t soSoWe(uint64_t b) {
        return (b >> 17ULL) & NOT_H_FILE;
    }

    uint64_t popcnt(uint64_t b) {
        return __builtin_popcountll(b);
    }

    int bitscanForward(uint64_t b) {
        int result = b ? __builtin_ffsll(b) - 1 : 0;
        assert(result >= 0 && result < 64);
        return result;
    }

    int bitscanReverse(uint64_t b) {
        int result =  b ? __builtin_clzll(b) ^ 63 : 0;
        assert(result >= 0 && result < 64);
        return result;
    }

    uint64_t soutOccl(uint64_t pieceBB, uint64_t empty) {
        pieceBB |= empty & (pieceBB >> 8ULL);
        empty &= (empty >> 8ULL);
        pieceBB |= empty & (pieceBB >> 16ULL);
        empty &= (empty >> 16ULL);
        pieceBB |= empty & (pieceBB >> 32ULL);
        return soutOne(pieceBB);
    }

    uint64_t nortOccl(uint64_t pieceBB, uint64_t empty) {
        pieceBB |= empty & (pieceBB << 8ULL);
        empty &= (empty << 8ULL);
        pieceBB |= empty & (pieceBB << 16ULL);
        empty &= (empty << 16ULL);
        pieceBB |= empty & (pieceBB << 32ULL);
        return nortOne(pieceBB);
    }

    uint64_t eastOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_A_FILE;
        pieceBB |= empty & (pieceBB << 1ULL);
        empty &= (empty << 1ULL);
        pieceBB |= empty & (pieceBB << 2ULL);
        empty &= (empty << 2ULL);
        pieceBB |= empty & (pieceBB << 4ULL);
        return eastOne(pieceBB);
    }

    uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_A_FILE;
        pieceBB |= empty & (pieceBB << 9ULL);
        empty &= (empty << 9ULL);
        pieceBB |= empty & (pieceBB << 18ULL);
        empty &= (empty << 18ULL);
        pieceBB |= empty & (pieceBB << 36ULL);
        return noEaOne(pieceBB);
    }

    uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_A_FILE;
        pieceBB |= empty & (pieceBB >> 7ULL);
        empty &= (empty >> 7ULL);
        pieceBB |= empty & (pieceBB >> 14ULL);
        empty &= (empty >> 14ULL);
        pieceBB |= empty & (pieceBB >> 28ULL);
        return soEaOne(pieceBB);
    }

    uint64_t westOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_H_FILE;
        pieceBB |= empty & (pieceBB >> 1ULL);
        empty &= (empty >> 1ULL);
        pieceBB |= empty & (pieceBB >> 2ULL);
        empty &= (empty >> 2ULL);
        pieceBB |= empty & (pieceBB >> 4ULL);
        return westOne(pieceBB);
    }

    uint64_t soWeOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_H_FILE;
        pieceBB |= empty & (pieceBB >> 9ULL);
        empty &= (empty >> 9ULL);
        pieceBB |= empty & (pieceBB >> 18ULL);
        empty &= (empty >> 18ULL);
        pieceBB |= empty & (pieceBB >> 36ULL);
        return soWeOne(pieceBB);
    }

    uint64_t noWeOccl(uint64_t pieceBB, uint64_t empty) {
        empty &= NOT_H_FILE;
        pieceBB |= empty & (pieceBB << 7ULL);
        empty &= (empty << 7ULL);
        pieceBB |= empty & (pieceBB << 14ULL);
        empty &= (empty << 14ULL);
        pieceBB |= empty & (pieceBB << 28ULL);
        return noWeOne(pieceBB);
    }

    uint64_t getWhitePawnEastAttacks(uint64_t wPawns) {
        return noEaOne(wPawns);
    }

    uint64_t getWhitePawnWestAttacks(uint64_t wPawns) {
        return noWeOne(wPawns);
    }

    uint64_t calculateKnightAttacks(uint64_t knights) {
        uint64_t l1 = (knights >> 1ULL) & NOT_H_FILE;
        uint64_t l2 = (knights >> 2ULL) & NOT_GH_FILE;
        uint64_t r1 = (knights << 1ULL) & NOT_A_FILE;
        uint64_t r2 = (knights << 2ULL) & NOT_AB_FILE;
        uint64_t h1 = l1 | r1;
        uint64_t h2 = l2 | r2;
        return (h1 << 16ULL) | (h1 >> 16ULL) | (h2 << 8ULL) | (h2 >> 8ULL);
    }

    uint64_t calculateKingAttacks(uint64_t kingSet) {
        uint64_t attacks = eastOne(kingSet) | westOne(kingSet);
        kingSet |= attacks;
        attacks |= nortOne(kingSet) | soutOne(kingSet);

        return attacks;
    }

    uint64_t getBlackPawnEastAttacks(uint64_t bPawns) {
        return soEaOne(bPawns);
    }

    uint64_t getBlackPawnWestAttacks(uint64_t bPawns) {
        return soWeOne(bPawns);
    }
}