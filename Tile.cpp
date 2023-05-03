
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
    _spriteRevealbomb = revealbomb;
    _spriteXbomb = xbomb;
}

//draw
void Tile::draw(sf::RenderWindow &window) const {
    if (!_revealed){
        window.draw(_spriteUnopened);
    } else if (_flagged){
        window.draw(_spriteFlagged);
    } else {
        window.draw(_spriteOpened);
    }
}

//getters
bool Tile::isRevealed() const { return _revealed; }
bool Tile::isBomb() const { return _identity == 0; }