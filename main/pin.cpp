#include "pin.hpp"

#include "driver/gpio.h"
#include "esp_timer.h"


#define LOW     0
#define HIGH    1

#define millis() (esp_timer_get_time() / 1000LL)

#define pinMode(_pin, INPUT_PULLUP)                         \
    gpio_set_direction((gpio_num_t)_pin, GPIO_MODE_INPUT);  \
    gpio_pullup_en((gpio_num_t)_pin)

#define digitalRead(_pin) gpio_get_level((gpio_num_t)_pin)


Pin::Pin(uint8_t pin, uint16_t debounce_ms)
    :  _pin(pin)
    ,  _delay(debounce_ms)
    ,  _state(HIGH)
    ,  _ignore_until(0)
    ,  _has_changed(false)
{
}

void Pin::begin()
{
    pinMode(_pin, INPUT_PULLUP);
}


bool Pin::read() {
    // ignore pin changes until after this delay time
    if (_ignore_until > millis()) {
        // ignore any changes during this period
    } else if (digitalRead(_pin) != _state) { // pin has changed
        _ignore_until = millis() + _delay;
        _state = !_state;
        _has_changed = true;
    }


    if (!_state && has_changed()) {
        /* Call callback */
        if (_pressed_callback != nullptr) {
            _pressed_callback(this, _user_data);
        }
    }

    return _state;
}

// has the pin been toggled from on -> off, or vice versa
bool Pin::is_toggled() {
    read();
    return has_changed();
}

// mostly internal, tells you if a pin has changed after calling the read() function
bool Pin::has_changed() {
    if (_has_changed) {
        _has_changed = false;
        return true;
    }
    return false;
}

// has the pin gone from off -> on
bool Pin::is_pressed() {
    // return (read() == PRESSED && has_changed());

    if ((read() == PRESSED && has_changed())) {
        /* Call callback */
        if (_pressed_callback != nullptr)
            {
                _pressed_callback(this, _user_data);
            }
        return true;
    }
    return false;
}

// has the pin gone from on -> off
bool Pin::is_released() {
    return (read() == RELEASED && has_changed());
}


void Pin::setPin(uint8_t pin) {
    _pin = pin;
}

void Pin::setDebounce(uint16_t ms) {
    _delay = ms;
}
