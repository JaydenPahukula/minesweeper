#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
    public:

        // constructor
        Timer();
        
        // control
        void reset();
        void start();
        void stop();

        // getters
        unsigned int seconds() const;

    private:

        bool _running;
        std::chrono::steady_clock _clock;
        std::chrono::time_point<std::chrono::steady_clock> _startTime, _endTime;

};


#endif