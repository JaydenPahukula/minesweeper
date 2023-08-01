
#include "App.h"
#include "definitions.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

#include <ctime>
#include <cmath>



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
    // initialize menu
    _menuOpen = false;
    _menuWidth = 9*TILESIZE;
    _menuHeight = 450;
    _zoomEnabled = DEFAULTZOOMENABLED;
    _chordingEnabled = DEFAULTCHORDING;
    _nextGameWidth = DEFAULTGAMEWIDTH;
    _nextGameHeight = DEFAULTGAMEHEIGHT;
    _nextNumBombs = DEFAULTNUMBOMBS;
    
    // panning
    _holding = false;
    _panning = false;
    // timer
    _timerRunning = false;
    _currTime = 0;
    _startTime = 0;

    // load assets
    if (!_loadAssets()){
        cerr << endl << "error loading assets" << endl;
    }

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

    // draw menu
    if (_menuOpen){
        _drawMenu();
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
        Rect<int> menuBox(_menux, 0, _menuWidth, _menuHeight);
        if (mouse.button == Mouse::Left && menuBox.contains(Vector2i(mouse.x, mouse.y))){
            // clicked on menu option 1: zooming enabled
            Rect<int> box1(_menux+TILESIZE*7.5, 1.5*TILESIZE, TILESIZE, TILESIZE);
            if (box1.contains(Vector2i(mouse.x, mouse.y))){
                if (_zoomEnabled) _resetBoardView();
                _zoomEnabled = !_zoomEnabled;
            }
            // clicked on menu option 2: chording enabled
            Rect<int> box2(_menux+TILESIZE*7.5, 2.5*TILESIZE, TILESIZE, TILESIZE);
            if (box2.contains(Vector2i(mouse.x, mouse.y))){
                _chordingEnabled = !_chordingEnabled;
            }
            // clicked on menu option 3: game width
            Rect<int> box3(_menux+TILESIZE*6.2, 3.5*TILESIZE, TILESIZE, TILESIZE);
            if (box3.contains(Vector2i(mouse.x, mouse.y))){
                _nextGameWidth = max((int)_nextGameWidth-1, MINGAMEWIDTH);
            }
            Rect<int> box4(_menux+TILESIZE*7.7, 3.5*TILESIZE, TILESIZE, TILESIZE);
            if (box4.contains(Vector2i(mouse.x, mouse.y))){
                _nextGameWidth = min((int)_nextGameWidth+1, MAXGAMEWIDTH);
            }
            // clicked on menu option 4: game height
            Rect<int> box5(_menux+TILESIZE*6.2, 4.5*TILESIZE, TILESIZE, TILESIZE);
            if (box5.contains(Vector2i(mouse.x, mouse.y))){
                _nextGameHeight = max((int)_nextGameHeight-1, MINGAMEWIDTH);
            }
            Rect<int> box6(_menux+TILESIZE*7.7, 4.5*TILESIZE, TILESIZE, TILESIZE);
            if (box6.contains(Vector2i(mouse.x, mouse.y))){
                _nextGameHeight = min((int)_nextGameHeight+1, MAXGAMEWIDTH);
            }
            // clicked on menu option 5: num bombs
            Rect<int> box7(_menux+TILESIZE*6.2, 5.5*TILESIZE, TILESIZE, TILESIZE);
            if (box7.contains(Vector2i(mouse.x, mouse.y))){
                _nextNumBombs = max((int)_nextNumBombs-1, 1);
            }
            Rect<int> box8(_menux+TILESIZE*7.7, 5.5*TILESIZE, TILESIZE, TILESIZE);
            if (box8.contains(Vector2i(mouse.x, mouse.y))){
                _nextNumBombs = min((int)_nextNumBombs+1, min((int)_nextGameHeight*(int)_nextGameWidth, MAXNUMBOMBS));
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
            if (mouse.button == Mouse::Left && mouse.x > _windowWidth/2.-TILESIZE && mouse.x < _windowWidth/2.+TILESIZE && mouse.y > TILESIZE/2. && mouse.y < TILESIZE*2.5){
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
            if (!_game->gameOver() && mouse.x > TILESIZE && mouse.x < (int)_windowWidth-TILESIZE && mouse.y > TILESIZE*3 && mouse.y < (int)_windowHeight-TILESIZE){ 
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
    if (key.code == 36){
        _menuOpen = !_menuOpen;
    }
    return;
}



unsigned int App::windowWidth() const { return _windowWidth; }
unsigned int App::windowHeight() const { return _windowHeight; }


void App::_updateSpriteLocations(){
    // uodate background
    _background.setSize(Vector2f(_windowWidth, _windowHeight));
    // update menu
    _menuShading.setSize(Vector2f(_windowWidth, _windowHeight));
    _menux = (_windowWidth/2) - (_menuWidth/2);
    _menuTitleText.setPosition(_menux+TILESIZE*3.2, TILESIZE/3);
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


void App::_drawMenu(){

    // shading
    _window->draw(_menuShading);

    // background
    for (unsigned int y = 0; y < _menuHeight-TILESIZE; y += TILESIZE){
        // middle area
        for (unsigned int x = _menux+TILESIZE; x < _menux+_menuWidth-TILESIZE; x += TILESIZE){
            _m.setPosition(x, y);
            _window->draw(_m);
        }
        // left edge
        _l.setPosition(_menux, y);
        _window->draw(_l);
        // right edge
        _r.setPosition(_menux+_menuWidth-TILESIZE, y);
        _window->draw(_r);
    }
    // bottom edge
    for (unsigned int x = _menux+TILESIZE; x < _menux+_menuWidth-TILESIZE; x += TILESIZE){
        _b.setPosition(x, _menuHeight-TILESIZE);
        _window->draw(_b);
    }
    // corners
    _bl.setPosition(_menux, _menuHeight-TILESIZE);
    _window->draw(_bl);
    _br.setPosition(_menux+_menuWidth-TILESIZE, _menuHeight-TILESIZE);
    _window->draw(_br);

    // title
    _window->draw(_menuTitleText);

    // option 1: zoom enabled
    _menuText.setPosition(_menux+TILESIZE/2, 1.7*TILESIZE);
    _menuText.setString("Zooming enabled");
    _window->draw(_menuText);
    if (_zoomEnabled){
        _checkBoxTrue.setPosition(_menux+TILESIZE*7.5, 1.5*TILESIZE);
        _window->draw(_checkBoxTrue);
    } else {
        _checkBoxFalse.setPosition(_menux+TILESIZE*7.5, 1.5*TILESIZE);
        _window->draw(_checkBoxFalse);
    }

    // option 2: chording enabled
    _menuText.setPosition(_menux+TILESIZE/2, 2.7*TILESIZE);
    _menuText.setString("Chording enabled");
    _window->draw(_menuText);
    if (_chordingEnabled){
        _checkBoxTrue.setPosition(_menux+TILESIZE*7.5, 2.5*TILESIZE);
        _window->draw(_checkBoxTrue);
    } else {
        _checkBoxFalse.setPosition(_menux+TILESIZE*7.5, 2.5*TILESIZE);
        _window->draw(_checkBoxFalse);
    }

    // option 3: game width
    _menuText.setPosition(_menux+TILESIZE/2, 3.7*TILESIZE);
    _menuText.setString("Game width");
    _window->draw(_menuText);
    _menuText.setString(to_string(_nextGameWidth));
    _menuText.setPosition(_menux+TILESIZE*7.33-_menuText.getGlobalBounds().width/2., 3.7*TILESIZE);
    _window->draw(_menuText);
    _leftArrow.setPosition(_menux+TILESIZE*6, 3.5*TILESIZE);
    _window->draw(_leftArrow);
    _rightArrow.setPosition(_menux+TILESIZE*7.7, 3.5*TILESIZE);
    _window->draw(_rightArrow);

    // option 4: game height
    _menuText.setPosition(_menux+TILESIZE/2, 4.7*TILESIZE);
    _menuText.setString("Game height");
    _window->draw(_menuText);
    _menuText.setString(to_string(_nextGameHeight));
    _menuText.setPosition(_menux+TILESIZE*7.33-_menuText.getLocalBounds().width/2., 4.7*TILESIZE);
    _window->draw(_menuText);
    _leftArrow.setPosition(_menux+TILESIZE*6, 4.5*TILESIZE);
    _window->draw(_leftArrow);
    _rightArrow.setPosition(_menux+TILESIZE*7.7, 4.5*TILESIZE);
    _window->draw(_rightArrow);

    // option 5: number of bombs
    _menuText.setPosition(_menux+TILESIZE/2, 5.7*TILESIZE);
    _menuText.setString("Number of bombs");
    _window->draw(_menuText);
    _menuText.setString(to_string(_nextNumBombs));
    _menuText.setPosition(_menux+TILESIZE*7.33-_menuText.getLocalBounds().width/2., 5.7*TILESIZE);
    _window->draw(_menuText);
    _leftArrow.setPosition(_menux+TILESIZE*6, 5.5*TILESIZE);
    _window->draw(_leftArrow);
    _rightArrow.setPosition(_menux+TILESIZE*7.7, 5.5*TILESIZE);
    _window->draw(_rightArrow);
    return;
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


bool App::_loadAssets(){

    if (!_font.loadFromFile(FONTFILE)){
        return false;
    }

    _menuShading.setFillColor(Color(0, 0, 0, 80));
    _menuShading.setPosition(0, 0);
    _menuText.setFont(_font);
    _menuText.setFillColor(Color::Black);
    _menuText.setCharacterSize(TILESIZE/2);
    _menuTitleText.setFont(_font);
    _menuTitleText.setFillColor(Color::Black);
    _menuTitleText.setCharacterSize(TILESIZE*3/4);
    _menuTitleText.setPosition(_menux+TILESIZE*3.2, TILESIZE/3);
    _menuTitleText.setString("MENU:");

    // background rectangle
    _background.setFillColor(Color(90, 90, 90));
    _background.setPosition(0, 0);
    _background.setSize(Vector2f(_windowWidth, _windowHeight));
    
    // load texture from sprite sheet
    if(!_appspritesheet.loadFromFile(APPSPRITESHEET)){
        return false;
    }
    _appspritesheet.setSmooth(false);

    const float scale = (float)TILESIZE / SPRITETILESIZE;
    // top left
    _tl.setTexture(_appspritesheet);
    _tl.setTextureRect(IntRect(0, 0, SPRITETILESIZE, SPRITETILESIZE));
    _tl.setScale(scale, scale);
    // top
    _t.setTexture(_appspritesheet);
    _t.setTextureRect(IntRect(16, 0, SPRITETILESIZE, SPRITETILESIZE));
    _t.setScale(scale, scale);
    // top right
    _tr.setTexture(_appspritesheet);
    _tr.setTextureRect(IntRect(32, 0, SPRITETILESIZE, SPRITETILESIZE));
    _tr.setScale(scale, scale);
    // left
    _l.setTexture(_appspritesheet);
    _l.setTextureRect(IntRect(0, 16, SPRITETILESIZE, SPRITETILESIZE));
    _l.setScale(scale, scale);
    // middle
    _m.setTexture(_appspritesheet);
    _m.setTextureRect(IntRect(16, 16, SPRITETILESIZE, SPRITETILESIZE));
    _m.setScale(scale, scale);
    // right
    _r.setTexture(_appspritesheet);
    _r.setTextureRect(IntRect(32, 16, SPRITETILESIZE, SPRITETILESIZE));
    _r.setScale(scale, scale);
    // bottom left
    _bl.setTexture(_appspritesheet);
    _bl.setTextureRect(IntRect(0, 32, SPRITETILESIZE, SPRITETILESIZE));
    _bl.setScale(scale, scale);
    // bottom middle
    _b.setTexture(_appspritesheet);
    _b.setTextureRect(IntRect(16, 32, SPRITETILESIZE, SPRITETILESIZE));
    _b.setScale(scale, scale);
    // bottom right
    _br.setTexture(_appspritesheet);
    _br.setTextureRect(IntRect(32, 32, SPRITETILESIZE, SPRITETILESIZE));
    _br.setScale(scale, scale);
    // top left grid corner
    _tlg.setTexture(_appspritesheet);
    _tlg.setTextureRect(IntRect(0, 48, SPRITETILESIZE, SPRITETILESIZE));
    _tlg.setScale(scale, scale);
    // top grid middle
    _tg.setTexture(_appspritesheet);
    _tg.setTextureRect(IntRect(16, 48, SPRITETILESIZE, SPRITETILESIZE));
    _tg.setScale(scale, scale);
    // top right grid corner
    _trg.setTexture(_appspritesheet);
    _trg.setTextureRect(IntRect(32, 48, SPRITETILESIZE, SPRITETILESIZE));
    _trg.setScale(scale, scale);
    // left grid side
    _lg.setTexture(_appspritesheet);
    _lg.setTextureRect(IntRect(0, 64, SPRITETILESIZE, SPRITETILESIZE));
    _lg.setScale(scale, scale);
    // right grid side
    _rg.setTexture(_appspritesheet);
    _rg.setTextureRect(IntRect(32, 64, SPRITETILESIZE, SPRITETILESIZE));
    _rg.setScale(scale, scale);
    // bottom left grid corner
    _blg.setTexture(_appspritesheet);
    _blg.setTextureRect(IntRect(0, 80, SPRITETILESIZE, SPRITETILESIZE));
    _blg.setScale(scale, scale);
    // bottom grid middle
    _bg.setTexture(_appspritesheet);
    _bg.setTextureRect(IntRect(16, 80, SPRITETILESIZE, SPRITETILESIZE));
    _bg.setScale(scale, scale);
    // bottom right grid corner
    _brg.setTexture(_appspritesheet);
    _brg.setTextureRect(IntRect(32, 80, SPRITETILESIZE, SPRITETILESIZE));
    _brg.setScale(scale, scale);
    
    // load happy face
    _happyFaceSprite.setTexture(_appspritesheet);
    _happyFaceSprite.setScale(scale, scale);
    _happyFaceSprite.setTextureRect(IntRect(0, 96, 32, 32));
    _happyFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    // load cool face
    _coolFaceSprite.setTexture(_appspritesheet);
    _coolFaceSprite.setScale(scale, scale);
    _coolFaceSprite.setTextureRect(IntRect(32, 96, 32, 32));
    _coolFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    // load sad face
    _sadFaceSprite.setTexture(_appspritesheet);
    _sadFaceSprite.setScale(scale, scale);
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
    digitSprite.setScale(scale, scale);
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

    // check box
    _checkBoxFalse.setTexture(_appspritesheet);
    _checkBoxFalse.setTextureRect(IntRect(3*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    _checkBoxFalse.setScale(scale, scale);
    _checkBoxTrue.setTexture(_appspritesheet);
    _checkBoxTrue.setTextureRect(IntRect(4*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    _checkBoxTrue.setScale(scale, scale);

    // left and right arrows
    _leftArrow.setTexture(_appspritesheet);
    _leftArrow.setTextureRect(IntRect(5*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    _leftArrow.setScale(scale, scale);
    _rightArrow.setTexture(_appspritesheet);
    _rightArrow.setTextureRect(IntRect(6*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    _rightArrow.setScale(scale, scale);

    return true;
}
