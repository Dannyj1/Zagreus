//
// Created by Danny on 22-3-2022.
//

#pragma once

#include <vector>
#include <map>
#include "tile.h"
#include "piece.h"

namespace Chess {
    enum Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        UP_LEFT,
        UP_RIGHT,
        DOWN_LEFT,
        DOWN_RIGHT
    };

    enum GameState {
        WHITE_TURN,
        BLACK_TURN,
        WHITE_WIN,
        BLACK_WIN,
        DRAW
    };

    template<typename Base, typename T>
    inline bool instanceof(const T *t) {
        return dynamic_cast<const Base *>(t) != nullptr;
    }

    class Board {
    private:
        Tile board[8][8];
        GameState gameState = WHITE_TURN;
        std::map<int, std::tuple<int8_t, int8_t>> piece_positions;

        void setPieceAtPosition(int x, int y, std::shared_ptr<Piece> piece);

    public:
        Board();

        Tile *getTile(int x, int y);

        Tile *getTileInDirection(int x, int y, Direction direction);

        std::vector<Tile *>
        getTilesInDirection(int x, int y, Direction direction, PieceColor movingColor, bool ignoreColor);

        std::tuple<int8_t, int8_t> getPiecePosition(int id);

        void makeMove(int x, int y, std::shared_ptr<Piece> movingPiece);

        void updateGameState();

        void print();

        [[nodiscard]] GameState getState() const;

        void setState(GameState state);

        static std::tuple<int8_t, int8_t> getDirectionValues(Direction direction);

        std::vector<std::shared_ptr<Piece>> getPieces(PieceColor color);

        std::vector<std::shared_ptr<Piece>> getPieces();

        bool isDraw();
    };
}
