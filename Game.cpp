
#include "definitions.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

//constructor
Game::Game(){

    _width = 0;
    _height = 0;
    _numBombs = 0;
    _numBombsRemaining = 0;
    _gameOver = 0;
    _timerRunning = false;
    _startTime = 0;
    _currTime = 0;

    //seed random
    srand(time(0));
    srand(time(0));

}



bool Game::init(const string configfilename){
    
    //open config file
    ifstream configfile(configfilename);
    if (!configfile.is_open()){
        cerr << endl << "error opening config file" << endl;
        return false;
    }
    
    //parse config file
    string id;
    int value;
    while(!configfile.eof()){
        //parse line
        getline(configfile, id, '=');
        if (id[0] == '\n'){ id.erase(0, 1); }
        configfile >> value;

        //initialize values
        if (id == "gamewidth"){
            _width = value;
        } else if (id == "gameheight"){
            _height = value;
        } else if (id == "numbombs"){
            _numBombs = value;
        }
    }

    if (_width < MINGAMEWIDTH || _width > MAXGAMEWIDTH || _height < MINGAMEHEIGHT || _height > MAXGAMEHEIGHT ||
        _numBombs < 1 || _numBombs > _width*_height || _numBombs > 999){
        cerr << endl << "invalid configuration" << endl;
        return false;
    }
    
    //initialize grid
    _grid = vector<vector<Tile*>>(_height, vector<Tile*>(_width, nullptr));
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x] = new Tile;
        }
    }
    
    //load textures sprites
    if (!_loadGameSprites()){
        cerr << endl << "error opening game sprite sheet" << endl;
        return false;
    }
    
    //reset all game states
    this->reset();
    
    return true;
}


//destructor
Game::~Game(){
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            delete _grid[y][x];
        }
    }
}



//reset
void Game::reset(){

    _gameOver = 0;
    _timerRunning = false;
    _currTime = 0;
    _numBombsRemaining = _numBombs;

    //reset each tile
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x]->reset();
        }
    }
    //make bombs
    for (unsigned int i = 0; i < _numBombs; i++){
        if (!_grid[rand()%_height][rand()%_width]->init(9)){
            i--;
        }
    }

    //init tiles
    if (!_loadTileSprites()){
        cerr << endl << "error opening tile sprite sheet" << endl;
    }

    return;
}



//draw
void Game::draw(sf::RenderWindow& window){

    //update timer
    if (_timerRunning) _currTime = time(0) - _startTime;

    //count num bombs remaining
    if (!_gameOver){
        _numBombsRemaining = _numBombs;
        unsigned int numUnrevealed = 0;
        for (unsigned int y = 0; y < _height; y++){
            for (unsigned int x = 0; x < _width; x++){
                _numBombsRemaining -= _grid[y][x]->isFlagged();
                numUnrevealed += !_grid[y][x]->isRevealed();
            }
        }
        if (_numBombsRemaining == 0 || numUnrevealed == _numBombsRemaining){
            _numBombsRemaining = 0;
            _gameOver = 2;
            _timerRunning = false;
        }
    }

    //draw background
    for (unsigned int i = 0; i < _bkgSprites.size(); i++){
        window.draw(_bkgSprites[i]);
    }

    //draw smiley face
    switch (_gameOver){
        case 0:
            window.draw(_happySprite); break;
        case 1:
            window.draw(_sadSprite); break;
        case 2:
            window.draw(_coolSprite); break;
        default:
            break;
    }

    //draw bomb count
    window.draw(_digitSprites[0][(_numBombsRemaining/100)%10]);
    window.draw(_digitSprites[1][(_numBombsRemaining/10)%10]);
    window.draw(_digitSprites[2][_numBombsRemaining%10]);

    //draw timer
    window.draw(_digitSprites[3][(_currTime/100)%10]);
    window.draw(_digitSprites[4][(_currTime/10)%10]);
    window.draw(_digitSprites[5][_currTime%10]);
    
    //draw each tile
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            switch (_gameOver){
                case 0:
                    _grid[y][x]->draw(window); break;
                case 1:
                    _grid[y][x]->drawLose(window); break;
                case 2:
                    _grid[y][x]->drawWin(window); break;
                default:
                    break;
            }
        }
    }
    return;
}



//click
void Game::click(const sf::Event::MouseButtonEvent mouse){

    if (mouse.x > (int)_width/2*TILESIZE && mouse.x < (int)(_width+4)/2*TILESIZE && mouse.y > 16 && mouse.y < 80 && mouse.button == Mouse::Left){
        this->reset();
    }
    if (!_gameOver && mouse.x > TILESIZE && mouse.x < (int)(_width+1)*TILESIZE && mouse.y > (int)TILESIZE*3 && mouse.y < (int)(_height+3)*TILESIZE){ 
        if (!_timerRunning){
            _startTime = time(0);
            _timerRunning = true;
        }
        int tilex = mouse.x / TILESIZE - 1;
        int tiley = mouse.y / TILESIZE - 3;
        if (mouse.button == Mouse::Left && !_grid[tiley][tilex]->isFlagged()){
            if(_grid[tiley][tilex]->reveal()){
                _gameOver = 1;
                _timerRunning = false;
            } else {
                this->_checkZeroTile(tilex, tiley);
            }
        } else if (mouse.button == Mouse::Right){
            _grid[tiley][tilex]->flag();
        }
    }

    return;
}



