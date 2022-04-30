//
// Created by Danny on 22-3-2022.
//

#include "piece.h"
#include "board.h"

namespace Chess {
    PieceColor Piece::getColor() {
        return this->color;
    }

    int Piece::getInitialX() const {
        return this->initialX;
    }

    int Piece::getInitialY() const {
        return this->initialY;
    }

    bool Piece::getHasMoved() const {
        return this->hasMoved;
    }

    void Piece::setHasMoved(bool moved) {
        this->hasMoved = moved;
    }

    int Piece::getId() const {
        return id;
    }

    void Bishop::getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : BISHOP_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), false);
        }
    }

    void Bishop::getAttackedTiles(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : BISHOP_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), true);
        }
    }

    void Bishop::setAttackedTiles(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : BISHOP_DIRECTIONS) {
            board->setAttackedTilesInDirection(loc.x, loc.y, this->shared_from_this(), direction, getColor(), true);
        }
    }

    int Bishop::getBaseWeight() {
        return 350;
    }

    int Bishop::getWeight(Board* board) {
        return 350;
    }

    std::string Bishop::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bB";
        } else {
            return "wB";
        }
    }

    PieceType Bishop::getPieceType() {
        return PieceType::BISHOP;
    }

    std::string Bishop::getFEN() {
        if (getColor() == PieceColor::WHITE) {
            return "B";
        } else {
            return "b";
        }
    }

    int Bishop::getMobilityScore(Board* board) {
        int badMoves = 0;
        std::vector<Tile*> pseudoLegalMoves;
        this->getPseudoLegalMoves(&pseudoLegalMoves, board);

        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty() && board->mvvlva(tile, getColor()) < 0) {
                badMoves += 1;
            }
        }

        return pseudoLegalMoves.size() - badMoves;
    }

    void King::getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

            if (tile != nullptr) {
                if ((tile->getPiece() == nullptr || (tile->getPiece()->getPieceType() != PieceType::KING && tile->getPiece()->getColor() != this->getColor()))) {
                    result->push_back(tile);
                }
            }
        }

        TileLocation aRookLoc;
        TileLocation hRookLoc;

        if (getColor() == PieceColor::BLACK) {
            aRookLoc = board->getPiecePosition(BLACK_A_ROOK_ID);
            hRookLoc = board->getPiecePosition(BLACK_H_ROOK_ID);
        } else {
            aRookLoc = board->getPiecePosition(WHITE_A_ROOK_ID);
            hRookLoc = board->getPiecePosition(WHITE_H_ROOK_ID);
        }


        if (!getHasMoved() && !isChecked(board)) {
            Tile* aRookTile = board->getTile(aRookLoc.x, aRookLoc.y);
            Tile* hRookTile = board->getTile(hRookLoc.x, hRookLoc.y);

            if (aRookTile != nullptr) {
                std::shared_ptr<Piece> aRook = aRookTile->getPiece();

                if (aRook != nullptr && !aRook->getHasMoved()) {
                    bool canCastle = true;

                    for (int i = 1; i < 3; i++) {
                        Tile* tile = board->getTile(aRookLoc.x + i, aRookLoc.y);

                        if (tile->getPiece() != nullptr ||
                            !tile->getAttackersByColor(getOppositeColor(getColor())).empty()) {
                            canCastle = false;
                            break;
                        }
                    }

                    if (canCastle) {
                        result->push_back(board->getTile(aRookLoc.x, aRookLoc.y));
                    }
                }
            }

            if (hRookTile != nullptr) {
                std::shared_ptr<Piece> hRook = board->getTile(hRookLoc.x, hRookLoc.y)->getPiece();

                if (hRook != nullptr && !hRook->getHasMoved()) {
                    bool canCastle = true;

                    for (int i = 1; i < 4; i++) {
                        Tile* tile = board->getTile(hRookLoc.x - i, hRookLoc.y);

                        if (tile->getPiece() != nullptr ||
                            !tile->getAttackersByColor(getOppositeColor(getColor())).empty()) {
                            canCastle = false;
                            break;
                        }
                    }

                    if (canCastle) {
                        result->push_back(board->getTile(hRookLoc.x, hRookLoc.y));
                    }
                }
            }
        }
    }

    void King::getAttackedTiles(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

            if (tile != nullptr) {
                result->push_back(tile);
            }
        }
    }

    void King::setAttackedTiles(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (const DirectionValue &direction : knightDirections) {
            Tile* tile = board->getTile(loc.x + direction.dx, loc.y + direction.dy);

            if (tile != nullptr) {
                tile->addAttacker(this->shared_from_this());
            }
        }
    }

    int King::getBaseWeight() {
        return 10000;
    }

    int King::getWeight(Board* board) {
        return 10000;
    }

    std::string King::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bK";
        } else {
            return "wK";
        }
    }

    bool King::isChecked(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());
        Tile* tile = board->getTile(loc.x, loc.y);
        PieceColor oppositeColor = getOppositeColor(this->getColor());

        return !tile->getAttackersByColor(oppositeColor).empty();
    }

    bool King::isCheckMate(Board* board) {
        if (!isChecked(board)) {
            return false;
        }

        std::vector<Tile*> legalMoves;
        legalMoves.reserve(10);

        for (std::shared_ptr<Piece> piece : board->getPieces(getColor())) {
            piece->getPseudoLegalMoves(&legalMoves, board);

            if (!legalMoves.empty()) {
                legalMoves = board->removeMovesCausingCheck(&legalMoves, piece);

                if (!legalMoves.empty()) {
                    return false;
                }
            }
        }

        return true;
    }

    PieceType King::getPieceType() {
        return PieceType::KING;
    }

    std::string King::getFEN() {
        if (getColor() == PieceColor::WHITE) {
            return "K";
        } else {
            return "k";
        }
    }

    int King::getMobilityScore(Board* board) {
        int badMoves = 0;
        std::vector<Tile*> pseudoLegalMoves;
        this->getPseudoLegalMoves(&pseudoLegalMoves, board);

        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty()) {
                badMoves += 1;
            }
        }

        return pseudoLegalMoves.size() - badMoves;
    }

    void Knight::getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (const DirectionValue &direction : knightDirections) {
            Tile* tile = board->getTile(loc.x + direction.dx, loc.y + direction.dy);

            if (tile != nullptr) {
                std::shared_ptr<Piece> piece = tile->getPiece();

                if ((piece == nullptr || (tile->getPiece()->getPieceType() != PieceType::KING && piece->getColor() != this->getColor()))) {
                    result->push_back(tile);
                }
            }
        }
    }

    void Knight::getAttackedTiles(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (const DirectionValue &direction : knightDirections) {
            Tile* tile = board->getTile(loc.x + direction.dx, loc.y + direction.dy);

            if (tile != nullptr) {
                result->push_back(tile);
            }
        }
    }

    void Knight::setAttackedTiles(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (const DirectionValue &direction : knightDirections) {
            Tile* tile = board->getTile(loc.x + direction.dx, loc.y + direction.dy);

            if (tile != nullptr) {
                tile->addAttacker(this->shared_from_this());
            }
        }
    }

    int Knight::getBaseWeight() {
        return 350;
    }

    int Knight::getWeight(Board* board) {
        return 350;
    }

    std::string Knight::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bN";
        } else {
            return "wN";
        }
    }

    PieceType Knight::getPieceType() {
        return PieceType::KNIGHT;
    }

    std::string Knight::getFEN() {
        if (getColor() == PieceColor::WHITE) {
            return "N";
        } else {
            return "n";
        }
    }

    int Knight::getMobilityScore(Board* board) {
        int badMoves = 0;
        std::vector<Tile*> pseudoLegalMoves;
        this->getPseudoLegalMoves(&pseudoLegalMoves, board);

        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty() && board->mvvlva(tile, getColor()) < 0) {
                badMoves += 1;
            }
        }

        return pseudoLegalMoves.size() - badMoves;
    }

    void Pawn::getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());
        int direction = this->getColor() == PieceColor::BLACK ? 1 : -1;
        Tile* frontTile = board->getTile(loc.x, loc.y + direction);

        if (frontTile != nullptr && frontTile->getPiece() == nullptr) {
            result->push_back(frontTile);

            if (!this->getHasMoved()) {
                Tile* secondFrontTile = board->getTile(loc.x, loc.y + direction * 2);

                if (secondFrontTile != nullptr && secondFrontTile->getPiece() == nullptr) {
                    result->push_back(secondFrontTile);
                }
            }
        }

        std::vector<Tile*> attackedTiles;
        this->getAttackedTiles(&attackedTiles, board);

        for (Tile* attackedTile : attackedTiles) {
            if ((attackedTile->getEnPassantTarget() != nullptr && attackedTile->getEnPassantTarget()->getColor() != this->getColor())
                || (attackedTile->getPiece() != nullptr && attackedTile->getPiece()->getColor() != this->getColor() && attackedTile->getPiece()->getPieceType() != PieceType::KING)) {
                result->push_back(attackedTile);
            }
        }
    }

    void Pawn::getAttackedTiles(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : this->getColor() == PieceColor::WHITE ? WHITE_PAWN_ATTACK_DIRECTIONS : BLACK_PAWN_ATTACK_DIRECTIONS) {
            Tile* tile = board->getTile(loc.x + DIRECTION_VALUES[direction].dx, loc.y + DIRECTION_VALUES[direction].dy);

            if (tile != nullptr) {
                result->push_back(tile);
            }
        }
    }

    void Pawn::setAttackedTiles(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : this->getColor() == PieceColor::WHITE ? WHITE_PAWN_ATTACK_DIRECTIONS : BLACK_PAWN_ATTACK_DIRECTIONS) {
            Tile* tile = board->getTile(loc.x + DIRECTION_VALUES[direction].dx, loc.y + DIRECTION_VALUES[direction].dy);

            if (tile != nullptr) {
                tile->addAttacker(this->shared_from_this());
            }
        }
    }

    int Pawn::getBaseWeight() {
        return 100;
    }

    int Pawn::getWeight(Board* board) {
        return 100;
    }

    std::string Pawn::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bP";
        } else {
            return "wP";
        }
    }

    PieceType Pawn::getPieceType() {
        return PieceType::PAWN;
    }

    std::string Pawn::getFEN() {
        if (getColor() == PieceColor::WHITE) {
            return "P";
        } else {
            return "p";
        }
    }

    int Pawn::getMobilityScore(Board* board) {
        return 0;
    }

    void Queen::getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, this->getColor(), false);
        }
    }

    void Queen::getAttackedTiles(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, this->getColor(), true);
        }
    }

    void Queen::setAttackedTiles(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            board->setAttackedTilesInDirection(loc.x, loc.y, this->shared_from_this(), direction, this->getColor(), true);
        }
    }

    int Queen::getBaseWeight() {
        return 1000;
    }

    int Queen::getWeight(Board* board) {
        return 1000;
    }

    std::string Queen::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bQ";
        } else {
            return "wQ";
        }
    }

    PieceType Queen::getPieceType() {
        return PieceType::QUEEN;
    }

    std::string Queen::getFEN() {
        if (getColor() == PieceColor::WHITE) {
            return "Q";
        } else {
            return "q";
        }
    }

    int Queen::getMobilityScore(Board* board) {
        int badMoves = 0;
        std::vector<Tile*> pseudoLegalMoves;
        this->getPseudoLegalMoves(&pseudoLegalMoves, board);

        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty() && board->mvvlva(tile, getColor()) < 0) {
                badMoves += 1;
            }
        }

        return pseudoLegalMoves.size() - badMoves;
    }

    void Rook::getPseudoLegalMoves(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ROOK_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), false);
        }
    }

    void Rook::getAttackedTiles(std::vector<Tile*>* result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ROOK_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), true);
        }
    }

    void Rook::setAttackedTiles(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ROOK_DIRECTIONS) {
            board->setAttackedTilesInDirection(loc.x, loc.y, this->shared_from_this(), direction, getColor(), true);
        }
    }

    int Rook::getBaseWeight() {
        return 525;
    }

    int Rook::getWeight(Board* board) {
        return 525;
    }

    std::string Rook::getSymbol() {
        if (this->getColor() == PieceColor::BLACK) {
            return "bR";
        } else {
            return "wR";
        }
    }

    PieceType Rook::getPieceType() {
        return PieceType::ROOK;
    }

    std::string Rook::getFEN() {
        if (getColor() == PieceColor::WHITE) {
            return "R";
        } else {
            return "r";
        }
    }

    int Rook::getMobilityScore(Board* board) {
        int badMoves = 0;
        std::vector<Tile*> pseudoLegalMoves;
        this->getPseudoLegalMoves(&pseudoLegalMoves, board);

        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty() && board->mvvlva(tile, getColor()) < 0) {
                badMoves += 1;
            }
        }

        return pseudoLegalMoves.size() - badMoves;
    }
}