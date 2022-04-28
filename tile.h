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
        std::vector<Piece*> attackers;
        Piece* enPassantTarget = nullptr;

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

        Tile(Piece* piece, int x, int y) {
            this->piece = std::move(piece);
            this->x = x;
            this->y = y;
            attackers.reserve(32);
        }

        [[nodiscard]] Piece* getPiece();

        void setPiece(Piece* piece);

        [[nodiscard]] int getX() const;

        [[nodiscard]] int getY() const;

        void clearAttackers();

        void addAttacker(Piece* attacker);

        [[nodiscard]] std::vector<Piece*> getAttackers();

        [[nodiscard]] Piece* getEnPassantTarget();

        void setEnPassantTarget(Piece* targetPiece);

        [[nodiscard]] std::vector<Piece*> getAttackersByColor(PieceColor color);

        void removeAttacker(Piece* sharedPtr);
    };
}
