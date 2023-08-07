#include "definitions.h"

#include "App.h"
#include "Game.h"
#include "Menu.h"

#include APPSPRITESHEETPATH

#include <SFML/Graphics.hpp>
using namespace sf;

#include <math.h>
using namespace std;

#include <ctime>



App::App(RenderWindow &window){

    // seed random
    srand(time(0));

    // initialize game parameters
    _gameWidth = DEFAULTGAMEWIDTH;
    _gameHeight = DEFAULTGAMEHEIGHT;
    _numBombs = DEFAULTNUMBOMBS;
    // initialize window
    _window = &window;
    _minWindowWidth = (8+2)*TILESIZE;
    _minWindowHeight = (8+4)*TILESIZE;
    _windowWidth = (_gameWidth+2)*TILESIZE;
    _windowHeight = (_gameHeight+4)*TILESIZE;
    _window->setSize(Vector2u(_windowWidth, _windowHeight));
    // create game
    _game = new Game(_gameWidth, _gameHeight, _numBombs);
    // initialize board
    _boardx = TILESIZE;
    _boardy = TILESIZE*3;
    _boardTileSize =  TILESIZE;
    
    // panning
    _holding = false;
    _panning = false;
    // timer
    _timerRunning = false;
    _currTime = 0;
    _startTime = 0;

    // load assets
    _loadAssets();

    // initialize menu
    _menu = new Menu(&_appspritesheet);
    _menuOpen = false;
    _zoomEnabled = DEFAULTZOOMENABLED;
    _menu->addBoolItem(&_zoomEnabled, "Zoom enabled");
    _chordingEnabled = DEFAULTCHORDING;
    _menu->addBoolItem(&_chordingEnabled, "Chording enabled");
    _nextGameWidth = DEFAULTGAMEWIDTH;
    _menu->addIntItem(&_nextGameWidth, "Game width");
    _nextGameHeight = DEFAULTGAMEHEIGHT;
    _menu->addIntItem(&_nextGameHeight, "Game height");
    _nextNumBombs = DEFAULTNUMBOMBS;
    _menu->addIntItem(&_nextNumBombs, "Number of bombs");

    // update all dynamic sprite locations
    _updateSpriteLocations();

}



App::~App(){
    delete _game;
}



void App::draw(){

    // update values
    unsigned int isGameOver = _game->gameOver();
    unsigned int numBombsRemaining = _game->numBombsRemaining();
    // update timer
    if (isGameOver) _timerRunning = false;
    if (_timerRunning) _currTime = time(0) - _startTime;

    // draw background rectangle
    _window->draw(_background);

    // draw game
    RenderStates states;
    states.transform = Transform().translate(_boardx, _boardy).scale(_boardTileSize/TILESIZE, _boardTileSize/TILESIZE);
    _game->draw(*_window, states);
    
    // draw background border
    _drawBorder();

    // draw smiley face
    switch (isGameOver){
        case 0: _window->draw(_happyFaceSprite); break;
        case 1: _window->draw(_sadFaceSprite); break;
        case 2: _window->draw(_coolFaceSprite); break;
        default: break;
    }

    // draw bomb counter
    _window->draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    _window->draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    _window->draw(_digitSprites[2][numBombsRemaining%10]);

    // draw timer
    _window->draw(_digitSprites[3][(_currTime/100)%10]);
    _window->draw(_digitSprites[4][(_currTime/10)%10]);
    _window->draw(_digitSprites[5][_currTime%10]);

    // debug
    // CircleShape x(5, 20);
    // x.setFillColor(Color::Red);
    // x.setPosition(_minBoardx-5, _minBoardy-5);
    // _window->draw(x);
    // x.setPosition(_minBoardx-5, _maxBoardy-5);
    // _window->draw(x);
    // x.setPosition(_maxBoardx-5, _minBoardy-5);
    // _window->draw(x);
    // x.setPosition(_maxBoardx-5, _maxBoardy-5);
    // _window->draw(x);


    if (_menuOpen){
        // dim background
        _window->draw(_backgroundDim);
        // draw menu
        _menu->draw(*_window, _windowWidth);
    }

    return;
}



