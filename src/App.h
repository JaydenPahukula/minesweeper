#ifndef APP_H
#define APP_H

#include "Confetti.h"
#include "Game.h"
#include "Menu.h"

#include <SFML/Graphics.hpp>


class App {
    public:

        // constructor
        App();
        // destructor
        ~App();

        // update the app
        void update();

        // draw the app
        void draw();

        // is app open
        bool isOpen();
    
    private:
        
        // game
        Game* _game;

        // window
        sf::RenderWindow _window;
        sf::Image _icon;
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
        void _updateDynamicAssets();
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
        bool _mouseHolding, _mousePanning;
        unsigned int _mouseAnchorx, _mouseAnchory;
        int _mouseLastx, _mouseLasty;

        // confetti
        ConfettiCannon _confetti;
        bool _confettiLaunched;

};


#endif