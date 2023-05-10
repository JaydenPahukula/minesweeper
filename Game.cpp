
#include "definitions.h"
#include "Game.h"
#include "GameSprites.cpp"

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

    _width = 0;
    _height = 0;
    _numBombs = 0;
    _gameOver = 0;
    _timerRunning = false;
    _startTime = 0;
    _currTime = 0;

    //seed random
    srand(time(0));
    srand(time(0)); // <-- it breaks if this isn't here, dont ask why

}




Game::~Game(){
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            delete _grid[y][x];
        }
    }
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

        //read setting name
        getline(configfile, id, '=');
        if (id[0] == '\n'){ id.erase(0, 1); }
        //read value
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

    //verify that configuration is valid
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




void Game::reset(){

    //reset values
    _gameOver = 0;
    _timerRunning = false;
    _currTime = 0;

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

    unsigned int numBombsRemaining = _numBombs;
    if (!_gameOver){
        //count remaining bombs and unrevealed tiles
        unsigned int numUnrevealed = 0;
        for (unsigned int y = 0; y < _height; y++){
            for (unsigned int x = 0; x < _width; x++){
                numBombsRemaining -= _grid[y][x]->isFlagged();
                numUnrevealed += !_grid[y][x]->isRevealed();
            }
        }
        //if no bombs remaining or all non-bomb tiles revealed
        if (numBombsRemaining == 0 || numUnrevealed == numBombsRemaining){
            //player wins!
            _gameOver = 2;
            //stop the timer
            _timerRunning = false;
        }
    } else {
        numBombsRemaining = 0;
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

    //draw bomb counter
    window.draw(_digitSprites[0][(numBombsRemaining/100)%10]);
    window.draw(_digitSprites[1][(numBombsRemaining/10)%10]);
    window.draw(_digitSprites[2][numBombsRemaining%10]);

    //update timer
    if (_timerRunning) _currTime = time(0) - _startTime;

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




void Game::click(const sf::Event::MouseButtonEvent mouse){

    //clicked on smiley face
    if (mouse.x > (int)_width/2*TILESIZE && mouse.x < (int)(_width+4)/2*TILESIZE && mouse.y > 16 && mouse.y < 80 && mouse.button == Mouse::Left){
        this->reset();
    }
    //clicked on game feild
    if (!_gameOver && mouse.x > TILESIZE && mouse.x < (int)(_width+1)*TILESIZE && mouse.y > (int)TILESIZE*3 && mouse.y < (int)(_height+3)*TILESIZE){ 
        //start timer if not already started
        if (!_timerRunning){
            _timerRunning = true;
            _startTime = time(0);
        }

        int tilex = mouse.x / TILESIZE - 1;
        int tiley = mouse.y / TILESIZE - 3;

        //if left clicked on tile and tile not flagged
        if (mouse.button == Mouse::Left && !_grid[tiley][tilex]->isFlagged()){
            //reveal tile and check if bomb
            if(_grid[tiley][tilex]->reveal()){
                //player loses :(
                _gameOver = 1;
                //stop the timer
                _timerRunning = false;
            } else {
                //start checking if zero tile
                this->_checkZeroTile(tilex, tiley);
            }
        //if right clicked
        } else if (mouse.button == Mouse::Right){
            //flag the tile
            _grid[tiley][tilex]->flag();
        }
    }

    return;
}




unsigned int Game::width() const { return _width; }
unsigned int Game::height() const { return _height; }




void Game::_checkZeroTile(unsigned int x, unsigned int y) const {
    //return if out of bounds
    if (x < 0 || x >= _width || y < 0 || y >= _height) return; 
    
    if (_grid[y][x]->isZero()){
        //reveal all adjacent tiles, and recurse on zero tiles
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
