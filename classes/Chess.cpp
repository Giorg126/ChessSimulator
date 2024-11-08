#include "Chess.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

Chess::Chess() : moveGenerator(this){

}

Chess::~Chess()
{
}

//
// make a chess piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    int gameTag = (playerNumber == 0 ? 0 : 128) + piece;
    bit->setGameTag(gameTag);

    return bit;
}



void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    //
    // we want white to be at the bottom of the screen so we need to reverse the board
    //
    char piece[2];
    piece[1] = 0;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * (_gameOptions.rowY - y) + pieceSize));
            _grid[y][x].initHolder(position, "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0);
            piece[0] = bitToPieceNotation(y,x);
            _grid[y][x].setNotation(piece);
        }
    }

    // initializeBoard();
    placePieceAt(0, Queen, 4, 4);
    placePieceAt(1, Pawn, 1, 1);
}


void Chess::placePieceAt(int playerNumber, ChessPiece piece, int x, int y)
{
    Bit* bit = PieceForPlayer(playerNumber, piece);
    bit->setPosition(_grid[y][x].getPosition());
    bit->setParent(&_grid[y][x]);
    _grid[y][x].setBit(bit);
}

void Chess::initializeBoard()
{
    // pawns
    for (int x = 0; x < 8; x++) {
        placePieceAt(0, Pawn, x, 1);
        placePieceAt(1, Pawn, x, 6);
    }

    // rooks
    placePieceAt(0, Rook, 0, 0);
    placePieceAt(0, Rook, 7, 0);
    placePieceAt(1, Rook, 0, 7);
    placePieceAt(1, Rook, 7, 7);

    // knights
    placePieceAt(0, Knight, 1, 0);
    placePieceAt(0, Knight, 6, 0);
    placePieceAt(1, Knight, 1, 7);
    placePieceAt(1, Knight, 6, 7);

    // bishops
    placePieceAt(0, Bishop, 2, 0);
    placePieceAt(0, Bishop, 5, 0);
    placePieceAt(1, Bishop, 2, 7);
    placePieceAt(1, Bishop, 5, 7);

    // queens
    placePieceAt(0, Queen, 3, 0);
    placePieceAt(1, Queen, 3, 7);

    // kings
    placePieceAt(0, King, 4, 0);
    placePieceAt(1, King, 4, 7);
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return true;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src) {
    Player *currentPlayer = getCurrentPlayer();
    int currentPlayerNumber = currentPlayer->playerNumber();

    if ((bit.gameTag() < 128 && currentPlayerNumber != 0) || 
        (bit.gameTag() >= 128 && currentPlayerNumber != 1)) {
        return false; 
    }
    return true;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    if (bit.gameTag() == 5 || bit.gameTag() == 133) {
        auto validMoves = moveGenerator.generateQueenMoves(bit, src);
        for (auto move : validMoves) {
            if (move == &dst) {
                return true;
            }
        }
    }

    if (bit.gameTag() == 1 || bit.gameTag() == 129) {
        return canMovePawn(bit, src, dst);
    }

    return false;
}



void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    endTurn();
}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{
    // check to see if either player has won
    return nullptr;
}

bool Chess::checkForDraw()
{
    // check to see if the board is full
    return false;
}

//
// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
//
const char Chess::bitToPieceNotation(int row, int column) const {
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }

    const char* wpieces = { "?PNBRQK" };
    const char* bpieces = { "?pnbrqk" };
    unsigned char notation = '0';
    Bit* bit = _grid[row][column].bit();
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag() & 127];
    } else {
        notation = '0';
    }
    return notation;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            s += bitToPieceNotation(y, x);
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit(PieceForPlayer(playerNumber - 1, Pawn));
            } else {
                _grid[y][x].setBit(nullptr);
            }
        }
    }
}


//
// this is the function that will be called by the AI
//
void Chess::updateAI() 
{
}

