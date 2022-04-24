//
// Created by Danny on 22-3-2022.
//

#include "piece.h"
#include "board.h"

namespace Chess {
    std::vector<std::tuple<int, int>> knightDirections{{-1, 2},
                                                       {1,  2},
                                                       {-1, -2},
                                                       {1,  -2},
                                                       {2,  1},
                                                       {2,  -1},
                                                       {-2, 1},
                                                       {-2, -1}};

    PieceColor Piece::getColor() {
        return this->color;
    }

    int Piece::getInitialX() const {
        return this->initialX;
    }

    int Piece::getInitialY() const {
        return this->initialY;
    }

    int Piece::getId() const {
        return this->id;
    }

    bool Piece::getHasMoved() const {
        return this->hasMoved;
    }

    void Piece::setHasMoved(bool moved) {
        this->hasMoved = moved;
    }

    // TODO: remove moves that cause a check
    std::vector<Tile*> Bishop::getLegalMoves(Board* board) {
        std::vector<Tile*> legalMoves{};
        std::vector<Direction> directions{Direction::UP_LEFT, Direction::UP_RIGHT, Direction::DOWN_LEFT,
                                          Direction::DOWN_RIGHT};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        for (Direction direction: directions) {
            std::vector<Tile*> tiles = board->getTilesInDirection(x, y, direction, getColor(), false);

            for (Tile* tile: tiles) {
                legalMoves.push_back(tile);
            }
        }

        return legalMoves;
    }

    std::vector<Tile*> Bishop::getAttackedTiles(Board* board) {
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());
        std::vector<Tile*> attackedTiles{};
        std::vector<Direction> directions{Direction::UP_LEFT, Direction::UP_RIGHT, Direction::DOWN_LEFT,
                                          Direction::DOWN_RIGHT};

        for (Direction direction: directions) {
            std::vector<Tile*> tiles = board->getTilesInDirection(x, y, direction, getColor(), true);

            for (Tile* tile: tiles) {
                attackedTiles.push_back(tile);
            }
        }

