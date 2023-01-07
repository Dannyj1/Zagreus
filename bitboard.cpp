//
// Created by Danny on 8-5-2022.
//

#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <algorithm>
#include <cassert>
#include <immintrin.h>

#include "bitboard.h"
#include "move_gen.h"
#include "types.h"
#include "senjo/Output.h"
#include "magics.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
namespace Zagreus {

    Bitboard::Bitboard() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

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
        initializeBetweenLookup();
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

    void Bitboard::makeNullMove() {
        moveHistoryIndex++;

        pushUndoData(pieceBB, pieceSquareMapping, colorBB, occupiedBB, enPassantSquare, castlingRights, zobristHash,
                     ply,
                     halfMoveClock, fullmoveClock);

        movingColor = getOppositeColor(movingColor);

        if (getMovingColor() == PieceColor::BLACK) {
            fullmoveClock += 1;
        }

        zobristHash ^= zobristConstants[768];
    }

    uint64_t Bitboard::getQueenAttacks(int square, uint64_t occupancy) {
        return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
    }

    uint64_t Bitboard::getBishopAttacks(int square, uint64_t occupancy) {
        occupancy &= getBishopMask(square);
        occupancy *= getBishopMagic(square);
        occupancy >>= 64 - BBits[square];

        return getBishopMagicAttacks(square, occupancy);
    }

    uint64_t Bitboard::getRookAttacks(int square, uint64_t occupancy) {
        occupancy &= getRookMask(square);
        occupancy *= getRookMagic(square);
        occupancy >>= 64 - RBits[square];

        return getRookMagicAttacks(square, occupancy);
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

    bool Bitboard::isOpenFile(int square) {
        uint64_t fileMask = rayAttacks[Direction::NORTH][square] | rayAttacks[Direction::SOUTH][square];
        uint64_t occupied = getPieceBoard(PieceType::WHITE_PAWN) | getPieceBoard(PieceType::BLACK_PAWN);

        return fileMask == (fileMask & occupied);
    }

    bool Bitboard::isSemiOpenFile(int square, PieceColor color) {
        uint64_t fileMask = rayAttacks[Direction::NORTH][square] | rayAttacks[Direction::SOUTH][square];
        uint64_t ownOccupied = getPieceBoard(
                color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN);
        uint64_t opponentOccupied = getPieceBoard(
                color == PieceColor::WHITE ? PieceType::BLACK_PAWN : PieceType::WHITE_PAWN);

        return fileMask == (fileMask & ownOccupied) && fileMask != (fileMask & opponentOccupied);
    }

    unsigned int Bitboard::getHalfMoveClock() const {
        return halfMoveClock;
    }

    int Bitboard::getFullmoveClock() const {
        return fullmoveClock;
    }

    // Also returns true when it is an open file
    bool Bitboard::isSemiOpenFileLenient(int square, PieceColor color) {
        uint64_t fileMask = rayAttacks[Direction::NORTH][square] | rayAttacks[Direction::SOUTH][square];
        uint64_t ownOccupied = getPieceBoard(
                color == PieceColor::WHITE ? PieceType::WHITE_PAWN : PieceType::BLACK_PAWN);

        return fileMask == (fileMask & ownOccupied);
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

        pushUndoData(pieceBB, pieceSquareMapping, colorBB, occupiedBB, enPassantSquare, castlingRights, zobristHash,
                     ply,
                     halfMoveClock, fullmoveClock);

        ply += 1;
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
                handleCastling(Square::A1, Square::E1, PieceType::WHITE_ROOK, PieceType::WHITE_KING);
                return;
            } else if (fromSquare == Square::H1 && (castlingRights & CastlingRights::WHITE_KINGSIDE)) {
                handleCastling(Square::H1, Square::E1, PieceType::WHITE_ROOK, PieceType::WHITE_KING);
                return;
            }
        }

