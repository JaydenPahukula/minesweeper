
#include "App.h"
#include "definitions.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

#include <ctime>



App::App(){

    // initialize values
    _timerRunning = false;
    _currTime = 0;
    _startTime = 0;
    _minWindowWidth = (GAMEWIDTH+2)*TILESIZE;
    _minWindowHeight = (GAMEHEIGHT+4)*TILESIZE;
    _windowWidth = _minWindowWidth;
    _windowHeight = _minWindowHeight;

    // seed random
    srand(time(0));

    // create game
    _game = new Game;

    // load sprites
    if (!_loadBorderSprites()){
        cerr << endl << "error opening game sprite sheet" << endl;
    }

}



App::~App(){
    delete _game;
}



void App::draw(RenderWindow &window){

    // draw background rectangle
    _background.setSize(Vector2f(_windowWidth, _windowHeight));
    window.draw(_background);

    // draw game
    _game->draw(window);

    // draw border
    // top left corner
    _tlc.setPosition(0, 0);
    window.draw(_tlc);
    // top row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _t.setPosition(x, 0);
        window.draw(_t);
    }
    // top right corner
    _trc.setPosition(_windowWidth-TILESIZE, 0);
    window.draw(_trc);
    // top left side
    _tls.setPosition(0, TILESIZE);
    window.draw(_tls);
    // top middle row
    for (unsigned int x = TILESIZE; x < _windowWidth-TILESIZE; x += TILESIZE){
        _tm.setPosition(x, TILESIZE);
        window.draw(_tm);
    }
    // top right side
    _trs.setPosition(_windowWidth-TILESIZE, TILESIZE);
    window.draw(_trs);
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


    // draw smiley face
    unsigned int isGameOver = _game->gameOver();
    switch (isGameOver){
        case 0:
            _happyFaceSprite.setPosition(_windowWidth/2.-TILESIZE, 16);
            window.draw(_happyFaceSprite); break;
        case 1:
            _sadFaceSprite.setPosition(_windowWidth/2.-TILESIZE, 16);
            window.draw(_sadFaceSprite); break;
        case 2:
            _coolFaceSprite.setPosition(_windowWidth/2.-TILESIZE, 16);
            window.draw(_coolFaceSprite); break;
        default:
            break;
    }

    // draw bomb counter
    unsigned int numBombsRemaining = _game->numBombsRemaining();
    _digitSprites[0][(numBombsRemaining/100)%10].setPosition(TILESIZE*(3./4), TILESIZE*(3./4));
    window.draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    _digitSprites[1][(numBombsRemaining/10)%10].setPosition(TILESIZE*(7./4), TILESIZE*(3./4));
    window.draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    _digitSprites[2][numBombsRemaining%10].setPosition(TILESIZE*(11./4), TILESIZE*(3./4));
    window.draw(_digitSprites[2][numBombsRemaining%10]);

    // update timer
    if (isGameOver){
        _timerRunning = false;
    }
    if (_timerRunning){
        _currTime = time(0) - _startTime;
    }

    // draw timer
    _digitSprites[3][(_currTime/100)%10].setPosition(_windowWidth-TILESIZE*(15./4), TILESIZE*(3./4));
    window.draw(_digitSprites[3][(_currTime/100)%10]);
    _digitSprites[4][(_currTime/10)%10].setPosition(_windowWidth-TILESIZE*(11./4), TILESIZE*(3./4));
    window.draw(_digitSprites[4][(_currTime/10)%10]);
    _digitSprites[5][_currTime%10].setPosition(_windowWidth-TILESIZE*(7./4), TILESIZE*(3./4));
    window.draw(_digitSprites[5][_currTime%10]);

    return;
}




void App::resize(const Event::SizeEvent newSize, RenderWindow &window){
    _windowWidth = max(_minWindowWidth, newSize.width);
    _windowHeight = max(_minWindowHeight, newSize.height);
    window.setSize(Vector2u(_windowWidth, _windowHeight));
    window.setView(View(Vector2f(_windowWidth/2, _windowHeight/2), Vector2f((float)_windowWidth, (float)_windowHeight)));
}




void App::click(const Event::MouseButtonEvent mouse){

    // clicked on smiley face
    if (mouse.button == Mouse::Left && mouse.x > _windowWidth/2.-TILESIZE && mouse.x < _windowWidth/2.+TILESIZE && mouse.y > 16 && mouse.y < 80){
        _game->reset();
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

        int tilex = mouse.x / TILESIZE - 1;
        int tiley = mouse.y / TILESIZE - 3;
        _game->click(mouse, tilex, tiley);
    }

    return;
}



unsigned int App::gameWidth() const { return _game->width(); }
unsigned int App::gameHeight() const { return _game->height(); }
unsigned int App::windowWidth() const { return _windowWidth; }
unsigned int App::windowHeight() const { return _windowHeight; }



