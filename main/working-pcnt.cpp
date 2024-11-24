/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "dial_encoder.h"

static const char *TAG = "main";

#define EXAMPLE_PCNT_HIGH_LIMIT 100
#define EXAMPLE_PCNT_LOW_LIMIT  -100

#define EXAMPLE_EC11_GPIO_A 1
#define EXAMPLE_EC11_GPIO_B 2

#define HAL_PIN_ENCODER_A       41
#define HAL_PIN_ENCODER_B       40
#define HAL_PIN_PWR_WAKE_UP     42


static int _last_count = 0;
void _dial_moved_callback(ESP32Encoder* dial, void* userData)
{
    int new_shit = static_cast<int>(dial->getCount()) / 2;
    if (new_shit < _last_count)
        {
            ESP_LOGI(TAG, "click down");
            // printf("???\n");

            _last_count = new_shit;
        }
    else
        {
            // printf("666\n");
            ESP_LOGI(TAG, "click up");

            _last_count = new_shit;
        }
}

    void _dial_button_pressed_callback(Button* button, void* userData)
    {

ESP_LOGI(TAG, "btn click");
    }


extern "C"
void app_main(void)
{

    // auto dial = ESP32Encoder();
    // dial.attach(HAL_PIN_ENCODER_A, HAL_PIN_ENCODER_B);
    // dial.setMovedCallback(_dial_moved_callback, 0);

    
    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config;
    unit_config.high_limit = EXAMPLE_PCNT_HIGH_LIMIT;
    unit_config.low_limit = EXAMPLE_PCNT_LOW_LIMIT;

    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = EXAMPLE_EC11_GPIO_A,
        .level_gpio_num = -1, // EXAMPLE_EC11_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));

    ESP_LOGI(TAG, "add watch points and register callbacks");
    int watch_points[] = {EXAMPLE_PCNT_LOW_LIMIT, -50, 0, 50, EXAMPLE_PCNT_HIGH_LIMIT};
    for (size_t i = 0; i < sizeof(watch_points) / sizeof(watch_points[0]); i++) {
        ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, watch_points[i]));
    }

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));



    /* Set button pressed callback */
    // dial.btn.setPressedCallback(_dial_button_pressed_callback, this);
    // dial.btn.setPin(HAL_PIN_PWR_WAKE_UP);
    // dial.btn.setPressedCallback(_dial_button_pressed_callback, 0);

    int seconds_elapsed = 0;   // To track elapsed time in seconds

    // Report counter value
    int pulse_count = 0;

    int sampling_interval = 3;

    while (1) {
        pcnt_unit_get_count(pcnt_unit, &pulse_count);

        seconds_elapsed++;


        int pulses_per_minute = pulse_count * (60 / sampling_interval);

        ESP_LOGI(TAG, "Pulse count: %d RPM: %d", pulse_count, pulses_per_minute);


        if (seconds_elapsed > sampling_interval) {
            pcnt_unit_clear_count(pcnt_unit);

            seconds_elapsed = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Print every second

    }
}
