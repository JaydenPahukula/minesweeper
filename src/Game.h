#ifndef GAME_H
#define GAME_H

#include "Tile.h"
#include "Timer.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>


class Game {
    public:
    
        // constructor
        Game(const unsigned int width, const unsigned int height, const unsigned int numBombs, const bool autoOpen);
        // destructor
        ~Game();

        // draw the game
        void draw(sf::RenderWindow &window, const sf::RenderStates &states);
        void drawProbabilities(sf::RenderWindow &window, const sf::RenderStates &states);

        // click event
        void click(const sf::Event::MouseButtonEvent mouse, unsigned int x, unsigned int y, bool chordingEnabled);

        // getters
        unsigned int width() const;
        unsigned int height() const;
        unsigned int numBombs() const;
        unsigned int numBombsRemaining() const;
        unsigned int gameOver() const;
        unsigned int seconds() const;
        unsigned int get3BV() const;
        float tilesPerSecond() const;

    private:

        // game parameters
        unsigned int _width, _height, _numBombs;

        // game state
        std::vector<std::vector<Tile*>> _grid;
        std::vector<std::vector<int>> _probabilities;
        unsigned int _gameOver;
        unsigned int _numBombsRemaining;
        unsigned int _numRevealed;

        // textures and sprites
        void _loadTileSprites();
        sf::Texture _tilespritesheet;
        sf::Font _font;

        // game functions
        std::vector<sf::Vector2u> _getAdjacentCoords(const sf::Vector2u a);
        void _playerWins();
        void _playerLoses();
        void _update();
        void _revealTile(unsigned int x, unsigned int y);
        void _checkZeroTile(unsigned int x, unsigned int y, bool first);
        void _chord(unsigned int x, unsigned int y);

        // timer
        Timer _timer;

        // stats
        unsigned int _3BV;
        void _update3BV();
        unsigned int _numClicks;
        void _updateProbabilities();
};


#endif