void App::resize(const Event::SizeEvent newSize){
    // resize window
    _windowWidth = max(_minWindowWidth, newSize.width);
    _windowHeight = max(_minWindowHeight, newSize.height);
    _window->setSize(Vector2u(_windowWidth, _windowHeight));
    _window->setView(View(Vector2f(_windowWidth/2, _windowHeight/2), Vector2f(_windowWidth, _windowHeight)));
    // make sure board is still in view
    if (!_zoomEnabled) _resetBoardView();
    _keepBoardInView();
    // update sprite locations
    _updateSpriteLocations();
    return;
}



void App::zoom(const Event::MouseWheelScrollEvent mouse){
    if (_zoomEnabled){
        // check that mouse is on the board
        if (mouse.x < (int)_boardx || mouse.x > _boardx + _boardTileSize*_gameWidth) return;
        if (mouse.y < (int)_boardy || mouse.y > _boardy + _boardTileSize*_gameHeight) return;
        // zoom
        unsigned int oldsize = _boardTileSize;
        _boardTileSize *= 1 + mouse.delta*SCROLLSPEED/100;
        _boardx -= (mouse.x-_boardx)*(_boardTileSize/oldsize - 1);
        _boardy -= (mouse.y-_boardy)*(_boardTileSize/oldsize - 1);
        // make sure board is still in view
        _keepBoardInView();
    }
    return;
}



void App::mouseClick(const Event::MouseButtonEvent mouse){
    if (_menuOpen){
        if (mouse.button == Mouse::Left && _menu->getBounds(_windowWidth).contains(mouse.x, mouse.y)){
            if (_menu->click(mouse)){
                _boundMenuOptions();
            }
        } else {
            // clicked outside menu, so close menu
            _menuOpen = false;
        }
    } else if (_zoomEnabled) {
        // start holding
        _holding = true;
        _panx = mouse.x;
        _pany = mouse.y;
        return;
    }
}



void App::mouseRelease(const Event::MouseButtonEvent mouse){
    if (!_menuOpen){
        if (!_panning){
            // clicked on smiley face
            if (mouse.button == Mouse::Left && _happyFaceSprite.getGlobalBounds().contains(mouse.x, mouse.y)){
                // delete old game
                delete _game;
                // reset values
                _gameWidth = _nextGameWidth;
                _gameHeight = _nextGameHeight;
                _numBombs = _nextNumBombs;
                _currTime = 0;
                _timerRunning = false;
                // make new game
                _game = new Game(_gameWidth, _gameHeight, _numBombs);
                _resetBoardView();
                // draw new game
                this->draw();
            }
            // clicked on game feild
            IntRect gameFeild(TILESIZE, 3*TILESIZE, _windowWidth-2*TILESIZE, _windowHeight-4*TILESIZE); 
            if (!_game->gameOver() && gameFeild.contains(mouse.x, mouse.y)){ 
                // start timer if not already started
                if (!_timerRunning){
                    _startTime = time(0);
                    _timerRunning = true;
                }
                // get transformed coords
                Transform boardTransform = Transform().translate(_boardx, _boardy).scale(_boardTileSize/TILESIZE, _boardTileSize/TILESIZE);
                Vector2f transformedMouse = boardTransform.getInverse().transformPoint(Vector2f(mouse.x, mouse.y));
                int tilex = transformedMouse.x / TILESIZE;
                int tiley = transformedMouse.y / TILESIZE;
                // click
                _game->click(mouse, tilex, tiley, _chordingEnabled);
            }
        }
        _holding = false;
        _panning = false;
    }
    return;
}



