#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>


class Menu {
    public:
        // constructor
        Menu(sf::Texture *appSpriteSheet);

        // draw
        void draw(sf::RenderWindow& window, unsigned int windowWidth);

        // click
        bool click(const sf::Event::MouseButtonEvent mouse);

        // add menu item
        void addBoolItem(bool *setting, const std::string name);
        void addIntItem(unsigned int *setting, const std::string name);

        // get menu bounds
        sf::IntRect getBounds(const unsigned int windowWidth) const;

    private:

        struct Setting {
            // constructor
            Setting(Menu *parent, const std::string name, const bool isInt, bool *boolSetting, unsigned int *intSetting);
            // data members
            Menu* parent;
            std::string name;
            bool isInt;
            bool *bSetting;
            unsigned int *iSetting;
            // draw
            void draw(sf::RenderWindow& window, unsigned int menux, unsigned int yindex);
            // click
            bool checkClick(const sf::Event::MouseButtonEvent& mouse);
            // assets
            sf::Sprite checkboxTrue, checkboxFalse, arrowUp, arrowDown;
            sf::Text text;
            void loadAssets();
        };

        unsigned int _width, _height;
        std::vector<Setting> _settings;

        // assets
        sf::Texture *_appspritesheet;
        sf::Sprite _l, _m, _r, _bl, _b, _br;
        sf::Font _font;
        sf::Text _titleText;
        void _loadAssets();

};



#endif