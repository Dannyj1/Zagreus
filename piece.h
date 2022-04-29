//
// Created by Danny on 22-3-2022.
//

#pragma once

#include <vector>
#include <memory>
#include <string>

namespace Chess {
    class Tile;

    class Board;

    enum PieceColor {
        WHITE,
        BLACK,
        NONE
    };

    enum PieceType {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    inline PieceColor getOppositeColor(PieceColor color) {
        if (color == PieceColor::WHITE) {
            return PieceColor::BLACK;
        } else {
            return PieceColor::WHITE;
        }
    }

    class Piece : public std::enable_shared_from_this<Piece> {
    private:
        PieceColor color;
        int initialX;
        int initialY;
        int id;
        bool hasMoved = false;

    public:
        explicit Piece(PieceColor color, int initialX, int initialY, int id) {
            this->id = id;
            this->color = color;
            this->initialX = initialX;
            this->initialY = initialY;
        }

        virtual ~Piece() = default;

        [[nodiscard]] int getId() const;

        PieceColor getColor();

        [[nodiscard]] int getInitialX() const;

        [[nodiscard]] int getInitialY() const;

        [[nodiscard]] bool getHasMoved() const;

        void setHasMoved(bool hasMoved);

        virtual PieceType getPieceType() = 0;

        virtual void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) = 0;

        virtual void getAttackedTiles(std::vector<Tile*>* result, Board* board) = 0;

        virtual void setAttackedTiles(Board* board) = 0;

        virtual int getBaseWeight() = 0;

        virtual int getWeight(Board* board) = 0;

        virtual std::string getSymbol() = 0;

        virtual std::string getFEN() = 0;
    };

    class Bishop : public Piece {
    public:
        explicit Bishop(PieceColor color, int initialX, int initialY, int id) : Piece(color, initialX, initialY, id) {

        }

        PieceType getPieceType() override;

        void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) override;

        void getAttackedTiles(std::vector<Tile*>* result, Board* board) override;

        void setAttackedTiles(Board* board) override;

        int getBaseWeight() override;

        int getWeight(Board* board) override;

        std::string getSymbol() override;

        std::string getFEN() override;
    };

    class King : public Piece {
    public:
        explicit King(PieceColor color, int initialX, int initialY, int id) : Piece(color, initialX, initialY, id) {

        }

        PieceType getPieceType() override;

        void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) override;

        void getAttackedTiles(std::vector<Tile*>* result, Board* board) override;

        void setAttackedTiles(Board* board) override;

        int getBaseWeight() override;

        int getWeight(Board* board) override;

        std::string getSymbol() override;

        bool isChecked(Board* board);

        bool isCheckMate(Board* board);

        std::string getFEN() override;
    };

    class Knight : public Piece {
    public:
        explicit Knight(PieceColor color, int initialX, int initialY, int id) : Piece(color, initialX, initialY, id) {

        }

        PieceType getPieceType() override;

        void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) override;

        void getAttackedTiles(std::vector<Tile*>* result, Board* board) override;

        void setAttackedTiles(Board* board) override;

        int getBaseWeight() override;

        int getWeight(Board* board) override;

        std::string getSymbol() override;

        std::string getFEN() override;
    };

    class Pawn : public Piece {
    public:
        explicit Pawn(PieceColor color, int initialX, int initialY, int id) : Piece(color, initialX, initialY, id) {

        }

        PieceType getPieceType() override;

        void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) override;

        void getAttackedTiles(std::vector<Tile*>* result, Board* board) override;

        void setAttackedTiles(Board* board) override;

        int getBaseWeight() override;

        int getWeight(Board* board) override;

        std::string getSymbol() override;

        std::string getFEN() override;
    };

    class Queen : public Piece {
    public:
        explicit Queen(PieceColor color, int initialX, int initialY, int id) : Piece(color, initialX, initialY, id) {

        }

        PieceType getPieceType() override;

        void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) override;

        void getAttackedTiles(std::vector<Tile*>* result, Board* board) override;

        void setAttackedTiles(Board* board) override;

        int getBaseWeight() override;

        int getWeight(Board* board) override;

        std::string getSymbol() override;

        std::string getFEN() override;
    };

    class Rook : public Piece {
    public:
        explicit Rook(PieceColor color, int initialX, int initialY, int id) : Piece(color, initialX, initialY, id) {

        }

        PieceType getPieceType() override;

        void getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) override;

        void getAttackedTiles(std::vector<Tile*>* result, Board* board) override;

        void setAttackedTiles(Board* board) override;

        int getBaseWeight() override;

        int getWeight(Board* board) override;

        std::string getSymbol() override;

        std::string getFEN() override;
    };
}