//
// Created by Danny on 22-3-2022.
//

#include "tile.h"

#include <utility>

namespace Chess {
    Piece* Tile::getPiece() {
        return this->piece;
    }

    void Tile::setPiece(Piece* newPiece) {
        this->piece = std::move(newPiece);
    }

    int Tile::getX() const {
        return this->x;
    }

    int Tile::getY() const {
        return this->y;
    }

    Piece* Tile::getEnPassantTarget() {
        return this->enPassantTarget;
    }

    void Tile::setEnPassantTarget(Piece* targetPiece) {
        this->enPassantTarget = std::move(targetPiece);
    }

    std::string Tile::getNotation() {
        switch (getX()) {
            case 0:
                return "a" + std::to_string(8 - getY());
            case 1:
                return "b" + std::to_string(8 - getY());
            case 2:
                return "c" + std::to_string(8 - getY());
            case 3:
                return "d" + std::to_string(8 - getY());
            case 4:
                return "e" + std::to_string(8 - getY());
            case 5:
                return "f" + std::to_string(8 - getY());
            case 6:
                return "g" + std::to_string(8 - getY());
            case 7:
                return "h" + std::to_string(8 - getY());
        }

        return "error";
    }
}