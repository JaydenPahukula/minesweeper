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
        Game(const unsigned int width, const unsigned int height, const unsigned int numBombs, const bool autoOpen, const int seed = 0);
        // destructor
        ~Game();

        // draw the game
        void draw(sf::RenderWindow &window, const sf::RenderStates &states);

        // click event
        void click(const sf::Event::MouseButtonEvent mouse, unsigned int x, unsigned int y, bool chordingEnabled);

        // getters
        unsigned int width() const;
        unsigned int height() const;
        unsigned int numBombs() const;
        unsigned int numBombsRemaining() const;
        unsigned int gameOver() const;

    private:

        // game parameters
        unsigned int _width, _height, _numBombs;

        // game state
        std::vector<std::vector<Tile*>> _grid;
        unsigned int _gameOver;
        unsigned int _numBombsRemaining;

        // textures and sprites
        void _loadTileSprites();
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