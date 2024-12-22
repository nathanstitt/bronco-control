#pragma once

#include "esp_err.h"
//#include <i2c_cxx.hpp>
#include "../defines.h"
//#include "driver/i2c.h"
//#include <i2c_cxx.hpp>

class Relay {
public:
    Relay();

    esp_err_t set_state(bool state);

private:
  //  idf::I2CMaster *i2c;
};
