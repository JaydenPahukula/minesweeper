
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
    _numbombs = 0;
    _gameOver = false;

    //seed random
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
            _numbombs = value;
        }
    }

    if (_width < MINGAMEWIDTH || _width > MAXGAMEWIDTH || _height < MINGAMEHEIGHT || _height > MAXGAMEHEIGHT ||
        _numbombs < 1 || _numbombs > _width*_height){
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
    //
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            delete _grid[y][x];
        }
    }
}



//reset
void Game::reset(){

    _gameOver = false;

    //reset each tile
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x]->reset();
        }
    }
    //make bombs
    for (unsigned int i = 0; i < _numbombs; i++){
        if (!_grid[rand()%_height][rand()%_width]->init(0)){
            i--;
        }
    }

    //init tiles
    if (!_loadTileSprites()){
        cerr << endl << "error opening tile sprite sheet" << endl;
    }

    return;
}



void Game::draw(sf::RenderWindow& window){

    //draw background
    for (unsigned int i = 0; i < _bkgSprites.size(); i++){
        window.draw(_bkgSprites[i]);
    }
    
    //draw each tile
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x]->draw(window);
        }
    }
    return;
}



//click
void Game::click(const sf::Event::MouseButtonEvent mouse){
    cout << "clicked" << endl;
    return;
}



//getters
unsigned int Game::width() const { return _width; }
unsigned int Game::height() const { return _height; }



//load game sprites
bool Game::_loadGameSprites(){

    //load texture from sprite sheet
    if(!_gamespritesheet.loadFromFile(GAMESPRITESHEET)){
        return false;
    }
    _gamespritesheet.setSmooth(false);

    //set background sprites
    Sprite newSprite(_gamespritesheet);
    newSprite.setScale(TILESIZE/32.0, TILESIZE/32.0);
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
    unopenedSprite.setScale(TILESIZE/32.0, TILESIZE/32.0);
    Sprite flaggedSprite(_tilespritesheet, IntRect(32, 0, 32, 32));
    flaggedSprite.setScale(TILESIZE/32.0, TILESIZE/32.0);
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
    for (int i = 0; i < 10; i++){ openedSprite[i].setScale(TILESIZE/32.0, TILESIZE/32.0); }
    Sprite revealbombSprite(_tilespritesheet, IntRect(96, 0, 32, 32));
    revealbombSprite.setScale(TILESIZE/32.0, TILESIZE/32.0);
    Sprite xbombSprite(_tilespritesheet, IntRect(32, 32, 32, 32));
    xbombSprite.setScale(TILESIZE/32.0, TILESIZE/32.0);

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
                if (y > 0 && x > 0 && _grid[y-1][x-1]->isBomb())                nearbyBombCount++;
                if (y > 0 && _grid[y-1][x]->isBomb())                           nearbyBombCount++;
                if (y > 0 && x < _width-1 && _grid[y-1][x+1]->isBomb())         nearbyBombCount++;
                if (x > 0 && _grid[y][x-1]->isBomb())                           nearbyBombCount++;
                if (x < _width-1 && _grid[y][x+1]->isBomb())                    nearbyBombCount++;
                if (y < _height-1 && x > 0 && _grid[y+1][x-1]->isBomb())        nearbyBombCount++;
                if (y < _height-1 && _grid[y+1][x]->isBomb())                   nearbyBombCount++;
                if (y < _height-1 && x < _width-1 && _grid[y+1][x+1]->isBomb()) nearbyBombCount++;
                //initialize tile
                _grid[y][x]->init(nearbyBombCount);
                //set sprites
                openedSprite[nearbyBombCount].setPosition(tilex, tiley);
                _grid[y][x]->initSprites(unopenedSprite, flaggedSprite, openedSprite[nearbyBombCount], revealbombSprite, xbombSprite);
            
            } else { //already a bomb
                //set sprites
                openedSprite[9].setTextureRect(IntRect(0, 32, 32, 32));
                _grid[y][x]->initSprites(unopenedSprite, flaggedSprite, openedSprite[9], revealbombSprite, xbombSprite);
            }
        }
    }

    return true;
}
