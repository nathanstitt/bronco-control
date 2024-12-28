#include "dial.hpp"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "hal/pcnt_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char *TAG = "DIAL";
static pcnt_unit_handle_t pcnt_unit = NULL;

Dial::Dial() {

    const int8_t PCNT_HIGH_LIMIT = 100;
    const int8_t PCNT_LOW_LIMIT = -100;
    const int8_t GPIO_A = 41;
    const int8_t GPIO_B = 40;


    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config;
    //unit_config.pulse_gpio_num = 2;
    unit_config.intr_priority = 3;

    unit_config.low_limit = PCNT_LOW_LIMIT;
    unit_config.high_limit = PCNT_HIGH_LIMIT;

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {};
    chan_a_config.edge_gpio_num = GPIO_A;
    chan_a_config.level_gpio_num = GPIO_B;

    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {};
    chan_b_config.edge_gpio_num = GPIO_B;
    chan_b_config.level_gpio_num = GPIO_A;

    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

int Dial::motion() {
    int count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    return count; // each click of dial is approx 4 counts
}
