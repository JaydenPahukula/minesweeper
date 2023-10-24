
#include "definitions.h"
#include "App.h"

#include ICONPATH

#include <SFML/Graphics.hpp>
using namespace sf;


int main() {
    
    // create window
    RenderWindow window(VideoMode(10, 10), "Minesweeper", sf::Style::Default);
    window.setFramerateLimit(60);

    // set window icon
    Image icon;
    icon.loadFromMemory(ICONFILE, sizeof(ICONFILE));
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // create game
    App app(window);

    // draw loop
    Event event;
    while( window.isOpen() ) {
        window.clear();

        // draw game
        app.draw();

        window.display();

        // handle events
        while(window.pollEvent(event)) {
            app.handleEvent(event);
        }
    }

    return 0;
}