        if (pieceType == PieceType::BLACK_ROOK && toSquare == Square::E8) {
            if (fromSquare == Square::A8 && (castlingRights & CastlingRights::BLACK_QUEENSIDE)) {
                handleCastling(Square::A8, Square::E8, PieceType::BLACK_ROOK, PieceType::BLACK_KING);
                return;
            } else if (fromSquare == Square::H8 && (castlingRights & CastlingRights::BLACK_KINGSIDE)) {
                handleCastling(Square::H8, Square::E8, PieceType::BLACK_ROOK, PieceType::BLACK_KING);
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
//            assert(capturedPiece != PieceType::WHITE_KING);
//            assert(capturedPiece != PieceType::BLACK_KING);
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

    void Bitboard::pushUndoData(const uint64_t pieceBB[12], const PieceType pieceSquareMapping[64],
                                const uint64_t colorBB[2],
                                uint64_t occupiedBB, const int enPassantSquare[2],
                                uint8_t castlingRights, uint64_t zobristHash, unsigned int movesMade,
                                unsigned int halfMoveClock, int fullMoveClock) {
        for (int i = 0; i < 12; i++) {
            undoStack[undoStackIndex].pieceBB[i] = pieceBB[i];
        }

        for (int i = 0; i < 64; i++) {
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
            pieceBB[i] = undoData.pieceBB[i];
        }

        for (int i = 0; i < 64; i++) {
            pieceSquareMapping[i] = undoData.pieceSquareMapping[i];
        }

        colorBB[0] = undoData.colorBB[0];
        colorBB[1] = undoData.colorBB[1];
        occupiedBB = undoData.occupiedBB;
        enPassantSquare[0] = undoData.enPassantSquare[0];
        enPassantSquare[1] = undoData.enPassantSquare[1];
        castlingRights = undoData.castlingRights;
        movingColor = getOppositeColor(movingColor);
        zobristHash = undoData.zobristHash;
        ply = undoData.movesMade;
        halfMoveClock = undoData.halfMoveClock;
        fullmoveClock = undoData.fullMoveClock;
    }

    int Bitboard::getEnPassantSquare(PieceColor color) {
        return enPassantSquare[color];
    }

    bool Bitboard::setFromFEN(const std::string &fen) {
        int index = 63;
        int spaces = 0;

        castlingRights = 0;
        zobristHash = 0;
        ply = 0;
        halfMoveClock = 0;
        fullmoveClock = 1;
        enPassantSquare[0] = -1;
        enPassantSquare[1] = -1;

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

    bool Bitboard::isPawnEndgame() {
        int pieceCount = popcnt(colorBB[PieceColor::WHITE]) + popcnt(colorBB[PieceColor::BLACK]);
        uint64_t pawnBB = pieceBB[PieceType::WHITE_PAWN] | pieceBB[PieceType::BLACK_PAWN];

        return popcnt(pawnBB) == (pieceCount - 2);
    }

    void Bitboard::setPieceFromFENChar(const char character, int index) {
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
    }

    void Bitboard::print() {
        int bbAmount = sizeof(pieceBB) / sizeof(uint64_t);
        char boardChars[64];

        std::fill_n(boardChars, 64, ' ');

        for (int i = 0; i < bbAmount; i++) {
            uint64_t bb = pieceBB[i];
            char pieceChar = getCharacterForPieceType(static_cast<PieceType>(i));

            while (bb) {
                int index = bitscanForward(bb);
                bb = _blsr_u64(bb);

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

    void Bitboard::printAvailableMoves(const std::vector<Move>& moves) {
        int bbAmount = sizeof(pieceBB) / sizeof(uint64_t);
        char boardChars[64];

        std::fill_n(boardChars, 64, ' ');

        for (int i = 0; i < bbAmount; i++) {
            uint64_t bb = pieceBB[i];
            char pieceChar = getCharacterForPieceType(static_cast<PieceType>(i));

            while (bb) {
                int index = bitscanForward(bb);
                bb = _blsr_u64(bb);

                boardChars[index] = pieceChar;
            }
        }
        
        for (Move move : moves) {
            makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (!isKingInCheck(getOppositeColor(movingColor))) {
                boardChars[move.toSquare] = 'X';
            }

            unmakeMove();
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
        return getSquareAttacksByColor(square, color) != 0;
    }

    uint64_t Bitboard::getSquareAttacks(int square) {
        uint64_t queenBB = getPieceBoard(PieceType::WHITE_QUEEN) | getPieceBoard(PieceType::BLACK_QUEEN);
        uint64_t straightSlidingPieces =
                getPieceBoard(PieceType::WHITE_ROOK) | getPieceBoard(PieceType::BLACK_ROOK) | queenBB;
        uint64_t diagonalSlidingPieces =
                getPieceBoard(PieceType::WHITE_BISHOP) | getPieceBoard(PieceType::BLACK_BISHOP) | queenBB;

        uint64_t pawnAttacks = getPawnAttacks(square, PieceColor::BLACK) & getPieceBoard(PieceType::WHITE_PAWN);
        pawnAttacks |= getPawnAttacks(square, PieceColor::WHITE) & getPieceBoard(PieceType::BLACK_PAWN);
        uint64_t rookAttacks = getRookAttacks(square, getOccupiedBoard()) & straightSlidingPieces;
        uint64_t bishopAttacks = getBishopAttacks(square, getOccupiedBoard()) & diagonalSlidingPieces;
        uint64_t knightAttacks = getKnightAttacks(square) &
                                 (getPieceBoard(PieceType::WHITE_KNIGHT) | getPieceBoard(PieceType::BLACK_KNIGHT));
        uint64_t kingAttacks =
                getKingAttacks(square) & (getPieceBoard(PieceType::WHITE_KING) | getPieceBoard(PieceType::BLACK_KING));

        return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
    }

    uint64_t Bitboard::getSquareAttacksByColor(int square, PieceColor color) {
        uint64_t queenBB = getPieceBoard(PieceType::WHITE_QUEEN + color);
        uint64_t straightSlidingPieces = getPieceBoard(PieceType::WHITE_ROOK + color) | queenBB;
        uint64_t diagonalSlidingPieces = getPieceBoard(PieceType::WHITE_BISHOP + color) | queenBB;

        uint64_t pawnAttacks = getPawnAttacks(square, Bitboard::getOppositeColor(color)) &
                               getPieceBoard(PieceType::WHITE_PAWN + color);
        uint64_t rookAttacks = getRookAttacks(square, getOccupiedBoard()) & straightSlidingPieces;
        uint64_t bishopAttacks = getBishopAttacks(square, getOccupiedBoard()) & diagonalSlidingPieces;
        uint64_t knightAttacks = getKnightAttacks(square) & getPieceBoard(PieceType::WHITE_KNIGHT + color);
        uint64_t kingAttacks = getKingAttacks(square) & getPieceBoard(PieceType::WHITE_KING + color);

        return pawnAttacks | rookAttacks | bishopAttacks | knightAttacks | kingAttacks;
    }

    PieceColor Bitboard::getMovingColor() const {
        return movingColor;
    }

    bool Bitboard::isKingInCheck(PieceColor color) {
        uint64_t kingBB = getPieceBoard(PieceType::WHITE_KING + color);

        if (!kingBB) {
            return false;
        }

        int kingLocation = bitscanForward(kingBB);

        return isSquareAttackedByColor(kingLocation, getOppositeColor(color));
    }

    bool Bitboard::isDraw() {
        std::vector<Move> moves = generateLegalMoves(*this, movingColor);
        bool hasLegalMove = false;

        for (Move &move : moves) {
            makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (!isKingInCheck(getOppositeColor(movingColor))) {
                unmakeMove();
                hasLegalMove = true;
                break;
            }

            unmakeMove();
        }

        if (!hasLegalMove) {
            return true;
        }

        if (halfMoveClock >= 100) {
            return true;
        }

        // Check if the same position has occurred 3 times using the movehistory array
        int samePositionCount = 0;
        uint64_t boardHash = getZobristHash();

        for (int i = moveHistoryIndex; i >= 0; i--) {
            assert(moveHistory[i] != 0);

            if (moveHistory[i] == boardHash) {
                samePositionCount++;
            }

            if (samePositionCount >= 3) {
                return true;
            }
        }

        return isInsufficientMaterial();
    }

    uint64_t Bitboard::getTilesBetween(int from, int to) {
        return betweenTable[from][to];
    }

    int Bitboard::getPly() const {
        return ply;
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
            int index = bitscanForward(availableMoves);
            availableMoves = _blsr_u64(availableMoves);

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

        for (Move &move : moves) {
            assert(move.fromSquare != move.toSquare);

            makeMove(move.fromSquare, move.toSquare, move.pieceType, move.promotionPiece);

            if (isKingInCheck(getOppositeColor(color))) {
                unmakeMove();
                continue;
            }

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

    void Bitboard::handleCastling(Square rookSquare, Square kingSquare, PieceType rookType, PieceType kingType) {
        auto it = destinations.find(rookSquare);

        assert(it != destinations.end());
        assert(rookType != PieceType::EMPTY);
        assert(kingType != PieceType::EMPTY);
        if (it == destinations.end()) {
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
                return 350 - ((8 - popcnt(getPieceBoard(PieceType::WHITE_PAWN))) * 5);
            case BLACK_KNIGHT:
                return 350 - ((8 - popcnt(getPieceBoard(PieceType::BLACK_PAWN))) * 5);
            case WHITE_BISHOP:
            case BLACK_BISHOP:
                return 350;
            case WHITE_ROOK:
                return 525 + ((8 - popcnt(getPieceBoard(PieceType::WHITE_PAWN))) * 5);
            case BLACK_ROOK:
                return 525 + ((8 - popcnt(getPieceBoard(PieceType::BLACK_PAWN))) * 5);
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

    bool Bitboard::isPinnedStraight(int attackedSquare, PieceColor attackingColor) {
        uint64_t attackedBy = getSquareAttacksByColor(attackedSquare, attackingColor);
        uint64_t straightSlidingPieces =
                getPieceBoard(attackingColor == PieceColor::WHITE ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK) |
                getPieceBoard(attackingColor == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);
        uint64_t attackers = attackedBy & straightSlidingPieces;
        uint64_t opponentPieces = getBoardByColor(getOppositeColor(attackingColor));
        uint64_t ownPieces = getBoardByColor(attackingColor);

        // TODO: refactor and find ways to improve performance
        while (attackers) {
            int index = bitscanForward(attackers);
            attackers = _blsr_u64(attackers);

            uint64_t northRay = rayAttacks[Direction::NORTH][index];
            uint64_t southRay = rayAttacks[Direction::SOUTH][index];
            uint64_t eastRay = rayAttacks[Direction::EAST][index];
            uint64_t westRay = rayAttacks[Direction::WEST][index];

            if (popcnt(northRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanForward(northRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanForward(northRay & opponentPieces & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(getPieceOnSquare(pinnedPieceIndex));
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }

            if (popcnt(southRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanReverse(southRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanReverse((southRay & opponentPieces) & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(getPieceOnSquare(pinnedPieceIndex));
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }

            if (popcnt(eastRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanForward(eastRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanForward(eastRay & opponentPieces & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(getPieceOnSquare(pinnedPieceIndex));
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }

            if (popcnt(westRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanReverse(westRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanReverse(westRay & opponentPieces & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(getPieceOnSquare(pinnedPieceIndex));
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool Bitboard::isPinnedDiagonally(int attackedSquare, PieceColor attackingColor) {
        uint64_t attackedBy = getSquareAttacksByColor(attackedSquare, attackingColor);
        uint64_t straightSlidingPieces =
                getPieceBoard(attackingColor == PieceColor::WHITE ? PieceType::WHITE_BISHOP : PieceType::BLACK_BISHOP) |
                getPieceBoard(attackingColor == PieceColor::WHITE ? PieceType::WHITE_QUEEN : PieceType::BLACK_QUEEN);
        uint64_t attackers = attackedBy & straightSlidingPieces;
        uint64_t opponentPieces = getBoardByColor(getOppositeColor(attackingColor));
        uint64_t ownPieces = getBoardByColor(attackingColor);

        // TODO: refactor and find ways to improve performance
        while (attackers) {
            int index = bitscanForward(attackers);
            attackers = _blsr_u64(attackers);

            uint64_t noWeRay = rayAttacks[Direction::NORTH_WEST][index];
            uint64_t noEaRay = rayAttacks[Direction::NORTH_EAST][index];
            uint64_t soEaRay = rayAttacks[Direction::SOUTH_EAST][index];
            uint64_t soWeRay = rayAttacks[Direction::SOUTH_WEST][index];

            if (popcnt(noWeRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanForward(noWeRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanForward(noWeRay & opponentPieces & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(pinnedPieceType);
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }

            if (popcnt(noEaRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanForward(noEaRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanForward((noEaRay & opponentPieces) & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(pinnedPieceType);
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }

            if (popcnt(soEaRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanReverse(soEaRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanReverse(soEaRay & opponentPieces & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(pinnedPieceType);
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }

            if (popcnt(soWeRay & opponentPieces) >= 2) {
                uint64_t pinnedPieceIndex = bitscanReverse(soWeRay & opponentPieces);
                uint64_t pinnedToIndex = bitscanReverse(soWeRay & opponentPieces & ~(1ULL << pinnedPieceIndex));
                PieceType pinnedToPieceType = getPieceOnSquare(pinnedToIndex);
                PieceType pinnedPieceType = getPieceOnSquare(pinnedPieceIndex);
                uint64_t between = betweenTable[pinnedPieceIndex][pinnedToIndex];

                if (!(between & ownPieces)) {
                    if ((pinnedToPieceType == PieceType::WHITE_KING || pinnedToPieceType == PieceType::BLACK_KING)
                        && (pinnedPieceType != PieceType::WHITE_PAWN && pinnedPieceType != PieceType::BLACK_PAWN)) {
                        return true;
                    }

                    int pinnedPieceValue = getPieceWeight(pinnedPieceType);
                    int pinnedtoValue = getPieceWeight(pinnedToPieceType);

                    if (pinnedtoValue > pinnedPieceValue) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool Bitboard::isPinned(int attackedSquare, PieceColor attackingColor) {
        return isPinnedStraight(attackedSquare, attackingColor) || isPinnedDiagonally(attackedSquare, attackingColor);
    }

    int Bitboard::getLeastValuablePieceWeight(int attackedSquare, PieceColor color) {
        PieceType pieceOnSquare = pieceSquareMapping[attackedSquare];
        int leastValuableWeight = getPieceColor(pieceOnSquare) == color ? getPieceWeight(pieceOnSquare) : 9999999;
        uint64_t attacks = getSquareAttacksByColor(attackedSquare, color);

        while (attacks) {
            int attackerSquare = bitscanForward(attacks);
            attacks = _blsr_u64(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight < leastValuableWeight) {
                leastValuableWeight = weight;
            }
        }

        return leastValuableWeight;
    }

    int Bitboard::getSmallestAttackerSquare(int square, PieceColor attackingColor) {
        uint64_t attacks = getSquareAttacksByColor(square, attackingColor);
        int smallestAttackerSquare = -1;
        int smallestAttackerWeight = 999999999;

        while (attacks) {
            int attackerSquare = bitscanForward(attacks);
            attacks = _blsr_u64(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight < smallestAttackerWeight) {
                smallestAttackerWeight = weight;
                smallestAttackerSquare = attackerSquare;
            }
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
        return getMostValuablePieceWeight(attackedSquare, attackingColor) -
               getLeastValuablePieceWeight(attackedSquare, getOppositeColor(attackingColor));
    }

    int Bitboard::getMostValuablePieceWeight(int attackedSquare, PieceColor color) {
        PieceType pieceOnSquare = pieceSquareMapping[attackedSquare];
        int mostValuableWeight = getPieceColor(pieceOnSquare) == color ? getPieceWeight(pieceOnSquare) : -9999999;
        uint64_t attacks = getSquareAttacksByColor(attackedSquare, color);

        while (attacks) {
            int attackerSquare = bitscanForward(attacks);
            attacks = _blsr_u64(attacks);
            PieceType pieceType = pieceSquareMapping[attackerSquare];
            int weight = getPieceWeight(pieceType);

            if (weight > mostValuableWeight) {
                mostValuableWeight = weight;
            }
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
        uint64_t blocker = attacks & occupied;
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

    unsigned int Bitboard::getWhiteTimeIncrement() const {
        return whiteTimeIncrement;
    }

    void Bitboard::setWhiteTimeIncrement(unsigned int whiteTimeIncrement) {
        Bitboard::whiteTimeIncrement = whiteTimeIncrement;
    }

    unsigned int Bitboard::getBlackTimeIncrement() const {
        return blackTimeIncrement;
    }

    void Bitboard::setBlackTimeIncrement(unsigned int blackTimeIncrement) {
        Bitboard::blackTimeIncrement = blackTimeIncrement;
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

    uint32_t encodeMove(Move &move) {
        return (move.promotionPiece << 25) | (move.promotionPiece << 20) | (move.pieceType << 15) |
               (move.fromSquare << 7) | move.toSquare;
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
        return b ? _tzcnt_u64(b) : 0;
    }

    int bitscanReverse(uint64_t b) {
        return b ? _lzcnt_u64(b) ^ 63 : 0;
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
#pragma clang diagnostic pop