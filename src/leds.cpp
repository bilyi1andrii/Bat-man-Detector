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

void set_specific_led(neopixel_led* leds, uint16_t number_leds, uint16_t led_position, rgb_color color) {
    if (led_position >= number_leds)
        return;
    
    for (int j = 0; j < 8; j++) {
        leds[led_position].r[j] = (color.r & (1 << (7 - j))) ? LED_LOGICAL_ONE : LED_LOGICAL_ZERO;
        leds[led_position].g[j] = (color.g & (1 << (7 - j))) ? LED_LOGICAL_ONE : LED_LOGICAL_ZERO;
        leds[led_position].b[j] = (color.b & (1 << (7 - j))) ? LED_LOGICAL_ONE : LED_LOGICAL_ZERO;
    }
}