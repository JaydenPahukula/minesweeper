
#include "Timer.h"

#include <SFML/Graphics.hpp>
using namespace sf;

#include <chrono>
using namespace std;


Timer::Timer(){
    this->reset();
}


void Timer::reset(){
    _running = false;
    _startTime = _endTime = _clock.now();
    return;
}


void Timer::start(){
    if (!_running){
        _running = true;
        _startTime = _clock.now();
    }
    return;
}


void Timer::stop(){
    if (_running){
        _running = false;
        _endTime = _clock.now();
    }
    return;
}


unsigned int Timer::seconds(){
    
    chrono::duration<double> diff;

    if (_running) diff = _clock.now() - _startTime;
    else diff = _endTime - _startTime;
    
    return (unsigned int)diff.count();
}
