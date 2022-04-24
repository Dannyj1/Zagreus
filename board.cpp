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
        for (int x = 0; x < 8; x++) {
            for (int y = 0; x < 8; x++) {
                Tile tile = Tile{x, y};
            }
        }

        int id = 0;
        const std::vector<PieceColor> colors{PieceColor::BLACK, PieceColor::WHITE};
        for (PieceColor color: colors) {
            int y = color == PieceColor::BLACK ? 0 : 7;

            setPieceAtPosition(0, y, std::make_shared<Rook>(color, 0, y, id++));
            setPieceAtPosition(1, y, std::make_shared<Knight>(color, 1, y, id++));
            setPieceAtPosition(2, y, std::make_shared<Bishop>(color, 2, y, id++));
            setPieceAtPosition(3, y, std::make_shared<Queen>(color, 3, y, id++));
            setPieceAtPosition(4, y, std::make_shared<King>(color, 4, y, id++));
            setPieceAtPosition(5, y, std::make_shared<Bishop>(color, 5, y, id++));
            setPieceAtPosition(6, y, std::make_shared<Knight>(color, 6, y, id++));
            setPieceAtPosition(7, y, std::make_shared<Rook>(color, 7, y, id++));

            y += color == PieceColor::BLACK ? 1 : -1;
            for (int x = 0; x < 8; x++) {
                setPieceAtPosition(x, y, std::make_shared<Pawn>(color, x, y, id++));
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
        int dx, dy;
        std::tie(dx, dy) = getDirectionValues(direction);

        return getTile(x + dx, y + dy);
    }

    std::vector<Tile*>
    Board::getTilesInDirection(int x, int y, Direction direction, PieceColor movingColor, bool ignoreColor) {
        std::vector<Tile*> result{};
        Tile* tile = getTileInDirection(x, y, direction);

        while (tile != nullptr) {
            std::shared_ptr<Piece> piece = tile->getPiece();

            if (piece != nullptr) {
                if (piece->getColor() != movingColor || ignoreColor) {
                    result.push_back(tile);
                }

                return result;
            }

            result.push_back(tile);
            tile = getTileInDirection(tile->getX(), tile->getY(), direction);
        }

        return result;
    }

    void Board::print() {
        for (int y = 0; y < 8; y++) {
            for (auto &x: this->board) {
                std::shared_ptr<Piece> piece = x[y].getPiece();

                if (piece) {
                    std::cout << piece->getSymbol() << " ";
                } else {
                    std::cout << ".. ";
                }
            }

            std::cout << std::endl;
        }
    }

    void Board::setPieceAtPosition(int x, int y, std::shared_ptr<Piece> piece) {
        if (piece_positions.contains(piece->getId())) {
            piece_positions.erase(piece->getId());
        }

        piece_positions.insert({piece->getId(), std::make_tuple(x, y)});
        this->board[x][y].setPiece(std::move(piece));
    }

    void Board::updateGameState() {
        for (auto &x: this->board) {
            for (auto &tile: x) {
                tile.clearAttackers();
            }
        }

        for (auto &x: this->board) {
            for (auto &tile: x) {
                std::shared_ptr<Piece> piece = tile.getPiece();

                if (piece) {
                    std::vector<Tile*> attackedTiles = piece->getAttackedTiles(this);

                    for (auto attackedTile: attackedTiles) {
                        attackedTile->addAttacker(piece);
                    }
                }
            }
        }

        // TODO: check for draw
        for (std::shared_ptr<Piece> piece: this->getPieces()) {
            if (piece->getPieceType() == PieceType::KING) {
                std::shared_ptr<King> king = std::dynamic_pointer_cast<King>(piece);

                if (king->isCheckMate(this)) {
                    if (king->getColor() == PieceColor::WHITE) {
                        this->gameState = GameState::BLACK_WIN;
                    } else {
                        this->gameState = GameState::WHITE_WIN;
                    }
                }
            }
        }

        if (isDraw(this)) {
            this->gameState = GameState::DRAW;
        }
    }

    // TODO: castling, en passant, promotion
    void Board::makeMove(int x, int y, std::shared_ptr<Piece> movingPiece) {
        int fromX, fromY;
        std::tie(fromX, fromY) = piece_positions.at(movingPiece->getId());
        Tile* fromTile = getTile(fromX, fromY);
        Tile* toTile = getTile(x, y);
        std::shared_ptr<Piece> toPiece = toTile->getPiece();

        piece_positions.erase(movingPiece->getId());
        piece_positions.insert({movingPiece->getId(), {x, y}});


        if (toPiece) {
            piece_positions.erase(toPiece->getId());
        }

        if (movingPiece->getPieceType() == PieceType::PAWN) {
            if (toTile->getEnPassantTarget() != nullptr) {
                int enPassantX, enPassantY;
                std::shared_ptr<Piece> enPassantTarget = toTile->getEnPassantTarget();
                std::tie(enPassantX, enPassantY) = piece_positions.at(enPassantTarget->getId());
                Tile* enPassantTile = getTile(enPassantX, enPassantY);

                piece_positions.erase(enPassantTarget->getId());
                enPassantTile->setPiece(nullptr);
            }

            if (!movingPiece->getHasMoved() && std::abs(fromX - x) == 2) {
                Direction direction = movingPiece->getColor() == PieceColor::WHITE ? Direction::UP : Direction::DOWN;
                Tile* enPassantTile = getTileInDirection(fromX, fromY, direction);

                enPassantTile->setEnPassantTarget(movingPiece);
            }
        }

/*        if (movingPiece->getPieceType() == PieceType::PAWN && (y == 0 || y == 7)) {

        }*/

        fromTile->setPiece(nullptr);
        movingPiece->setHasMoved(true);
        toTile->setPiece(std::move(movingPiece));
        updateGameState();
    }

    std::tuple<int8_t, int8_t> Board::getDirectionValues(Direction direction) {
        switch (direction) {
            case UP:
                return {0, -1};
            case DOWN:
                return {0, 1};
            case LEFT:
                return {-1, 0};
            case RIGHT:
                return {1, 0};
            case UP_LEFT:
                return {-1, -1};
            case UP_RIGHT:
                return {1, -1};
            case DOWN_LEFT:
                return {-1, 1};
            case DOWN_RIGHT:
                return {1, 1};
        }

        return {0, 0};
    }

    std::tuple<int8_t, int8_t> Board::getPiecePosition(int id) {
        return piece_positions.at(id);
    }

    GameState Board::getState() const {
        return gameState;
    }

    void Board::setState(GameState state) {
        Board::gameState = state;
    }

    std::vector<std::shared_ptr<Piece>> Board::getPieces(PieceColor color) {
        std::vector<std::shared_ptr<Piece>> pieces;

        for (auto &x: this->board) {
            for (auto tile: x) {
                std::shared_ptr<Piece> piece = tile.getPiece();

                if (piece && piece->getColor() == color) {
                    pieces.push_back(piece);
                }
            }
        }

        return pieces;
    }

    std::vector<std::shared_ptr<Piece>> Board::getPieces() {
        std::vector<std::shared_ptr<Piece>> pieces;

        for (auto &x: this->board) {
            for (auto tile: x) {
                std::shared_ptr<Piece> piece = tile.getPiece();

                if (piece) {
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

        std::vector<std::shared_ptr<Piece>> pieces = board->getPieces(color);
        for (const std::shared_ptr<Piece> &piece : pieces) {
            if (!piece->getLegalMoves(this).empty()) {
                return true;
            }
        }

        // TODO: implement other draw types (repetition, fifty-move, insufficient material)

        return false;
    }
}