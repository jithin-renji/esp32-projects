#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define LED_PIN         15
#define BUTTON_PIN      25


void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);

    int isOn = 0;
    int prev = 1;
    for (;;) {
        int current = gpio_get_level(BUTTON_PIN);

        if (prev == 1 && current == 0) {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(BUTTON_PIN) == 0) {
                isOn = !isOn;
                gpio_set_level(LED_PIN, isOn);
            }
        }

        prev = current;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
