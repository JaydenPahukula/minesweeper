#include "definitions.h"

#include "App.h"
#include "Game.h"
#include "Menu.h"
#include "Timer.h"
#include "Confetti.h"

#include APPSPRITESHEETPATH
#include ICONPATH

#include <SFML/Graphics.hpp>
using namespace sf;

#include <math.h>
using namespace std;

#include <cstdlib>
#include <ctime>



App::App(){

    // seed random
    srand(time(0));

    // create game
    _game = new Game(DEFAULTGAMEWIDTH, DEFAULTGAMEHEIGHT, DEFAULTNUMBOMBS, DEFAULTAUTOOPENING);

    // initialize window
    _minWindowWidth = (DEFAULTGAMEWIDTH+2)*TILESIZE;
    _minWindowHeight = (DEFAULTGAMEHEIGHT+4)*TILESIZE;
    _window.create(VideoMode(_minWindowWidth, _minWindowHeight), "Minesweeper", Style::Default);
    _window.setFramerateLimit(60);
    // set window icon
    _icon.loadFromMemory(ICONFILE, sizeof(ICONFILE));
    _window.setIcon(_icon.getSize().x, _icon.getSize().y, _icon.getPixelsPtr());

    // initialize board
    _boardx = TILESIZE;
    _boardy = TILESIZE*3;
    _boardTileSize =  TILESIZE;
    
    // panning
    _holding = false;
    _panning = false;

    // timer
    _timer = Timer();

    // confetti
    _confetti = ConfettiCannon();
    _confettiLaunched = false;

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
    _autoOpeningEnabled = DEFAULTAUTOOPENING;
    _menu->addBoolItem(&_autoOpeningEnabled, "Automatic opening");

    _menu->updateAssets(TILESIZE, _window.getSize().y);

    // update all dynamic sprite locations
    _updateAssets();

}



App::~App(){
    delete _game;
}



void App::draw(){
    
    // handle events
    Event event;
    while (_window.pollEvent(event)){ _handleEvent(event); }
    // update game values
    unsigned int isGameOver = _game->gameOver();
    unsigned int numBombsRemaining = _game->numBombsRemaining();
    // update timer
    if (isGameOver) _timer.stop();
    int seconds = _timer.seconds() % 1000;

    // clear window
    _window.clear();

    // draw background rectangle
    _window.draw(_background);

    // draw game
    RenderStates boardState;
    boardState.transform = Transform().translate(_boardx, _boardy).scale(_boardTileSize/SPRITETILESIZE, _boardTileSize/SPRITETILESIZE);
    _game->draw(_window, boardState);
    
    // draw background border
    _drawBorder();

    // draw smiley face
    switch (isGameOver){
        case 0: _window.draw(_happyFaceSprite); break;
        case 1: _window.draw(_sadFaceSprite); break;
        case 2: _window.draw(_coolFaceSprite); break;
        default: break;
    }

    // draw bomb counter
    _window.draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    _window.draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    _window.draw(_digitSprites[2][numBombsRemaining%10]);

    // draw timer
    _window.draw(_digitSprites[3][(seconds/100)%10]);
    _window.draw(_digitSprites[4][(seconds/10)%10]);
    _window.draw(_digitSprites[5][seconds%10]);

    // draw confetti
    if (isGameOver == 2){
        if (!_confettiLaunched){
            _confetti.launch(Vector2f(_window.getSize().x/2, -20));
            _confettiLaunched = true;
        }
        _confetti.draw(_window);
    }

    if (_menuOpen){
        // dim background
        _window.draw(_backgroundDim);
        // draw menu
        _menu->draw(_window);
    }

    // display everything
    _window.display();

    return;
}



bool App::isOpen(){
    return _window.isOpen();
}



