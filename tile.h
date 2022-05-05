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
        std::shared_ptr<Piece> enPassantTarget = nullptr;

    public:
        Tile() {
            this->x = 0;
            this->y = 0;
        }

        Tile(int x, int y) {
            this->x = x;
            this->y = y;
        }

        Tile(std::shared_ptr<Piece> piece, int x, int y) {
            this->piece = piece;
            this->x = x;
            this->y = y;
        }

        [[nodiscard]] std::shared_ptr<Piece> getPiece();

        void setPiece(std::shared_ptr<Piece> piece);

        [[nodiscard]] int getX() const;

        [[nodiscard]] int getY() const;

        [[nodiscard]] std::shared_ptr<Piece> getEnPassantTarget();

        void setEnPassantTarget(std::shared_ptr<Piece> targetPiece);

        std::string getNotation();
    };
}
