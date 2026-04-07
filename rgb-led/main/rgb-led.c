#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_BLUE       15
#define LEDC_GREEN      4
#define LEDC_RED        5
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

    ledc_channel_config_t ledc_channel_blue = {
        .speed_mode         = LEDC_MODE,
        .channel            = 0,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_BLUE,
        .duty               = 0,
        .hpoint             = 0
    };

    ledc_channel_config_t ledc_channel_green = {
        .speed_mode         = LEDC_MODE,
        .channel            = 1,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_GREEN,
        .duty               = 0,
        .hpoint             = 0
    };

    ledc_channel_config_t ledc_channel_red = {
        .speed_mode         = LEDC_MODE,
        .channel            = 2,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_RED,
        .duty               = 0,
        .hpoint             = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_blue));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_green));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_red));
}

void app_main(void)
{
    gpio_reset_pin(LEDC_BLUE);
    gpio_reset_pin(LEDC_GREEN);
    gpio_reset_pin(LEDC_RED);

    ledc_init();
    while (1) {
        for (int c = 0; c < 3; c++) {
            for (int i = 0; i < 8000; i += 50) {
                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, c, i));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, c));
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, 0, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, 1, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, 2, 0));
        
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, 0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, 1));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, 2));
    }
}
