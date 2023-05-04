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

        bool reveal();
        void flag();

        //getters
        bool isRevealed() const;
        bool isBomb() const;
        bool isZero() const;

    private:

        //tile state
        int _identity;
        bool _revealed;
        bool _flagged;

        //sprites
        sf::Sprite _spriteUnopened,
                   _spriteFlagged,
                   _spriteOpened,
                   _spriteRevealBomb,
                   _spriteXBomb;

};



#endif