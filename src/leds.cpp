#include "leds.h"

rgb_color red = {255, 0, 0};
rgb_color orange = {255, 165, 0};
rgb_color yellow = {255, 255, 0};
rgb_color green = {0, 255, 0};
rgb_color blue = {0, 0, 255};
rgb_color black = {0, 0, 0};


void reset_all()
{
    ARGB_Clear();
    show_leds();
}

void light_led(uint8_t index, rgb_color color)
{
    ARGB_SetRGB(index, color.r, color.g, color.b);
    show_leds();

}



void show_leds()
{
    ARGB_Show();
}

void test_leds()
{
    ARGB_SetBrightness(10);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        light_led(i, blue);
        HAL_Delay(100);
        ARGB_Clear();
    }
    reset_all();
}
