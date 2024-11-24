#include "hal.h"

#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "hal/pcnt_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


static const char* TAG = "HAL";

#define PCNT_HIGH_LIMIT 100
#define PCNT_LOW_LIMIT -100
#define ROTARY_GPIO_A 41
#define ROTARY_GPIO_B 40



// static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
// {
//     BaseType_t high_task_wakeup;
//     QueueHandle_t queue = (QueueHandle_t)user_ctx;
//     // send event data to queue, from this interrupt callback
//     xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
//     return (high_task_wakeup == pdTRUE);
// }


Hal*
initialize_hal() {
    ESP_LOGI(TAG, "setup");

    Hal *hal = (Hal*)malloc(sizeof(Hal));
    hal->tach = initialize_tach();

    initialize_setup();
    initialze_setup();

}


void hal_loop() {
    tach_calculate_rpm();
}
