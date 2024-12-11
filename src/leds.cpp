#include "leds.h"


// reset all
void reset_all_leds(neopixel_led* leds, uint16_t number_leds) {
    for (int i = 0; i < number_leds; i++) {
        for (int j = 0; j < 8; j++) {
            (leds + i) -> r[j] = LED_LOGICAL_ZERO;
            (leds + i) -> g[j] = LED_LOGICAL_ZERO;
            (leds + i) -> b[j] = LED_LOGICAL_ZERO;
        }
    }
}

void reset_specific_led(neopixel_led* leds, uint16_t led_position) {
    for (int j = 0; j < 8; j++) {
        leds[led_position].r[j] = LED_LOGICAL_ZERO;
        leds[led_position].g[j] = LED_LOGICAL_ZERO;
        leds[led_position].b[j] = LED_LOGICAL_ZERO;
    }
}

void set_specific_led(neopixel_led* leds, uint16_t led_position, rgb_color color) {
    for (int j = 0; j < 8; j++) {
    if (color.r & 0x1 << j) {
        (leds + led_position) -> r[7 - j] = LED_LOGICAL_ONE;
    }
    else {
        (leds + led_position) -> r[7 - j] = LED_LOGICAL_ZERO;
    }
    if (color.g & 0x1 << j) {
        (leds + led_position) -> g[7 - j] = LED_LOGICAL_ONE;
    }
    else {
        (leds + led_position) -> g[7 - j] = LED_LOGICAL_ZERO;
    }
    if (color.b & 0x1 << j) {
        (leds + led_position) -> b[7 - j] = LED_LOGICAL_ONE;
    }
    else {
        (leds + led_position) -> b[7 - j] = LED_LOGICAL_ZERO;
    }
}
    
}