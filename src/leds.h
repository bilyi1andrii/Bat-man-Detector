#ifndef LEDS_H
#define LEDS_H

#include <stdio.h>
#include "ARGB.h"

// one rgb color
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;

} rgb_color;

extern rgb_color red;
extern rgb_color orange;
extern rgb_color yellow;
extern rgb_color green;
extern rgb_color blue;

void reset_all();
void light_led(uint8_t index, rgb_color color);
void show_leds();
void test_leds();

#endif // LEDS_H