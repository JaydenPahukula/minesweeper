#include "Game.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <fstream>
#include <iostream>
#include <string>
using namespace std;


Game::Game(const string filename){

    //open file
    ifstream configfile(filename);
    if (!configfile.is_open()){
        cerr << endl << "error opening config file" << endl;
        throw exception();
    }

    //parse file
    string id;
    int value;
    while(!configfile.eof()){
        //parse line
        getline(configfile, id, '=');
        if (id[0] == '\n'){ id.erase(0, 1); }
        configfile >> value;

        //initialize values
        if (id == "gamewidth"){
            _width = value;
        } else if (id == "gameheight"){
            _height = value;
        }
    }

}



void Game::draw(const sf::RenderWindow window){
    return;
}


void Game::click(const sf::Event::MouseButtonEvent mouse){
    cout << "clicked" << endl;
    return;
}


unsigned int Game::width() const {
    return _width;
}

unsigned int Game::height() const {
    return _height;
}
