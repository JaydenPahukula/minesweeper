#include "definitions.h"
#include "Game.h"

#include GAMESPRITESHEETPATH
#include FONTPATH

#include <SFML/Graphics.hpp>
using namespace sf;

#include <string>
#include <vector>
#include <queue>
using namespace std;

#include <cstdlib>



Game::Game(const unsigned int width, const unsigned int height, const unsigned int numBombs, const bool autoOpen){

    // initialize tile grid
    _gameOver = 0;
    _width = width;
    _height = height;
    _grid = vector<vector<Tile*>>(_height, vector<Tile*>(_width, nullptr));
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x] = new Tile;
        }
    }

    // seed random
    srand(time(0));

    // make bombs
    _numBombs = numBombs;
    for (unsigned int i = 0; i < _numBombs; i++){
        if (!_grid[rand()%_height][rand()%_width]->init(9)){
            i--;
        }
    }
    
    // init tiles
    _loadTileSprites();
    
    // create timer
    _timer = Timer();

    // stats
    _numBombsRemaining = _numBombs;
    _numRevealed = 0;
    _update3BV();
    _numClicks = 0;
    
    if (autoOpen){
        // count tiles
        Tile* tile;
        vector<Vector2i> zeroTiles, numberTiles;
        for (unsigned int y = 0; y < _height; y++){
            for (unsigned int x = 0; x < _width; x++){
                tile = _grid[y][x];
                if (tile->getIdentity() == 0){
                    zeroTiles.push_back(Vector2i(x, y));
                } else if (!tile->isBomb()){
                    numberTiles.push_back(Vector2i(x, y));
                }
            }
        }
        
        // reveal random zero tile, else random number tile
        Vector2i coords;
        if (zeroTiles.size() > 0){
            coords = zeroTiles[rand() % zeroTiles.size()];
        } else {
            coords = numberTiles[rand() % numberTiles.size()];
        }
        // open tile
        _revealTile(coords.x, coords.y);
        // start timer
        _timer.start();
        // auto open counts as a click
        _numClicks++;

    }
    
    // update game
    _update();
}




Game::~Game(){
    // deallocate tile grid
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            delete _grid[y][x];
        }
    }
}



void Game::draw(RenderWindow& window, const sf::RenderStates &states){
    // draw each tile
    Tile* tile;
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            tile = _grid[y][x];
            switch (_gameOver){
                case 0:
                    tile->draw(window, states); break;
                case 1:
                    tile->drawLose(window, states); break;
                case 2:
                    tile->drawWin(window, states); break;
                default:
                    break;
            }
        }
    }
    return;
}



void Game::drawProbabilities(RenderWindow& window, const sf::RenderStates &states){
    // draw each tile probability
    Tile* tile;
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            tile = _grid[y][x];
            // if not revealed, draw probability
            if (!tile->isRevealed() && !tile->isFlagged() && (!_gameOver || !tile->isBomb())){
                tile->drawText(window, states, to_string(_probabilities[y][x]));
            }
        }
    }
    return;
}




