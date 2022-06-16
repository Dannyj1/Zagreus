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
        Piece* piece = nullptr;
        Piece* enPassantTarget = nullptr;

    public:
        Tile() {
            this->x = 0;
            this->y = 0;
        }

        Tile(int x, int y) {
            this->x = x;
            this->y = y;
        }

        Tile(Piece* piece, int x, int y) {
            this->piece = piece;
            this->x = x;
            this->y = y;
        }

        [[nodiscard]] Piece* getPiece();

        void setPiece(Piece* piece);

        [[nodiscard]] int getX() const;

        [[nodiscard]] int getY() const;

        [[nodiscard]] Piece* getEnPassantTarget();

        void setEnPassantTarget(Piece* targetPiece);

        std::string getNotation();
    };
}
