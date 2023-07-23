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
        // zoom event
        void zoom(const sf::Event::MouseWheelScrollEvent mouse);

        // click event
        void mouseClick(const sf::Event::MouseButtonEvent mouse);
        // release event
        void mouseRelease(const sf::Event::MouseButtonEvent mouse);
        // move event
        void mouseMove(const sf::Event::MouseMoveEvent mouse);

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

        // board
        int _minBoardx, _maxBoardx, _minBoardy, _maxBoardy;
        int _boardx, _boardy;
        float _boardTileSize;
        void _updateBoardRestrictions();
        void _resetBoardView();
        void _keepBoardInView();

        // mouse/panning
        bool _holding, _panning;
        unsigned int _panx, _pany;
        int _lastMousex, _lastMousey;

        // timer
        bool _timerRunning;
        time_t _startTime, _currTime;

        // sprites
        sf::Texture _appspritesheet;
        sf::Sprite _tlc, _t, _trc, _tls, _tm, _trs, _tlg, _tg, _trg, _lg, _rg, _blg, _bg, _brg;
        sf::Sprite _happyFaceSprite, _coolFaceSprite, _sadFaceSprite;
        sf::Sprite _digitSprites[6][10];
        sf::RectangleShape _background;
        bool _loadBorderSprites();

};


#endif