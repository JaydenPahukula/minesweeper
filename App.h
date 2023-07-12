#ifndef APP_H
#define APP_H

#include "Game.h"

#include <SFML/Graphics.hpp>
#include <vector>


class App {
    public:

        // constructor
        App();
        // destructor
        ~App();

        // draw the app
        void draw(sf::RenderWindow &window);

        // click event
        void click(const sf::Event::MouseButtonEvent mouse);

        // getters
        unsigned int gameWidth() const;
        unsigned int gameHeight() const;
    
    private:

        Game* _game;

        // timer
        bool _timerRunning;
        time_t _startTime, _currTime;

        // sprites
        sf::Texture _appspritesheet;
        std::vector<sf::Sprite> _borderSprites;
        sf::Sprite _happyFaceSprite, _coolFaceSprite, _sadFaceSprite;
        sf::Sprite _digitSprites[6][10];

        bool _loadBorderSprites();

};


#endif