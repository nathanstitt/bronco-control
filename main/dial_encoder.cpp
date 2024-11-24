#include "dial_encoder.h"


#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "hal/pcnt_types.h"

static const char* TAG_ENCODER = "ESP32Encoder";
static const char *TAG = "DIAL";

enum puType ESP32Encoder::useInternalWeakPullResistors = DOWN;
//ESP32Encoder* ESP32Encoder::encoders[MAX_ESP32_ENCODERS] = {nullptr};

ESP32Encoder::ESP32Encoder(bool always_interrupt_, enc_isr_cb_t enc_isr_cb, void* enc_isr_cb_data)
    : always_interrupt{always_interrupt_},
      aPinNumber{GPIO_NUM_NC},
      bPinNumber{GPIO_NUM_NC},
      count{0},
      _enc_isr_cb(enc_isr_cb),
      _enc_isr_cb_data(enc_isr_cb_data),
      attached{false} {}

ESP32Encoder::~ESP32Encoder() {
    detach();
}

#define EXAMPLE_PCNT_HIGH_LIMIT 100
#define EXAMPLE_PCNT_LOW_LIMIT  -100

void ESP32Encoder::attach(int pinA, int pinB) {
    if (attached) {
        ESP_LOGE(TAG_ENCODER, "Already attached");
        return;
    }

    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config;
    unit_config.high_limit = 100;
    unit_config.low_limit = -100;

    //pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = pinB,
        .level_gpio_num = pinA,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));


     // ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_INVERSE, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));


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

    attached = true;
}



int64_t ESP32Encoder::getCount() {
    int raw_count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &raw_count));
    return count + raw_count;
}

void ESP32Encoder::clearCount() {
    count = 0;
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
}
