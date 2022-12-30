//
// Created by Danny on 8-5-2022.
//

#include <cstdint>
#include <iostream>
#include <psdk_inc/intrin-impl.h>
#include <map>

#include "bitboard.h"
#include "move_gen.h"

namespace Chess {

    Bitboard::Bitboard() {
        uint64_t sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            kingAttacks[sq] = calculateKingAttacks(sqBB);
        }

        sqBB = 1ULL;
        for (int sq = 0; sq < 64; sq++, sqBB <<= 1ULL) {
            knightAttacks[sq] = calculateKnightAttacks(sqBB);
        }
    }

    uint64_t Bitboard::getPieceBoard(int pieceType) {
        return pieceBB[pieceType];
    }

    uint64_t Bitboard::getWhiteBoard() {
        return whiteBB;
    }

    uint64_t Bitboard::getBlackBoard() {
        return blackBB;
    }

    uint64_t Bitboard::getEmptyBoard() {
        return ~occupiedBB;
    }

    uint64_t Bitboard::getOccupiedBoard() {
        return occupiedBB;
    }

    uint64_t Bitboard::getKingAttacks(int square) {
        return this->kingAttacks[square];
    }

    uint64_t Bitboard::getKnightAttacks(int square) {
        return this->knightAttacks[square];
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

    uint64_t Bitboard::getBishopAttacks(uint64_t bb) {
        uint64_t bishopAttacks = 0ULL;
        uint64_t emptyBB = getEmptyBoard();

        bishopAttacks |= noEaOccl(bb, emptyBB);
        bishopAttacks |= noWeOccl(bb, emptyBB);
        bishopAttacks |= soEaOccl(bb, emptyBB);
        bishopAttacks |= soWeOccl(bb, emptyBB);

        return bishopAttacks;
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
        pieceBB[pieceType] |= 1ULL << index;
        occupiedBB |= 1ULL << index;

        if (pieceType % 2 == 0) {
            whiteBB |= 1ULL << index;
        } else {
            blackBB |= 1ULL << index;
        }
    }

    void Bitboard::removePiece(int index, PieceType pieceType) {
        pieceBB[pieceType] &= ~(1ULL << index);
        occupiedBB &= ~(1ULL << index);
        whiteBB &= ~(1ULL << index);
        blackBB &= ~(1ULL << index);
    }

    void Bitboard::makeMove(int fromSquare, int toSquare, PieceType pieceType) {
        PieceType capturedPiece = getPieceOnSquare(toSquare);

        undoStack.push({{}, whiteBB, blackBB, occupiedBB, whiteEnPassantSquare, blackEnPassantSquare, castlingRights, whiteAttackMap, blackAttackMap});

        for (int i = 0; i < 12; i++) {
            undoStack.top().pieceBB[i] = pieceBB[i];
        }

        if (pieceType == PieceType::WHITE_PAWN || pieceType == PieceType::BLACK_PAWN) {
            if (fromSquare - toSquare == 16) {
                whiteEnPassantSquare = -1;
                blackEnPassantSquare = toSquare + 8;
            } else if (fromSquare - toSquare == -16) {
                whiteEnPassantSquare = toSquare - 8;
                blackEnPassantSquare = -1;
            } else {
                if (std::abs(fromSquare - toSquare) == 7 || std::abs(fromSquare - toSquare) == 9) {
                    if (whiteEnPassantSquare == toSquare) {
                        removePiece(toSquare + 8, PieceType::WHITE_PAWN);
                    } else if (blackEnPassantSquare == toSquare) {
                        removePiece(toSquare - 8, PieceType::BLACK_PAWN);
                    }
                }

                whiteEnPassantSquare = -1;
                blackEnPassantSquare = -1;
            }
        } else {
            whiteEnPassantSquare = -1;
            blackEnPassantSquare = -1;
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

        if (pieceType == PieceType::WHITE_KING) {
            castlingRights &= ~(CastlingRights::WHITE_KINGSIDE | CastlingRights::WHITE_QUEENSIDE);
        } else if (pieceType == PieceType::BLACK_KING) {
            castlingRights &= ~(CastlingRights::BLACK_KINGSIDE | CastlingRights::BLACK_QUEENSIDE);
        }

        if (pieceType == PieceType::WHITE_ROOK) {
            if (fromSquare == Square::A1) {
                castlingRights &= ~CastlingRights::WHITE_QUEENSIDE;
            } else if (fromSquare == Square::H1) {
                castlingRights &= ~CastlingRights::WHITE_KINGSIDE;
            }
        }

        if (pieceType == PieceType::BLACK_ROOK) {
            if (fromSquare == Square::A8) {
                castlingRights &= ~CastlingRights::BLACK_QUEENSIDE;
            } else if (fromSquare == Square::H8) {
                castlingRights &= ~CastlingRights::BLACK_KINGSIDE;
            }
        }

        if (capturedPiece != PieceType::EMPTY) {
            removePiece(toSquare, capturedPiece);
        }

        removePiece(fromSquare, pieceType);
        setPiece(toSquare, pieceType);

        whiteAttackMap = calculateAttackedTilesForColor(PieceColor::WHITE);
        blackAttackMap = calculateAttackedTilesForColor(PieceColor::BLACK);
    }

    void Bitboard::unmakeMove() {
        if (undoStack.empty()) {
            std::cout << "Unmake move: Undo stack is empty!" << std::endl;
            return;
        }

        UndoData undoData = undoStack.top();
        undoStack.pop();

        for (int i = 0; i < 12; i++) {
            pieceBB[i] = undoData.pieceBB[i];
        }

        whiteBB = undoData.whiteBB;
        blackBB = undoData.blackBB;
        occupiedBB = undoData.occupiedBB;
        whiteEnPassantSquare = undoData.whiteEnPassantSquare;
        blackEnPassantSquare = undoData.blackEnPassantSquare;
        castlingRights = undoData.castlingRights;
        whiteAttackMap = undoData.whiteAttackMap;
        blackAttackMap = undoData.blackAttackMap;
    }

    int Bitboard::getBlackEnPassantSquare() const {
        return blackEnPassantSquare;
    }

    int Bitboard::getWhiteEnPassantSquare() const {
        return whiteEnPassantSquare;
    }

    bool Bitboard::setFromFEN(const std::string &fen) {
        int index = 63;
        int spaces = 0;

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
                    setPieceFromFENChar(character, index);
                    index--;
                    continue;
                } else {
                    std::cout << "Invalid character!" << std::endl;
                    return false;
                }
            }

            /*if (spaces == 1) {
                gameState = character == 'w' ? GameState::WHITE_TURN : GameState::BLACK_TURN;
                continue;
            }

            if (spaces == 2) {
                if (character == '-') {
                    continue;
                }

                if (character == 'K') {
                    TileLocation whiteRookLoc = getPiecePosition(WHITE_H_ROOK_ID);
                    Piece* piece = getTileUnsafe(whiteRookLoc.x, whiteRookLoc.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                if (character == 'Q') {
                    TileLocation whiteQueenLocation = getPiecePosition(WHITE_A_ROOK_ID);
                    Piece* piece = getTileUnsafe(whiteQueenLocation.x, whiteQueenLocation.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                if (character == 'k') {
                    TileLocation blackKingLocation = getPiecePosition(BLACK_A_ROOK_ID);
                    Piece* piece = getTileUnsafe(blackKingLocation.x, blackKingLocation.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                if (character == 'q') {
                    TileLocation blackQueenLocation = getPiecePosition(BLACK_H_ROOK_ID);
                    Piece* piece = getTileUnsafe(blackQueenLocation.x, blackQueenLocation.y)->getPiece();
                    piece->setHasMoved(false);
                    continue;
                }

                continue;
            }*/
        }

        return true;
    }

    PieceColor Bitboard::getPieceColor(PieceType type) {
        if (type % 2 == 0) {
            return PieceColor::WHITE;
        } else {
            return PieceColor::BLACK;
        }
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

        for (Move move : availableMoves) {
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

    uint64_t Bitboard::calculateAttackedTilesForColor(PieceColor color) {
        uint64_t attacks = 0;

        if (color == PieceColor::WHITE) {
            attacks |= getWhitePawnAttacks(getPieceBoard(PieceType::WHITE_PAWN));
            attacks |= calculateKnightAttacks(getPieceBoard(PieceType::WHITE_KNIGHT));
            attacks |= getBishopAttacks(getPieceBoard(PieceType::WHITE_BISHOP));
            attacks |= getRookAttacks(getPieceBoard(PieceType::WHITE_ROOK));
            attacks |= getQueenAttacks(getPieceBoard(PieceType::WHITE_QUEEN));
            attacks |= calculateKingAttacks(getPieceBoard(PieceType::WHITE_KING));
        } else {
            attacks |= getBlackPawnAttacks(getPieceBoard(PieceType::BLACK_PAWN));
            attacks |= calculateKnightAttacks(getPieceBoard(PieceType::BLACK_KNIGHT));
            attacks |= getBishopAttacks(getPieceBoard(PieceType::BLACK_BISHOP));
            attacks |= getRookAttacks(getPieceBoard(PieceType::BLACK_ROOK));
            attacks |= getQueenAttacks(getPieceBoard(PieceType::BLACK_QUEEN));
            attacks |= calculateKingAttacks(getPieceBoard(PieceType::BLACK_KING));
        }

        return attacks;
    }

    bool Bitboard::isKingInCheck(PieceColor color) {
        uint64_t kingBB = getPieceBoard(color == PieceColor::WHITE ? PieceType::WHITE_KING : PieceType::BLACK_KING);
        uint64_t kingLocation = bitscanForward(kingBB);
        uint64_t attacks = getAttackedTilesForColor(getOppositeColor(color));

        return attacks & (1ULL << kingLocation);
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
        if (color == PieceColor::WHITE) {
            return getWhiteBoard();
        } else {
            return getBlackBoard();
        }
    }

    Chess::PieceColor Bitboard::getOppositeColor(PieceColor color) {
        if (color == PieceColor::WHITE) {
            return PieceColor::BLACK;
        } else {
            return PieceColor::WHITE;
        }
    }

    PieceType Bitboard::getPieceOnSquare(int square) {
        int bbAmount = sizeof(pieceBB) / sizeof(uint64_t);

        for (int i = 0; i < bbAmount; i++) {
            uint64_t bb = pieceBB[i];

            if (bb & (1ULL << square)) {
                return static_cast<PieceType>(i);
            }
        }

        return PieceType::EMPTY;
    }

    bool Bitboard::isWinner(PieceColor color) {
        if (!isKingInCheck(getOppositeColor(color))) {
            return false;
        }

        std::vector<Move> moves = Chess::generatePseudoLegalMoves(*this, color);

        for (Move move : moves) {
            makeMove(move.fromSquare, move.toSquare, move.pieceType);
            bool isKingInCheck = this->isKingInCheck(getOppositeColor(color));
            unmakeMove();

            if (!isKingInCheck) {
                return false;
            }
        }

        return true;
    }

    void Bitboard::handleCastling(Square rookSquare, Square kingSquare, PieceType rookType, PieceType kingType) {
        static const std::map<Square, std::pair<Square, Square>> destinations = {
                { Square::A1, { Square::C1, Square::D1 } },
                { Square::H1, { Square::G1, Square::F1 } },
                { Square::A8, { Square::C8, Square::D8 } },
                { Square::H8, { Square::G8, Square::F8 } }
        };

        auto it = destinations.find(rookSquare);

        if (it == destinations.end()) {
            return;
        }

        removePiece(rookSquare, rookType);
        removePiece(kingSquare, kingType);
        setPiece(it->second.first, kingType);
        setPiece(it->second.second, rookType);

        if (kingType == PieceType::WHITE_KING) {
            castlingRights &= ~(CastlingRights::WHITE_QUEENSIDE | CastlingRights::WHITE_KINGSIDE);
        } else {
            castlingRights &= ~(CastlingRights::BLACK_QUEENSIDE | CastlingRights::BLACK_KINGSIDE);
        }
    }

    uint64_t Bitboard::getAttackedTilesForColor(PieceColor color) {
        if (color == PieceColor::WHITE) {
            return getWhiteAttacksBB();
        } else {
            return getBlackAttacksBB();
        }
    }

    uint64_t Bitboard::getWhiteAttacksBB() {
        return whiteAttackMap;
    }

    uint64_t Bitboard::getBlackAttacksBB() {
        return blackAttackMap;
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
        return (b >>  6ULL) & NOT_AB_FILE;
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

    unsigned long bitscanForward(uint64_t b) {
        unsigned long index;
        _BitScanForward64(&index, b);
        return index;
    }

    unsigned long bitscanReverse(uint64_t b) {
        unsigned long index;

        _BitScanReverse64(&index, b);
        return index;
    }

    uint64_t soutOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        flood |= pieceBB = (pieceBB >> 8ULL) & empty;
        flood |= pieceBB = (pieceBB >> 8ULL) & empty;
        flood |= pieceBB = (pieceBB >> 8ULL) & empty;
        flood |= pieceBB = (pieceBB >> 8ULL) & empty;
        flood |= pieceBB = (pieceBB >> 8ULL) & empty;
        flood |= (pieceBB >> 8ULL) & empty;
        return flood >> 8ULL;
    }

    uint64_t nortOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        flood |= pieceBB = (pieceBB << 8ULL) & empty;
        flood |= pieceBB = (pieceBB << 8ULL) & empty;
        flood |= pieceBB = (pieceBB << 8ULL) & empty;
        flood |= pieceBB = (pieceBB << 8ULL) & empty;
        flood |= pieceBB = (pieceBB << 8ULL) & empty;
        flood |= (pieceBB << 8ULL) & empty;
        return flood << 8ULL;
    }

    uint64_t eastOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        empty &= NOT_A_FILE;
        flood |= pieceBB = (pieceBB << 1ULL) & empty;
        flood |= pieceBB = (pieceBB << 1ULL) & empty;
        flood |= pieceBB = (pieceBB << 1ULL) & empty;
        flood |= pieceBB = (pieceBB << 1ULL) & empty;
        flood |= pieceBB = (pieceBB << 1ULL) & empty;
        flood |= (pieceBB << 1ULL) & empty;
        return (flood << 1ULL) & NOT_A_FILE;
    }

    uint64_t noEaOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        empty &= NOT_A_FILE;
        flood |= pieceBB = (pieceBB << 9ULL) & empty;
        flood |= pieceBB = (pieceBB << 9ULL) & empty;
        flood |= pieceBB = (pieceBB << 9ULL) & empty;
        flood |= pieceBB = (pieceBB << 9ULL) & empty;
        flood |= pieceBB = (pieceBB << 9ULL) & empty;
        flood |= (pieceBB << 9ULL) & empty;
        return (flood << 9ULL) & NOT_A_FILE;
    }

    uint64_t soEaOccl(uint64_t pieceBB, uint64_t empty) {
        uint64_t flood = pieceBB;
        empty &= NOT_A_FILE;
        flood |= pieceBB = (pieceBB >> 7ULL) & empty;
        flood |= pieceBB = (pieceBB >> 7ULL) & empty;
        flood |= pieceBB = (pieceBB >> 7ULL) & empty;
        flood |= pieceBB = (pieceBB >> 7ULL) & empty;
        flood |= pieceBB = (pieceBB >> 7ULL) & empty;
        flood |= (pieceBB >> 7ULL) & empty;
        return (flood >> 7ULL) & NOT_A_FILE;
    }

    uint64_t westOccl(uint64_t rooks, uint64_t empty) {
        uint64_t flood = rooks;
        empty &= NOT_H_FILE;
        flood |= rooks = (rooks >> 1ULL) & empty;
        flood |= rooks = (rooks >> 1ULL) & empty;
        flood |= rooks = (rooks >> 1ULL) & empty;
        flood |= rooks = (rooks >> 1ULL) & empty;
        flood |= rooks = (rooks >> 1ULL) & empty;
        flood |= (rooks >> 1ULL) & empty;
        return (flood >> 1ULL) & NOT_H_FILE;
    }

    uint64_t soWeOccl(uint64_t bishops, uint64_t empty) {
        uint64_t flood = bishops;
        empty &= NOT_H_FILE;
        flood |= bishops = (bishops >> 9ULL) & empty;
        flood |= bishops = (bishops >> 9ULL) & empty;
        flood |= bishops = (bishops >> 9ULL) & empty;
        flood |= bishops = (bishops >> 9ULL) & empty;
        flood |= bishops = (bishops >> 9ULL) & empty;
        flood |= (bishops >> 9ULL) & empty;
        return (flood >> 9ULL) & NOT_H_FILE;
    }

    uint64_t noWeOccl(uint64_t bishops, uint64_t empty) {
        uint64_t flood = bishops;
        empty &= NOT_H_FILE;
        flood |= bishops = (bishops << 7ULL) & empty;
        flood |= bishops = (bishops << 7ULL) & empty;
        flood |= bishops = (bishops << 7ULL) & empty;
        flood |= bishops = (bishops << 7ULL) & empty;
        flood |= bishops = (bishops << 7ULL) & empty;
        flood |= (bishops << 7ULL) & empty;
        return (flood << 7ULL) & NOT_H_FILE;
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