        return attackedTiles;
    }

    int Bishop::getBaseWeight() {
        return 350;
    }

    int Bishop::getWeight(Board* board) {
        return 350;
    }

    std::string Bishop::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bB";
        } else {
            return "wB";
        }
    }

    PieceType Bishop::getPieceType() {
        return PieceType::BISHOP;
    }

    std::vector<Tile*> King::getLegalMoves(Board* board) {
        std::vector<Tile*> legalMoves{};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        for (int i = 0; i < 8; i++) {
            auto direction = static_cast<Direction>(i);
            Tile* tile = board->getTileInDirection(x, y, direction);
            PieceColor oppositeColor = getOppositeColor(this->getColor());

            if (tile != nullptr) {
                std::shared_ptr<Piece> piece = tile->getPiece();

                if ((tile->getPiece() == nullptr || tile->getPiece()->getColor() != this->getColor()) &&
                    tile->getAttackersByColor(oppositeColor).empty()) {
                    legalMoves.push_back(tile);
                }
            }
        }

        return legalMoves;
    }

    std::vector<Tile*> King::getAttackedTiles(Board* board) {
        std::vector<Tile*> attackedTiles{};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        for (int i = 0; i < 8; i++) {
            auto direction = static_cast<Direction>(i);
            Tile* tile = board->getTileInDirection(x, y, direction);
            PieceColor oppositeColor = getOppositeColor(this->getColor());

            if (tile != nullptr) {
                std::shared_ptr<Piece> piece = tile->getPiece();

                if (tile->getAttackersByColor(oppositeColor).empty()) {
                    attackedTiles.push_back(tile);
                }
            }
        }

        return attackedTiles;
    }

    int King::getBaseWeight() {
        return 10000;
    }

    int King::getWeight(Board* board) {
        return 10000;
    }

    std::string King::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bK";
        } else {
            return "wK";
        }
    }

    bool King::isInCheck(Board* board) {
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());
        Tile* tile = board->getTile(x, y);
        PieceColor oppositeColor = getOppositeColor(this->getColor());

        if (tile->getAttackersByColor(oppositeColor).empty()) {
            return false;
        }

        return true;
    }

    bool King::isCheckMate(Board* board) {
        if (!isInCheck(board)) {
            return false;
        }

        bool isMate = true;
        std::vector<std::shared_ptr<Piece>> pieces = board->getPieces(this->getColor());

        for (const std::shared_ptr<Piece> &piece: pieces) {
            std::vector<Tile*> legalMoves = piece->getLegalMoves(board);

            if (!piece->getLegalMoves(board).empty()) {
                isMate = false;
                break;
            }
        }

        return isMate;
    }

    PieceType King::getPieceType() {
        return PieceType::KING;
    }

    std::vector<Tile*> Knight::getLegalMoves(Board* board) {
        std::vector<Tile*> legalMoves{};

        for (std::tuple<int, int> direction: knightDirections) {
            Tile* tile = board->getTile(std::get<0>(direction), std::get<1>(direction));

            if (tile != nullptr) {
                std::shared_ptr<Piece> piece = tile->getPiece();

                if (piece == nullptr || piece->getColor() != this->getColor()) {
                    legalMoves.push_back(tile);
                }
            }
        }

        return legalMoves;
    }

    std::vector<Tile*> Knight::getAttackedTiles(Board* board) {
        std::vector<Tile*> attackedTiles{};

        for (std::tuple<int, int> direction: knightDirections) {
            Tile* tile = board->getTile(std::get<0>(direction), std::get<1>(direction));

            if (tile != nullptr) {
                attackedTiles.push_back(tile);
            }
        }

        return attackedTiles;
    }

    int Knight::getBaseWeight() {
        return 350;
    }

    int Knight::getWeight(Board* board) {
        return 350;
    }

    std::string Knight::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bN";
        } else {
            return "wN";
        }
    }

    PieceType Knight::getPieceType() {
        return PieceType::KNIGHT;
    }

    std::vector<Tile*> Pawn::getLegalMoves(Board* board) {
        std::vector<Tile*> legalMoves{};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());
        Tile* frontTile = board->getTile(x, y + (this->getColor() == PieceColor::BLACK ? 1 : -1));

        if (frontTile != nullptr && frontTile->getPiece() == nullptr) {
            legalMoves.push_back(frontTile);

            if (!this->getHasMoved()) {
                Tile* secondFrontTile = board->getTile(x, y + (this->getColor() == PieceColor::BLACK ? 2 : -2));

                if (secondFrontTile != nullptr && frontTile->getPiece() == nullptr) {
                    legalMoves.push_back(secondFrontTile);
                }
            }
        }

        for (Tile* attackedTile: this->getAttackedTiles(board)) {
            if (attackedTile->getEnPassantTarget() != nullptr
            || (attackedTile->getPiece() != nullptr && attackedTile->getPiece()->getColor() != this->getColor())) {
                legalMoves.push_back(attackedTile);
            }
        }

        return legalMoves;
    }

    std::vector<Tile*> Pawn::getAttackedTiles(Board* board) {
        std::vector<Tile*> attackedTiles{};
        std::vector<Direction> directions;
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        if (this->getColor() == PieceColor::WHITE) {
            directions = {Direction::UP_LEFT, Direction::UP_RIGHT};
        } else {
            directions = {Direction::DOWN_LEFT, Direction::DOWN_RIGHT};
        }


        for (Direction direction: directions) {
            int dx, dy;
            std::tie(dx, dy) = Board::getDirectionValues(direction);
            Tile* tile = board->getTile(x + dx, y + dy);

            if (tile != nullptr) {
                attackedTiles.push_back(tile);
            }
        }

        return attackedTiles;
    }

    int Pawn::getBaseWeight() {
        return 100;
    }

    int Pawn::getWeight(Board* board) {
        return 100;
    }

    std::string Pawn::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bP";
        } else {
            return "wP";
        }
    }

    PieceType Pawn::getPieceType() {
        return PieceType::PAWN;
    }

    std::vector<Tile*> Queen::getLegalMoves(Board* board) {
        std::vector<Tile*> legalMoves{};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        for (int i = 0; i < 8; i++) {
            auto direction = static_cast<Direction>(i);
            int dx, dy;
            std::tie(dx, dy) = Board::getDirectionValues(direction);
            std::vector<Tile*> tiles = board->getTilesInDirection(x, y, direction, this->getColor(), false);

            for (Tile* tile: tiles) {
                legalMoves.push_back(tile);
            }
        }

        return legalMoves;
    }

    std::vector<Tile*> Queen::getAttackedTiles(Board* board) {
        std::vector<Tile*> attackedTiles{};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        for (int i = 0; i < 8; i++) {
            auto direction = static_cast<Direction>(i);
            int dx, dy;
            std::tie(dx, dy) = Board::getDirectionValues(direction);
            std::vector<Tile*> tiles = board->getTilesInDirection(x, y, direction, this->getColor(), true);

            for (Tile* tile: tiles) {
                attackedTiles.push_back(tile);
            }
        }

        return attackedTiles;
    }

    int Queen::getBaseWeight() {
        return 1000;
    }

    int Queen::getWeight(Board* board) {
        return 1000;
    }

    std::string Queen::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bQ";
        } else {
            return "wQ";
        }
    }

    PieceType Queen::getPieceType() {
        return PieceType::QUEEN;
    }

    std::vector<Tile*> Rook::getLegalMoves(Board* board) {
        std::vector<Tile*> legalMoves{};
        std::vector<Direction> directions{Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());

        for (Direction direction: directions) {
            std::vector<Tile*> tiles = board->getTilesInDirection(x, y, direction, getColor(), false);

            for (Tile* tile: tiles) {
                legalMoves.push_back(tile);
            }
        }

        return legalMoves;
    }

    std::vector<Tile*> Rook::getAttackedTiles(Board* board) {
        int x, y;
        std::tie(x, y) = board->getPiecePosition(this->getId());
        std::vector<Tile*> attackedTiles{};
        std::vector<Direction> directions{Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};

        for (Direction direction: directions) {
            std::vector<Tile*> tiles = board->getTilesInDirection(x, y, direction, getColor(), true);

            for (Tile* tile: tiles) {
                attackedTiles.push_back(tile);
            }
        }

        return attackedTiles;
    }

    int Rook::getBaseWeight() {
        return 525;
    }

    int Rook::getWeight(Board* board) {
        return 525;
    }

    std::string Rook::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bR";
        } else {
            return "wR";
        }
    }

    PieceType Rook::getPieceType() {
        return PieceType::ROOK;
    }
}