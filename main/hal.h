#ifndef HAL_H
#define HAL_H

#include "./rotary_dial.h"
#include "./tachometer.h"

// struct for hal that contains tach and dial

struct Hal {
    RotaryDial dial;
    Tachometer tach;
};

Hal* initialize_hal();


// class Hal {
// public:

//     void setup();

//     RotaryDial dial;
//     Tachometer tach;

//     void loop();

// private:

// };

#endif
