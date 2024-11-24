#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"

static const char *TAG = "example";



pcnt_unit_handle_t dial_pcnt_unit = NULL;


extern void create_dial();
extern void create_tach();
extern int dial_motion();
extern int tach_rpm();

void app_main(void) {
  create_tach();
  create_dial();

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));

    int rpm_pulse_count = 0;
    int dial_pulse_count = 0;
    /* pcnt_unit_get_count(tach_pcnt_unit, &dial_pulse_count); */
    /* pcnt_unit_get_count(dial_pcnt_unit, &rpm_pulse_count); */

    ESP_LOGI(TAG, "RPM: %d  dial %d", tach_rpm(), dial_motion());
  }
}
