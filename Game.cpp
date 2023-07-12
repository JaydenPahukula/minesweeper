
#include "definitions.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <cstdlib>
#include <ctime>



Game::Game(){

    // initialize tile grid
    _width = DEFAULTGAMEWIDTH;
    _height = DEFAULTGAMEHEIGHT;
    _grid = vector<vector<Tile*>>(_height, vector<Tile*>(_width, nullptr));
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x] = new Tile;
        }
    }

    // seed random
    srand(time(0));
    srand(time(0)); //  <-- it breaks if this isn't here, dont ask why

    // reset values
    this->reset();

    // load textures sprites
    if (!_loadGameSprites()){
        cerr << endl << "error opening game sprite sheet" << endl;
    }

}




Game::~Game(){
    // deallocate tile grid
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            delete _grid[y][x];
        }
    }
}




void Game::reset(){

    // reset values
    _width = DEFAULTGAMEWIDTH;
    _height = DEFAULTGAMEHEIGHT;
    _numBombs = DEFAULTNUMBOMBS;
    _chordingEnabled = CHORDING;
    _gameOver = 0;
    _timerRunning = false;
    _startTime = 0;
    _currTime = 0;

    // reset each tile
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x]->reset();
        }
    }

    // make bombs
    for (unsigned int i = 0; i < _numBombs; i++){
        if (!_grid[rand()%_height][rand()%_width]->init(9)){
            i--;
        }
    }

    // init tiles
    if (!_loadTileSprites()){
        cerr << endl << "error opening tile sprite sheet" << endl;
    }

    return;
}




