#include "engine.hpp"

static const char *TAG = "TACH";

Engine::Engine() {

        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_POSEDGE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << GPIO_TACH_PIN);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&io_conf);

        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);

        gpio_isr_handler_add(GPIO_TACH_PIN, Engine::pulse, (void*)this);

        for (int i = 0; i < BUFFER_SIZE; i++) {
            nodes[i].value = 0;
            nodes[i].next = &nodes[(i + 1) % BUFFER_SIZE];
        }
        current = &nodes[0];

}

void IRAM_ATTR Engine::pulse(void* arg) {
    Engine* tach = static_cast<Engine*>(arg); // Cast void* to Engine*

    tach->pulse_count += 1;
    auto now = esp_timer_get_time();
    auto interval = now - tach->last_recording;
    if (interval < RPM_PULSE_MIN_INTERVAL) {
        return;
    }
    tach->current = tach->current->next;
    tach->current->value = interval;
    tach->last_recording = now;
}

uint32_t
Engine::rpm() const {
    return last_rpm;
}

bool
Engine::is_stopped() const {
    return last_rpm == 0;
}

bool
Engine::is_cranking() const {
    return last_rpm > 0 && last_rpm < MAX_CRANKING_RPM;
}

bool
Engine::is_running() const {
    return last_rpm >= MAX_CRANKING_RPM;
}

void
Engine::calc_rpm() {
    // if no longer recording don't bother calculating anything
    if (last_recording + NO_PULSE_TIME_OUT < esp_timer_get_time())  {
        last_rpm = 0;
    }
    ESP_LOGD(TAG, "rpm pulse count = %ld", pulse_count);
    uint64_t totalElapsed = 0;

    auto n = current;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        totalElapsed += n->value;
        n = n->next;
    }

    if (totalElapsed == 0) {
        last_rpm = 0;
        return;
    }
    uint32_t avg = totalElapsed / BUFFER_SIZE;
    ESP_LOGD(TAG, "avg = %ld", avg);
    uint32_t per_min = (MICROSEC_PER_MINUTE / avg);
    ESP_LOGD(TAG, "per min = %ld", per_min);

    last_rpm = per_min / PULSE_PER_REVOLUTION;
}

// Debugging: Print the buffer values
void
Engine::printBuffer() const {
    auto n = current;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        std::cout << "[" << n << "]" << " = " << n->value << "  :  ";
        n = n->next;
    }
}
