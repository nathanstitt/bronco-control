#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "hal/pcnt_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#include "ring-buffer.hpp"

#define ESP_INTR_FLAG_DEFAULT 0

static  RingBuffer rb;

// static pcnt_unit_handle_t pcnt_unit = NULL;

static const gpio_num_t GPIO_TACH_PIN = GPIO_NUM_1;
static const char *TAG = "TACH";
int64_t last_time;
static int32_t last_rpm;

const int MIN_INTERVAL = 50000;
const auto PCNT_HIGH_LIMIT = 1000;
const auto PCNT_LOW_LIMIT = -10; // should never go negative
const int8_t GPIO_B = -1;

// const int8_t CYLINDERS = 6;
// const int8_t PULSE_PER_REVOLUTION = CYLINDERS / 2;
// 60,000,000 microsectons per minute
const uint32_t TICKS_PER_MINUTE = 60000000; // / INTERVAL;


void IRAM_ATTR gpio_isr_handler(void* arg) {
    rb.pulse(arg);
}


void create_tach() {

    // Configure the GPIO pin as input with an interrupt on a falling edge
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE; //GPIO_TACH_PIN;   // Interrupt on falling edge
    io_conf.mode = GPIO_MODE_INPUT;          // Set as input mode
    io_conf.pin_bit_mask = (1ULL << GPIO_TACH_PIN); // Select GPIO pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;    // No pull-down
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        // No pull-up
    gpio_config(&io_conf);

    // Install GPIO ISR service with specific interrupt flags
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_TACH_PIN, gpio_isr_handler, (void*) GPIO_TACH_PIN);

    // last_time = esp_timer_get_time();
    // last_rpm = 0;

    // ESP_LOGI(TAG, "install pcnt unit");
    // pcnt_unit_config_t unit_config;

    // unit_config.high_limit = PCNT_HIGH_LIMIT;
    // unit_config.low_limit = PCNT_LOW_LIMIT;

    // ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    // ESP_LOGI(TAG, "set glitch filter");
    // pcnt_glitch_filter_config_t filter_config = {
    //         .max_glitch_ns = 1000,
    // };
    // ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    // ESP_LOGI(TAG, "install pcnt channels");
    // pcnt_chan_config_t chan_a_config;
    // chan_a_config.edge_gpio_num = GPIO_A;
    // chan_a_config.level_gpio_num = GPIO_B;

    // pcnt_channel_handle_t pcnt_chan_a = NULL;
    // ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    // ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    // ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));

    // ESP_LOGI(TAG, "enable pcnt unit");
    // ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    // ESP_LOGI(TAG, "clear pcnt unit");
    // ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    // ESP_LOGI(TAG, "start pcnt unit");
    // ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    // gpio_config_t io_conf = {
    //     .pin_bit_mask = (1ULL << GPIO_A), // Pin mask for the specific GPIO
    //     .mode = GPIO_MODE_INPUT,                 // Set as input mode
    //     .pull_up_en = GPIO_PULLUP_DISABLE,        // Enable pull-up resistor
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,   // Disable pull-down resistor
    //     .intr_type =  GPIO_INTR_DISABLE           // No interrupt
    // };
    // gpio_config(&io_conf);

}


uint32_t read_tach_rpm() {
    auto elapsed = rb.average();
    rb.printBuffer();
    ESP_LOGI(TAG, "read_tach_rpm %ld", elapsed);

    if (elapsed == 0) {
        return 0;
    }
    return (TICKS_PER_MINUTE / elapsed) / 3;

        // int64_t time = esp_timer_get_time();

        // if (last_time + MIN_INTERVAL > time) {
        //         return last_rpm;
        // }

        // auto elapsed = time - last_time;

        // int pulse_count = 0;
        // pcnt_unit_get_count(pcnt_unit, &pulse_count);

        // last_rpm = (pulse_count / 2) * (TICKS_PER_MINUTE / elapsed);

        // last_time = time;
        // pcnt_unit_clear_count(pcnt_unit);

        // ESP_LOGI(TAG, "read_tach_rpm %lld", elapsed);

        // return last_rpm;
}

bool is_engine_running() {
    return last_rpm > 400;
}
