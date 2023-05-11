#ifndef GAME_H
#define GAME_H

#include "Tile.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include <ctime>


class Game {
    public:
    
        //constructor
        Game();
        //destructor
        ~Game();

        //initialize the game from config file
        bool init(const std::string configfilename);

        //reset the game
        void reset();

        //draw the game
        void draw(sf::RenderWindow &window);

        //click event
        void click(const sf::Event::MouseButtonEvent mouse);

        //getters
        unsigned int width() const;
        unsigned int height() const;

    private:

        //game parameters
        unsigned int _width, _height, _numBombs;
        bool _chordingEnabled;

        //game state
        std::vector<std::vector<Tile*>> _grid;
        unsigned int _gameOver;
        // _gameOver key:
        // 0 - game in progress
        // 1 - game over, player lost
        // 2 - game over, player won

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
        time_t _startTime, _currTime;

        //game functions
        void _checkZeroTile(unsigned int x, unsigned int y) const;
        void _chord(unsigned int x, unsigned int y);

};


#endif