void App::mouseMove(const Event::MouseMoveEvent mouse){
    if (_zoomEnabled){
        if (_panning){
            // move board
            _boardx += mouse.x - _lastMousex;
            _boardy += mouse.y - _lastMousey;
            _keepBoardInView();
        } else if (_holding && sqrt(pow(mouse.x-_panx, 2) + pow(mouse.y-_pany, 2)) > MINPANDISTANCE){
            _panning = true;
            _boardx += mouse.x-_panx;
            _boardy += mouse.y-_pany;
            _keepBoardInView();
        }
        _lastMousex = mouse.x;
        _lastMousey = mouse.y;
    }
    return;
}



void App::keyPress(const Event::KeyEvent key){
    if (key.code == 36){ // esc
        _menuOpen = !_menuOpen;
    }
    return;
}



unsigned int App::windowWidth() const { return _windowWidth; }
unsigned int App::windowHeight() const { return _windowHeight; }


void App::_updateSpriteLocations(){
    // uodate background
    _background.setSize(Vector2f(_windowWidth, _windowHeight));
    // update background dim
    _backgroundDim.setSize(Vector2f(_windowWidth, _windowHeight));
    // update faces
    _happyFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2.);
    _coolFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2.);
    _sadFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2.);
    // update bomb counter digits
    for (int digit = 0; digit < 10; digit++){
        _digitSprites[3][digit].setPosition(_windowWidth-TILESIZE*(15./4), TILESIZE*(3./4));
        _digitSprites[4][digit].setPosition(_windowWidth-TILESIZE*(11./4), TILESIZE*(3./4));
        _digitSprites[5][digit].setPosition(_windowWidth-TILESIZE*(7./4), TILESIZE*(3./4));
    }
    return;
}


void App::_updateBoardRestrictions(){
    float boardRatio = (float)_gameWidth / (float)_gameHeight;
    float windowRatio = (float)(_windowWidth-2*TILESIZE) / (float)(_windowHeight-4*TILESIZE);
    if (windowRatio > boardRatio){ // wide
        _minBoardx = (_windowWidth/2) - ((_windowHeight-4*TILESIZE) * boardRatio / 2.);
        _maxBoardx = (_windowWidth/2) + ((_windowHeight-4*TILESIZE) * boardRatio / 2.);
        _minBoardy = TILESIZE*3;
        _maxBoardy = _windowHeight-TILESIZE;
    } else if (windowRatio < boardRatio){ // tall
        _minBoardx = TILESIZE;
        _maxBoardx = _windowWidth-TILESIZE;
        _minBoardy = ((_windowHeight+TILESIZE)/2) - ((_windowWidth-2*TILESIZE) / boardRatio / 2.);
        _maxBoardy = ((_windowHeight+TILESIZE)/2) + ((_windowWidth-2*TILESIZE) / boardRatio / 2.);
    } else { // square
        _minBoardx = TILESIZE;
        _maxBoardx = _windowWidth-TILESIZE;
        _minBoardy = TILESIZE*3;
        _maxBoardy = _windowHeight-TILESIZE;
    }
    return;
}


void App::_resetBoardView(){
    _updateBoardRestrictions();
    _boardTileSize = (float)(_maxBoardx-_minBoardx)/(float)_gameWidth;
    _boardx = _minBoardx;
    _boardy = _minBoardy;
}


void App::_keepBoardInView(){
    _updateBoardRestrictions();
    _boardTileSize = max(_boardTileSize, (float)(_maxBoardx-_minBoardx)/_gameWidth);
    _boardx = min(_boardx, _minBoardx);
    _boardx = max((float)_boardx, _maxBoardx-_boardTileSize*_gameWidth);
    _boardy = min(_boardy, _minBoardy);
    _boardy = max((float)_boardy, _maxBoardy-_boardTileSize*_gameHeight);
}


void App::_boundMenuOptions(){
    _nextGameWidth = max(_nextGameWidth, (unsigned int)MINGAMEWIDTH);
    _nextGameWidth = min(_nextGameWidth, (unsigned int)MAXGAMEWIDTH);
    _nextGameHeight = max(_nextGameHeight, (unsigned int)MINGAMEHEIGHT);
    _nextGameHeight = min(_nextGameHeight, (unsigned int)MAXGAMEHEIGHT);
    _nextNumBombs = max((int)_nextNumBombs, 1);
    _nextNumBombs = min(_nextNumBombs, (unsigned int)MAXNUMBOMBS);
    _nextNumBombs = min(_nextNumBombs, _nextGameWidth*_nextGameHeight);
}


