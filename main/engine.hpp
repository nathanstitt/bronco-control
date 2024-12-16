#pragma once

#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "hal/pcnt_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include <iostream>

#include "../defines.h"

class Engine {
public:

    struct Node {
        uint64_t value;
        Node* next;
    };

    Engine();

    static void IRAM_ATTR pulse(void* arg);

    uint32_t rpm() const;

    bool is_stopped() const;

    bool is_cranking() const;

    bool is_running() const;

    void calc_rpm();

    // Debugging: Print the buffer values
    void printBuffer() const;


    Node* current;

private:
    int64_t last_recording;
    static const int BUFFER_SIZE = 3;  // Size of the ring buffer
    Node nodes[BUFFER_SIZE];
    uint32_t last_rpm = 0;
    uint32_t pulse_count = 0;

};

