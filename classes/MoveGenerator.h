#pragma once

#include "Bit.h"
#include "BitHolder.h"
#include <vector>
#include <utility>

class Chess;

class MoveGenerator {
public:
    MoveGenerator(Chess* game);

    std::vector<BitHolder*> generateQueenMoves(Bit &bit, BitHolder &src);
    std::vector<BitHolder*> generatePawnMoves(Bit &bit, BitHolder &src);

    // Placeholder for other piece movement generation functions (to be implemented)
    // std::vector<BitHolder*> generateRookMoves(Bit &bit, BitHolder &src);
    // std::vector<BitHolder*> generateBishopMoves(Bit &bit, BitHolder &src);
    // std::vector<BitHolder*> generateKnightMoves(Bit &bit, BitHolder &src);

private:
    Chess* _game; 
};

