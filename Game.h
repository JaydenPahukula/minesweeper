#ifndef GAME_H
#define GAME_H


#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
    public:
        //constructor
        Game(const std::string filename);

        //draw
        void draw(const sf::RenderWindow window);

        //click
        void click(const sf::Event::MouseButtonEvent mouse);

        //getters
        unsigned int width() const;
        unsigned int height() const;

    private:
        class Tile;

        std::vector<std::vector<Tile*>>* _grid;
        unsigned int _width, _height;
        


};






#endif