#ifndef GAME_H
#define GAME_H

#include "Tile.h"

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
    public:
        //constructor
        Game(const std::string filename);
        //destructor
        ~Game();

        //draw
        void draw(sf::RenderWindow &window);

        //click
        void click(const sf::Event::MouseButtonEvent mouse);

        //getters
        unsigned int width() const;
        unsigned int height() const;

    private:

        std::vector<std::vector<Tile*>> _grid;
        unsigned int _width, _height;

        bool _loadSprites();
        sf::Texture _spritesheet;
        struct SpriteGroup {
            sf::Sprite borderTL, borderTR, borderBL, borderBR, unopen, open0;
            std::vector<sf::Sprite> borderTlist;
            std::vector<sf::Sprite> borderRlist;
            std::vector<sf::Sprite> borderBlist;
            std::vector<sf::Sprite> borderLlist;
        };
        SpriteGroup _sprites;

};






#endif