#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
using namespace std;

int main() {
    RenderWindow window( VideoMode(640, 640), "minesweeper", sf::Style::Close);




    Event event;

    while( window.isOpen() ) {
        window.clear();






        window.display();

        while( window.pollEvent(event) ) {
            if(event.type == Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}