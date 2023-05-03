
#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

#include "definitions.h"
#include "Game.h"


int main() {

    //create game
    Game game;

    //initialize game
    if (!game.init(CONFIGFILE)){
        return -1;
    }

    //create window
    RenderWindow window( VideoMode((game.width()+2)*TILESIZE, (game.height()+4)*TILESIZE), TITLE, sf::Style::Close);

    //set icon
    Image icon;
    icon.loadFromFile(ICONFILE);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    //draw loop
    Event event;
    while( window.isOpen() ) {
        window.clear();


        //draw game
        game.draw(window);


        window.display();
        while( window.pollEvent(event) ) {
            if (event.type == Event::MouseButtonPressed){ //if mouse clicked
                game.click(event.mouseButton);
            } else if(event.type == Event::Closed) { //if window closed
                window.close();
            }
        }
    }

    return 0;
}