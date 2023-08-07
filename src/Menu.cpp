#include "definitions.h"

#include "Menu.h"

#include FONTPATH

#include <SFML/Graphics.hpp>
using namespace sf;

#include <string>
#include <vector>
using namespace std;

Menu::Menu(Texture *appSpriteSheet){
    _width = 9*TILESIZE;
    _height = 450;
    _appspritesheet = appSpriteSheet;
    _loadAssets();
}



void Menu::draw(sf::RenderWindow& window, unsigned int windowWidth){
    unsigned int xLocation =  (windowWidth/2) - (_width/2);

    // background
    for (unsigned int y = 0; y < _height-TILESIZE; y += TILESIZE){
        // middle area
        for (unsigned int x = xLocation+TILESIZE; x < xLocation+_width-TILESIZE; x += TILESIZE){
            _m.setPosition(x, y);
            window.draw(_m);
        }
        // left edge
        _l.setPosition(xLocation, y);
        window.draw(_l);
        // right edge
        _r.setPosition(xLocation+_width-TILESIZE, y);
        window.draw(_r);
    }
    // bottom edge
    for (unsigned int x = xLocation+TILESIZE; x < xLocation+_width-TILESIZE; x += TILESIZE){
        _b.setPosition(x, _height-TILESIZE);
        window.draw(_b);
    }
    // corners
    _bl.setPosition(xLocation, _height-TILESIZE);
    window.draw(_bl);
    _br.setPosition(xLocation+_width-TILESIZE, _height-TILESIZE);
    window.draw(_br);

    // title text
    _titleText.setPosition(xLocation+TILESIZE*3.2, TILESIZE/3);
    window.draw(_titleText);

    // settings
    for (unsigned int i = 0; i < _settings.size(); i++){
        _settings[i].draw(window, xLocation, i);
    }

    return;
}



bool Menu::click(const Event::MouseButtonEvent mouse){
    if (mouse.button == Mouse::Left){
        // check if each setting was clicked
        for (unsigned int i = 0; i < _settings.size(); i++){
            if (_settings[i].checkClick(mouse)){
                return true;
            }
        }
    }
    return false;
}



void Menu::addIntItem(unsigned int *setting, const std::string name){
    _settings.push_back(Setting(this, name, true, nullptr, setting));
}


void Menu::addBoolItem(bool *setting, const std::string name){
    _settings.push_back(Setting(this, name, false, setting, nullptr));
}


IntRect Menu::getBounds(const unsigned int windowWidth) const {
    return IntRect((windowWidth/2)-(_width/2), 0, _width, _height);
}



void Menu::_loadAssets(){

    const Vector2f scale((float)TILESIZE/SPRITETILESIZE, (float)TILESIZE/SPRITETILESIZE);
    // left
    _l.setTexture(*_appspritesheet);
    _l.setTextureRect(IntRect(0, 16, SPRITETILESIZE, SPRITETILESIZE));
    _l.setScale(scale);
    // middle
    _m.setTexture(*_appspritesheet);
    _m.setTextureRect(IntRect(16, 16, SPRITETILESIZE, SPRITETILESIZE));
    _m.setScale(scale);
    // right
    _r.setTexture(*_appspritesheet);
    _r.setTextureRect(IntRect(32, 16, SPRITETILESIZE, SPRITETILESIZE));
    _r.setScale(scale);
    // bottom left
    _bl.setTexture(*_appspritesheet);
    _bl.setTextureRect(IntRect(0, 32, SPRITETILESIZE, SPRITETILESIZE));
    _bl.setScale(scale);
    // bottom middle
    _b.setTexture(*_appspritesheet);
    _b.setTextureRect(IntRect(16, 32, SPRITETILESIZE, SPRITETILESIZE));
    _b.setScale(scale);
    // bottom right
    _br.setTexture(*_appspritesheet);
    _br.setTextureRect(IntRect(32, 32, SPRITETILESIZE, SPRITETILESIZE));
    _br.setScale(scale);

    // load font
    _font.loadFromMemory(FONTFILE, sizeof(FONTFILE));

    // set title text
    _titleText.setFont(_font);
    _titleText.setFillColor(Color::Black);
    _titleText.setCharacterSize(TILESIZE*3/4);
    _titleText.setString("MENU:");

}




