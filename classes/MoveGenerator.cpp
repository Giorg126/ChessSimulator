#include "MoveGenerator.h"
#include "Chess.h"

MoveGenerator::MoveGenerator(Chess* game) : _game(game) {}

std::vector<BitHolder*> MoveGenerator::generateQueenMoves(Bit &bit, BitHolder &src) {
    std::vector<BitHolder*> validMoves;
    int srcX = src._x;
    int srcY = src._y;

    // directions
    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} 
    };

    for (const auto& [dx, dy] : directions) {
        int x = srcX + dx;
        int y = srcY + dy;

        while (x >= 0 && x < 8 && y >= 0 && y < 8) {
            BitHolder &currentHolder = _game->getHolderAt(x, y);
            if (currentHolder.bit() != nullptr) {
                if (currentHolder.bit()->getOwner() != bit.getOwner()) {
                    validMoves.push_back(&currentHolder);
                }
                break;
            }
            validMoves.push_back(&currentHolder);

            x += dx;
            y += dy;
        }
    }
    return validMoves;
}

std::vector<BitHolder*> MoveGenerator::generatePawnMoves(Bit &bit, BitHolder &src) {
    std::vector<BitHolder*> validMoves;
    int srcX = src._x;
    int srcY = src._y;

    int direction = (bit.getOwner()->playerNumber() == 0) ? 1 : -1; // white up black down

    int newY = srcY + direction;
    if (newY >= 0 && newY < 8) {
        BitHolder &forwardHolder = _game->getHolderAt(srcX, newY);
        
        if (forwardHolder.bit() == nullptr) { 
            validMoves.push_back(&forwardHolder); 
        }
    }

    std::cout << "Generated Pawn Moves from (" << srcX << ", " << srcY << "): ";
    for (auto move : validMoves) {
        std::cout << "(" << move->_x << ", " << move->_y << ") ";
    }
    std::cout << std::endl;

    return validMoves;
}
