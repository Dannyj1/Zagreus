//
// Created by Danny on 22-3-2022.
//

#include "tile.h"

#include <utility>

namespace Chess {
    std::shared_ptr<Piece> Tile::getPiece() {
        return this->piece;
    }

    void Tile::setPiece(std::shared_ptr<Piece> newPiece) {
        this->piece = std::move(newPiece);
    }

    int Tile::getX() const {
        return this->x;
    }

    int Tile::getY() const {
        return this->y;
    }

    void Tile::clearAttackers() {
        this->attackers.clear();
    }

    void Tile::addAttacker(std::shared_ptr<Piece> attacker) {
        this->attackers.push_back(attacker);
    }

    std::vector<std::shared_ptr<Piece>> Tile::getAttackers() {
        return this->attackers;
    }

    std::vector<std::shared_ptr<Piece>> Tile::getAttackersByColor(PieceColor color) {
        std::vector<std::shared_ptr<Piece>> attackersByColor;

        for (const auto &attacker: this->attackers) {
            if (attacker->getColor() == color) {
                attackersByColor.push_back(attacker);
            }
        }

        return attackersByColor;
    }

    std::shared_ptr<Piece> Tile::getEnPassantTarget() {
        return this->enPassantTarget;
    }

    void Tile::setEnPassantTarget(std::shared_ptr<Piece> targetPiece) {
        this->enPassantTarget = std::move(targetPiece);
    }
}