void App::_handleEvent(const Event e){
    if(e.type == Event::MouseButtonPressed){            // mouse clicked
        _mouseClick(e.mouseButton);
    } else if(e.type == Event::MouseButtonReleased){    // mouse released
        _mouseRelease(e.mouseButton);
    } else if(e.type == Event::MouseMoved){             // mouse moved
        _mouseMove(e.mouseMove);
    } else if(e.type == Event::MouseWheelScrolled){     // mouse scrolled
        _zoom(e.mouseWheelScroll);
    } else if(e.type == Event::KeyPressed){             // key pressed
        _keyPress(e.key);
    } else if(e.type == Event::Resized){                // window resized
        _resize(e.size);
    } else if(e.type == Event::Closed){                 // window closed
        _window.close();
    }
}



void App::_keyPress(const Event::KeyEvent key){
    if (key.code == 36){ // esc
        _menuOpen = !_menuOpen;
    }
    return;
}



void App::_mouseClick(const Event::MouseButtonEvent mouse){
    if (_menuOpen){
        if (mouse.button == Mouse::Left && _menu->getBounds().contains(mouse.x, mouse.y)){
            if (_menu->click(mouse)){
                _boundMenuOptions();
                _updateAssets();
                _boundWindowSize();
                _resetBoardView();
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



void App::_mouseMove(const Event::MouseMoveEvent mouse){
    if (_zoomEnabled){
        if (_panning){
            // move board
            _boardx += mouse.x - _lastMousex;
            _boardy += mouse.y - _lastMousey;
            _boundBoardView();
        } else if (_holding && sqrt(pow(mouse.x-_panx, 2) + pow(mouse.y-_pany, 2)) > MINPANDISTANCE){
            _panning = true;
            _boardx += mouse.x-_panx;
            _boardy += mouse.y-_pany;
            _boundBoardView();
        }
        _lastMousex = mouse.x;
        _lastMousey = mouse.y;
    }
    return;
}



void App::_mouseRelease(const Event::MouseButtonEvent mouse){
    if (!_menuOpen){
        if (!_panning){
            // clicked on smiley face
            if (mouse.button == Mouse::Left && _happyFaceSprite.getGlobalBounds().contains(mouse.x, mouse.y)){
                // delete old game
                delete _game;
                // reset timer
                _timer.reset();
                // reset confetti
                _confettiLaunched = false;
                // make new game
                _game = new Game(_nextGameWidth, _nextGameHeight, _nextNumBombs, _autoOpeningEnabled);
                // start timer if auto opening is enabled
                if (_autoOpeningEnabled) _timer.start();
                // draw new game
                _resetBoardView();
                this->draw();
            }
            // clicked on game feild
            IntRect gameFeild(TILESIZE, 3*TILESIZE, (_window.getSize().x-2)*TILESIZE, (_window.getSize().y-4)*TILESIZE); 
            if (!_game->gameOver() && gameFeild.contains(mouse.x, mouse.y)){ 
                // get transformed coords
                Transform boardTransform = Transform().translate(_boardx, _boardy).scale(_boardTileSize/SPRITETILESIZE, _boardTileSize/SPRITETILESIZE);
                Vector2f transformedMouse = boardTransform.getInverse().transformPoint(Vector2f(mouse.x, mouse.y));
                int tilex = transformedMouse.x / SPRITETILESIZE;
                int tiley = transformedMouse.y / SPRITETILESIZE;
                // check if clicked on a tile
                if (tilex >= 0 && tilex < (int)_game->width() && tiley >= 0 && tiley < (int)_game->height()){
                    // click
                    _game->click(mouse, tilex, tiley, _chordingEnabled);
                    // start timer if not already started
                    _timer.start();
                } 
            }
        }
        _holding = false;
        _panning = false;
    }
    return;
}




void App::_resize(const Event::SizeEvent newSize){
    // resize window
    _window.setSize(Vector2u(newSize.width, newSize.height));
    // make sure size is valid
    _boundWindowSize();
    // make sure board is still in view
    if (!_zoomEnabled) _resetBoardView();
    _boundBoardView();
    // update sprite locations
    _updateAssets();
    return;
}




void App::_zoom(const Event::MouseWheelScrollEvent mouse){
    if (_zoomEnabled){
        // check that mouse is on the board
        if (mouse.x < (int)_boardx || mouse.x > _boardx + _boardTileSize*_game->width()) return;
        if (mouse.y < (int)_boardy || mouse.y > _boardy + _boardTileSize*_game->height()) return;
        // zoom
        unsigned int oldsize = _boardTileSize;
        _boardTileSize *= 1 + mouse.delta*SCROLLSPEED/100;
        _boardx -= (mouse.x-_boardx)*(_boardTileSize/oldsize - 1);
        _boardy -= (mouse.y-_boardy)*(_boardTileSize/oldsize - 1);
        // make sure board is still in view
        _boundBoardView();
    }
    return;
}



void App::_boundWindowSize(){
    _minWindowWidth = (_game->width()+2)*TILESIZE;
    _minWindowHeight = (_game->height()+4)*TILESIZE;
    Vector2u boundedSize(max(_window.getSize().x, _minWindowWidth), max(_window.getSize().y, _minWindowHeight));
    _window.setSize(boundedSize);
    _window.setView(View(Vector2f(_window.getSize().x/2., _window.getSize().y/2.), Vector2f(_window.getSize().x, _window.getSize().y)));
}



void App::_updateAssets(){
    const Vector2f scale((float)TILESIZE/SPRITETILESIZE, (float)TILESIZE/SPRITETILESIZE);
    // update background
    _background.setSize(Vector2f(_window.getSize().x, _window.getSize().y));
    // update background dim
    _backgroundDim.setSize(Vector2f(_window.getSize().x, _window.getSize().y));
    // update border
    _tl.setScale(scale);
    _t.setScale(scale);
    _tr.setScale(scale);
    _l.setScale(scale);
    _m.setScale(scale);
    _r.setScale(scale);
    _bl.setScale(scale);
    _b.setScale(scale);
    _br.setScale(scale);
    _tlg.setScale(scale);
    _tg.setScale(scale);
    _trg.setScale(scale);
    _lg.setScale(scale);
    _rg.setScale(scale);
    _blg.setScale(scale);
    _bg.setScale(scale);
    _brg.setScale(scale);
    // update faces
    _happyFaceSprite.setScale(scale);
    _happyFaceSprite.setPosition(_window.getSize().x/2.-TILESIZE, TILESIZE/2.);
    _coolFaceSprite.setScale(scale);
    _coolFaceSprite.setPosition(_window.getSize().x/2.-TILESIZE, TILESIZE/2.);
    _sadFaceSprite.setScale(scale);
    _sadFaceSprite.setPosition(_window.getSize().x/2.-TILESIZE, TILESIZE/2.);
    // update digits
    for (int digit = 0; digit < 10; digit++){
        _digitSprites[0][digit].setScale(scale);
        _digitSprites[0][digit].setPosition(TILESIZE*(3./4), TILESIZE*(3./4));
        _digitSprites[1][digit].setScale(scale);
        _digitSprites[1][digit].setPosition(TILESIZE*(7./4), TILESIZE*(3./4));
        _digitSprites[2][digit].setScale(scale);
        _digitSprites[2][digit].setPosition(TILESIZE*(11./4), TILESIZE*(3./4));
        _digitSprites[3][digit].setScale(scale);
        _digitSprites[3][digit].setPosition(_window.getSize().x-TILESIZE*(15./4), TILESIZE*(3./4));
        _digitSprites[4][digit].setScale(scale);
        _digitSprites[4][digit].setPosition(_window.getSize().x-TILESIZE*(11./4), TILESIZE*(3./4));
        _digitSprites[5][digit].setScale(scale);
        _digitSprites[5][digit].setPosition(_window.getSize().x-TILESIZE*(7./4), TILESIZE*(3./4));
    }
    // update menu
    _menu->updateAssets(TILESIZE, _window.getSize().x);
    return;
}


void App::_updateBoardBounds(){
    const float boardRatio = (float)_game->width() / (float)_game->height();
    const float windowRatio = (float)(_window.getSize().x-2*TILESIZE) / (float)(_window.getSize().y-4*TILESIZE);
    if (windowRatio > boardRatio){ // wide
        _minBoardx = (_window.getSize().x/2) - ((_window.getSize().y-4*TILESIZE) * boardRatio / 2.);
        _maxBoardx = (_window.getSize().x/2) + ((_window.getSize().y-4*TILESIZE) * boardRatio / 2.);
        _minBoardy = TILESIZE*3;
        _maxBoardy = _window.getSize().y-TILESIZE;
    } else if (windowRatio < boardRatio){ // tall
        _minBoardx = TILESIZE;
        _maxBoardx = _window.getSize().x-TILESIZE;
        _minBoardy = ((_window.getSize().y+TILESIZE)/2) - ((_window.getSize().x-2*TILESIZE) / boardRatio / 2.);
        _maxBoardy = ((_window.getSize().y+TILESIZE)/2) + ((_window.getSize().x-2*TILESIZE) / boardRatio / 2.);
    } else { // square
        _minBoardx = TILESIZE;
        _maxBoardx = _window.getSize().x-TILESIZE;
        _minBoardy = TILESIZE*3;
        _maxBoardy = _window.getSize().y-TILESIZE;
    }
    return;
}


void App::_resetBoardView(){
    _updateBoardBounds();
    _boardTileSize = (float)(_maxBoardx-_minBoardx)/(float)_game->width();
    _boardx = _minBoardx;
    _boardy = _minBoardy;
}


void App::_boundBoardView(){
    _updateBoardBounds();
    _boardTileSize = max(_boardTileSize, (float)(_maxBoardx-_minBoardx)/_game->width());
    _boardx = min(_boardx, _minBoardx);
    _boardx = max((float)_boardx, _maxBoardx-_boardTileSize*_game->width());
    _boardy = min(_boardy, _minBoardy);
    _boardy = max((float)_boardy, _maxBoardy-_boardTileSize*_game->height());
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
    _window.draw(_tl);
    // top row
    for (unsigned int x = TILESIZE; x < _window.getSize().x-TILESIZE; x += TILESIZE){
        _t.setPosition(x, 0);
        _window.draw(_t);
    }
    // top right corner
    _tr.setPosition(_window.getSize().x-TILESIZE, 0);
    _window.draw(_tr);
    // top left side
    _l.setPosition(0, TILESIZE);
    _window.draw(_l);
    // top middle row
    for (unsigned int x = TILESIZE; x < _window.getSize().x-TILESIZE; x += TILESIZE){
        _m.setPosition(x, TILESIZE);
        _window.draw(_m);
    }
    // top right side
    _r.setPosition(_window.getSize().x-TILESIZE, TILESIZE);
    _window.draw(_r);
    // top left grid corner
    _tlg.setPosition(0, TILESIZE*2);
    _window.draw(_tlg);
    // top grid row
    for (unsigned int x = TILESIZE; x < _window.getSize().x-TILESIZE; x += TILESIZE){
        _tg.setPosition(x, TILESIZE*2);
        _window.draw(_tg);
    }
    // top right grid corner
    _trg.setPosition(_window.getSize().x-TILESIZE, TILESIZE*2);
    _window.draw(_trg);
    // left grid edge
    for (unsigned int y = TILESIZE*3; y < _window.getSize().y-TILESIZE; y += TILESIZE){
        _lg.setPosition(0, y);
        _window.draw(_lg);
    }
    // right grid edge
    for (unsigned int y = TILESIZE*3; y < _window.getSize().y-TILESIZE; y += TILESIZE){
        _rg.setPosition(_window.getSize().x-TILESIZE, y);
        _window.draw(_rg);
    }
    // bottom left grid corner
    _blg.setPosition(0, _window.getSize().y-TILESIZE);
    _window.draw(_blg);
    // bottom grid row
    for (unsigned int x = TILESIZE; x < _window.getSize().x-TILESIZE; x += TILESIZE){
        _bg.setPosition(x, _window.getSize().y-TILESIZE);
        _window.draw(_bg);
    }
    // bottom right grid corner
    _brg.setPosition(_window.getSize().x-TILESIZE, _window.getSize().y-TILESIZE);
    _window.draw(_brg);
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

    // top left
    _tl.setTexture(_appspritesheet);
    _tl.setTextureRect(IntRect(0, 0, SPRITETILESIZE, SPRITETILESIZE));
    // top
    _t.setTexture(_appspritesheet);
    _t.setTextureRect(IntRect(16, 0, SPRITETILESIZE, SPRITETILESIZE));
    // top right
    _tr.setTexture(_appspritesheet);
    _tr.setTextureRect(IntRect(32, 0, SPRITETILESIZE, SPRITETILESIZE));
    // left
    _l.setTexture(_appspritesheet);
    _l.setTextureRect(IntRect(0, 16, SPRITETILESIZE, SPRITETILESIZE));
    // middle
    _m.setTexture(_appspritesheet);
    _m.setTextureRect(IntRect(16, 16, SPRITETILESIZE, SPRITETILESIZE));
    // right
    _r.setTexture(_appspritesheet);
    _r.setTextureRect(IntRect(32, 16, SPRITETILESIZE, SPRITETILESIZE));
    // bottom left
    _bl.setTexture(_appspritesheet);
    _bl.setTextureRect(IntRect(0, 32, SPRITETILESIZE, SPRITETILESIZE));
    // bottom middle
    _b.setTexture(_appspritesheet);
    _b.setTextureRect(IntRect(16, 32, SPRITETILESIZE, SPRITETILESIZE));
    // bottom right
    _br.setTexture(_appspritesheet);
    _br.setTextureRect(IntRect(32, 32, SPRITETILESIZE, SPRITETILESIZE));
    // top left grid corner
    _tlg.setTexture(_appspritesheet);
    _tlg.setTextureRect(IntRect(0, 48, SPRITETILESIZE, SPRITETILESIZE));
    // top grid middle
    _tg.setTexture(_appspritesheet);
    _tg.setTextureRect(IntRect(16, 48, SPRITETILESIZE, SPRITETILESIZE));
    // top right grid corner
    _trg.setTexture(_appspritesheet);
    _trg.setTextureRect(IntRect(32, 48, SPRITETILESIZE, SPRITETILESIZE));
    // left grid side
    _lg.setTexture(_appspritesheet);
    _lg.setTextureRect(IntRect(0, 64, SPRITETILESIZE, SPRITETILESIZE));
    // right grid side
    _rg.setTexture(_appspritesheet);
    _rg.setTextureRect(IntRect(32, 64, SPRITETILESIZE, SPRITETILESIZE));
    // bottom left grid corner
    _blg.setTexture(_appspritesheet);
    _blg.setTextureRect(IntRect(0, 80, SPRITETILESIZE, SPRITETILESIZE));
    // bottom grid middle
    _bg.setTexture(_appspritesheet);
    _bg.setTextureRect(IntRect(16, 80, SPRITETILESIZE, SPRITETILESIZE));
    // bottom right grid corner
    _brg.setTexture(_appspritesheet);
    _brg.setTextureRect(IntRect(32, 80, SPRITETILESIZE, SPRITETILESIZE));
    
    // load happy face
    _happyFaceSprite.setTexture(_appspritesheet);
    _happyFaceSprite.setTextureRect(IntRect(0, 96, 32, 32));
    // load cool face
    _coolFaceSprite.setTexture(_appspritesheet);
    _coolFaceSprite.setTextureRect(IntRect(32, 96, 32, 32));
    // load sad face
    _sadFaceSprite.setTexture(_appspritesheet);
    _sadFaceSprite.setTextureRect(IntRect(64, 96, 32, 32));

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
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        _digitSprites[0][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
        _digitSprites[1][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(spriteLocations[digit]);
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
