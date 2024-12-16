#pragma once

#include "esp_err.h"

class Relay {
public:
    Relay();

    esp_err_t set_state(bool state);
};
