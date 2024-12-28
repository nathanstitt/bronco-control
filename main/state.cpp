#include "state.hpp"

#include <esp_sleep.h>
#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include <chrono>
#include "bsp/m5dial.h"


static const char *TAG = "STATE";


State::State() {
    clutch->begin();
    start_btn->begin();
}



void
State::start_engine() {
    is_cranking_engine = true;

    idf::esp_timer::ESPTimer timer([this]() {
        is_cranking_engine = false;
        ESP_LOGI(TAG, "Lambda Timer triggered after 5 seconds!");
    }, "StopCrankingTimer");

    ESP_LOGI(TAG, "Starting timer...");
    timer.start(std::chrono::seconds(5));
}

void
State::stop_engine() {

    sleep();
}

void
State::sleep() {
    // basically deepSleep from https://github.com/m5stack/M5Unified/blob/master/src/utility/Power_Class.hpp
    // display->sleep();
    // display->waitDisplay();

    display->sleep();

    int value = gpio_get_level(CLUTCH_GPIO_PIN);
    int level = value ? 0 : 1;
    ESP_LOGI(TAG, "Button level: %d", level);

    auto err = esp_sleep_enable_ext0_wakeup(CLUTCH_GPIO_PIN, level);

    if (ESP_OK != err) {
      ESP_LOGE(TAG, "Failed to enable wakeup source: %s", esp_err_to_name(err));
    }
    gpio_wakeup_enable(CLUTCH_GPIO_PIN, gpio_int_type_t::GPIO_INTR_LOW_LEVEL);

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);

    esp_deep_sleep_start();
}

void
State::update() {
    auto current_time = esp_timer_get_time();

    if ((last_tick_time + 1000000) > current_time) {
        return;
    }

    last_tick_time = current_time;
    engine->calc_rpm();
    update_display();

    if (clutch->is_pressed() && start_btn->is_pressed()) {
        if (engine->is_running()) {
            this->stop_engine();
        } else if (!engine->is_stopped()) {
            this->start_engine();
        }
    }
}

void
State::update_display() {
    auto motion = dial->motion();

    display->update(engine->rpm());

    ESP_LOGI(TAG, "RPM: %ld   DIAL %d", engine->rpm(), motion);

    if (motion != 0) {
        relay->set_state(motion > 0);
    }

    if (motion > 4) {
        this->sleep();
    }

}
