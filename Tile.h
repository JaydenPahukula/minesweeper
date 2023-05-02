#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>


class Tile {
    public:
        //constructor
        Tile();

        //reset
        void reset();

        //init
        bool init(const int value);
        void initSprites(sf::Sprite unopened, sf::Sprite flag, sf::Sprite opened, sf::Sprite revealbomb, sf::Sprite xbomb);

        //draw
        void draw(sf::RenderWindow &window) const;
        void drawGameOver(sf::RenderWindow &window) const;

        //getters
        bool isRevealed() const;
        bool isBomb() const;

    private:

        //tile state
        int _identity;
        bool _revealed;
        bool _flagged;

        //sprites
        bool _loadsprites();
        sf::Sprite _spriteUnopened,
                   _spriteFlagged,
                   _spriteOpened,
                   _spriteRevealbomb,
                   _spriteXbomb;

};



#endif