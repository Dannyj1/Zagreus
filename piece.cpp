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

    void Bishop::getPseudoLegalMoves(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : BISHOP_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), false);
        }
    }

    void Bishop::getAttackedTiles(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : BISHOP_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), true);
        }
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
        legalTiles.clear();
        this->getPseudoLegalMoves(legalTiles, board);

        return legalTiles.size() * 2;
    }

    int Bishop::getEvaluationScore(Board* board) {
        int score = 0;
        TileLocation loc = board->getPiecePosition(this->getId());
        attackedTiles.clear();
        this->getAttackedTiles(attackedTiles, board);

        for (Tile* tile : attackedTiles) {
            if (board->isInCenter(tile->getX(), tile->getY())) {
                score += 2;
            } else if (board->isInExtendedCenter(tile->getX(), tile->getY())) {
                score += 1;
            }
        }

        int bishopsCaptured = 2;
        for (std::shared_ptr<Piece> piece : board->getPieces(getColor())) {
            if (piece->getPieceType() == PieceType::BISHOP) {
                bishopsCaptured -= 1;
                break;
            }
        }

        if (bishopsCaptured >= 1) {
            score -= 100;
        }

        bool isFianchetto = false;
        if (getColor() == PieceColor::WHITE) {
            if ((loc.x == 1 && loc.y == 6) || (loc.x == 6 && loc.y == 6)) {
                isFianchetto = true;
            }
        } else {
            if ((loc.x == 1 && loc.y == 1) || (loc.x == 6 && loc.y == 1)) {
                isFianchetto = true;
            }
        }

        if (isFianchetto) {
            for (Direction direction : getColor() == PieceColor::WHITE ? WHITE_FIANCHETTO_DIRECTIONS
                                                                       : BLACK_FIANCHETTO_DIRECTIONS) {
                Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

                if (tile->getPiece() && tile->getPiece()->getPieceType() == PieceType::PAWN && tile->getPiece()->getColor() == getColor()) {
                    continue;
                }

                isFianchetto = false;
                break;
            }
        }

        if (isFianchetto) {
            score += 25;
        }

        return score;
    }

    int Bishop::getTempo(Board* board) {
        if (getHasMoved()) {
            return 1;
        } else {
            return 0;
        }
    }

    void King::getPseudoLegalMoves(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

            if (tile != nullptr) {
                if ((tile->getPiece() == nullptr || (tile->getPiece()->getPieceType() != PieceType::KING && tile->getPiece()->getColor() != this->getColor()))) {
                    result.push_back(tile);
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
                        Tile* square = board->getTile(aRookLoc.x + i, aRookLoc.y);

                        if (square->getPiece() != nullptr ||
                                board->isTileAttackedByColor(square, getOppositeColor(aRook->getColor()))) {
                            canCastle = false;
                            break;
                        }
                    }

                    if (canCastle) {
                        result.push_back(board->getTile(aRookLoc.x, aRookLoc.y));
                    }
                }
            }

            if (hRookTile != nullptr) {
                std::shared_ptr<Piece> hRook = board->getTile(hRookLoc.x, hRookLoc.y)->getPiece();

                if (hRook != nullptr && !hRook->getHasMoved()) {
                    bool canCastle = true;

                    for (int i = 1; i < 4; i++) {
                        Tile* square = board->getTile(hRookLoc.x - i, hRookLoc.y);

                        if (square->getPiece() != nullptr ||
                                board->isTileAttackedByColor(square, getOppositeColor(hRook->getColor()))) {
                            canCastle = false;
                            break;
                        }
                    }

                    if (canCastle) {
                        result.push_back(board->getTile(hRookLoc.x, hRookLoc.y));
                    }
                }
            }
        }
    }

    void King::getAttackedTiles(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

            if (tile != nullptr) {
                result.push_back(tile);
            }
        }
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

        return board->isTileAttackedByColor(tile, oppositeColor);
    }

    bool King::isCheckMate(Board* board) {
        if (!isChecked(board)) {
            return false;
        }

        for (std::shared_ptr<Piece> piece : board->getPieces(getColor())) {
            legalTiles.clear();
            piece->getPseudoLegalMoves(legalTiles, board);

            if (!legalTiles.empty()) {
                legalTiles = board->removeMovesCausingCheck(legalTiles, piece);

                if (!legalTiles.empty()) {
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
        legalTiles.clear();
        this->getPseudoLegalMoves(legalTiles, board);

/*
        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty()) {
                badMoves += 1;
            }
        }
*/

        return legalTiles.size() * 6;
    }

    int King::getTempo(Board* board) {
        // TODO: handle castling tempo
        return 0;
    }

    int King::getEvaluationScore(Board* board) {
        int score = 0;
        TileLocation loc = board->getPiecePosition(this->getId());

        if (getColor() == PieceColor::WHITE) {
            if (board->getHasWhiteCastled()) {
                score += 25;
            }
        } else {
            if (board->getHasBlackCastled()) {
                score += 25;
            }
        }

        if (getHasMoved()) {
            for (Direction direction : getColor() == PieceColor::WHITE ? WHITE_PAWN_SHIELD_DIRECTIONS
                                                                       : BLACK_PAWN_SHIELD_DIRECTIONS) {
                Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

                if (tile != nullptr && tile->getPiece() != nullptr &&
                    tile->getPiece()->getPieceType() == PieceType::PAWN
                    && tile->getPiece()->getColor() == getColor()) {
                    score += 5;
                }
            }
        }

        /*for (Direction direction : ALL_DIRECTIONS) {
            Tile* tile = board->getTileInDirection(loc.x, loc.y, direction);

            if (tile != nullptr) {
                std::vector<std::shared_ptr<Piece>> attackers = tile->getAttackersByColor(getOppositeColor(getColor()));

                for (std::shared_ptr<Piece> attacker : attackers) {
                    score -= 5 * (attacker->getWeight(board) / 100);
                }
            }
        }*/

        return score;
    }

    void Knight::getPseudoLegalMoves(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (const DirectionValue &direction : knightDirections) {
            Tile* tile = board->getTile(loc.x + direction.dx, loc.y + direction.dy);

            if (tile != nullptr) {
                std::shared_ptr<Piece> piece = tile->getPiece();

                if ((piece == nullptr || (tile->getPiece()->getPieceType() != PieceType::KING && piece->getColor() != this->getColor()))) {
                    result.push_back(tile);
                }
            }
        }
    }

    void Knight::getAttackedTiles(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (const DirectionValue &direction : knightDirections) {
            Tile* tile = board->getTile(loc.x + direction.dx, loc.y + direction.dy);

            if (tile != nullptr) {
                result.push_back(tile);
            }
        }
    }

    int Knight::getWeight(Board* board) {
        int pawnsCaptured = 8;

        for (std::shared_ptr<Piece> piece : board->getPieces(getColor())) {
            if (piece->getPieceType() == PieceType::PAWN) {
                pawnsCaptured -= 1;
            }
        }

        return 350 - (pawnsCaptured * 10);
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
        legalTiles.clear();
        this->getPseudoLegalMoves(legalTiles, board);

        return legalTiles.size() * 2;
    }

    int Knight::getEvaluationScore(Board* board) {
        int score = 0;
        attackedTiles.clear();
        TileLocation loc = board->getPiecePosition(this->getId());
        this->getAttackedTiles(attackedTiles, board);

        for (Tile* tile : attackedTiles) {
            if (board->isInCenter(tile->getX(), tile->getY())) {
                score += 2;
            } else if (board->isInExtendedCenter(tile->getX(), tile->getY())) {
                score += 1;
            }
        }

        // TODO: add check to see if controlled by pawn
/*        for (std::shared_ptr<Piece> defender : ownTile->getAttackersByColor(getColor())) {
            if (defender->getPieceType() == PieceType::PAWN) {
                score += 2;
            }
        }*/

        return score;
    }

    int Knight::getTempo(Board* board) {
        if (getHasMoved()) {
            return 1;
        } else {
            return 0;
        }
    }

    void Pawn::getPseudoLegalMoves(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());
        int direction = this->getColor() == PieceColor::BLACK ? 1 : -1;
        Tile* frontTile = board->getTile(loc.x, loc.y + direction);

        if (frontTile != nullptr && frontTile->getPiece() == nullptr) {
            result.push_back(frontTile);

            if (!this->getHasMoved()) {
                Tile* secondFrontTile = board->getTile(loc.x, loc.y + direction * 2);

                if (secondFrontTile != nullptr && secondFrontTile->getPiece() == nullptr) {
                    result.push_back(secondFrontTile);
                }
            }
        }

        attackedTiles.clear();
        this->getAttackedTiles(attackedTiles, board);

        for (Tile* attackedTile : attackedTiles) {
            if ((attackedTile->getEnPassantTarget() != nullptr && attackedTile->getEnPassantTarget()->getColor() != this->getColor())
                || (attackedTile->getPiece() != nullptr && attackedTile->getPiece()->getColor() != this->getColor() && attackedTile->getPiece()->getPieceType() != PieceType::KING)) {
                result.push_back(attackedTile);
            }
        }
    }

    void Pawn::getAttackedTiles(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : this->getColor() == PieceColor::WHITE ? WHITE_PAWN_ATTACK_DIRECTIONS : BLACK_PAWN_ATTACK_DIRECTIONS) {
            Tile* tile = board->getTile(loc.x + DIRECTION_VALUES[direction].dx, loc.y + DIRECTION_VALUES[direction].dy);

            if (tile != nullptr) {
                result.push_back(tile);
            }
        }
    }

    int Pawn::getWeight(Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        return PAWN_WEIGHTS[loc.x];
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

    int Pawn::getTempo(Board* board) {
        return 0;
    }

    int Pawn::getEvaluationScore(Board* board) {
        int score = 0;
        TileLocation loc = board->getPiecePosition(this->getId());
        attackedTiles.clear();
        this->getAttackedTiles(attackedTiles, board);

        for (Tile* tile : attackedTiles) {
            if (board->isInCenter(tile->getX(), tile->getY())) {
                score += 2;
            } else if (board->isInExtendedCenter(tile->getX(), tile->getY())) {
                score += 1;
            }
        }

        if (board->getMovesMade() <= 20) {
            if (board->isInCenter(loc.x, loc.y)) {
                score += 6;
            } else if (board->isInExtendedCenter(loc.x, loc.y)) {
                score += 3;
            }
        }

        return score;
    }

    void Queen::getPseudoLegalMoves(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, this->getColor(), false);
        }
    }

    void Queen::getAttackedTiles(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ALL_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, this->getColor(), true);
        }
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
        legalTiles.clear();
        this->getPseudoLegalMoves(legalTiles, board);

