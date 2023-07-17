#ifndef APP_H
#define APP_H

#include "Game.h"

#include <SFML/Graphics.hpp>
#include <vector>

#include <ctime>


class App {
    public:

        // constructor
        App();
        // destructor
        ~App();

        // draw the app
        void draw(sf::RenderWindow &window);

        // window resized event
        void resize(const sf::Event::SizeEvent newSize, sf::RenderWindow &window);

        // click event
        void click(const sf::Event::MouseButtonEvent mouse);

        // getters
        unsigned int gameWidth() const;
        unsigned int gameHeight() const;
        unsigned int windowWidth() const;
        unsigned int windowHeight() const;
    
    private:

        Game* _game;

        // window
        unsigned int _windowWidth;
        unsigned int _windowHeight;
        unsigned int _minWindowWidth;
        unsigned int _minWindowHeight;
        unsigned int _minBoardx, _maxBoardx, _minBoardy, _maxBoardy;
        unsigned int _boardx, _boardy;
        float _boardTileSize;

        // timer
        bool _timerRunning;
        time_t _startTime, _currTime;

        // sprites
        sf::Texture _appspritesheet;
        sf::Sprite _tlc, _t, _trc, _tls, _tm, _trs, _tlg, _tg, _trg, _lg, _rg, _blg, _bg, _brg;
        sf::Sprite _happyFaceSprite, _coolFaceSprite, _sadFaceSprite;
        sf::Sprite _digitSprites[6][10];
        sf::RectangleShape _background;

        void _updateBoardRestrictions();
        bool _loadBorderSprites();

};


#endif