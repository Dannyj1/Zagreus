//
// Created by Danny on 22-3-2022.
//

#include <stdexcept>
#include <iostream>
#include <utility>

#include "board.h"
#include "piece.h"

namespace Chess {
    Board::Board() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                Tile tile = Tile{x, y};

                board[x][y] = tile;
            }
        }

        setPieceAtPosition(4, 0, new King(PieceColor::BLACK, 4, 0, BLACK_KING_ID));
        setPieceAtPosition(4, 7, new King(PieceColor::WHITE, 4, 7, WHITE_KING_ID));
        setPieceAtPosition(0, 0, new Rook(PieceColor::BLACK, 0, 0, BLACK_A_ROOK_ID));
        setPieceAtPosition(0, 7, new Rook(PieceColor::WHITE, 0, 7, WHITE_A_ROOK_ID));
        setPieceAtPosition(7, 0, new Rook(PieceColor::BLACK, 7, 0, BLACK_H_ROOK_ID));
        setPieceAtPosition(7, 7, new Rook(PieceColor::WHITE, 7, 7, WHITE_H_ROOK_ID));

        int id = PIECE_ID_START;
        const std::vector<PieceColor> colors{PieceColor::BLACK, PieceColor::WHITE};
        for (PieceColor color : colors) {
            int y = color == PieceColor::BLACK ? 0 : 7;

            setPieceAtPosition(1, y, new Knight(color, 1, y, id++));
            setPieceAtPosition(2, y, new Bishop(color, 2, y, id++));
            setPieceAtPosition(3, y, new Queen(color, 3, y, id++));
            setPieceAtPosition(5, y, new Bishop(color, 5, y, id++));
            setPieceAtPosition(6, y, new Knight(color, 6, y, id++));

            y += color == PieceColor::BLACK ? 1 : -1;
            for (int x = 0; x < 8; x++) {
                setPieceAtPosition(x, y, new Pawn(color, x, y, id++));
            }
        }
    }

    Tile* Board::getTile(int x, int y) {
        if (x < 0 || x > 7 || y < 0 || y > 7) {
            return nullptr;
        }

        return &board[x][y];
    }

    Tile* Board::getTileInDirection(int x, int y, Direction direction) {
        return getTile(x + DIRECTION_VALUES[direction].dx, y + DIRECTION_VALUES[direction].dy);
    }

    void
    Board::getTilesInDirection(std::vector<Tile*>* result, int x, int y, Direction direction, PieceColor movingColor, bool ignoreColor) {
        Tile* tile = getTileInDirection(x, y, direction);

        while (tile != nullptr) {
            Piece* piece = tile->getPiece();

            if (piece != nullptr) {
                if (ignoreColor || (piece->getPieceType() != PieceType::KING && piece->getColor() != movingColor)) {
                    result->push_back(tile);
                }

                return;
            }

            result->push_back(tile);
            tile = getTileInDirection(tile->getX(), tile->getY(), direction);
        }
    }

    void Board::print() {
        for (int y = 0; y < 8; y++) {
            for (auto &x : this->board) {
                Piece* piece = x[y].getPiece();

                if (piece) {
                    std::cout << piece->getSymbol() << " ";
                } else {
                    std::cout << ".. ";
                }
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    void Board::setPieceAtPosition(int x, int y, Piece* piece) {
        if (piecePositions.contains(piece->getId())) {
            piecePositions.erase(piece->getId());
        }

        piecePositions.insert({piece->getId(), {x, y}});
        this->board[x][y].setPiece(std::move(piece));
    }

    void Board::updateGameState(PieceColor movingColor) {
        std::vector<Piece*> attackers;
        attackers.reserve(32);

        for (auto &x : this->board) {
            for (auto &tile : x) {
                tile.clearAttackers();

                if (tile.getEnPassantTarget() != nullptr && tile.getEnPassantTarget()->getColor() != movingColor) {
                    tile.setEnPassantTarget(nullptr);
                }

                if (tile.getPiece()) {
                    attackers.push_back(tile.getPiece());
                }
            }
        }

        for (Piece* piece : attackers) {
            piece->setAttackedTiles(this);
        }
    }

    std::vector<Tile*> Board::removeMovesCausingCheck(const std::vector<Tile*> &moves, Piece*movingPiece) {
        std::vector<Tile*> result;
        result.reserve(moves.size());

        for (Tile* move : moves) {
            makeMove(move->getX(), move->getY(), movingPiece);

            if (!isKingChecked(movingPiece->getColor())) {
                result.push_back(move);
            }

            unmakeMove();
        }

        return result;
    }

    void Board::makeMove(int toX, int toY, Piece* movingPiece) {
        TileLocation fromLoc = piecePositions.at(movingPiece->getId());
        Tile* fromTile = getTile(fromLoc.x, fromLoc.y);
        Tile* toTile = getTile(toX, toY);
        Piece* toPiece = toTile->getPiece();

        if (movingPiece->getPieceType() == PieceType::PAWN && fromLoc.x != toX && toTile->getEnPassantTarget() != nullptr && toTile->getPiece() == nullptr) {
            handleEnPassant(movingPiece, toTile);
        } else if (toPiece && movingPiece->getPieceType() == PieceType::KING && toPiece->getPieceType() == PieceType::ROOK) {
            UndoData data;
            data.originalPositions.insert({movingPiece, {fromLoc.x, fromLoc.y}});
            data.originalPositions.insert({toPiece, {toX, toY}});
            data.hasMoved.insert({movingPiece->getId(), movingPiece->getHasMoved()});
            data.hasMoved.insert({movingPiece->getId(), toPiece->getHasMoved()});

            for (int i = 0; i < 8; i++) {
                Tile* tile = getTile(i, 2);
                Tile* tile2 = getTile(i, 5);

                data.enPassantTargets.insert({{tile->getX(), tile->getY()}, tile->getEnPassantTarget()});
                data.enPassantTargets.insert({{tile2->getX(), tile2->getY()}, tile2->getEnPassantTarget()});
            }

            undoData.push(data);

            handleKingCastle(toY, movingPiece, fromTile, toTile, toPiece);
        } else {
            UndoData data;
            data.originalPositions.insert({movingPiece, {fromLoc.x, fromLoc.y}});
            data.hasMoved.insert({movingPiece->getId(), movingPiece->getHasMoved()});

            if (toPiece) {
                data.captures.insert({toPiece, {toX, toY}});
                data.hasMoved.insert({toPiece->getId(), toPiece->getHasMoved()});
                piecePositions.erase(toPiece->getId());
            }

            for (int i = 0; i < 8; i++) {
                Tile* tile = getTile(i, 2);
                Tile* tile2 = getTile(i, 5);

                data.enPassantTargets.insert({{tile->getX(), tile->getY()}, tile->getEnPassantTarget()});
                data.enPassantTargets.insert({{tile2->getX(), tile2->getY()}, tile2->getEnPassantTarget()});
            }

            if (movingPiece->getPieceType() == PieceType::PAWN) {
                // TODO: handle other promotions(Field in pawn class?)
                // TODO: will be problematic whhen undoing
                if (toY == 0 || toY == 7) {
                    auto* queen = new Queen(movingPiece->getColor(), toX, toY, movingPiece->getId());
                    movingPiece = queen;
                }

                if (std::abs(fromLoc.y - toY) == 2) {
                    int enPassantY = movingPiece->getColor() == PieceColor::WHITE ? 5 : 2;
                    Tile* enPassantTile = getTile(toX, enPassantY);

                    enPassantTile->setEnPassantTarget(movingPiece);
                }
            }

            undoData.push(data);
            fromTile->setPiece(nullptr);
            movingPiece->setHasMoved(true);
            toTile->setPiece(movingPiece);
            piecePositions.erase(movingPiece->getId());
            piecePositions.insert({movingPiece->getId(), {toX, toY}});
        }

        updateGameState(movingPiece->getColor());
    }

    void Board::handleEnPassant(Piece*movingPiece, Tile* toTile) {
        Piece* enPassantTarget = toTile->getEnPassantTarget();
        TileLocation fromLoc = piecePositions.at(movingPiece->getId());
        Tile* fromTile = getTile(fromLoc.x, fromLoc.y);
        TileLocation enPassantTargetLoc = piecePositions.at(enPassantTarget->getId());
        Tile* enPassantTargetTile = getTile(enPassantTargetLoc.x, enPassantTargetLoc.y);

        UndoData data;
        data.originalPositions.insert({movingPiece, {fromLoc.x, fromLoc.y}});
        data.captures.insert({enPassantTarget, {enPassantTargetLoc.x, enPassantTargetLoc.y}});
        data.hasMoved.insert({movingPiece->getId(), movingPiece->getHasMoved()});
        data.hasMoved.insert({enPassantTarget->getId(), enPassantTarget->getHasMoved()});

        for (int i = 0; i < 8; i++) {
            Tile* tile = getTile(i, 2);
            Tile* tile2 = getTile(i, 5);

            data.enPassantTargets.insert({{tile->getX(), tile->getY()}, tile->getEnPassantTarget()});
            data.enPassantTargets.insert({{tile2->getX(), tile2->getY()}, tile2->getEnPassantTarget()});
        }

        undoData.push(data);

        piecePositions.erase(enPassantTarget->getId());
        fromTile->setPiece(nullptr);
        enPassantTargetTile->setPiece(nullptr);
        toTile->setEnPassantTarget(nullptr);
        toTile->setPiece(movingPiece);
        piecePositions.erase(movingPiece->getId());
        piecePositions.insert({movingPiece->getId(), {toTile->getX(), toTile->getY()}});
    }

    void Board::handleKingCastle(int y, Piece* &movingPiece, Tile* fromTile, Tile* toTile, Piece* &toPiece) {
        // movingPiece is king, toPiece is rook
        TileLocation rookLoc = piecePositions.at(toPiece->getId());
        int castleRookX;
        int castleKingX;

        if (rookLoc.x == 0) {
            castleRookX = 3;
            castleKingX = 2;
        } else {
            castleRookX = 5;
            castleKingX = 6;
        }

        Tile* castleRookTile = getTile(castleRookX, y);
        Tile* castleKingTile = getTile(castleKingX, y);
        piecePositions.erase(movingPiece->getId());
        piecePositions.erase(toPiece->getId());
        piecePositions.insert({movingPiece->getId(), {castleKingX, y}});
        piecePositions.insert({toPiece->getId(), {castleRookX, y}});
        fromTile->setPiece(nullptr);
        toTile->setPiece(nullptr);
        movingPiece->setHasMoved(true);
        toPiece->setHasMoved(true);
        castleKingTile->setPiece(movingPiece);
        castleRookTile->setPiece(toPiece);
    }

    TileLocation Board::getPiecePosition(int id) {
        if (!piecePositions.contains(id)) {
            return {-5, -5};
        }

        return piecePositions.at(id);
    }

    GameState Board::getState() const {
        return gameState;
    }

    void Board::setState(GameState state) {
        Board::gameState = state;
    }

    std::vector<Piece*> Board::getPieces(PieceColor color) {
        std::vector<Piece*> pieces;
        pieces.reserve(16);

        for (auto &x : this->board) {
            for (auto tile : x) {
                Piece* piece = tile.getPiece();

                if (piece && piece->getColor() == color) {
                    pieces.push_back(piece);
                }
            }
        }

        return pieces;
    }

    bool Board::isDraw() {
        PieceColor color;

        if (getState() == GameState::WHITE_TURN) {
            color = PieceColor::WHITE;
        } else if (getState() == GameState::BLACK_TURN) {
            color = PieceColor::BLACK;
        } else {
            return false;
        }

        std::vector<Move> legalMoves = getLegalMoves(color, false);

        if (legalMoves.empty()) {
            return true;
        }

        // TODO: implement other draw types (repetition, fifty-move, insufficient material)

        return false;
    }

    // TODO: handle en passant
    void Board::unmakeMove() {
        if (undoData.empty()) {
            return;
        }

        UndoData move = undoData.top();
        undoData.pop();

        for (std::pair<Piece*, TileLocation> entry : move.originalPositions) {
            Piece* piece = entry.first;
            int toX = entry.second.x;
            int toY = entry.second.y;
            TileLocation loc = getPiecePosition(piece->getId());

            if (loc.x != -5 && loc.y != -5) {
                getTile(loc.x, loc.y)->setPiece(nullptr);
            }

            piece->setHasMoved(move.hasMoved.at(piece->getId()));
            getTile(toX, toY)->setPiece(piece);
            piecePositions.erase(piece->getId());
            piecePositions.insert({piece->getId(), {toX, toY}});
        }

        for (std::pair<Piece*, TileLocation> entry : move.captures) {
            Piece* piece = entry.first;
            int toX = entry.second.x;
            int toY = entry.second.y;

            Tile* tile = getTile(toX, toY);
            piece->setHasMoved(move.hasMoved.at(piece->getId()));
            tile->setPiece(piece);
            piecePositions.insert({piece->getId(), {toX, toY}});
        }

        for (std::pair<std::pair<int, int>, Piece*> entry : move.enPassantTargets) {
            int enPassantX = entry.first.first;
            int enPassantY = entry.first.second;
            Piece* piece = entry.second;
            Tile* tile = getTile(enPassantX, enPassantY);

            tile->setEnPassantTarget(piece);
        }
    }

    GameState Board::getGameState() const {
        return gameState;
    }

    void Board::setGameState(GameState gameState) {
        this->gameState = gameState;
    }

    bool Board::isKingChecked(PieceColor color) {
        TileLocation loc = getPiecePosition(color == PieceColor::WHITE ? WHITE_KING_ID : BLACK_KING_ID);
        King* king = reinterpret_cast<King*>(getTile(loc.x, loc.y)->getPiece());

        return king->isChecked(this);
    }

    std::vector<Move> Board::getLegalMoves(PieceColor color, bool pseudoLegal) {
        std::vector<Move> legalMoves;
        legalMoves.reserve(64);

        for (Piece* piece : this->getPieces(color)) {
            std::vector<Tile*> moves;
            moves.reserve(15);

            piece->getPseudoLegalMoves(&moves, this);

            if (!pseudoLegal) {
                moves = removeMovesCausingCheck(moves, piece);
            }

            for (Tile* move : moves) {
                legalMoves.push_back({move, piece});
            }
        }

        return legalMoves;
    }

    PieceColor Board::getWinner() {
        for (PieceColor color : {PieceColor::WHITE, PieceColor::BLACK}) {
            TileLocation loc = getPiecePosition(color == PieceColor::WHITE ? WHITE_KING_ID : BLACK_KING_ID);
            King* king = reinterpret_cast<King*>(getTile(loc.x, loc.y)->getPiece());

            if (king->isCheckMate(this)) {
                return getOppositeColor(color);
            }
        }

        return PieceColor::NONE;
    }

    Board::~Board() {
        for (auto & y : board) {
            for (auto tile : y) {
                delete tile.getPiece();
                tile.setPiece(nullptr);
            }
        }
    }

    void Board::setAttackedTilesInDirection(int x, int y, Piece* attackingPiece, Direction direction, PieceColor movingColor, bool ignoreColor) {
        Tile* tile = getTileInDirection(x, y, direction);

        while (tile != nullptr) {
            Piece* piece = tile->getPiece();

            if (piece != nullptr) {
                if (ignoreColor || (piece->getPieceType() != PieceType::KING && piece->getColor() != movingColor)) {
                    tile->addAttacker(attackingPiece);
                }

                return;
            }

            tile->addAttacker(attackingPiece);
            tile = getTileInDirection(tile->getX(), tile->getY(), direction);
        }
    }
}