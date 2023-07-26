#ifndef GAME_H
#define GAME_H

#include "Tile.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include <ctime>


class Game {
    public:
    
        // constructor
        Game();
        // destructor
        ~Game();

        // initialize the game from config file
        bool init();

        // reset the game
        void reset();

        // draw the game
        void draw(sf::RenderWindow &window, const sf::RenderStates &states);

        // click event
        void click(const sf::Event::MouseButtonEvent mouse, unsigned int x, unsigned int y);

        // getters
        unsigned int gameOver() const;
        unsigned int numBombsRemaining() const;

    private:

        // game parameters
        unsigned int _width, _height, _numBombs;
        bool _chordingEnabled;

        // game state
        std::vector<std::vector<Tile*>> _grid;
        unsigned int _gameOver;
        unsigned int _numBombsRemaining;

        // textures and sprites
        bool _loadTileSprites();
        sf::Texture _tilespritesheet;

        // game functions
        void _playerWins();
        void _playerLoses();
        void _update();
        void _revealTile(unsigned int x, unsigned int y);
        void _checkZeroTile(unsigned int x, unsigned int y, bool first);
        void _chord(unsigned int x, unsigned int y);

};


#endif