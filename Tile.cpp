
#include "Tile.h"

#include <SFML/Graphics.hpp>
using namespace sf;


Tile::Tile(){
    this->reset();
}


void Tile::reset(){
    //reset all values
    _identity = -1;
    _revealed = false;
    _flagged = false;
}


bool Tile::init(const int value){
    //update identity if not already set
    if (_identity == -1){
        _identity = value;
        return true;
    } else {
        return false;
    }
}


void Tile::setSprites(Sprite unopened, Sprite flagged, Sprite opened, Sprite revealbomb, Sprite xbomb){
    _spriteUnopened = unopened;
    _spriteFlagged = flagged;
    _spriteOpened = opened;
    _spriteRevealBomb = revealbomb;
    _spriteXBomb = xbomb;
}


void Tile::draw(RenderWindow &window) const {
    //draw the appropriate tile
    if (_revealed){
        window.draw(_spriteOpened);
    } else if (_flagged){
        window.draw(_spriteFlagged);
    } else {
        window.draw(_spriteUnopened);
    }
}


void Tile::drawLose(RenderWindow &window) const {
    //if tile is a bomb
    if (_identity == 9){
        if (_revealed){
            window.draw(_spriteOpened);
        } else if (_flagged){
            window.draw(_spriteXBomb);
        } else {
            window.draw(_spriteRevealBomb);
        }
    //else draw normally
    } else {
        this->draw(window);
    }
}


void Tile::drawWin(RenderWindow &window) const {
    //if bomb
    if (_identity == 9){
        window.draw(_spriteFlagged);
    } else {
        window.draw(_spriteOpened);
    }
}

bool Tile::reveal(){
    _revealed = true;
    return this->isBomb();
}

bool Tile::revealZero(){
    bool previouslyRevealed = _revealed;
    _revealed = true;
    return (!previouslyRevealed && this->isZero());
}

void Tile::flag(){
    _flagged = !_flagged;
    return;
}

//getters
bool Tile::isRevealed() const { return _revealed; }
bool Tile::isBomb() const     { return _identity == 9; }
bool Tile::isZero() const     { return _identity == 0; }
bool Tile::isFlagged() const  { return !_revealed && _flagged; }
int Tile::getIdentity() const { return _identity; }