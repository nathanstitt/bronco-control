#ifndef RING_BUGGER_HPP
#define RING_BUGGER_HPP

#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include <cstdint>
#include <iostream>

static const int64_t TIME_OUT = 250000; // 250ms
static const int64_t DEBOUNCE =   1000; //   1ms

class RingBuffer {
public:

    struct Node {
        uint64_t value; // Holds the timestamp
        Node* next;     // Points to the next node
    };

    RingBuffer() {

        for (int i = 0; i < BUFFER_SIZE; i++) {
            nodes[i].value = 0;
            nodes[i].next = &nodes[(i + 1) % BUFFER_SIZE];
        }
        current = &nodes[0];
    }

    // Write a value to the current position and move the pointer to the next
    void pulse(void* arg) {
        auto now = esp_timer_get_time();
        auto interval = now - last_recording;
        if (interval < DEBOUNCE) {
            return;
        }
        current = current->next;
        current->value = interval;
        last_recording = now;
    }

    uint32_t average() const {
        // if no longer recording don't bother calculating anything
        if (last_recording + TIME_OUT < esp_timer_get_time())  {
            return 0;
        }

        uint64_t totalElapsed = 0;

        auto n = current;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            totalElapsed += n->value;
            n = n->next;
        }

        return totalElapsed / BUFFER_SIZE;


    }


    // Debugging: Print the buffer values
    void printBuffer() const {
        auto n = current;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            std::cout << "[" << n << "]" << " = " << n->value << "  :  ";
            n = n->next;
        }
    }

private:
    int64_t last_recording;
    static const int BUFFER_SIZE = 3;  // Size of the ring buffer
    Node nodes[BUFFER_SIZE];

    Node* current;
};



#endif