/*        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty() && board->mvvlva(tile, getColor()) < 0) {
                badMoves += 1;
            }
        }*/

        return legalTiles.size() * 5;
    }

    int Queen::getTempo(Board* board) {
        if (getHasMoved()) {
            return board->getMovesMade() <= 25 ? -1 : 1;
        } else {
            return 0;
        }
    }

    int Queen::getEvaluationScore(Board* board) {
        int score = 0;
        attackedTiles.clear();
        this->getAttackedTiles(attackedTiles, board);

        for (Tile* tile : attackedTiles) {
            if (board->isInCenter(tile->getX(), tile->getY())) {
                score += 2;
            } else if (board->isInExtendedCenter(tile->getX(), tile->getY())) {
                score += 1;
            }
        }

        return score;
    }

    void Rook::getPseudoLegalMoves(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ROOK_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), false);
        }
    }

    void Rook::getAttackedTiles(std::vector<Tile*> &result, Board* board) {
        TileLocation loc = board->getPiecePosition(this->getId());

        for (Direction direction : ROOK_DIRECTIONS) {
            board->getTilesInDirection(result, loc.x, loc.y, direction, getColor(), true);
        }
    }

    int Rook::getWeight(Board* board) {
        int pawnsCaptured = 8;

        for (std::shared_ptr<Piece> piece : board->getPieces(getColor())) {
            if (piece->getPieceType() == PieceType::PAWN) {
                pawnsCaptured -= 1;
            }
        }

        return 525 + (pawnsCaptured * 10);
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
        legalTiles.clear();
        this->getPseudoLegalMoves(legalTiles, board);

/*        for (Tile* tile : pseudoLegalMoves) {
            if (!tile->getAttackersByColor(getOppositeColor(getColor())).empty() && board->mvvlva(tile, getColor()) < 0) {
                badMoves += 1;
            }
        }*/

        return legalTiles.size() * 4;
    }

    int Rook::getTempo(Board* board) {
        if (getHasMoved()) {
            if (board->hasCastled(getColor())) {
                return 1;
            } else {
                return -1;
            }
        } else {
            return 0;
        }
    }

    int Rook::getEvaluationScore(Board* board) {
        int score = 0;
        attackedTiles.clear();
        this->getAttackedTiles(attackedTiles, board);

        for (Tile* tile : attackedTiles) {
            if (board->isInCenter(tile->getX(), tile->getY())) {
                score += 2;
            } else if (board->isInExtendedCenter(tile->getX(), tile->getY())) {
                score += 1;
            }
        }

        return score;
    }
}