void App::_drawBorder(){
    // top left corner
    _tl.setPosition(0, 0);
    _window->draw(_tl);
    // top row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _t.setPosition(x, 0);
        _window->draw(_t);
    }
    // top right corner
    _tr.setPosition(_windowWidth-TILESIZE, 0);
    _window->draw(_tr);
    // top left side
    _l.setPosition(0, TILESIZE);
    _window->draw(_l);
    // top middle row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _m.setPosition(x, TILESIZE);
        _window->draw(_m);
    }
    // top right side
    _r.setPosition(_windowWidth-TILESIZE, TILESIZE);
    _window->draw(_r);
    // top left grid corner
    _tlg.setPosition(0, TILESIZE*2);
    _window->draw(_tlg);
    // top grid row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _tg.setPosition(x, TILESIZE*2);
        _window->draw(_tg);
    }
    // top right grid corner
    _trg.setPosition(_windowWidth-TILESIZE, TILESIZE*2);
    _window->draw(_trg);
    // left grid edge
    for (unsigned int y = TILESIZE*3; y < _windowHeight-TILESIZE; y += TILESIZE){
        _lg.setPosition(0, y);
        _window->draw(_lg);
    }
    // right grid edge
    for (unsigned int y = TILESIZE*3; y < _windowHeight-TILESIZE; y += TILESIZE){
        _rg.setPosition(_windowWidth-TILESIZE, y);
        _window->draw(_rg);
    }
    // bottom left grid corner
    _blg.setPosition(0, _windowHeight-TILESIZE);
    _window->draw(_blg);
    // bottom grid row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _bg.setPosition(x, _windowHeight-TILESIZE);
        _window->draw(_bg);
    }
    // bottom right grid corner
    _brg.setPosition(_windowWidth-TILESIZE, _windowHeight-TILESIZE);
    _window->draw(_brg);
}


