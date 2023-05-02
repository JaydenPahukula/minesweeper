
#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;


const char* CONFIGFILE = "config.txt";


int main() {

    Game game(CONFIGFILE);


    RenderWindow window( VideoMode(game.width()*32+64, game.height()*32+64), "minesweeper", sf::Style::Close);
    Event event;

    while( window.isOpen() ) {
        window.clear();


        game.draw(window);


        window.display();

        while( window.pollEvent(event) ) {
            if (event.type == Event::MouseButtonPressed){
                game.click(event.mouseButton);
            } else if(event.type == Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}