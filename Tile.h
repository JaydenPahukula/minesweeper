#ifndef TILE_H
#define TILE_H


class Tile {
    public:
        Tile(const unsigned int X, const unsigned int Y);

        bool isRevealed() const;
    private:
        unsigned int x, y;
        int identity;
        bool revealed;

};



#endif