#include "state.hpp"


extern "C"
void app_main(void) {

    auto state = new State();

    while (1) {
        state->update();

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}
