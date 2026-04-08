#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define PIN_TOP_LEFT        25
#define PIN_TOP             33
#define PIN_TOP_RIGHT       32
#define PIN_MIDDLE          23
#define PIN_BOTTOM_LEFT     4
#define PIN_BOTTOM          18
#define PIN_BOTTOM_RIGHT    21

#define PIN_DOT             15
#define BLINK_TIME          50

#define BUTTON_PIN          13

#define NUM_PINS            7

static uint8_t pins[] = {
    PIN_TOP_LEFT, PIN_TOP, PIN_TOP_RIGHT,
    PIN_MIDDLE,
    PIN_BOTTOM_LEFT, PIN_BOTTOM, PIN_BOTTOM_RIGHT,
    PIN_DOT
};

// Ignore the dot
// Set each segment ON or OFF depending on position given in pins[].
// Eg. 111 0 111 means: TOP_LEFT: ON, TOP: ON, TOP_RIGHT: ON, MIDDLE: OFF, etc...
uint8_t digits[] = {
    0b1110111, 0b0010001, 0b0111110, 0b0111011, 0b1011001,
    0b1101011, 0b1101111, 0b0110001, 0b1111111, 0b1111011
};

void blink(uint8_t pin)
{
    for (int i = 0; i < 2; i++) {
        gpio_set_level(pin, 1);
        vTaskDelay(pdMS_TO_TICKS(BLINK_TIME));

        gpio_set_level(pin, 0);
        vTaskDelay(pdMS_TO_TICKS(BLINK_TIME));
    }
}

void show_digit(uint8_t digit)
{
    uint8_t repr = digits[digit];
    for (uint8_t i = 0; i < NUM_PINS; i++) {
        uint8_t seg_set = repr & 1;
        if (seg_set) {
            gpio_set_level(pins[NUM_PINS - i - 1], 1);
        }

        repr >>= 1;
    }
}

void reset_display(void)
{
    for (uint8_t i = 0; i < NUM_PINS; i++) {
        gpio_set_level(pins[i], 0);
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);

    for (int i = 0; i < NUM_PINS + 1; i++) {
        gpio_reset_pin(pins[i]);
        gpio_set_direction(pins[i], GPIO_MODE_OUTPUT);
    }
 
    int count = 0;
    show_digit(count);

    int prev = 1;
    for (;;) {
        int current = gpio_get_level(BUTTON_PIN);

        if (prev == 1 && current == 0) {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(BUTTON_PIN) == 0) {
                if (count + 1 > 9)
                    blink(PIN_DOT);

                count = (count + 1) % 10;

                reset_display();
                show_digit(count);
            }
        }

        prev = current;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
