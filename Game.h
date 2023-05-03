#ifndef GAME_H
#define GAME_H

#include "Tile.h"

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
    public:
        //constructor
        Game();
        //destructor
        ~Game();

        //init
        bool init(const std::string configfilename);

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
        bool _loadGameSprites();
        bool _loadTileSprites();
        sf::Texture _gamespritesheet, _tilespritesheet;
        std::vector<sf::Sprite> _bkgSprites;

};


#endif