bool App::_loadBorderSprites(){

    // background rectangle
    _background.setFillColor(Color(90, 90, 90));
    _background.setPosition(0, 0);
    
    // load texture from sprite sheet
    if(!_appspritesheet.loadFromFile(APPSPRITESHEET)){
        return false;
    }
    _appspritesheet.setSmooth(false);

    // top left corner
    _tlc.setTexture(_appspritesheet);
    _tlc.setTextureRect(IntRect(0, 0, SPRITETILESIZE, SPRITETILESIZE));
    _tlc.setScale(SPRITESCALE, SPRITESCALE);
    // top row
    _t.setTexture(_appspritesheet);
    _t.setTextureRect(IntRect(16, 0, SPRITETILESIZE, SPRITETILESIZE));
    _t.setScale(SPRITESCALE, SPRITESCALE);
    // top right corner
    _trc.setTexture(_appspritesheet);
    _trc.setTextureRect(IntRect(32, 0, SPRITETILESIZE, SPRITETILESIZE));
    _trc.setScale(SPRITESCALE, SPRITESCALE);
    // top left side
    _tls.setTexture(_appspritesheet);
    _tls.setTextureRect(IntRect(0, 16, SPRITETILESIZE, SPRITETILESIZE));
    _tls.setScale(SPRITESCALE, SPRITESCALE);
    // top middle row
    _tm.setTexture(_appspritesheet);
    _tm.setTextureRect(IntRect(16, 16, SPRITETILESIZE, SPRITETILESIZE));
    _tm.setScale(SPRITESCALE, SPRITESCALE);
    // top right side
    _trs.setTexture(_appspritesheet);
    _trs.setTextureRect(IntRect(32, 16, SPRITETILESIZE, SPRITETILESIZE));
    _trs.setScale(SPRITESCALE, SPRITESCALE);
    // top left grid corner
    _tlg.setTexture(_appspritesheet);
    _tlg.setTextureRect(IntRect(0, 32, SPRITETILESIZE, SPRITETILESIZE));
    _tlg.setScale(SPRITESCALE, SPRITESCALE);
    // top grid row
    _tg.setTexture(_appspritesheet);
    _tg.setTextureRect(IntRect(16, 32, SPRITETILESIZE, SPRITETILESIZE));
    _tg.setScale(SPRITESCALE, SPRITESCALE);
    // top right grid corner
    _trg.setTexture(_appspritesheet);
    _trg.setTextureRect(IntRect(32, 32, SPRITETILESIZE, SPRITETILESIZE));
    _trg.setScale(SPRITESCALE, SPRITESCALE);
    // left grid edge
    _lg.setTexture(_appspritesheet);
    _lg.setTextureRect(IntRect(0, 48, SPRITETILESIZE, SPRITETILESIZE));
    _lg.setScale(SPRITESCALE, SPRITESCALE);
    // right grid edge
    _rg.setTexture(_appspritesheet);
    _rg.setTextureRect(IntRect(32, 48, SPRITETILESIZE, SPRITETILESIZE));
    _rg.setScale(SPRITESCALE, SPRITESCALE);
    // bottom left grid corner
    _blg.setTexture(_appspritesheet);
    _blg.setTextureRect(IntRect(0, 64, SPRITETILESIZE, SPRITETILESIZE));
    _blg.setScale(SPRITESCALE, SPRITESCALE);
    // bottom grid row
    _bg.setTexture(_appspritesheet);
    _bg.setTextureRect(IntRect(16, 64, SPRITETILESIZE, SPRITETILESIZE));
    _bg.setScale(SPRITESCALE, SPRITESCALE);
    // bottom right grid corner
    _brg.setTexture(_appspritesheet);
    _brg.setTextureRect(IntRect(32, 64, SPRITETILESIZE, SPRITETILESIZE));
    _brg.setScale(SPRITESCALE, SPRITESCALE);
    

    // load happy face
    _happyFaceSprite.setTexture(_appspritesheet);
    _happyFaceSprite.setScale(SPRITESCALE, SPRITESCALE);
    _happyFaceSprite.setTextureRect(IntRect(0, 96, 32, 32));
    // load cool face
    _coolFaceSprite.setTexture(_appspritesheet);
    _coolFaceSprite.setScale(SPRITESCALE, SPRITESCALE);
    _coolFaceSprite.setTextureRect(IntRect(32, 96, 32, 32));
    // load sad face
    _sadFaceSprite.setTexture(_appspritesheet);
    _sadFaceSprite.setScale(SPRITESCALE, SPRITESCALE);
    _sadFaceSprite.setTextureRect(IntRect(64, 96, 32, 32));

    // load digits
    IntRect locations[10] = { IntRect(48, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(64, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(80, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(96, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(112, 0, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(48, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(64, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(80, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(96, 24, SPRITETILESIZE, SPRITETILESIZE*1.5),
                              IntRect(112, 24, SPRITETILESIZE, SPRITETILESIZE*1.5)};
    Sprite digitSprite(_appspritesheet);
    digitSprite.setScale(SPRITESCALE, SPRITESCALE);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[0][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[1][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[2][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[3][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[4][digit] = digitSprite;
    }
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[5][digit] = digitSprite;
    }

    return true;
}
