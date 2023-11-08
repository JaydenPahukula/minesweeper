#ifndef CONFETTI_H
#define CONFETTI_H

#include <SFML/Graphics.hpp>

#include <unordered_set>


class ConfettiCannon {
    public:
        // constructor
        ConfettiCannon();

        // launch
        void launch(const sf::Vector2f pos);

        // draw
        void draw(sf::RenderWindow& window);

    private:

        // confetti object
        class Confetti {
            public:
                // constructor
                Confetti(const sf::Vector2f pos, std::unordered_set<Confetti*>* parentSet);
                // draw
                bool draw(sf::RenderWindow& window);
            private:
                // data
                sf::ConvexShape _shape;
                float _vx, _vy;
                bool _rotationDir;
                std::unordered_set<Confetti*>* _parentSet;
        };

        // set of confetti
        std::unordered_set<Confetti*> set;

};



#endif