//getters
unsigned int Game::width() const { return _width; }
unsigned int Game::height() const { return _height; }


void Game::_checkZeroTile(unsigned int x, unsigned int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) return; 
    
    if (_grid[y][x]->isZero()){
        if (y > 0 && x > 0 && _grid[y-1][x-1]->revealZero())                _checkZeroTile(x-1, y-1);
        if (y > 0 && _grid[y-1][x]->revealZero())                           _checkZeroTile(x, y-1);
        if (y > 0 && x < _width-1 && _grid[y-1][x+1]->revealZero())         _checkZeroTile(x+1, y-1);
        if (x > 0 && _grid[y][x-1]->revealZero())                           _checkZeroTile(x-1, y);
        if (x < _width-1 && _grid[y][x+1]->revealZero())                    _checkZeroTile(x+1, y);
        if (y < _height-1 && x > 0 && _grid[y+1][x-1]->revealZero())        _checkZeroTile(x-1, y+1);
        if (y < _height-1 && _grid[y+1][x]->revealZero())                   _checkZeroTile(x, y+1);
        if (y < _height-1 && x < _width-1 && _grid[y+1][x+1]->revealZero()) _checkZeroTile(x+1, y+1);
    }
    return;
}


std::vector<Tile*> Game::_getNearbyTiles(unsigned int x, unsigned int y) const {

    vector<Tile*> nearbyTiles;
    if (y > 0 && x > 0)                nearbyTiles.push_back(_grid[y-1][x-1]);
    if (y > 0)                         nearbyTiles.push_back(_grid[y-1][x]);
    if (y > 0 && x < _width-1)         nearbyTiles.push_back(_grid[y-1][x+1]);
    if (x > 0)                         nearbyTiles.push_back(_grid[y][x-1]);
    if (x < _width-1)                  nearbyTiles.push_back(_grid[y][x+1]);
    if (y < _height-1 && x > 0)        nearbyTiles.push_back(_grid[y+1][x-1]);
    if (y < _height-1)                 nearbyTiles.push_back(_grid[y+1][x]);
    if (y < _height-1 && x < _width-1) nearbyTiles.push_back(_grid[y+1][x+1]);

    return nearbyTiles;
}



//load game sprites
bool Game::_loadGameSprites(){

    //load texture from sprite sheet
    if(!_gamespritesheet.loadFromFile(GAMESPRITESHEET)){
        return false;
    }
    _gamespritesheet.setSmooth(false);

    //set background sprites
    Sprite newSprite(_gamespritesheet);
    //top left corner
    newSprite.setTextureRect(IntRect(0, 0, 32, 32));
    newSprite.setPosition(0, 0);
    _bkgSprites.push_back(newSprite);
    //top right corner
    newSprite.setTextureRect(IntRect(64, 0, 32, 32));
    newSprite.setPosition((_width+1)*TILESIZE, 0);
    _bkgSprites.push_back(newSprite);
    //top left side
    newSprite.setTextureRect(IntRect(0, 32, 32, 32));
    newSprite.setPosition(0, 32);
    _bkgSprites.push_back(newSprite);
    //top right side
    newSprite.setTextureRect(IntRect(64, 32, 32, 32));
    newSprite.setPosition((_width+1)*TILESIZE, 32);
    _bkgSprites.push_back(newSprite);
    //top left grid corner
    newSprite.setTextureRect(IntRect(0, 64, 32, 32));
    newSprite.setPosition(0, 2*TILESIZE);
    _bkgSprites.push_back(newSprite);
    //top right grid corner
    newSprite.setTextureRect(IntRect(64, 64, 32, 32));
    newSprite.setPosition((_width+1)*TILESIZE, 2*TILESIZE);
    _bkgSprites.push_back(newSprite);
    //bottom left corner
    newSprite.setTextureRect(IntRect(0, 128, 32, 32));
    newSprite.setPosition(0, (_height+3)*TILESIZE);
    _bkgSprites.push_back(newSprite);
    //bottom right corner
    newSprite.setTextureRect(IntRect(64, 128, 32, 32));
    newSprite.setPosition((_width+1)*TILESIZE, (_height+3)*TILESIZE);
    _bkgSprites.push_back(newSprite);
    //top row
    newSprite.setTextureRect(IntRect(32, 0, 32, 32));
    for (unsigned int i = 0; i < _width*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, 0);
        _bkgSprites.push_back(newSprite);
    }
    //top middle row
    newSprite.setTextureRect(IntRect(32, 32, 32, 32));
    for (unsigned int i = 0; i < _width*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, TILESIZE);
        _bkgSprites.push_back(newSprite);
    }
    //top grid row
    newSprite.setTextureRect(IntRect(32, 64, 32, 32));
    for (unsigned int i = 0; i < _width*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, TILESIZE*2);
        _bkgSprites.push_back(newSprite);
    }
    //bottom row
    newSprite.setTextureRect(IntRect(32, 128, 32, 32));
    for (unsigned int i = 0; i < _width*TILESIZE; i += TILESIZE){
        newSprite.setPosition(TILESIZE+i, (_height+3)*TILESIZE);
        _bkgSprites.push_back(newSprite);
    }
    //left column
    newSprite.setTextureRect(IntRect(0, 96, 32, 32));
    for (unsigned int i = 0; i < _height*TILESIZE; i += TILESIZE){
        newSprite.setPosition(0, (TILESIZE*3)+i);
        _bkgSprites.push_back(newSprite);
    }
    //right column
    newSprite.setTextureRect(IntRect(64, 96, 32, 32));
    for (unsigned int i = 0; i < _height*TILESIZE; i += TILESIZE){
        newSprite.setPosition((_width+1)*TILESIZE, (TILESIZE*3)+i);
        _bkgSprites.push_back(newSprite);
    }

    //load smiley faces
    _happySprite.setTexture(_gamespritesheet);
    _happySprite.setTextureRect(IntRect(0, 192, 64, 64));
    _happySprite.setPosition(_width/2*TILESIZE, 16);
    _coolSprite.setTexture(_gamespritesheet);
    _coolSprite.setTextureRect(IntRect(64, 192, 64, 64));
    _coolSprite.setPosition(_width/2*TILESIZE, 16);
    _sadSprite.setTexture(_gamespritesheet);
    _sadSprite.setTextureRect(IntRect(128, 192, 64, 64));
    _sadSprite.setPosition(_width/2*TILESIZE, 16);

    //load digits
    IntRect locations[10] = {IntRect(96, 0, 32, 48), IntRect(128, 0, 32, 48), IntRect(160, 0, 32, 48), IntRect(192, 0, 32, 48), IntRect(224, 0, 32, 48),
                             IntRect(96, 48, 32, 48), IntRect(128, 48, 32, 48), IntRect(160, 48, 32, 48), IntRect(192, 48, 32, 48), IntRect(224, 48, 32, 48)};
    Sprite digitSprite(_gamespritesheet);
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
    digitSprite.setPosition((_width-1.75)*TILESIZE, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[3][digit] = digitSprite;
    }
    digitSprite.setPosition((_width-0.75)*TILESIZE, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[4][digit] = digitSprite;
    }
    digitSprite.setPosition((_width+0.25)*TILESIZE, 24);
    for (int digit = 0; digit < 10; digit++){
        digitSprite.setTextureRect(locations[digit]);
        _digitSprites[5][digit] = digitSprite;
    }

    return true;
}

