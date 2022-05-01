//
// Created by Danny on 22-3-2022.
//

#pragma once

#include <memory>
#include <utility>
#include <vector>
#include "piece.h"

namespace Chess {
    class Piece;

    class Tile {
    private:
        int x = 0;
        int y = 0;
        std::shared_ptr<Piece> piece = nullptr;
        std::vector<std::shared_ptr<Piece>> attackers;
        std::shared_ptr<Piece> enPassantTarget = nullptr;

    public:
        Tile() {
            this->x = 0;
            this->y = 0;
            attackers.reserve(32);
        }

        Tile(int x, int y) {
            this->x = x;
            this->y = y;
            attackers.reserve(32);
        }

        Tile(std::shared_ptr<Piece> piece, int x, int y) {
            this->piece = std::move(piece);
            this->x = x;
            this->y = y;
            attackers.reserve(32);
        }

        [[nodiscard]] std::shared_ptr<Piece> getPiece();

        void setPiece(std::shared_ptr<Piece> piece);

        [[nodiscard]] int getX() const;

        [[nodiscard]] int getY() const;

        void clearAttackers();

        void addAttacker(const std::shared_ptr<Piece>& attacker);

        [[nodiscard]] std::vector<std::shared_ptr<Piece>> getAttackers();

        [[nodiscard]] std::shared_ptr<Piece> getEnPassantTarget();

        void setEnPassantTarget(std::shared_ptr<Piece> targetPiece);

        [[nodiscard]] std::vector<std::shared_ptr<Piece>> getAttackersByColor(PieceColor color);

        void removeAttacker(const std::shared_ptr<Piece>& sharedPtr);

        std::string getNotation();
    };
}
