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

    void Tile::addAttacker(const std::shared_ptr<Piece> &attacker) {
        this->attackers.push_back(attacker);
    }

    std::vector<std::shared_ptr<Piece>> Tile::getAttackers() {
        return this->attackers;
    }

    std::vector<std::shared_ptr<Piece>> Tile::getAttackersByColor(PieceColor color) {
        std::vector<std::shared_ptr<Piece>> attackersByColor;
        attackersByColor.reserve(16);

        for (const auto &attacker : this->attackers) {
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

    void Tile::removeAttacker(const std::shared_ptr<Piece> &sharedPtr) {
        for (auto it = this->attackers.begin(); it != this->attackers.end(); ++it) {
            if (*it == sharedPtr) {
                this->attackers.erase(it);
                break;
            }
        }
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