void Game::click(const sf::Event::MouseButtonEvent mouse, unsigned int x, unsigned int y, bool chordingEnabled){

    // return if out of bounds
    if (x < 0 || x >= _width || y < 0 || y >= _height) return; 

    // start timer if not already
    _timer.start();

    // increment click
    _numClicks++;

    // if left clicked and tile not flagged or revealed
    if (mouse.button == Mouse::Left && !_grid[y][x]->isFlagged() && !_grid[y][x]->isRevealed()){
        // reveal tile
        _revealTile(x, y);
        // update game
        _update();
    // if left clicked, tile already revealed, and chording is enabled
    } else if (mouse.button == Mouse::Left && _grid[y][x]->isRevealed() && chordingEnabled){
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



unsigned int Game::width() const { return _width; }
unsigned int Game::height() const { return _height; }
unsigned int Game::numBombs() const { return _numBombs; }
unsigned int Game::numBombsRemaining() const { return _numBombsRemaining; }
unsigned int Game::gameOver() const { return _gameOver; }
unsigned int Game::seconds() const { return _timer.seconds(); }
unsigned int Game::get3BV() const { return _3BV; }
float Game::tilesPerSecond() const { return (_timer.seconds() == 0) ? 0. : (float)_numRevealed / _timer.seconds(); }



vector<Vector2u> Game::_getAdjacentCoords(const Vector2u a){
    vector<Vector2u> neighbors;
    if (a.y > 0 && a.x > 0)                 neighbors.push_back(Vector2u(a.x-1, a.y-1));
    if (a.y > 0)                            neighbors.push_back(Vector2u(a.x, a.y-1));
    if (a.y > 0 && a.x < _width-1)          neighbors.push_back(Vector2u(a.x+1, a.y-1));
    if (a.x > 0)                            neighbors.push_back(Vector2u(a.x-1, a.y));
    if (a.x < _width-1)                     neighbors.push_back(Vector2u(a.x+1, a.y));
    if (a.y < _height-1 && a.x > 0)         neighbors.push_back(Vector2u(a.x-1, a.y+1));
    if (a.y < _height-1)                    neighbors.push_back(Vector2u(a.x, a.y+1));
    if (a.y < _height-1 && a.x < _width-1)  neighbors.push_back(Vector2u(a.x+1, a.y+1));
    return neighbors;
}


void Game::_playerWins(){
    _gameOver = 2;
    _timer.stop();
    _numBombsRemaining = 0;
}


void Game::_playerLoses(){
    _gameOver = 1;
    _timer.stop();
}


void Game::_update(){
    // count remaining bombs and unrevealed tiles
    unsigned int numFlagged = 0;
    _numRevealed = 0;
    for (unsigned int y1 = 0; y1 < _height; y1++){
        for (unsigned int x1 = 0; x1 < _width; x1++){
            numFlagged += _grid[y1][x1]->isFlagged();
            _numRevealed += _grid[y1][x1]->isRevealed();
        }
    }
    if (numFlagged > _numBombs) numFlagged = _numBombs;
    
    // update _numBombsRemaining
    _numBombsRemaining = _numBombs - numFlagged;

    // update probabilities
    _updateProbabilities();
    
    // check if game is done
    if (!_gameOver && (_width * _height) - _numRevealed == _numBombs){
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
        for (Vector2u coords : this->_getAdjacentCoords(Vector2u(x,y))){
            _checkZeroTile(coords.x, coords.y, false);
        }
    }
    return;
}




void Game::_chord(unsigned int x, unsigned int y){
    // return if out of bounds
    if (x < 0 || x >= _width || y < 0 || y >= _height) return;
    // return if not revealed or zero
    if (!_grid[y][x]->isRevealed() || _grid[y][x]->getIdentity() == 0) return;

    // get adjacent tiles
    vector<Vector2u> adjacentTiles = this->_getAdjacentCoords(Vector2u(x,y));

    // count number of adjacent flags
    int adjacentFlagCount = 0;
    for (Vector2u coords : adjacentTiles){
        if (_grid[coords.y][coords.x]->isFlagged()) adjacentFlagCount++;
    }

    // if all adjacent bombs have been flagged
    if (adjacentFlagCount == _grid[y][x]->getIdentity()){

        // reveal each non-flagged adjacent tile
        Tile* neighbor;
        for (Vector2u coords : adjacentTiles){

            neighbor = _grid[coords.y][coords.x];
            
            // skip flagged or revealed tiles
            if (neighbor->isFlagged() || neighbor->isRevealed()) continue;

            // reveal tile
            _revealTile(coords.x, coords.y);
        }
    }

    return;
}



void Game::_update3BV(){
    const IntRect boundaries(0, 0, _width, _height);
    vector<vector<bool>> seen = vector<vector<bool>>(_height, vector<bool>(_width, false));
    unsigned int count = 0;

    // search all zero tiles
    for (unsigned int y1 = 0; y1 < _height; y1++){
        for (unsigned int x1 = 0; x1 < _width; x1++){
            if (!seen[y1][x1] && _grid[y1][x1]->getIdentity() == 0){
                
                queue<Vector2i> q;
                q.push(Vector2i(x1,y1));
                // bfs
                while (!q.empty()){
                    // process current tile
                    Vector2i curr = q.front(); q.pop();

                    // if zero tile, check neighbors
                    if (_grid[curr.y][curr.x]->getIdentity() == 0){
                        for (int dy = -1; dy <= 1; dy++){
                            for (int dx = -1; dx <= 1; dx++){
                                // if in bounds and not seen, add to queue
                                Vector2i neighbor(curr.x+dx, curr.y+dy);
                                if (boundaries.contains(neighbor) && !seen[neighbor.y][neighbor.x]){
                                    seen[neighbor.y][neighbor.x] = true;
                                    q.push(neighbor);
                                }
                            }
                        }
                    }
                }
                count++;
            }
        }
    }
    
    // count all remaining non-bomb tiles
    for (unsigned int y1 = 0; y1 < _height; y1++){
        for (unsigned int x1 = 0; x1 < _width; x1++){
            if (!seen[y1][x1] && !_grid[y1][x1]->isBomb()){
                count++;
            }
        }
    }
    
    // set 3BV
    _3BV = count;
    return;
}



void Game::_updateProbabilities(){
    _probabilities = vector<vector<int>>(_height, vector<int>(_width, -1));

    // first check all basic logic
    bool done = false;
    while (!done){
        done = true;
        for (unsigned int y = 0; y < _height; y++){
            for (unsigned int x = 0; x < _width; x++){
                if (!_grid[y][x]->isRevealed() || _grid[y][x]->getIdentity() == 0) continue;
                // look at each neighbor
                vector<Vector2u> mines, notMines, unknown;
                for (Vector2u coords : _getAdjacentCoords(Vector2u(x,y))){
                    Tile* neighbor = _grid[coords.y][coords.x];
                    if (!neighbor->isRevealed()){
                        if (_probabilities[coords.y][coords.x] == 100) mines.push_back(coords);
                        else if (_probabilities[coords.y][coords.x] == 0) notMines.push_back(coords);
                        else unknown.push_back(coords);
                    }
                }
                unsigned int identity = _grid[y][x]->getIdentity();
                // if identity = # of non-flag tiles, all non-flag tiles are mines
                if (identity == mines.size() + unknown.size()){
                    for (Vector2u& coords : unknown){
                        _probabilities[coords.y][coords.x] = 100;
                        done = false;
                    }
                }
                // if identity = # of mine tiles, all other tiles are safe
                if (identity == mines.size()){
                    for (Vector2u& coords : unknown){
                        _probabilities[coords.y][coords.x] = 0;
                        done = false;
                    }
                }
            }
        }
    }

    
    return;
}



void Game::_loadTileSprites(){

    // load texture from sprite sheet
    _tilespritesheet.loadFromMemory(GAMESPRITESHEETFILE, sizeof(GAMESPRITESHEETFILE));
    _tilespritesheet.setSmooth(false);

    // load sprites
    Sprite unopenedSprite(_tilespritesheet, IntRect(0, 0, 16, 16));
    Sprite flaggedSprite(_tilespritesheet, IntRect(16, 0, 16, 16));
    Sprite revealbombSprite(_tilespritesheet, IntRect(48, 0, 16, 16));
    Sprite xbombSprite(_tilespritesheet, IntRect(16, 16, 16, 16));
    Sprite openedSprite[] = { Sprite(_tilespritesheet, IntRect(32, 16, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(48, 16, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(0, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(16, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(32, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(48, 32, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(0, 48, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(16, 48, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(32, 48, 16, 16)),
                              Sprite(_tilespritesheet, IntRect(0, 16, 16, 16))};
    
    // load text
    _font.loadFromMemory(FONTFILE, sizeof(FONTFILE));
    Text text("0", _font, SPRITETILESIZE*12);
    text.setFillColor(Color::Black);
    text.setScale(Vector2f(1/32., 1/32.));

    // initialize tiles
    int adjacentBombCount;
    int tilex, tiley;
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){

            tilex = SPRITETILESIZE*x;
            tiley = SPRITETILESIZE*y;

            unopenedSprite.setPosition(tilex, tiley);
            flaggedSprite.setPosition(tilex, tiley);
            revealbombSprite.setPosition(tilex, tiley);
            xbombSprite.setPosition(tilex, tiley);
            text.setPosition(tilex, tiley);

            if (!_grid[y][x]->isBomb()){ // if not already a bomb
                // count nearby bombs
                adjacentBombCount = 0;
                for (Vector2u coords : this->_getAdjacentCoords(Vector2u(x,y))){
                    if (_grid[coords.y][coords.x]->isBomb()) adjacentBombCount++;
                }
                // initialize tile
                _grid[y][x]->init(adjacentBombCount);
                // set sprites
                openedSprite[adjacentBombCount].setPosition(tilex, tiley);
                _grid[y][x]->setSprites(unopenedSprite, flaggedSprite, openedSprite[adjacentBombCount], revealbombSprite, xbombSprite, text);
            
            } else { // already a bomb
                // set sprites
                openedSprite[9].setTextureRect(IntRect(0, 16, 16, 16));
                openedSprite[9].setPosition(tilex, tiley);
                _grid[y][x]->setSprites(unopenedSprite, flaggedSprite, openedSprite[9], revealbombSprite, xbombSprite, text);
            }
        }
    }

    return;
}