//draw
void Game::draw(sf::RenderWindow& window){

    unsigned int numBombsRemaining = _numBombs;
    if (!_gameOver){
        // count remaining bombs and unrevealed tiles
        unsigned int numUnrevealed = 0;
        for (unsigned int y = 0; y < _height; y++){
            for (unsigned int x = 0; x < _width; x++){
                numBombsRemaining -= _grid[y][x]->isFlagged();
                numUnrevealed += !_grid[y][x]->isRevealed();
            }
        }
        // if no bombs remaining or all non-bomb tiles revealed
        if (numBombsRemaining == 0 || numUnrevealed == numBombsRemaining){
            // player wins!
            _gameOver = 2;
            // stop the timer
            _timerRunning = false;
        }
    } else {
        numBombsRemaining = 0;
    }

    // draw background
    for (unsigned int i = 0; i < _bkgSprites.size(); i++){
        window.draw(_bkgSprites[i]);
    }

    // draw smiley face
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

    // draw bomb counter
    window.draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    window.draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    window.draw(_digitSprites[2][numBombsRemaining%10]);

    // update timer
    if (_timerRunning) _currTime = time(0) - _startTime;

    // draw timer
    window.draw(_digitSprites[3][(_currTime/100)%10]);
    window.draw(_digitSprites[4][(_currTime/10)%10]);
    window.draw(_digitSprites[5][_currTime%10]);
    
    // draw each tile
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




void Game::click(const sf::Event::MouseButtonEvent mouse){

    // clicked on smiley face
    if (mouse.x > (int)_width/2*TILESIZE && mouse.x < (int)(_width+4)/2*TILESIZE && mouse.y > 16 && mouse.y < 80 && mouse.button == Mouse::Left){
        this->reset();
    }
    // clicked on game feild
    if (!_gameOver && mouse.x > TILESIZE && mouse.x < (int)(_width+1)*TILESIZE && mouse.y > (int)TILESIZE*3 && mouse.y < (int)(_height+3)*TILESIZE){ 
        // start timer if not already started
        if (!_timerRunning){
            _timerRunning = true;
            _startTime = time(0);
        }

        int tilex = mouse.x / TILESIZE - 1;
        int tiley = mouse.y / TILESIZE - 3;

        // if left clicked on tile and tile not flagged or revealed
        if (mouse.button == Mouse::Left && !_grid[tiley][tilex]->isFlagged() && !_grid[tiley][tilex]->isRevealed()){
            // reveal tile and check if bomb
            _grid[tiley][tilex]->reveal();
            if(_grid[tiley][tilex]->isBomb()){
                // player loses :(
                _gameOver = 1;
                // stop the timer
                _timerRunning = false;
            } else {
                // start checking if zero tile
                this->_checkZeroTile(tilex, tiley, true);
            }
        // if left clicked, tile already revealed, and chording is enabled
        } else if (mouse.button == Mouse::Left && _grid[tiley][tilex]->isRevealed() && _chordingEnabled){
            _chord(tilex, tiley);
        // if right clicked and tile is not revealed
        } else if (mouse.button == Mouse::Right && !_grid[tiley][tilex]->isRevealed()){
            // flag the tile
            _grid[tiley][tilex]->flag();
        }
    }

    return;
}




unsigned int Game::width() const { return _width; }
unsigned int Game::height() const { return _height; }




void Game::_checkZeroTile(unsigned int x, unsigned int y, bool first) const {
    // return if out of bounds
    if (x < 0 || x >= _width || y < 0 || y >= _height) return; 

    // return if tile has already been visited
    if (_grid[y][x]->isRevealed() && _grid[y][x]->getIdentity() == 0 && !first) return;

    // open tile if not flagged
    if (!_grid[y][x]->isFlagged()) _grid[y][x]->reveal();

    // if tile is zero, recurse on adjacent tiles
    if (_grid[y][x]->getIdentity() == 0){
        if (y > 0 && x > 0)                 _checkZeroTile(x-1, y-1, false);
        if (y > 0)                          _checkZeroTile(x, y-1, false);
        if (y > 0 && x < _width-1)          _checkZeroTile(x+1, y-1, false);
        if (x > 0)                          _checkZeroTile(x-1, y, false);
        if (x < _width-1)                   _checkZeroTile(x+1, y, false);
        if (y < _height-1 && x > 0)         _checkZeroTile(x-1, y+1, false);
        if (y < _height-1)                  _checkZeroTile(x, y+1, false);
        if (y < _height-1 && x < _width-1)  _checkZeroTile(x+1, y+1, false);
    }
    return;
}




void Game::_chord(unsigned int x, unsigned int y){
    // return if out of bounds
    if (x < 0 || x >= _width || y < 0 || y >= _height) return;
    // return if not revealed or zero
    if (!_grid[y][x]->isRevealed() || _grid[y][x]->getIdentity() == 0) return;

    // count number of adjacent flags
    int adjacentFlagCount = 0;
    vector<unsigned int> nearbyTilesX;
    vector<unsigned int> nearbyTilesY;
    if (y > 0 && x > 0)                { nearbyTilesX.push_back(x-1); nearbyTilesY.push_back(y-1); }
    if (y > 0)                         { nearbyTilesX.push_back(x);   nearbyTilesY.push_back(y-1); }
    if (y > 0 && x < _width-1)         { nearbyTilesX.push_back(x+1); nearbyTilesY.push_back(y-1); }
    if (x > 0)                         { nearbyTilesX.push_back(x-1); nearbyTilesY.push_back(y);   }
    if (x < _width-1)                  { nearbyTilesX.push_back(x+1); nearbyTilesY.push_back(y);   }
    if (y < _height-1 && x > 0)        { nearbyTilesX.push_back(x-1); nearbyTilesY.push_back(y+1); }
    if (y < _height-1)                 { nearbyTilesX.push_back(x);   nearbyTilesY.push_back(y+1); }
    if (y < _height-1 && x < _width-1) { nearbyTilesX.push_back(x+1); nearbyTilesY.push_back(y+1); }
    for (unsigned int i = 0; i < nearbyTilesX.size(); i++){
        if (_grid[nearbyTilesY[i]][nearbyTilesX[i]]->isFlagged()) adjacentFlagCount++;
    }

    // if all adjacent bombs have been flagged
    if (adjacentFlagCount == _grid[y][x]->getIdentity()){

        // reveal each non-flagged adjacent tile
        Tile* neighbor;
        for (unsigned int i = 0; i < nearbyTilesX.size(); i++){

            neighbor = _grid[nearbyTilesY[i]][nearbyTilesX[i]];
            
            // skip flagged or revealed tiles
            if (neighbor->isFlagged() || neighbor->isRevealed()) continue;

            // reveal tile
            neighbor->reveal();

            if(neighbor->isBomb()){
                // player loses
                _gameOver = 1;
                // stop the timer
                _timerRunning = false;
            } else {
                // start checking if zero tile
                this->_checkZeroTile(nearbyTilesX[i], nearbyTilesY[i], true);
            }
        }
    }

}