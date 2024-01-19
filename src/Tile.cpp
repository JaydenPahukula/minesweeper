
#include "Tile.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <string>
using namespace std;


Tile::Tile(){
    _identity = -1;
    _revealed = false;
    _flagged = false;
}


bool Tile::init(const int value){
    // update identity if not already set
    if (_identity == -1){
        _identity = value;
        return true;
    } else {
        return false;
    }
}


void Tile::setSprites(Sprite unopened, Sprite flagged, Sprite opened, Sprite revealbomb, Sprite xbomb, Text text){
    _spriteUnopened = unopened;
    _spriteFlagged = flagged;
    _spriteOpened = opened;
    _spriteRevealBomb = revealbomb;
    _spriteXBomb = xbomb;
    _text = text;
}


void Tile::draw(RenderWindow &window, const sf::RenderStates &states) const {
    // draw the appropriate tile
    if (_revealed){
        window.draw(_spriteOpened, states);
    } else if (_flagged){
        window.draw(_spriteFlagged, states);
    } else {
        window.draw(_spriteUnopened, states);
    }
}


void Tile::drawLose(RenderWindow &window, const sf::RenderStates &states) const {
    // if tile is a bomb
    if (_identity == 9){
        if (_revealed){
            window.draw(_spriteOpened, states);
        } else if (_flagged){
            window.draw(_spriteFlagged, states);
        } else {
            window.draw(_spriteRevealBomb, states);
        }
    // tile is not a bomb
    } else {
        if (_flagged){
            window.draw(_spriteXBomb, states);
        } else if (_revealed){
            window.draw(_spriteOpened, states);
        } else {
            window.draw(_spriteUnopened, states);
        }
    }
}


void Tile::drawWin(RenderWindow &window, const sf::RenderStates &states) const {
    // if bomb
    if (_identity == 9){
        window.draw(_spriteFlagged, states);
    } else {
        window.draw(_spriteOpened, states);
    }
}

void Tile::drawText(RenderWindow &window, const sf::RenderStates &states, const string str){
    _text.setString(str);
    window.draw(_text, states);
}

void Tile::reveal(){
    _revealed = true;
    return;
}

void Tile::flag(){
    _flagged = !_flagged;
    return;
}

//getters
bool Tile::isRevealed() const { return _revealed; }
bool Tile::isBomb() const     { return _identity == 9; }
bool Tile::isFlagged() const  { return !_revealed && _flagged; }
int Tile::getIdentity() const { return _identity; }