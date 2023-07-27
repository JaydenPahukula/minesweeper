
#include "App.h"
#include "definitions.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

#include <ctime>
#include <cmath>



App::App(){

    // seed random
    srand(time(0));
    
    // initialize window
    _minWindowWidth = (8+2)*TILESIZE;
    _minWindowHeight = (8+4)*TILESIZE;
    _windowWidth = (GAMEWIDTH+2)*TILESIZE;
    _windowHeight = (GAMEHEIGHT+4)*TILESIZE;
    // create game
    _game = new Game;
    // initialize board
    _updateBoardRestrictions();
    _boardx = TILESIZE;
    _boardy = TILESIZE*3;
    _boardTileSize =  TILESIZE;
    // initialize menu
    _menuOpen = false;
    _menuWidth = 9*TILESIZE;
    _menuHeight = 450;
    _zoomEnabled = DEFAULTZOOMENABLED;
    _chordingEnabled = DEFAULTCHORDING;
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



void App::draw(RenderWindow &window){

    // update values
    unsigned int isGameOver = _game->gameOver();
    unsigned int numBombsRemaining = _game->numBombsRemaining();
    // update timer
    if (isGameOver) _timerRunning = false;
    if (_timerRunning) _currTime = time(0) - _startTime;

    // draw background rectangle
    window.draw(_background);

    // draw game
    RenderStates states;
    states.transform = Transform().translate(_boardx, _boardy).scale(_boardTileSize/TILESIZE, _boardTileSize/TILESIZE);
    _game->draw(window, states);
    
    // draw background border
    _drawBorder(window);

    // draw smiley face
    switch (isGameOver){
        case 0: window.draw(_happyFaceSprite); break;
        case 1: window.draw(_sadFaceSprite); break;
        case 2: window.draw(_coolFaceSprite); break;
        default: break;
    }

    // draw bomb counter
    window.draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    window.draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    window.draw(_digitSprites[2][numBombsRemaining%10]);

    // draw timer
    window.draw(_digitSprites[3][(_currTime/100)%10]);
    window.draw(_digitSprites[4][(_currTime/10)%10]);
    window.draw(_digitSprites[5][_currTime%10]);

    // draw menu
    if (_menuOpen){
        _drawMenu(window);
    }

    return;
}



void App::resize(const Event::SizeEvent newSize, RenderWindow &window){
    // resize window
    _windowWidth = max(_minWindowWidth, newSize.width);
    _windowHeight = max(_minWindowHeight, newSize.height);
    window.setSize(Vector2u(_windowWidth, _windowHeight));
    window.setView(View(Vector2f(_windowWidth/2, _windowHeight/2), Vector2f(_windowWidth, _windowHeight)));
    // make sure board is still in view
    _keepBoardInView();
    // update sprite locations
    _updateSpriteLocations();
    return;
}



void App::zoom(const Event::MouseWheelScrollEvent mouse){
    if (_zoomEnabled){
        // check that mouse is on the board
        if (mouse.x < (int)_boardx || mouse.x > _boardx + _boardTileSize*GAMEWIDTH) return;
        if (mouse.y < (int)_boardy || mouse.y > _boardy + _boardTileSize*GAMEHEIGHT) return;
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
            if (mouse.button == Mouse::Left && mouse.x > _windowWidth/2.-TILESIZE && mouse.x < _windowWidth/2.+TILESIZE && mouse.y > 16 && mouse.y < 80){
                _game->reset();
                _resetBoardView();
                _currTime = 0;
                _timerRunning = false;
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
        } else if (_holding && sqrt(pow(mouse.x-_panx, 2) + pow(mouse.y-_pany, 2)) > MINPANDISTANCE){
            _panning = true;
            _boardx += mouse.x-_panx;
            _boardy += mouse.y-_pany;
        }
        _keepBoardInView();
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
    _happyFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    _coolFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    _sadFaceSprite.setPosition(_windowWidth/2.-TILESIZE, TILESIZE/2);
    // update bomb counter digits
    for (int digit = 0; digit < 10; digit++){
        _digitSprites[3][digit].setPosition(_windowWidth-TILESIZE*(15./4), TILESIZE*(3./4));
        _digitSprites[4][digit].setPosition(_windowWidth-TILESIZE*(11./4), TILESIZE*(3./4));
        _digitSprites[5][digit].setPosition(_windowWidth-TILESIZE*(7./4), TILESIZE*(3./4));
    }
    return;
}


void App::_updateBoardRestrictions(){
    float boardRatio = (float)GAMEWIDTH / (float)GAMEHEIGHT;
    float windowRatio = (float)(_windowWidth-2*TILESIZE) / (float)(_windowHeight-4*TILESIZE);
    if (windowRatio > boardRatio){ // wide
        _minBoardx = (_windowWidth/2) - ((_windowHeight-4*TILESIZE) / boardRatio / 2);
        _maxBoardx = (_windowWidth/2) + ((_windowHeight-4*TILESIZE) / boardRatio / 2);
        _minBoardy = TILESIZE*3;
        _maxBoardy = _windowHeight-TILESIZE;
    } else if (windowRatio < boardRatio){ // tall
        _minBoardx = TILESIZE;
        _maxBoardx = _windowWidth-TILESIZE;
        _minBoardy = ((_windowHeight+TILESIZE)/2) - ((_windowWidth-2*TILESIZE) * boardRatio / 2);
        _maxBoardy = ((_windowHeight+TILESIZE)/2) + ((_windowWidth-2*TILESIZE) * boardRatio / 2);
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
    _boardTileSize = (_maxBoardx-_minBoardx)/GAMEWIDTH;
    _boardx = _minBoardx;
    _boardy = _minBoardy;
}


void App::_keepBoardInView(){
    _updateBoardRestrictions();
    _boardTileSize = max(_boardTileSize, (float)(_maxBoardx-_minBoardx)/GAMEWIDTH);
    _boardx = min(_boardx, _minBoardx);
    _boardx = max((float)_boardx, _maxBoardx-_boardTileSize*GAMEWIDTH);
    _boardy = min(_boardy, _minBoardy);
    _boardy = max((float)_boardy, _maxBoardy-_boardTileSize*GAMEHEIGHT);
}


void App::_drawMenu(RenderWindow &window){

    // shading
    window.draw(_menuShading);

    // background
    for (unsigned int y = 0; y < _menuHeight-TILESIZE; y += TILESIZE){
        // middle area
        for (unsigned int x = _menux+TILESIZE; x < _menux+_menuWidth-TILESIZE; x += TILESIZE){
            _m.setPosition(x, y);
            window.draw(_m);
        }
        // left edge
        _l.setPosition(_menux, y);
        window.draw(_l);
        // right edge
        _r.setPosition(_menux+_menuWidth-TILESIZE, y);
        window.draw(_r);
    }
    // bottom edge
    for (unsigned int x = _menux+TILESIZE; x < _menux+_menuWidth-TILESIZE; x += TILESIZE){
        _b.setPosition(x, _menuHeight-TILESIZE);
        window.draw(_b);
    }
    // corners
    _bl.setPosition(_menux, _menuHeight-TILESIZE);
    window.draw(_bl);
    _br.setPosition(_menux+_menuWidth-TILESIZE, _menuHeight-TILESIZE);
    window.draw(_br);

    // title
    window.draw(_menuTitleText);

    // option 1: zoom enabled
    _menuText.setPosition(_menux+TILESIZE/2, 1.7*TILESIZE);
    _menuText.setString("Zooming enabled");
    window.draw(_menuText);
    if (_zoomEnabled){
        _checkBoxTrue.setPosition(_menux+TILESIZE*7.5, 1.5*TILESIZE);
        window.draw(_checkBoxTrue);
    } else {
        _checkBoxFalse.setPosition(_menux+TILESIZE*7.5, 1.5*TILESIZE);
        window.draw(_checkBoxFalse);
    }

    // option 2: chording enabled
    _menuText.setPosition(_menux+TILESIZE/2, 2.7*TILESIZE);
    _menuText.setString("Chording enabled");
    window.draw(_menuText);
    if (_chordingEnabled){
        _checkBoxTrue.setPosition(_menux+TILESIZE*7.5, 2.5*TILESIZE);
        window.draw(_checkBoxTrue);
    } else {
        _checkBoxFalse.setPosition(_menux+TILESIZE*7.5, 2.5*TILESIZE);
        window.draw(_checkBoxFalse);
    }
    return;
}


void App::_drawBorder(RenderWindow &window){
    // top left corner
    _tl.setPosition(0, 0);
    window.draw(_tl);
    // top row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _t.setPosition(x, 0);
        window.draw(_t);
    }
    // top right corner
    _tr.setPosition(_windowWidth-TILESIZE, 0);
    window.draw(_tr);
    // top left side
    _l.setPosition(0, TILESIZE);
    window.draw(_l);
    // top middle row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _m.setPosition(x, TILESIZE);
        window.draw(_m);
    }
    // top right side
    _r.setPosition(_windowWidth-TILESIZE, TILESIZE);
    window.draw(_r);
    // top left grid corner
    _tlg.setPosition(0, TILESIZE*2);
    window.draw(_tlg);
    // top grid row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _tg.setPosition(x, TILESIZE*2);
        window.draw(_tg);
    }
    // top right grid corner
    _trg.setPosition(_windowWidth-TILESIZE, TILESIZE*2);
    window.draw(_trg);
    // left grid edge
    for (unsigned int y = TILESIZE*3; y < _windowHeight-TILESIZE; y += TILESIZE){
        _lg.setPosition(0, y);
        window.draw(_lg);
    }
    // right grid edge
    for (unsigned int y = TILESIZE*3; y < _windowHeight-TILESIZE; y += TILESIZE){
        _rg.setPosition(_windowWidth-TILESIZE, y);
        window.draw(_rg);
    }
    // bottom left grid corner
    _blg.setPosition(0, _windowHeight-TILESIZE);
    window.draw(_blg);
    // bottom grid row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _bg.setPosition(x, _windowHeight-TILESIZE);
        window.draw(_bg);
    }
    // bottom right grid corner
    _brg.setPosition(_windowWidth-TILESIZE, _windowHeight-TILESIZE);
    window.draw(_brg);
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

    _checkBoxFalse.setTexture(_appspritesheet);
    _checkBoxFalse.setTextureRect(IntRect(3*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    _checkBoxFalse.setScale(scale, scale);
    _checkBoxTrue.setTexture(_appspritesheet);
    _checkBoxTrue.setTextureRect(IntRect(4*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    _checkBoxTrue.setScale(scale, scale);

    return true;
}
