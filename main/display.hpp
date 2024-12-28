#pragma once

#include "bsp/m5dial.h"
#include <lvgl.h>

class Display
{
public:

    void update(uint32_t rpm);

    void sleep();



Display();

    private:

    lv_color_t blue = lv_color_make(0, 0, 200);

    lv_display_t *disp;
    lv_obj_t* screen;
    lv_obj_t* label;
    esp_lcd_touch_handle_t tp;
};
