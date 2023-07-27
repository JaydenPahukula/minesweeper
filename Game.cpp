
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



Game::Game(){

    // initialize tile grid
    _width = GAMEWIDTH;
    _height = GAMEHEIGHT;
    _grid = vector<vector<Tile*>>(_height, vector<Tile*>(_width, nullptr));
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x] = new Tile;
        }
    }

    // reset values
    this->reset();

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
    _width = GAMEWIDTH;
    _height = GAMEHEIGHT;
    _numBombs = NUMBOMBS;
    _chordingEnabled = CHORDING;
    _numBombsRemaining = _numBombs;
    _gameOver = 0;

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




void Game::draw(RenderWindow& window, const sf::RenderStates &states){
    
    // draw each tile
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            switch (_gameOver){
                case 0:
                    _grid[y][x]->draw(window, states); break;
                case 1:
                    _grid[y][x]->drawLose(window, states); break;
                case 2:
                    _grid[y][x]->drawWin(window, states); break;
                default:
                    break;
            }
        }
    }
    return;
}




void Game::click(const sf::Event::MouseButtonEvent mouse, unsigned int x, unsigned int y){

    // return if out of bounds
    if (x < 0 || x >= _width || y < 0 || y >= _height) return; 

    // if left clicked and tile not flagged or revealed
    if (mouse.button == Mouse::Left && !_grid[y][x]->isFlagged() && !_grid[y][x]->isRevealed()){
        // reveal tile
        _revealTile(x, y);
        // update game
        _update();
    // if left clicked, tile already revealed, and chording is enabled
    } else if (mouse.button == Mouse::Left && _grid[y][x]->isRevealed() && _chordingEnabled){
        // chord
        _chord(x, y);
        // update game
        _update();
    // if right clicked and tile is not revealed
    } else if (mouse.button == Mouse::Right && !_grid[y][x]->isRevealed()){
        // flag the tile
        _grid[y][x]->flag();
        // update game
        _update();
    }

    return;
}




unsigned int Game::gameOver() const { return _gameOver; }
unsigned int Game::numBombsRemaining() const { return _numBombsRemaining; }



void Game::_playerWins(){
    _gameOver = 2;
    _numBombsRemaining = 0;
}


void Game::_playerLoses(){
    _gameOver = 1;
}



void Game::_update(){

    // count remaining bombs and unrevealed tiles
    unsigned int numFlagged = 0;
    unsigned int numUnrevealed = 0;
    for (unsigned int y1 = 0; y1 < _height; y1++){
        for (unsigned int x1 = 0; x1 < _width; x1++){
            numFlagged += _grid[y1][x1]->isFlagged();
            numUnrevealed += !_grid[y1][x1]->isRevealed();
        }
    }
    if (numFlagged > _numBombs) numFlagged = _numBombs;

    // update _numBombsRemaining
    _numBombsRemaining = _numBombs - numFlagged;

    // check if game is done
    if (!_gameOver && numUnrevealed == _numBombs){
        _playerWins();
    }

}



void Game::_revealTile(unsigned int x, unsigned int y){

    // reveal tile
    _grid[y][x]->reveal();

    // if bomb
    if(_grid[y][x]->isBomb()){
        // player loses :(
        _playerLoses();
        return;
    }
    // start checking if zero tile
    _checkZeroTile(x, y, true);

}




void Game::_checkZeroTile(unsigned int x, unsigned int y, bool first){
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
            _revealTile(nearbyTilesX[i], nearbyTilesY[i]);
        }
    }

    return;
}




bool Game::_loadTileSprites(){

    // load texture from sprite sheet
    if (!_tilespritesheet.loadFromFile(GAMESPRITESHEET)){
        return false;
    }
    _tilespritesheet.setSmooth(false);

    const float scale = (float)TILESIZE / SPRITETILESIZE;
    // load sprites
    Sprite unopenedSprite(_tilespritesheet, IntRect(0, 0, 16, 16));
    unopenedSprite.setScale(scale, scale);
    Sprite flaggedSprite(_tilespritesheet, IntRect(16, 0, 16, 16));
    flaggedSprite.setScale(scale, scale);
    Sprite revealbombSprite(_tilespritesheet, IntRect(48, 0, 16, 16));
    revealbombSprite.setScale(scale, scale);
    Sprite xbombSprite(_tilespritesheet, IntRect(16, 16, 16, 16));
    xbombSprite.setScale(scale, scale);
    Sprite openedSprite[] = { Sprite(_tilespritesheet, IntRect(32, 16, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(48, 16, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(0, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(16, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(32, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(48, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(0, 48, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(16, 48, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(32, 48, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(0, 16, 16, 16))   };
    for (unsigned int i = 0; i < 10; i++){
        openedSprite[i].setScale(scale, scale);
    }

    // initialize tiles
    int adjacentBombCount;
    int tilex, tiley;
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){

            tilex = TILESIZE*x;
            tiley = TILESIZE*y;

            unopenedSprite.setPosition(tilex, tiley);
            flaggedSprite.setPosition(tilex, tiley);
            revealbombSprite.setPosition(tilex, tiley);
            xbombSprite.setPosition(tilex, tiley);

            if (!_grid[y][x]->isBomb()){ // if not already a bomb
                // count nearby bombs
                adjacentBombCount = 0;
                if (y > 0 && x > 0 && _grid[y-1][x-1]->isBomb())                adjacentBombCount++;
                if (y > 0 && _grid[y-1][x]->isBomb())                           adjacentBombCount++;
                if (y > 0 && x < _width-1 && _grid[y-1][x+1]->isBomb())         adjacentBombCount++;
                if (x > 0 && _grid[y][x-1]->isBomb())                           adjacentBombCount++;
                if (x < _width-1 && _grid[y][x+1]->isBomb())                    adjacentBombCount++;
                if (y < _height-1 && x > 0 && _grid[y+1][x-1]->isBomb())        adjacentBombCount++;
                if (y < _height-1 && _grid[y+1][x]->isBomb())                   adjacentBombCount++;
                if (y < _height-1 && x < _width-1 && _grid[y+1][x+1]->isBomb()) adjacentBombCount++;
                // initialize tile
                _grid[y][x]->init(adjacentBombCount);
                // set sprites
                openedSprite[adjacentBombCount].setPosition(tilex, tiley);
                _grid[y][x]->setSprites(unopenedSprite, flaggedSprite, openedSprite[adjacentBombCount], revealbombSprite, xbombSprite);
            
            } else { // already a bomb
                // set sprites
                openedSprite[9].setTextureRect(IntRect(0, 16, 16, 16));
                openedSprite[9].setPosition(tilex, tiley);
                _grid[y][x]->setSprites(unopenedSprite, flaggedSprite, openedSprite[9], revealbombSprite, xbombSprite);
            }
        }
    }

    return true;
}
