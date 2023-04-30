#ifndef TILE_H
#define TILE_H

#include "Game.h"

class Game::Tile {
    public:
        Tile(const unsigned int X, const unsigned int Y);

    private:
        unsigned int x, y;
        int identity;

};



#endif