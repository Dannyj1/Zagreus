//
// Created by Danny on 22-3-2022.
//

#pragma once

#include <vector>
#include <map>
#include <stack>
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

    struct TileLocation {
        int x, y;
    };

    struct Move {
        Tile* tile;
        std::shared_ptr<Piece> piece;
    };

    struct DirectionValue {
        int dx;
        int dy;
    };

    static const std::vector<Direction> ALL_DIRECTIONS = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

    static const std::vector<DirectionValue> knightDirections{{-1, 2},
                                                                   {1,  2},
                                                                   {-1, -2},
                                                                   {1,  -2},
                                                                   {2,  1},
                                                                   {2,  -1},
                                                                   {-2, 1},
                                                                   {-2, -1}};

    static const std::vector<Direction> BISHOP_DIRECTIONS{Direction::UP_LEFT, Direction::UP_RIGHT, Direction::DOWN_LEFT,
                                                          Direction::DOWN_RIGHT};
    static const std::vector<Direction> ROOK_DIRECTIONS{Direction::UP, Direction::DOWN, Direction::LEFT,
                                                        Direction::RIGHT};
    static const std::vector<Direction> WHITE_PAWN_ATTACK_DIRECTIONS{Direction::UP_LEFT, Direction::UP_RIGHT};
    static const std::vector<Direction> BLACK_PAWN_ATTACK_DIRECTIONS{Direction::DOWN_LEFT, Direction::DOWN_RIGHT};

    enum GameState {
        WHITE_TURN,
        BLACK_TURN,
        WHITE_WIN,
        BLACK_WIN,
        DRAW
    };

    static const int BLACK_KING_ID = 0;
    static const int WHITE_KING_ID = 1;
    static const int WHITE_A_ROOK_ID = 2;
    static const int WHITE_H_ROOK_ID = 3;
    static const int BLACK_A_ROOK_ID = 4;
    static const int BLACK_H_ROOK_ID = 5;
    static const int PIECE_ID_START = 6;

    class UndoData {
    public:
        std::map<std::shared_ptr<Piece>, TileLocation> originalPositions;
        std::map<int, bool> hasMoved;
        std::map<std::shared_ptr<Piece>, TileLocation> captures;
        std::map<std::pair<int, int>, std::shared_ptr<Piece>> enPassantTargets;
    };

    class Board {
    private:
        Tile board[8][8];
        GameState gameState = WHITE_TURN;
        std::map<int, TileLocation> piecePositions;
        std::vector<TileLocation> enPassantTargets;
        std::stack<UndoData> undoData;

        void setPieceAtPosition(int x, int y, std::shared_ptr<Piece> piece);

    public:
        Board();

        Tile* getTile(int x, int y);

        Tile* getTileInDirection(int x, int y, Direction direction);

        void getTilesInDirection(std::vector<Tile*>* result, int x, int y, Direction direction, PieceColor movingColor,
                                 bool ignoreColor);

        TileLocation getPiecePosition(int id);

        void makeMove(int toX, int toY, std::shared_ptr<Piece> movingPiece);

        void unmakeMove();

        void updateGameState(PieceColor movingColor);

        void print();

        [[nodiscard]] GameState getState() const;

        void setState(GameState state);

        static DirectionValue getDirectionValues(Direction direction);

        std::vector<std::shared_ptr<Piece>> getPieces(PieceColor color);

        bool isDraw();

        void handleKingCastle(int y, std::shared_ptr<Piece> &movingPiece, Tile* fromTile, Tile* toTile,
                              std::shared_ptr<Piece> &toPiece);

        void handleEnPassant(const std::shared_ptr<Piece> &movingPiece, Tile* toTile);

        std::vector<Move> getLegalMoves(PieceColor color, bool pseudoLegal);

        std::vector<Tile*>
        removeMovesCausingCheck(const std::vector<Tile*> &moves, const std::shared_ptr<Piece> &movingPiece);

        [[nodiscard]] GameState getGameState() const;

        void setGameState(GameState gameState);

        bool isKingChecked(PieceColor color);

        PieceColor getWinner();
    };
}
