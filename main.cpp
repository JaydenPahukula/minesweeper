
#include "definitions.h"
#include "App.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

int main() {
    
    // create game
    App app;
    
    // create window
    RenderWindow window( VideoMode((app.gameWidth()+2)*TILESIZE, (app.gameHeight()+4)*TILESIZE), "Minesweeper", sf::Style::Close);

    // set window icon
    Image icon;
    icon.loadFromFile(ICONFILE);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // draw loop
    Event event;
    while( window.isOpen() ) {
        window.clear();

        // draw game
        app.draw(window);

        window.display();
        while( window.pollEvent(event) ) {
            if (event.type == Event::MouseButtonPressed){ // if mouse clicked
                app.click(event.mouseButton);
            } else if(event.type == Event::Closed) { // if window closed
                window.close();
            }
        }
    }

    return 0;
}