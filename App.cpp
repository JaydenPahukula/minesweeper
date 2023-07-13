
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
    _windowWidth = (DEFAULTGAMEWIDTH+2)*TILESIZE;
    _windowHeight = (DEFAULTGAMEHEIGHT+4)*TILESIZE;

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

    // draw border
    for (unsigned int i = 0; i < _borderSprites.size(); i++){
        window.draw(_borderSprites[i]);
    }

    // draw smiley face
    unsigned int isGameOver = _game->gameOver();
    switch (isGameOver){
        case 0:
            window.draw(_happyFaceSprite); break;
        case 1:
            window.draw(_sadFaceSprite); break;
        case 2:
            window.draw(_coolFaceSprite); break;
        default:
            break;
    }

    // draw bomb counter
    unsigned int numBombsRemaining = _game->numBombsRemaining();
    window.draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    window.draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    window.draw(_digitSprites[2][numBombsRemaining%10]);

    // update timer
    if (isGameOver){
        _timerRunning = false;
    }
    if (_timerRunning){
        _currTime = time(0) - _startTime;
    }

    // draw timer
    window.draw(_digitSprites[3][(_currTime/100)%10]);
    window.draw(_digitSprites[4][(_currTime/10)%10]);
    window.draw(_digitSprites[5][_currTime%10]);

    // draw game
    _game->draw(window);

    return;
}




void App::resize(const Event::SizeEvent newSize, RenderWindow &window){
    _windowWidth = newSize.width;
    _windowHeight = newSize.height;
    window.setView(sf::View(Vector2f(_windowWidth/2, _windowHeight/2), sf::Vector2f((float)_windowWidth, (float)_windowHeight)));
}




void App::click(const Event::MouseButtonEvent mouse){

    unsigned int w = gameWidth();
    unsigned int h = gameHeight();

    // clicked on smiley face
    if (mouse.button == Mouse::Left && mouse.x > (int)w/2*TILESIZE && mouse.x < (int)(w+4)/2*TILESIZE && mouse.y > 16 && mouse.y < 80){
        _game->reset();
        _currTime = 0;
        _timerRunning = false;
    }

    // clicked on game feild
    if (!_game->gameOver() && mouse.x > TILESIZE && mouse.x < (int)(w+1)*TILESIZE && mouse.y > (int)TILESIZE*3 && mouse.y < (int)(h+3)*TILESIZE){ 

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
    
    // load texture from sprite sheet
    if(!_appspritesheet.loadFromFile(APPSPRITESHEET)){
        return false;
    }
    _appspritesheet.setSmooth(false);

    unsigned int w = gameWidth();
    unsigned int h = gameHeight();

    // set all background sprites
    Sprite newSprite(_appspritesheet);
    // top left corner
    newSprite.setTextureRect(IntRect(0, 0, 32, 32));
    newSprite.setPosition(0, 0);
    _borderSprites.push_back(newSprite);
    // top right corner
    newSprite.setTextureRect(IntRect(64, 0, 32, 32));
    newSprite.setPosition((w+1)*TILESIZE, 0);
    _borderSprites.push_back(newSprite);
    // top left side
    newSprite.setTextureRect(IntRect(0, 32, 32, 32));
    newSprite.setPosition(0, 32);
    _borderSprites.push_back(newSprite);
    // top right side
    newSprite.setTextureRect(IntRect(64, 32, 32, 32));
    newSprite.setPosition((w+1)*TILESIZE, 32);
    _borderSprites.push_back(newSprite);
    // top left grid corner
    newSprite.setTextureRect(IntRect(0, 64, 32, 32));
    newSprite.setPosition(0, 2*TILESIZE);
    _borderSprites.push_back(newSprite);
    // top right grid corner
    newSprite.setTextureRect(IntRect(64, 64, 32, 32));
    newSprite.setPosition((w+1)*TILESIZE, 2*TILESIZE);
    _borderSprites.push_back(newSprite);
    // bottom left grid corner
    newSprite.setTextureRect(IntRect(0, 128, 32, 32));
    newSprite.setPosition(0, (h+3)*TILESIZE);
    _borderSprites.push_back(newSprite);
    // bottom right grid corner
    newSprite.setTextureRect(IntRect(64, 128, 32, 32));
    newSprite.setPosition((w+1)*TILESIZE, (h+3)*TILESIZE);
    _borderSprites.push_back(newSprite);
    // top row
    newSprite.setTextureRect(IntRect(32, 0, 32, 32));
    for (unsigned int i = 0; i < w*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, 0);
        _borderSprites.push_back(newSprite);
    }
    // top middle row
    newSprite.setTextureRect(IntRect(32, 32, 32, 32));
    for (unsigned int i = 0; i < w*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, TILESIZE);
        _borderSprites.push_back(newSprite);
    }
    // top grid row
    newSprite.setTextureRect(IntRect(32, 64, 32, 32));
    for (unsigned int i = 0; i < w*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, TILESIZE*2);
        _borderSprites.push_back(newSprite);
    }
    // bottom row
    newSprite.setTextureRect(IntRect(32, 128, 32, 32));
    for (unsigned int i = 0; i < w*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, (h+3)*TILESIZE);
        _borderSprites.push_back(newSprite);
    }
    // left column
    newSprite.setTextureRect(IntRect(0, 96, 32, 32));
    for (unsigned int i = 0; i < h*TILESIZE; i += TILESIZE){
        newSprite.setPosition(0, (TILESIZE*3)+i);
        _borderSprites.push_back(newSprite);
    }
    // right column
    newSprite.setTextureRect(IntRect(64, 96, 32, 32));
    for (unsigned int i = 0; i < h*TILESIZE; i += TILESIZE){
        newSprite.setPosition((w+1)*TILESIZE, (TILESIZE*3)+i);
        _borderSprites.push_back(newSprite);
    }

    // load happy face
    _happyFaceSprite.setTexture(_appspritesheet);
    _happyFaceSprite.setTextureRect(IntRect(0, 192, 64, 64));
    _happyFaceSprite.setPosition(w/2*TILESIZE, 16);
    // load cool face
    _coolFaceSprite.setTexture(_appspritesheet);
    _coolFaceSprite.setTextureRect(IntRect(64, 192, 64, 64));
    _coolFaceSprite.setPosition(w/2*TILESIZE, 16);
    // load sad face
    _sadFaceSprite.setTexture(_appspritesheet);
    _sadFaceSprite.setTextureRect(IntRect(128, 192, 64, 64));
    _sadFaceSprite.setPosition(w/2*TILESIZE, 16);

    // load digits
    IntRect locations[10] = {IntRect(96, 0, 32, 48), IntRect(128, 0, 32, 48), IntRect(160, 0, 32, 48), IntRect(192, 0, 32, 48),
                             IntRect(224, 0, 32, 48), IntRect(96, 48, 32, 48), IntRect(128, 48, 32, 48), IntRect(160, 48, 32, 48),
                             IntRect(192, 48, 32, 48), IntRect(224, 48, 32, 48)};
    Sprite digitSprite(_appspritesheet);
    digitSprite.setPosition(24, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[0][digit] = digitSprite;
    }
    digitSprite.setPosition(56, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[1][digit] = digitSprite;
    }
    digitSprite.setPosition(88, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[2][digit] = digitSprite;
    }
    digitSprite.setPosition((w-1.75)*TILESIZE, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[3][digit] = digitSprite;
    }
    digitSprite.setPosition((w-0.75)*TILESIZE, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[4][digit] = digitSprite;
    }
    digitSprite.setPosition((w+0.25)*TILESIZE, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[5][digit] = digitSprite;
    }

    return true;
}
