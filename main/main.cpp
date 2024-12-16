// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include "esp_sleep.h"
// //#include "button.h"
// #include "esp_timer.h"
// #include "tachometer.hpp"
// #include <M5GFX.h>
// #include "../defines.h"

#include "state.hpp"



extern "C"
void app_main(void) {

    auto state = new State();

    while (1) {
        state->update();

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}
