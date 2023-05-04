
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

void Tile::drawGameOver(sf::RenderWindow &window) const {
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

bool Tile::reveal(){
    _revealed = true;
    return _identity == 9;
}

void Tile::flag(){
    _flagged = true;
    return;
}

//getters
bool Tile::isRevealed() const { return _revealed; }
bool Tile::isBomb() const { return _identity == 9; }
bool Tile::isZero() const { return _identity == 0; }