//
// Created by Danny on 22-3-2022.
//

#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include "tile.h"
#include "piece.h"

namespace Chess {
    enum Direction {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3,
        UP_LEFT = 4,
        UP_RIGHT = 5,
        DOWN_LEFT = 6,
        DOWN_RIGHT = 7
    };

    struct DirectionValue {
        int dx;
        int dy;
    };

    static const DirectionValue DIRECTION_VALUES[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };

    struct TileLocation {
        int x, y;
    };

    struct Move {
        Tile* tile;
        std::shared_ptr<Piece> piece;

        bool operator==(const Move &a) const {
            return tile->getX() == a.tile->getX() && tile->getY() == a.tile->getY() && piece->getPieceType() == a.piece->getPieceType();
        }
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
    static const std::vector<Direction> BLACK_FIANCHETTO_DIRECTIONS{Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    static const std::vector<Direction> WHITE_FIANCHETTO_DIRECTIONS{Direction::UP, Direction::LEFT, Direction::RIGHT};
    static const std::vector<Direction> BLACK_PAWN_SHIELD_DIRECTIONS{Direction::DOWN, Direction::DOWN_LEFT, Direction::DOWN_RIGHT};
    static const std::vector<Direction> WHITE_PAWN_SHIELD_DIRECTIONS{Direction::UP, Direction::UP_LEFT, Direction::UP_RIGHT};

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
        std::vector<std::pair<std::shared_ptr<Piece>, TileLocation>> originalPositions;
        std::vector<std::pair<std::shared_ptr<Piece>, bool>> hasMoved;
        std::vector<std::tuple<int, int, std::shared_ptr<Piece>>> enPassantTargets;
        GameState gameState;
        uint64_t zobristHash;
        int halfMoveClock;
        Tile* promotionTile = nullptr;
        bool hasBlackCastled = false;
        bool hasWhiteCastled = false;

        UndoData() {
            enPassantTargets.reserve(16);
            originalPositions.reserve(3);
            hasMoved.reserve(3);
        }
    };

    // Pre-allocated vectors for improved performance
    static std::vector<Tile*> attackedTiles{8};
    static std::vector<Tile*> legalTiles{25};

    class Board {
    private:
        Tile board[8][8];
        GameState gameState = WHITE_TURN;
        std::unordered_map<int, TileLocation> piecePositions;
        std::stack<UndoData> undoData;
        uint64_t whiteTimeMsec = 300000;
        uint64_t blackTimeMsec = 300000;
        uint64_t zobristHash = 0;
        uint64_t zobristConstants[781]{};
        int movesMade = 0;
        std::unordered_map<uint64_t, int> moveHistory;
        int halfMoveClock = 0;
        bool hasBlackCastled = false;
        bool hasWhiteCastled = false;

    public:
        Board();

        [[nodiscard]] int getMovesMade() const;

        void setPieceAtPosition(int x, int y, const std::shared_ptr<Piece>& piece);

        void removePieceFromPosition(int x, int y);

        Tile* getTile(int x, int y);

        Tile* getTileInDirection(int x, int y, Direction direction);

        void getTilesInDirection(std::vector<Tile*> &result, int x, int y, Direction direction, PieceColor movingColor,
                                 bool ignoreColor);

        TileLocation getPiecePosition(int id);

        void makeMove(int toX, int toY, std::shared_ptr<Piece> movingPiece);

        void unmakeMove();

        void updateGameState(PieceColor movingColor);

        void print();

        [[nodiscard]] GameState getState() const;

        void setState(GameState state);

        std::vector<std::shared_ptr<Piece>> getPieces();

        std::vector<std::shared_ptr<Piece>> getPieces(PieceColor color);

        bool isDraw();

        void handleKingCastle(int y, const std::shared_ptr<Piece>& king, Tile* fromTile, Tile* toTile, const std::shared_ptr<Piece>& rook);

        void handleEnPassant(const std::shared_ptr<Piece>& movingPiece, Tile* toTile);

        std::vector<Move> getPseudoLegalMoves(PieceColor color);

        [[nodiscard]] PieceColor getMovingColor() const;

        std::vector<Tile*>
        removeMovesCausingCheck(std::vector<Tile*> &moves, const std::shared_ptr<Piece>&movingPiece);

        [[nodiscard]] GameState getGameState() const;

        void setGameState(GameState gameState);

        bool isKingChecked(PieceColor color);

        PieceColor getWinner();

        bool isStalemate(PieceColor color);

        bool makeStrMove(const std::string &move);

        Tile* getTileFromString(const std::string &tile);

        std::string getFEN();

        bool setFromFEN(const std::string& fen);

        std::string getCastlingFEN();

        std::string getEnPassantFEN();

        [[nodiscard]] uint64_t getWhiteTimeMsec() const;

        void setWhiteTimeMsec(uint64_t whiteTimeMsec);

        [[nodiscard]] uint64_t getBlackTimeMsec() const;

        void setBlackTimeMsec(uint64_t blackTimeMsec);

        void setPieceFromFENChar(char character, int x, int y);

        int mvvlva(Tile* tile, PieceColor attackingColor);

        bool isInsufficientMaterial();

        uint64_t getZobristHash() const;

        uint64_t getZobristHashForMove(int toX, int toY, const std::shared_ptr<Piece>& movingPiece);

        void createInitialZobristHash();

        bool orderMoves(const Move &a, const Move &b);

        static bool isInCenter(int x, int y);

        static bool isInExtendedCenter(int x, int y);

        [[nodiscard]] bool getHasBlackCastled() const;

        [[nodiscard]] bool getHasWhiteCastled() const;

        bool hasCastled(PieceColor color);

        bool isTileAttackedByColor(Tile* targetSquare, PieceColor attackingColor);

        static bool canPieceAttackDirection(const std::shared_ptr<Piece>& piece, Direction direction);
    };
}
