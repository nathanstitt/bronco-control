#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>

#define GPIO_INPUT_PIN 1  // Define the GPIO pin you are using
#define ESP_INTR_FLAG_DEFAULT 0
#define DEBOUNCE_TIME_MS 50  // Set debounce time to 50 milliseconds

static const char *TAG = "Counter";

// Global counter and timestamp for debouncing
volatile int falling_edge_count = 0;
volatile int64_t last_time = 0;

// ISR handler for GPIO interrupt with debounce
void IRAM_ATTR gpio_isr_handler(void* arg) {
    int64_t current_time = esp_timer_get_time() / 1000; // Current time in ms
    if (current_time - last_time > DEBOUNCE_TIME_MS) {
        falling_edge_count++;
        last_time = current_time;
    }
}

void app_main(void) {
    // Configure the GPIO pin as input with an interrupt on a falling edge
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;   // Interrupt on falling edge
    io_conf.mode = GPIO_MODE_INPUT;          // Set as input mode
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_PIN); // Select GPIO pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;    // No pull-down
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        // No pull-up
    gpio_config(&io_conf);

    // Install GPIO ISR service with specific interrupt flags
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_INPUT_PIN, gpio_isr_handler, (void*) GPIO_INPUT_PIN);

    // Main loop to print the counter value periodically
    while (1) {
        ESP_LOGI(TAG, "Falling edge count: %d", falling_edge_count);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Print every second
    }
}
