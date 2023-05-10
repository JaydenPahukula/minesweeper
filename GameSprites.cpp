
#include "definitions.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <vector>
using namespace std;



bool Game::_loadGameSprites(){

    //load texture from sprite sheet
    if(!_gamespritesheet.loadFromFile(GAMESPRITESHEET)){
        return false;
    }
    _gamespritesheet.setSmooth(false);

    //set all background sprites
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
    //bottom left grid corner
    newSprite.setTextureRect(IntRect(0, 128, 32, 32));
    newSprite.setPosition(0, (_height+3)*TILESIZE);
    _bkgSprites.push_back(newSprite);
    //bottom right grid corner
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

    //load happy face
    _happySprite.setTexture(_gamespritesheet);
    _happySprite.setTextureRect(IntRect(0, 192, 64, 64));
    _happySprite.setPosition(_width/2*TILESIZE, 16);
    //load cool face
    _coolSprite.setTexture(_gamespritesheet);
    _coolSprite.setTextureRect(IntRect(64, 192, 64, 64));
    _coolSprite.setPosition(_width/2*TILESIZE, 16);
    //load sad face
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




bool Game::_loadTileSprites(){

    //load texture from sprite sheet
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
                              Sprite(_tilespritesheet, IntRect(0, 32, 32, 32))    };
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
                vector<Tile*> nearbyTiles;
                if (y > 0 && x > 0)                nearbyTiles.push_back(_grid[y-1][x-1]);
                if (y > 0)                         nearbyTiles.push_back(_grid[y-1][x]);
                if (y > 0 && x < _width-1)         nearbyTiles.push_back(_grid[y-1][x+1]);
                if (x > 0)                         nearbyTiles.push_back(_grid[y][x-1]);
                if (x < _width-1)                  nearbyTiles.push_back(_grid[y][x+1]);
                if (y < _height-1 && x > 0)        nearbyTiles.push_back(_grid[y+1][x-1]);
                if (y < _height-1)                 nearbyTiles.push_back(_grid[y+1][x]);
                if (y < _height-1 && x < _width-1) nearbyTiles.push_back(_grid[y+1][x+1]);
                for (unsigned int i = 0; i < nearbyTiles.size(); i++){
                    if (nearbyTiles[i]->isBomb()) nearbyBombCount++;
                }
                //initialize tile
                _grid[y][x]->init(nearbyBombCount);
                //set sprites
                openedSprite[nearbyBombCount].setPosition(tilex, tiley);
                _grid[y][x]->setSprites(unopenedSprite, flaggedSprite, openedSprite[nearbyBombCount], revealbombSprite, xbombSprite);
            
            } else { //already a bomb
                //set sprites
                openedSprite[9].setTextureRect(IntRect(0, 32, 32, 32));
                openedSprite[9].setPosition(tilex, tiley);
                _grid[y][x]->setSprites(unopenedSprite, flaggedSprite, openedSprite[9], revealbombSprite, xbombSprite);
            }
        }
    }

    return true;
}