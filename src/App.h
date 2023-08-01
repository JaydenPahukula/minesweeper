#ifndef APP_H
#define APP_H

#include "Game.h"

#include <SFML/Graphics.hpp>
#include <vector>

#include <ctime>


class App {
    public:

        // constructor
        App(sf::RenderWindow &window);
        // destructor
        ~App();

        // draw the app
        void draw();

        // event handlers
        void resize(const sf::Event::SizeEvent newSize);
        void zoom(const sf::Event::MouseWheelScrollEvent mouse);
        void mouseClick(const sf::Event::MouseButtonEvent mouse);
        void mouseRelease(const sf::Event::MouseButtonEvent mouse);
        void mouseMove(const sf::Event::MouseMoveEvent mouse);
        void keyPress(const sf::Event::KeyEvent key);

        // getters
        unsigned int windowWidth() const;
        unsigned int windowHeight() const;
    
    private:
        
        // game
        Game* _game;
        unsigned int _gameWidth;
        unsigned int _gameHeight;
        unsigned int _numBombs;

        // window
        sf::RenderWindow* _window;
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

        // graphics
        sf::RectangleShape _background;
        sf::Texture _appspritesheet;
        sf::Sprite _tl, _t, _tr, _l, _m, _r, _bl, _b, _br, _tlg, _tg, _trg, _lg, _rg, _blg, _bg, _brg;
        sf::Sprite _happyFaceSprite, _coolFaceSprite, _sadFaceSprite;
        sf::Sprite _digitSprites[6][10];
        bool _loadAssets();
        void _updateSpriteLocations();
        void _drawBorder();

        // menu
        bool _menuOpen;
        unsigned int _menux, _menuWidth, _menuHeight;
        sf::Font _font, _boldFont;
        sf::Text _menuText, _menuTitleText;
        sf::RectangleShape _menuShading;
        void _drawMenu();
        // menu options
        bool _zoomEnabled; sf::Sprite _zoomEnabledCheckboxTrue; sf::Sprite _zoomEnabledCheckboxFalse;
        bool _chordingEnabled; sf::Sprite _chordingEnabledCheckboxTrue; sf::Sprite _chordingEnabledCheckboxFalse;
        unsigned int _nextGameWidth; sf::Sprite _gameWidthDownArrow; sf::Sprite _gameWidthUpArrow;
        unsigned int _nextGameHeight; sf::Sprite _gameHeightDownArrow; sf::Sprite _gameHeightUpArrow;
        unsigned int _nextNumBombs; sf::Sprite _numBombsDownArrow; sf::Sprite _numBombsUpArrow;

        // mouse/panning
        bool _holding, _panning;
        unsigned int _panx, _pany;
        int _lastMousex, _lastMousey;

        // timer
        bool _timerRunning;
        time_t _startTime, _currTime;

};


#endif