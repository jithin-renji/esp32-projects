#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_OUT_IO     15
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_13_BIT
#define LEDC_CLK_SRC    LEDC_AUTO_CLK
#define LEDC_FREQUENCY  4000

void ledc_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode         = LEDC_MODE,
        .duty_resolution    = LEDC_DUTY_RES,
        .timer_num          = LEDC_TIMER,
        .freq_hz            = LEDC_FREQUENCY,
        .clk_cfg            = LEDC_CLK_SRC
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode         = LEDC_MODE,
        .channel            = LEDC_CHANNEL,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_OUT_IO,
        .duty               = 0,
        .hpoint             = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void app_main(void)
{
    ledc_init();
    while (1) {
        for (int i = 0; i < 8000; i += 50) {
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        for (int i = 8000; i > 0; i -= 50) {
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}
