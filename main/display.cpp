#include "display.hpp"
#include "esp_log.h"
#include "bsp/m5dial.h"
#include <lvgl.h>

#include "esp_lcd_gc9a01.h"
#include "esp_lcd_touch_ft5x06.h"
#include "esp_lvgl_port.h"
#include "bsp/touch.h"
#include "bsp/display.h"
#include "esp_lcd_gc9a01.h"
#include "esp_lcd_touch_ft5x06.h"


// #include "esp_err.h"
// #include "esp_check.h"
// #include "hal/lcd_types.h"

static const char *TAG = "DISPLAY";

extern const lv_font_t lv_font_montserrat_36;

Display::Display() {
    disp = bsp_display_start();

    lv_disp_set_default(disp);

    ESP_LOGI(TAG, "Starting display");

    bsp_display_backlight_on();
    screen = lv_obj_create(nullptr);

    lv_obj_set_size(screen, 240, 240);

    lv_obj_set_style_bg_color(screen, blue, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);
    label = lv_label_create(screen);
    lv_label_set_text(label, ":-)");
    lv_obj_center(label);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_48);
    lv_obj_add_style(label, &style, LV_PART_MAIN);

    bsp_display_brightness_set(50);

    lv_disp_load_scr(screen);
}


void
Display::update(uint32_t rpm) {
    char buf[30];
    lv_snprintf(buf, 30, "%d", rpm);

    lv_label_set_text(label, buf);
}

void
Display::sleep() {

}
