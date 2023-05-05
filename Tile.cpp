
#include "Tile.h"

#include <SFML/Graphics.hpp>

//constructor
Tile::Tile(){
    this->reset();
}

//reset
void Tile::reset(){
    _identity = -1;
    _revealed = false;
    _flagged = false;
}

//init
bool Tile::init(const int value){
    if (_identity == -1){
        _identity = value;
        return true;
    } else {
        return false;
    }
}

void Tile::initSprites(sf::Sprite unopened, sf::Sprite flagged, sf::Sprite opened, sf::Sprite revealbomb, sf::Sprite xbomb){
    _spriteUnopened = unopened;
    _spriteFlagged = flagged;
    _spriteOpened = opened;
    _spriteRevealBomb = revealbomb;
    _spriteXBomb = xbomb;
}

//draw
void Tile::draw(sf::RenderWindow &window) const {
    if (_revealed){
        window.draw(_spriteOpened);
    } else if (_flagged){
        window.draw(_spriteFlagged);
    } else {
        window.draw(_spriteUnopened);
    }
}

void Tile::drawLose(sf::RenderWindow &window) const {
    if (_identity == 9){
        if (_revealed){
            window.draw(_spriteOpened);
        } else if (_flagged){
            window.draw(_spriteXBomb);
        } else {
            window.draw(_spriteRevealBomb);
        }
    } else {
        this->draw(window);
    }
}

void Tile::drawWin(sf::RenderWindow &window) const {
    if (_identity != 9){
        window.draw(_spriteOpened);
    } else {
        window.draw(_spriteFlagged);
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
bool Tile::isFlagged() const  { return _flagged; }