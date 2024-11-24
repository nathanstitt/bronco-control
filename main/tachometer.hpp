#pragma once


#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "hal/pcnt_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_timer.h"



class Tachometer {
public:
    //static constexpr const auto TICKS_PER_MINUTE = 60000000; // one min
    static constexpr const auto MIN_INTERVAL = 100000 // microsectons, 100ms

    static constexpr const char* TAG = "TACH";
    static constexpr const int8_t PCNT_HIGH_LIMIT = 100;
    static constexpr const int8_t PCNT_LOW_LIMIT = -100;
    static constexpr const int8_t GPIO_A = 1;
    static constexpr const int8_t GPIO_B = -1;

    pcnt_unit_handle_t pcnt_unit = NULL;

    int64_t last_time = 0;
    int rpm = 0;

    void setup() {
        last_time = esp_timer_get_time();

        ESP_LOGI(TAG, "install pcnt unit");
        pcnt_unit_config_t unit_config;
        //unit_config.pulse_gpio_num = 1;

        unit_config.high_limit = PCNT_HIGH_LIMIT;
        unit_config.low_limit = PCNT_LOW_LIMIT;

        ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &this->pcnt_unit));

        ESP_LOGI(TAG, "set glitch filter");
        pcnt_glitch_filter_config_t filter_config = {
            .max_glitch_ns = 1000,
        };
        ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

        ESP_LOGI(TAG, "install pcnt channels");
        pcnt_chan_config_t chan_a_config;
        chan_a_config.edge_gpio_num = GPIO_A;
        chan_a_config.level_gpio_num = GPIO_B;

        pcnt_channel_handle_t pcnt_chan_a = NULL;
        ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

        pcnt_chan_config_t chan_b_config;
        chan_a_config.edge_gpio_num = GPIO_B;
        chan_a_config.level_gpio_num = GPIO_A;

        pcnt_channel_handle_t pcnt_chan_b = NULL;
        ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

        ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
        ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
        ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
        ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
        ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

        // ESP_LOGI(TAG, "add watch points and register callbacks");
        // int watch_points[] = {PCNT_LOW_LIMIT, -50, 0, 50, PCNT_HIGH_LIMIT};
        // for (size_t i = 0; i < sizeof(watch_points) / sizeof(watch_points[0]); i++) {
        //     ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, watch_points[i]));
        // }
        // pcnt_event_callbacks_t cbs = {
        //   .on_reach = example_pcnt_on_reach,
        // };
        // QueueHandle_t queue = xQueueCreate(10, sizeof(int));
        // ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue));

        ESP_LOGI(TAG, "enable pcnt unit");
        ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
        ESP_LOGI(TAG, "clear pcnt unit");
        ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
        ESP_LOGI(TAG, "start pcnt unit");
        ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    }

    void calculate_rpm() {

        auto time = esp_timer_get_time();

        if (last_time + MIN_INTERVAL > time) {
            return;
        }

        auto elapsed = time - last_time;

        int pulse_count = 0;
        pcnt_unit_get_count(this->pcnt_unit, &pulse_count);


//        return pulse_count;

        rpm = pulse_count * (MINUTE / elapsed);

        last_time = time;
        pcnt_unit_clear_count(this->pcnt_unit);




        // int pulses_per_minute = pulse_count * (60 / sampling_interval);

        ESP_LOGI(TAG, "Pulse count: %d ", pulse_count);


        // if (seconds_elapsed > sampling_interval) {
        //     pcnt_unit_clear_count(pcnt_unit);

        //     seconds_elapsed = 0;
        // }

        // vTaskDelay(pdMS_TO_TICKS(1000)); // Print every second

        // int count = 0;
        // ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));
        // ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
        // if (count < 0) return -1;
        // if (count > 0) return 1;
        // return 0;
    }

};
