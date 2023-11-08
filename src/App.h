#ifndef APP_H
#define APP_H

#include "Game.h"
#include "Menu.h"
#include "Timer.h"
#include "Confetti.h"

#include <SFML/Graphics.hpp>


class App {
    public:

        // constructor
        App(sf::RenderWindow &window);
        // destructor
        ~App();

        // draw the app
        void draw();

        // event handler
        void handleEvent(const sf::Event);
    
    private:
        
        // game
        Game* _game;

        // window
        sf::RenderWindow* _window;
        unsigned int _minWindowWidth, _minWindowHeight;
        void _boundWindowSize();

        // event handlers
        void _keyPress(const sf::Event::KeyEvent key);
        void _mouseClick(const sf::Event::MouseButtonEvent mouse);
        void _mouseMove(const sf::Event::MouseMoveEvent mouse);
        void _mouseRelease(const sf::Event::MouseButtonEvent mouse);
        void _resize(const sf::Event::SizeEvent newSize);
        void _zoom(const sf::Event::MouseWheelScrollEvent mouse);

        // board
        int _minBoardx, _maxBoardx, _minBoardy, _maxBoardy;
        int _boardx, _boardy;
        float _boardTileSize;
        void _updateBoardBounds();
        void _resetBoardView();
        void _boundBoardView();

        // graphics
        sf::RectangleShape _background;
        sf::Texture _appspritesheet;
        sf::Sprite _tl, _t, _tr, _l, _m, _r, _bl, _b, _br, _tlg, _tg, _trg, _lg, _rg, _blg, _bg, _brg;
        sf::Sprite _happyFaceSprite, _coolFaceSprite, _sadFaceSprite;
        sf::Sprite _digitSprites[6][10];
        sf::RectangleShape _backgroundDim;
        void _loadAssets();
        void _updateAssets();
        void _drawBorder();

        // menu
        Menu* _menu;
        bool _menuOpen;
        void _boundMenuOptions();
        // menu options
        bool _zoomEnabled;
        bool _chordingEnabled;
        unsigned int _nextGameWidth;
        unsigned int _nextGameHeight;
        unsigned int _nextNumBombs;
        bool _autoOpeningEnabled;

        // mouse/panning
        bool _holding, _panning;
        unsigned int _panx, _pany;
        int _lastMousex, _lastMousey;

        // timer
        Timer _timer;

        // confetti
        ConfettiCannon _confetti;
        bool _confettiLaunched;

};


#endif