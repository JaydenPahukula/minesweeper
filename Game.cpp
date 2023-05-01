#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


Game::Game(const string filename){

    //open file
    ifstream configfile(filename);
    if (!configfile.is_open()){
        cerr << endl << "error opening config file" << endl;
        throw exception();
    }

    //parse file
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
        }
    }

    //initialize grid
    _grid = vector<vector<Tile*>>(_height, vector<Tile*>(_width, nullptr));
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            _grid[y][x] = new Tile(x, y);
        }
    }

    //read textures
    if (!_loadSprites()){
        cerr << endl << "error opening sprite sheet" << endl;
        throw exception();
    }

}


Game::~Game(){
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            delete _grid[y][x];
        }
    }
}

bool Game::_loadSprites(){

    if(!_spritesheet.loadFromFile("assets/spritesheet.png")){
        return false;
    }
    _spritesheet.setSmooth(false);

    //top left corner
    _sprites.borderTL.setTexture(_spritesheet);
    _sprites.borderTL.setTextureRect(sf::IntRect(0, 0, 32, 32));
    _sprites.borderTL.setPosition(0, 0);
    //top right corner
    _sprites.borderTR.setTexture(_spritesheet);
    _sprites.borderTR.setTextureRect(sf::IntRect(96, 0, 32, 32));
    _sprites.borderTR.setPosition(_width*32+32, 0);
    //bottom left corner
    _sprites.borderBL.setTexture(_spritesheet);
    _sprites.borderBL.setTextureRect(sf::IntRect(0, 64, 32, 32));
    _sprites.borderBL.setPosition(0, _height*32+32);
    //bottom right corner
    _sprites.borderBR.setTexture(_spritesheet);
    _sprites.borderBR.setTextureRect(sf::IntRect(96, 64, 32, 32));
    _sprites.borderBR.setPosition(_width*32+32, _height*32+32);

    //top and bottom row
    for (unsigned int i = 0; i < _width*32; i += 32){
        sf::Sprite top;
        top.setTexture(_spritesheet);
        top.setTextureRect(sf::IntRect(32, 0, 32, 32));
        top.setPosition(32+i, 0);
        _sprites.borderTlist.push_back(top);

        sf::Sprite bottom;
        bottom.setTexture(_spritesheet);
        bottom.setTextureRect(sf::IntRect(32, 64, 32, 32));
        bottom.setPosition(32+i, _height*32+32);
        _sprites.borderBlist.push_back(bottom);
    }

    //left and right column
    for (unsigned int i = 0; i < _height*32; i += 32){
        sf::Sprite left;
        left.setTexture(_spritesheet);
        left.setTextureRect(sf::IntRect(0, 32, 32, 32));
        left.setPosition(0, 32+i);
        _sprites.borderLlist.push_back(left);

        sf::Sprite right;
        right.setTexture(_spritesheet);
        right.setTextureRect(sf::IntRect(96, 32, 32, 32));
        right.setPosition(32*_width+32, 32+i);
        _sprites.borderRlist.push_back(right);
    }

    _sprites.unopen.setTexture(_spritesheet);
    _sprites.unopen.setTextureRect(sf::IntRect(64, 32, 32, 32));
    _sprites.open0.setTexture(_spritesheet);
    _sprites.open0.setTextureRect(sf::IntRect(32, 32, 32, 32));

    return true;
}

void Game::draw(sf::RenderWindow& window){

    //draw border
    window.draw(_sprites.borderTL);
    window.draw(_sprites.borderTR);
    window.draw(_sprites.borderBR);
    window.draw(_sprites.borderBL);
    for (unsigned int i = 0; i < _width; i++){
        window.draw(_sprites.borderTlist[i]);
        window.draw(_sprites.borderBlist[i]);
    }
    for (unsigned int i = 0; i < _height; i++){
        window.draw(_sprites.borderLlist[i]);
        window.draw(_sprites.borderRlist[i]);
    }
    for (unsigned int y = 0; y < _height; y++){
        for (unsigned int x = 0; x < _width; x++){
            if (_grid[y][x]->isRevealed()){
                _sprites.open0.setPosition(x*32+32, y*32+32);
                window.draw(_sprites.open0);
            } else {
                _sprites.unopen.setPosition(x*32+32, y*32+32);
                window.draw(_sprites.unopen);
            }
        }
    }
    return;
}


void Game::click(const sf::Event::MouseButtonEvent mouse){
    cout << "clicked" << endl;
    return;
}


unsigned int Game::width() const {
    return _width;
}

unsigned int Game::height() const {
    return _height;
}