#ifndef GAME_H
#define GAME_H

#include "Tile.h"

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
    public:
        //constructor
        Game(const std::string filename);
        //destructor
        ~Game();

        //reset
        void reset();

        //draw
        void draw(sf::RenderWindow &window);

        //click
        void click(const sf::Event::MouseButtonEvent mouse);

        //getters
        unsigned int width() const;
        unsigned int height() const;

    private:

        //game parameters
        unsigned int _width, _height, _numbombs;

        //game state
        std::vector<std::vector<Tile*>> _grid;
        bool _gameOver;

        //sprites
        bool _loadSprites();
        sf::Texture _gamespritesheet, _tilespritesheet;
        std::vector<sf::Sprite> _bkgSprites;

};


#endif