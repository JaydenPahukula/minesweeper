#ifndef GAME_H
#define GAME_H

#include "Tile.h"

#include <ctime>
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
        unsigned int _width, _height, _numBombs;

        //game state
        std::vector<std::vector<Tile*>> _grid;
        unsigned int _gameOver, _numBombsRemaining;

        //
        void _checkZeroTile(unsigned int x, unsigned int y) const;
        std::vector<Tile*> _getNearbyTiles(unsigned int x, unsigned int y) const;

        //textures and sprites
        bool _loadGameSprites();
        bool _loadTileSprites();
        sf::Texture _gamespritesheet, _tilespritesheet;
        std::vector<sf::Sprite> _bkgSprites;
        sf::Sprite _happySprite,
                   _coolSprite,
                   _sadSprite;
        sf::Sprite _digitSprites[6][10];

        //timer
        bool _timerRunning;
        time_t _startTime;
        time_t _currTime;

};


#endif