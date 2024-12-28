#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"                         \
                                                \
    //#include <driver/i2c_master.h>

#define M5GFX_BOARD 12 //board_M5Dial

static const gpio_num_t M5EPD_MAIN_PWR_PIN = (gpio_num_t)2;

static const gpio_num_t GPIO_TACH_PIN = GPIO_NUM_1;
static const gpio_num_t CLUTCH_GPIO_PIN = GPIO_NUM_2;
static const gpio_num_t START_BUTTON_GPIO_PIN = GPIO_NUM_42;

static const gpio_num_t RELAY_SDA_GPIO_PIN = GPIO_NUM_13;
static const gpio_num_t RELAY_SCL_GPIO_PIN = GPIO_NUM_15;

static const int32_t MAX_CRANKING_RPM = 400;

static const uint8_t PULSE_PER_REVOLUTION = 3; // cylinder count / 2
static const uint32_t MICROSEC_PER_MINUTE = 60000000;
static const uint16_t MIN_INTERVAL = 50000;

static const uint32_t MAX_CRANKING_MICROSEC = 1000000 * 5;

static const uint32_t RPM_PULSE_MIN_INTERVAL = 1000; //   1ms

static const uint8_t RELAY_I2C_ADDRESS = 0x20; // 0x27;
static const i2c_port_t RELAY_I2C_PORT_NUM = I2C_NUM_0;
// set rpm to 0 if no p;ulses in this time
static const int64_t NO_PULSE_TIME_OUT = 250000; // 250ms