//load tile sprites
bool Game::_loadTileSprites(){

    //load textere from sprite sheet
    if (!_tilespritesheet.loadFromFile(TILESPRITESHEET)){
        return false;
    }
    _tilespritesheet.setSmooth(false);

    //load sprites
    Sprite unopenedSprite(_tilespritesheet, IntRect(0, 0, 32, 32));
    Sprite flaggedSprite(_tilespritesheet, IntRect(32, 0, 32, 32));
    Sprite openedSprite[] = { Sprite(_tilespritesheet, IntRect(64, 32, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(96, 32, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(0, 64, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(32, 64, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(64, 64, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(96, 64, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(0, 96, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(32, 96, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(64, 96, 32, 32)),
                              Sprite(_tilespritesheet, IntRect(0, 32, 32, 32))    }; //bomb
    Sprite revealbombSprite(_tilespritesheet, IntRect(96, 0, 32, 32));
    Sprite xbombSprite(_tilespritesheet, IntRect(32, 32, 32, 32));

    //initialize tiles
    int nearbyBombCount;
    int tilex, tiley;
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){

            tilex = TILESIZE*(x+1);
            tiley = TILESIZE*(y+3);

            unopenedSprite.setPosition(tilex, tiley);
            flaggedSprite.setPosition(tilex, tiley);
            revealbombSprite.setPosition(tilex, tiley);
            xbombSprite.setPosition(tilex, tiley);

            if (!_grid[y][x]->isBomb()){ //if not already a bomb
                //count nearby bombs
                nearbyBombCount = 0;
                vector<Tile*> nearbyTiles = _getNearbyTiles(x, y);
                for (unsigned int i = 0; i < nearbyTiles.size(); i++){
                    if (nearbyTiles[i]->isBomb()) nearbyBombCount++;
                }
                //initialize tile
                _grid[y][x]->init(nearbyBombCount);
                //set sprites
                openedSprite[nearbyBombCount].setPosition(tilex, tiley);
                _grid[y][x]->initSprites(unopenedSprite, flaggedSprite, openedSprite[nearbyBombCount], revealbombSprite, xbombSprite);
            
            } else { //already a bomb
                //set sprites
                openedSprite[9].setTextureRect(IntRect(0, 32, 32, 32));
                openedSprite[9].setPosition(tilex, tiley);
                _grid[y][x]->initSprites(unopenedSprite, flaggedSprite, openedSprite[9], revealbombSprite, xbombSprite);
            }
        }
    }

    return true;
}
