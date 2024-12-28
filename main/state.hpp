#pragma once

#include "esp_timer_cxx.hpp"


#include "../defines.h"
#include "engine.hpp"
#include "pin.hpp"
#include "dial.hpp"
#include "relay.hpp"
#include "display.hpp"
//#include <M5GFX.h>

class State
{
public:
    State();

    void update();

    //M5GFX *display = new M5GFX();
//    M5Canvas *canvas = new M5Canvas(display);
    Display *display = new Display();
    Engine *engine = new Engine();
    Dial *dial = new Dial();
    Relay *relay = new Relay();
    Pin *clutch = new Pin(CLUTCH_GPIO_PIN);
    Pin *start_btn = new Pin(START_BUTTON_GPIO_PIN);

private:
    void start_engine();
    void stop_engine();

    void sleep();

    void update_display();

    unsigned long last_tick_time = esp_timer_get_time();

    bool is_cranking_engine = false;
};