void App::_loadAssets(){

    // background rectangle
    _background.setFillColor(Color(90, 90, 90));
    _background.setPosition(0, 0);
    // background shading
    _backgroundDim.setFillColor(Color(0, 0, 0, 80));
    _backgroundDim.setPosition(0, 0);
    
    // load texture from sprite sheet
    _appspritesheet.loadFromMemory(APPSPRITESHEETFILE, sizeof(APPSPRITESHEETFILE));
    _appspritesheet.setSmooth(false);

    const Vector2f scale((float)TILESIZE/SPRITETILESIZE, (float)TILESIZE/SPRITETILESIZE);
    // top left
    _tl.setTexture(_appspritesheet);
    _tl.setTextureRect(IntRect(0, 0, SPRITETILESIZE, SPRITETILESIZE));
    _tl.setScale(scale);
    // top
    _t.setTexture(_appspritesheet);
    _t.setTextureRect(IntRect(16, 0, SPRITETILESIZE, SPRITETILESIZE));
    _t.setScale(scale);
    // top right
    _tr.setTexture(_appspritesheet);
    _tr.setTextureRect(IntRect(32, 0, SPRITETILESIZE, SPRITETILESIZE));
    _tr.setScale(scale);
    // left
    _l.setTexture(_appspritesheet);
    _l.setTextureRect(IntRect(0, 16, SPRITETILESIZE, SPRITETILESIZE));
    _l.setScale(scale);
    // middle
    _m.setTexture(_appspritesheet);
    _m.setTextureRect(IntRect(16, 16, SPRITETILESIZE, SPRITETILESIZE));
    _m.setScale(scale);
    // right
    _r.setTexture(_appspritesheet);
    _r.setTextureRect(IntRect(32, 16, SPRITETILESIZE, SPRITETILESIZE));
    _r.setScale(scale);
    // bottom left
    _bl.setTexture(_appspritesheet);
    _bl.setTextureRect(IntRect(0, 32, SPRITETILESIZE, SPRITETILESIZE));
    _bl.setScale(scale);
    // bottom middle
    _b.setTexture(_appspritesheet);
    _b.setTextureRect(IntRect(16, 32, SPRITETILESIZE, SPRITETILESIZE));
    _b.setScale(scale);
    // bottom right
    _br.setTexture(_appspritesheet);
    _br.setTextureRect(IntRect(32, 32, SPRITETILESIZE, SPRITETILESIZE));
    _br.setScale(scale);
    // top left grid corner
    _tlg.setTexture(_appspritesheet);
    _tlg.setTextureRect(IntRect(0, 48, SPRITETILESIZE, SPRITETILESIZE));
    _tlg.setScale(scale);
    // top grid middle
    _tg.setTexture(_appspritesheet);
    _tg.setTextureRect(IntRect(16, 48, SPRITETILESIZE, SPRITETILESIZE));
    _tg.setScale(scale);
    // top right grid corner
    _trg.setTexture(_appspritesheet);
    _trg.setTextureRect(IntRect(32, 48, SPRITETILESIZE, SPRITETILESIZE));
    _trg.setScale(scale);
    // left grid side
    _lg.setTexture(_appspritesheet);
    _lg.setTextureRect(IntRect(0, 64, SPRITETILESIZE, SPRITETILESIZE));
    _lg.setScale(scale);
    // right grid side
    _rg.setTexture(_appspritesheet);
    _rg.setTextureRect(IntRect(32, 64, SPRITETILESIZE, SPRITETILESIZE));
    _rg.setScale(scale);
    // bottom left grid corner
    _blg.setTexture(_appspritesheet);
    _blg.setTextureRect(IntRect(0, 80, SPRITETILESIZE, SPRITETILESIZE));
    _blg.setScale(scale);
    // bottom grid middle
    _bg.setTexture(_appspritesheet);
    _bg.setTextureRect(IntRect(16, 80, SPRITETILESIZE, SPRITETILESIZE));
    _bg.setScale(scale);
    // bottom right grid corner
    _brg.setTexture(_appspritesheet);
    _brg.setTextureRect(IntRect(32, 80, SPRITETILESIZE, SPRITETILESIZE));
    _brg.setScale(scale);
    
    // load happy face
    _happyFaceSprite.setTexture(_appspritesheet);
    _happyFaceSprite.setScale(scale);
    _happyFaceSprite.setTextureRect(IntRect(0, 96, 32, 32));
    _happyFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    // load cool face
    _coolFaceSprite.setTexture(_appspritesheet);
    _coolFaceSprite.setScale(scale);
    _coolFaceSprite.setTextureRect(IntRect(32, 96, 32, 32));
    _coolFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    // load sad face
    _sadFaceSprite.setTexture(_appspritesheet);
    _sadFaceSprite.setScale(scale);
    _sadFaceSprite.setTextureRect(IntRect(64, 96, 32, 32));
    _sadFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);

    // load digits
    IntRect spriteLocations[10] = {
        IntRect(48, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(64, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(80, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(96, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(112, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(48, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(64, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(80, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(96, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
        IntRect(112, 24, SPRITETILESIZE, SPRITETILESIZE*1.5)
    };
    Sprite digitSprite(_appspritesheet);
    digitSprite.setScale(scale);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        digitSprite.setPosition(TILESIZE*(3./4), TILESIZE*(3./4));
        _digitSprites[0][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        digitSprite.setPosition(TILESIZE*(7./4), TILESIZE*(3./4));
        _digitSprites[1][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        digitSprite.setPosition(TILESIZE*(11./4), TILESIZE*(3./4));
        _digitSprites[2][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        _digitSprites[3][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        _digitSprites[4][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        _digitSprites[5][digit] = digitSprite;
    }

    return;
}
