
#include "definitions.h"

#include "Confetti.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <cmath>
#include <cstdlib>
#include <unordered_set>
using namespace std;



ConfettiCannon::ConfettiCannon(){
    // set of confetti
    set = unordered_set<Confetti*>();
}



void ConfettiCannon::launch(const Vector2f pos){
    // create confetti
    for (int _ = 0; _ < NUMCONFETTI; _++){
        set.insert(new Confetti(pos, &set));
    }
    return;
}



void ConfettiCannon::draw(RenderWindow& window){
    if (set.size() > 0){
        bool done = false;
        for (Confetti* confetti: set){
            if (confetti->draw(window)){
                done = true;
            }
        }
        // clear set when all confetti are off screen
        if (!done){
            for (Confetti* confetti: set) delete confetti;
            set.clear();
        }
    }
    return;
}



ConfettiCannon::Confetti::Confetti(const Vector2f pos, unordered_set<Confetti*>* parentSet){

    // set parent set
    _parentSet = parentSet;

    // init velocity
    const float PI = 2 * acos(0.0);
    float angle  = (float)rand()/RAND_MAX*2*PI;
    float magnitude = (float)rand()/RAND_MAX*8;
    _vx = magnitude*cos(angle);
    _vy = magnitude*sin(angle);

    // init rotation
    _rotationDir = rand()%2 == 0;

    // create shape
    _shape = ConvexShape(4);
    _shape.setPoint(0, Vector2f(8, 8));
    _shape.setPoint(1, Vector2f(8, -8));
    _shape.setPoint(2, Vector2f(-8, -8));
    _shape.setPoint(3, Vector2f(-8, 8));
    _shape.setOutlineThickness(0);

    // position shape
    _shape.setPosition(pos);

    // set color
    Color color;
    switch (rand()%5){
        case 0: color = Color(220, 5, 5); break; // red
        case 1: color = Color(100, 240, 20); break; // green
        case 2: color = Color(20, 150, 250); break; // blue
        case 3: color = Color(255, 245, 0); break; // yellow
        case 4: color = Color(250, 145, 230); break; // pink
    }
    _shape.setFillColor(color);
}



bool ConfettiCannon::Confetti::draw(RenderWindow& window){
    // apply acceleration
    _vy += 0.6;
    
    // apply velocity (and scale _vx with window width)
    _shape.move(_vx*window.getSize().x/480, _vy);
    
    // rotate
    _shape.rotate(10*(_rotationDir ? 1 : -1));

    // draw shape
    window.draw(_shape);

    // return false if object is off the screen
    return _shape.getPosition().y <= window.getSize().y;
}

