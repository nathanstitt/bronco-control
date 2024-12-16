#include "state.hpp"

#include <esp_sleep.h>
#include "esp_timer_cxx.hpp"
#include "esp_exception.hpp"
#include <chrono>

static const char *TAG = "STATE";


State::State() {
    clutch->begin();
    start_btn->begin();
    //canvas = new M5Canvas(&display);
    display->begin();
    display->fillScreen(TFT_BLACK);

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
    display->sleep();
    display->waitDisplay();

    esp_sleep_enable_ext0_wakeup(START_BUTTON_GPIO_PIN, false);
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

    ESP_LOGI(TAG, "RPM: %ld dial", engine->rpm());
}


void
State::update_display() {
    canvas->clearDisplay(TFT_BLUE);
    canvas->setCursor(10, 100);
    canvas->printf("%4ld:%d\r\n", engine->rpm(), dial->motion());
    canvas->pushSprite(0, 0);

}
