#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
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
    for (;;) {
        int level = gpio_get_level(BUTTON_PIN);
        if (level == 0) {
            printf("Button pressed setting: %d\n", !gpio_get_level(LED_PIN));
            isOn = !isOn;
            gpio_set_level(LED_PIN, isOn);
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
