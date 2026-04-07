#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER         LEDC_TIMER_0
#define LEDC_MODE          LEDC_LOW_SPEED_MODE
#define LEDC_BLUE_IO       15
#define LEDC_GREEN_IO      4
#define LEDC_RED_IO        5
#define LEDC_BLUE_CHAN     LEDC_CHANNEL_0
#define LEDC_GREEN_CHAN    LEDC_CHANNEL_1
#define LEDC_RED_CHAN      LEDC_CHANNEL_2
#define LEDC_DUTY_RES      LEDC_TIMER_13_BIT
#define LEDC_CLK_SRC       LEDC_AUTO_CLK
#define LEDC_FREQUENCY     4000

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
        .channel            = LEDC_BLUE_CHAN,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_BLUE_IO,
        .duty               = 0,
        .hpoint             = 0
    };

    ledc_channel_config_t ledc_channel_green = {
        .speed_mode         = LEDC_MODE,
        .channel            = LEDC_GREEN_CHAN,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_GREEN_IO,
        .duty               = 0,
        .hpoint             = 0
    };

    ledc_channel_config_t ledc_channel_red = {
        .speed_mode         = LEDC_MODE,
        .channel            = LEDC_RED_CHAN,
        .timer_sel          = LEDC_TIMER,
        .gpio_num           = LEDC_RED_IO,
        .duty               = 0,
        .hpoint             = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_blue));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_green));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_red));
}

void app_main(void)
{
    gpio_reset_pin(LEDC_BLUE_IO);
    gpio_reset_pin(LEDC_GREEN_IO);
    gpio_reset_pin(LEDC_RED_IO);

    ledc_init();
    while (1) {
        for (int chan = 0; chan < 3; chan++) {
            for (int i = 0; i < 8000; i += 50) {
                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, chan, i));
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, chan));
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_BLUE_CHAN, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_GREEN_CHAN, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_RED_CHAN, 0));
        
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_BLUE_CHAN));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_GREEN_CHAN));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_RED_CHAN));
    }
}
