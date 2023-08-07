
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
            if(event.type == Event::MouseButtonPressed){            // mouse clicked
                app.mouseClick(event.mouseButton);
            } else if(event.type == Event::MouseButtonReleased){    // mouse released
                app.mouseRelease(event.mouseButton);
            } else if(event.type == Event::MouseMoved){             // mouse moved
                app.mouseMove(event.mouseMove);
            } else if(event.type == Event::MouseWheelScrolled){     // mouse scrolled
                app.zoom(event.mouseWheelScroll);
            } else if(event.type == Event::KeyPressed){             // key pressed
                app.keyPress(event.key);
            } else if(event.type == Event::Resized){                // window resized
                app.resize(event.size);
            } else if(event.type == Event::Closed){                 // window closed
                window.close();
            }
        }
    }

    return 0;
}