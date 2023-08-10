#include "definitions.h"

#include "Menu.h"

#include FONTPATH

#include <SFML/Graphics.hpp>
using namespace sf;

#include <string>
#include <vector>
using namespace std;


Menu::Menu(Texture *appSpriteSheet){
    _width = 0;
    _height = 2*SPRITETILESIZE;
    _tileSize = SPRITETILESIZE;
    _menux = 0;
    _settingCount = 0;
    _appspritesheet = appSpriteSheet;
    _loadAssets();
}



void Menu::draw(RenderWindow& window){
    // background
    for (unsigned int y = 0; y < _height-_tileSize; y += _tileSize){
        // middle area
        for (unsigned int x = _menux+_tileSize; x < _menux+_width-_tileSize; x += _tileSize){
            _m.setPosition(x, y);
            window.draw(_m);
        }
        // left edge
        _l.setPosition(_menux, y);
        window.draw(_l);
        // right edge
        _r.setPosition(_menux+_width-_tileSize, y);
        window.draw(_r);
    }
    // bottom edge
    for (unsigned int x = _menux+_tileSize; x < _menux+_width-_tileSize; x += _tileSize){
        _b.setPosition(x, _height-_tileSize);
        window.draw(_b);
    }
    // corners
    _bl.setPosition(_menux, _height-_tileSize);
    window.draw(_bl);
    _br.setPosition(_menux+_width-_tileSize, _height-_tileSize);
    window.draw(_br);

    // title text
    _titleText.setPosition(_menux+_tileSize*3.2, _tileSize/3);
    window.draw(_titleText);

    // settings
    for (unsigned int i = 0; i < _settingCount; i++){
        _settings[i].draw(window);
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
    _settings.push_back(Setting(this, _settingCount, name, true, nullptr, setting));
    _settingCount++;
}


void Menu::addBoolItem(bool *setting, const std::string name){
    _settings.push_back(Setting(this, _settingCount, name, false, setting, nullptr));
    _settingCount++;
}


IntRect Menu::getBounds() const {
    const unsigned int width = _tileSize*9;
    return IntRect(_menux, 0, width, _height);
}



void Menu::updateAssets(const unsigned int tileSize, const unsigned int windowWidth){
    _tileSize = tileSize;
    _height = _tileSize*(2+_settingCount);
    _width = _tileSize*9;
    _menux = (windowWidth/2.) - (_width/2.);

    const Vector2f scale((float)_tileSize/SPRITETILESIZE, (float)_tileSize/SPRITETILESIZE);
    _l.setScale(scale);
    _m.setScale(scale);
    _r.setScale(scale);
    _bl.setScale(scale);
    _b.setScale(scale);
    _br.setScale(scale);
    _titleText.setCharacterSize(_tileSize*3./4);

    for (unsigned int i = 0; i < _settingCount; i++){
        _settings[i].updateAssets();
    }
    return;
}



void Menu::_loadAssets(){

    // left
    _l.setTexture(*_appspritesheet);
    _l.setTextureRect(IntRect(0, 16, SPRITETILESIZE, SPRITETILESIZE));
    // middle
    _m.setTexture(*_appspritesheet);
    _m.setTextureRect(IntRect(16, 16, SPRITETILESIZE, SPRITETILESIZE));
    // right
    _r.setTexture(*_appspritesheet);
    _r.setTextureRect(IntRect(32, 16, SPRITETILESIZE, SPRITETILESIZE));
    // bottom left
    _bl.setTexture(*_appspritesheet);
    _bl.setTextureRect(IntRect(0, 32, SPRITETILESIZE, SPRITETILESIZE));
    // bottom middle
    _b.setTexture(*_appspritesheet);
    _b.setTextureRect(IntRect(16, 32, SPRITETILESIZE, SPRITETILESIZE));
    // bottom right
    _br.setTexture(*_appspritesheet);
    _br.setTextureRect(IntRect(32, 32, SPRITETILESIZE, SPRITETILESIZE));

    // load font
    _font.loadFromMemory(FONTFILE, sizeof(FONTFILE));

    // set title text
    _titleText.setFont(_font);
    _titleText.setFillColor(Color::Black);
    _titleText.setString("MENU:");

}




Menu::Setting::Setting(Menu *parent, const unsigned int yindex, const string name, const bool isInt, bool *boolSetting, unsigned int *intSetting){
    // set variables
    this->parent = parent;
    this->yindex = yindex;
    this->name = name;
    this->isInt = isInt;
    if (isInt){
        iSetting = intSetting;
        bSetting = nullptr;
    } else {
        iSetting = nullptr;
        bSetting = boolSetting;
    }
    // load assets
    nameText.setFont(parent->_font);
    nameText.setFillColor(Color::Black);
    nameText.setString(name);
    valueText.setFont(parent->_font);
    valueText.setFillColor(Color::Black);
    if (isInt){
        arrowUp = Sprite(*parent->_appspritesheet, IntRect(6*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
        arrowDown = Sprite(*parent->_appspritesheet, IntRect(5*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    } else {
        checkboxTrue = Sprite(*parent->_appspritesheet, IntRect(4*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
        checkboxFalse = Sprite(*parent->_appspritesheet, IntRect(3*SPRITETILESIZE, 3*SPRITETILESIZE, SPRITETILESIZE, SPRITETILESIZE));
    }
}



void Menu::Setting::draw(RenderWindow& window){
    // setting name
    window.draw(nameText);
    // draw checkbox or arrows
    if (isInt){
        // arrows
        window.draw(arrowUp);
        window.draw(arrowDown);
        // text
        valueText.setString(to_string(*iSetting));
        valueText.setPosition(parent->_menux+parent->_tileSize*7.33-valueText.getGlobalBounds().width/2., (1.7+yindex)*parent->_tileSize);
        window.draw(valueText);
    } else {
        // checkbox
        if (*bSetting){
            window.draw(checkboxTrue);
        } else {
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



void Menu::Setting::updateAssets(){
    const unsigned int tileSize = parent->_tileSize;
    const unsigned int menux = parent->_menux;
    const Vector2f scale((float)tileSize/SPRITETILESIZE, (float)tileSize/SPRITETILESIZE);

    nameText.setCharacterSize(tileSize/2.);
    nameText.setPosition(menux+tileSize/2., (1.7+yindex)*tileSize);
    if (isInt){
        arrowUp.setScale(scale);
        arrowUp.setPosition(menux+tileSize*7.7, (1.5+yindex)*tileSize);
        arrowDown.setScale(scale);
        arrowDown.setPosition(menux+tileSize*6, (1.5+yindex)*tileSize);
        valueText.setCharacterSize(tileSize/2.);
    } else {
        checkboxTrue.setScale(scale);
        checkboxTrue.setPosition(menux+tileSize*7.5, (1.5+yindex)*tileSize);
        checkboxFalse.setScale(scale);
        checkboxFalse.setPosition(menux+tileSize*7.5, (1.5+yindex)*tileSize);
    }
}