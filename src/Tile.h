#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>

#include <string>


class Tile {
    public:
        // constructor
        Tile();

        // initialize the tile
        bool init(const int value);
        // initialze the tile's sprites
        void setSprites(sf::Sprite unopened, sf::Sprite flag, sf::Sprite opened, sf::Sprite revealbomb, sf::Sprite xbomb, sf::Text text);

        // draw the tile
        void draw(sf::RenderWindow &window, const sf::RenderStates &states) const;
        void drawLose(sf::RenderWindow &window, const sf::RenderStates &states) const;
        void drawWin(sf::RenderWindow &window, const sf::RenderStates &states) const;
        void drawText(sf::RenderWindow &window, const sf::RenderStates &states, std::string str);

        // actions
        void reveal();
        void flag();

        // getters
        bool isRevealed() const;
        bool isBomb() const;
        bool isFlagged() const;
        int getIdentity() const;

    private:

        // tile state
        bool _revealed;
        bool _flagged;
        int _identity;
        /*
        identity key:
           -1 - uninitialized
          0-8 - the are x bombs nearby
            9 - bomb
        */

        // sprites
        sf::Sprite _spriteUnopened,
                   _spriteFlagged,
                   _spriteOpened,
                   _spriteRevealBomb,
                   _spriteXBomb;
        sf::Text _text;

};


#endif