Menu::Setting::Setting(Menu *parent, const string name, const bool isInt, bool *boolSetting, unsigned int *intSetting){
    this->parent = parent;
    this->name = name;
    this->isInt = isInt;
    if (isInt){
        iSetting = intSetting;
        bSetting = nullptr;
    } else {
        iSetting = nullptr;
        bSetting = boolSetting;
    }
    loadAssets();
}



void Menu::Setting::draw(RenderWindow& window, const unsigned int menux, const unsigned int yindex){
    // setting name
    text.setPosition(menux+TILESIZE/2, (1.7+yindex)*TILESIZE);
    text.setString(name);
    window.draw(text);
    // draw checkbox or arrows
    if (isInt){
        // arrows
        arrowUp.setPosition(menux+TILESIZE*7.7, (1.5+yindex)*TILESIZE);
        window.draw(arrowUp);
        arrowDown.setPosition(menux+TILESIZE*6, (1.5+yindex)*TILESIZE);
        window.draw(arrowDown);
        // text
        text.setString(to_string(*iSetting));
        text.setPosition(menux+TILESIZE*7.33-text.getGlobalBounds().width/2., (1.7+yindex)*TILESIZE);
        window.draw(text);
    } else {
        // checkbox
        if (*bSetting){
            checkboxTrue.setPosition(menux+TILESIZE*7.5, (1.5+yindex)*TILESIZE);
            window.draw(checkboxTrue);
        } else {
            checkboxFalse.setPosition(menux+TILESIZE*7.5, (1.5+yindex)*TILESIZE);
            window.draw(checkboxFalse);
        }
    }
}



bool Menu::Setting::checkClick(const Event::MouseButtonEvent& mouse){
    if (isInt){
        // check arrow up
        if (arrowUp.getGlobalBounds().contains(mouse.x, mouse.y)){
            *iSetting += 1;
            return true;
        }
        // check arrow down
        if (arrowDown.getGlobalBounds().contains(mouse.x, mouse.y)){
            *iSetting -= 1;
            return true;
        }
    } else {
        // check checkbox
        if (*bSetting){
            if (checkboxTrue.getGlobalBounds().contains(mouse.x, mouse.y)){
                *bSetting = !*bSetting;
                return true;
            }
        } else {
            if (checkboxFalse.getGlobalBounds().contains(mouse.x, mouse.y)){
                *bSetting = !*bSetting;
            }
        }
    }
    return false;
}



void Menu::Setting::loadAssets(){
    
    // text
    text.setFont(parent->_font);
    text.setFillColor(Color::Black);
    text.setCharacterSize(TILESIZE/2);

    const Vector2f scale((float)TILESIZE/SPRITETILESIZE, (float)TILESIZE/SPRITETILESIZE);
    if (isInt){
        // arrows
        const IntRect arrowUpRect(6*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE);
        const IntRect arrowDownRect(5*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE);
        arrowUp = Sprite(*parent->_appspritesheet, arrowUpRect);
        arrowUp.setScale(scale);
        arrowDown = Sprite(*parent->_appspritesheet, arrowDownRect);
        arrowDown.setScale(scale);
    } else {
        // checkboxes
        const IntRect checkboxFalseRect(3*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE);
        const IntRect checkboxTrueRect(4*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE);
        checkboxTrue = Sprite(*parent->_appspritesheet, checkboxTrueRect);
        checkboxTrue.setScale(scale);
        checkboxFalse = Sprite(*parent->_appspritesheet, checkboxFalseRect);
        checkboxFalse.setScale(scale);
    }
}