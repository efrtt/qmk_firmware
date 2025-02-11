#include <string.h>
#include "rgb.h"
#include "rgb_matrix.h"
#include "rgb_matrix_types.h"
#include "color.h"

/*
    COLS key / led
    PWM PWM00A - PWM21A (PWM15A unused)
    2ty transistors PNP driven high
    base      - GPIO
    collector - LED Col pins
    emitter   - VDD

    VDD     GPIO
    (E)     (B)
     |  PNP  |
     |_______|
         |
         |
        (C)
        LED

    ROWS RGB
    PWM PWM22A - PWM21B (PWM10B unused)
    C 0-15
    j3y transistors NPN driven low
    base      - GPIO
    collector - LED RGB row pins
    emitter   - GND

        LED
        (C)
         |
         |
      _______
     |  NPN  |
     |       |
    (B)     (E)
    GPIO    GND
*/

LED_TYPE led_state[LED_MATRIX_ROWS * LED_MATRIX_COLS];
LED_TYPE underglow_led_state[UNDERGLOW_HW_TOTAL];
uint8_t led_pos[MATRIX_LED_TOTAL];

uint8_t underglow_led_pos[UNDERGLOW_LED_TOTAL] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 60, 61, 62, 63, 79, 78, 75, 59, 55, 56, 32, 33, 34, 35, 36, 37, 41, 42, 43, 44, 45, 46, 47, 16, 17, 18, 22, 23, 24, 25 };

void process_backlight(uint8_t devid, volatile LED_TYPE* states);

void init(void) {
    unsigned int i = 0;
    for (unsigned int y = 0; y < LED_MATRIX_ROWS; y++) {
        for (unsigned int x = 0; x < LED_MATRIX_COLS; x++) {
            if (g_led_config.matrix_co[y][x] != NO_LED) {
                led_pos[g_led_config.matrix_co[y][x]] = i;
            }
            i++;
        }
    }
}

static void flush(void)
{
    process_backlight(0xE8, underglow_led_state);
}

void set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    if(index < MATRIX_LED_TOTAL)
    {
        int corrected_index = led_pos[index];
        led_state[corrected_index].r = r;
        led_state[corrected_index].g = g;
        led_state[corrected_index].b = b;
    }
    else
    {
        int corrected_index = underglow_led_pos[index - MATRIX_LED_TOTAL];
        underglow_led_state[corrected_index].r = r;
        underglow_led_state[corrected_index].g = g;
        underglow_led_state[corrected_index].b = b;
    }
}

static void set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i=0; i<DRIVER_LED_TOTAL; i++)
        set_color(i, r, g, b);
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init          = init,
    .flush         = flush,
    .set_color     = set_color,
    .set_color_all = set_color_all,
};

void led_set(uint8_t usb_led) {
    writePin(LED_CAPS_LOCK_PIN, !IS_LED_ON(usb_led, USB_LED_CAPS_LOCK));
    writePin(LED_SCROLL_LOCK_PIN, !IS_LED_ON(usb_led, USB_LED_SCROLL_LOCK));
}
