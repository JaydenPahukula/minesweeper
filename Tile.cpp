
#include "Tile.h"

Tile::Tile(const unsigned int X, const unsigned int Y){
    x = X;
    y = Y;
    identity = -1;
    revealed = false;
}

bool Tile::isRevealed() const{
    return revealed;
}