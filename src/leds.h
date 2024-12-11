#ifndef LEDS_H
#define LEDS_H

#include <stdio.h>
 
// LED Logic
// logical 1 - 0.8 us
// logical 0 - 0.4 us

#define LED_NUMBER         16
#define LED_LOGICAL_ONE    31
#define LED_LOGICAL_ZERO   15

// one rgb color
typedef struct {
    uint16_t g;
    uint16_t r;
    uint16_t b;

} rgb_color;

// one neopixel led
typedef struct {
    uint16_t g[8];
    uint16_t r[8];
    uint16_t b[8];

} neopixel_led;

void reset_all_leds(neopixel_led* leds, uint16_t number_leds);
void set_specific_led(neopixel_led* leds, uint16_t number_leds, uint16_t led_position, rgb_color color);

#endif // LEDS_H