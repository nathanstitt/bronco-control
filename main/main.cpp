#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "button.h"
#include "esp_timer.h"
#include <M5GFX.h>


static const char *TAG = "APP";

static const int32_t RUNNING_RPM = 400;

extern void create_dial();
extern void create_tach();
extern int dial_motion();
extern bool is_engine_running();
extern int32_t read_tach_rpm();

M5GFX display;
M5Canvas  canvas(&display);
auto clutch = new Button(2);
auto button = new Button(42);


bool IS_CRANKING = false;


void shutdown() {
     //M5Dial.Power.powerOff();
    display.powerSaveOn();
}

static void dial_button_pressed_callback(Button* button, void* userData) {
    if (button->read() && clutch->read()) {
        if (is_engine_running()) {
            shutdown();
        } else {
            IS_CRANKING = true;
        }

    }
    ESP_LOGI(TAG, "Button pressed");
}


// M5Canvas sub_canvas1(&canvas);
// M5Canvas sub_canvas2(&canvas);


extern "C"
void app_main(void) {


    int32_t x;
    int32_t y;
    display.begin();
    display.fillScreen(TFT_BLACK);

    x = display.width();// * .9; /// 2;
    y = display.height();// * .9; /// 2;


    canvas.createSprite(x, y);
    canvas.fillSprite(TFT_PINK);
    canvas.setCursor(15, 50);
    canvas.setTextColor(TFT_BLACK);
    canvas.setFont(&fonts::DejaVu56); //Orbitron_Light_32);
    //canvas.printf("%04d:%04d\r\n", 210, -1);
    canvas.println("M5Canvas");
    //display.startWrite();
    canvas.pushSprite(0, 0);
    //display.endWrite();

    //canvas.clear();


    // canvas.println("L3");
    // canvas.pushSprite(0, 0);

    // auto x = display.width() ; //0.9; //// / 2;
    // auto y = display.height() / 2;
    // canvas.createSprite(x, y);
    // canvas.fillSprite(TFT_PINK);
    // canvas.fillRect(10, 10, 20, 20, TFT_RED);
    // canvas.println("M5Canvas");

    ESP_LOGI(TAG, "Size: %ld x %ld", display.width(), display.height());

    // // Only the following process is actually drawn on the panel.
    // display.startWrite();
    // //display.println("Display");

    // canvas.pushSprite(x, y); //x, y);

    // display.endWrite();

    button->begin();
    button->setPressedCallback(dial_button_pressed_callback, NULL);

    auto last_time = esp_timer_get_time();

    create_tach();
    create_dial();

    while (1) {
        auto rpm = read_tach_rpm();

        auto current_time = esp_timer_get_time();

        // 100ms
        if (last_time + 1000000 < current_time) {
            last_time = current_time;

            auto motion = dial_motion();

            // canvas.createSprite(x, y);
            // canvas.fillSprite(TFT_PINK);
            // canvas.setCursor(10, 100);
            // canvas.setTextColor(TFT_BLACK);
            // canvas.setFont(&fonts::Orbitron_Light_32);
            // canvas.printf("%04d:%04d\r\n", tach, motion);
            // display.startWrite();

            // canvas.clear();
            // canvas.printf("%04d:%04d\r\n", tach, motion);
            // display.clear();
            // display.startWrite();
//            canvas.clear();
            canvas.clearDisplay(TFT_BLUE);
            canvas.setCursor(10, 100);
            canvas.printf("%4ld:%d\r\n", rpm, motion);
            canvas.pushSprite(0, 0);




            //canvas.pushSprite(0, 0);

            ESP_LOGI(TAG, "RPM: %ld  dial %d", rpm, motion);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}
