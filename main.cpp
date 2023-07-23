
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
    //RenderWindow window( VideoMode((app.gameWidth()+2)*TILESIZE, (app.gameHeight()+4)*TILESIZE), "Minesweeper", sf::Style::Default);
    RenderWindow window( VideoMode(app.windowWidth(), app.windowHeight()), "Minesweeper", sf::Style::Default);
    window.setFramerateLimit(60);

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
            if(event.type == Event::MouseButtonPressed){            // mouse clicked
                app.mouseClick(event.mouseButton);
            } else if(event.type == Event::MouseButtonReleased){    // mouse released
                app.mouseRelease(event.mouseButton);
            } else if(event.type == Event::MouseMoved){             // mouse moved
                app.mouseMove(event.mouseMove);
            } else if(event.type == Event::MouseWheelScrolled){     // mouse scrolled
                app.zoom(event.mouseWheelScroll);
            } else if(event.type == Event::Resized){                // window resized
                app.resize(event.size, window);
            } else if(event.type == Event::Closed){                 // window closed
                window.close();
            }
        }
    }

    return 0;
}