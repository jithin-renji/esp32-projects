#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"

#define RED_LED     15
#define BUILTIN_LED 2

#define DOT_DUR      200
#define DASH_DUR     800
#define CH_END_DUR   400
#define SEP_DUR      600
#define OFF_DUR      50
#define END_DUR      2000

const char *morse[26] = {
    ".-", "-...", "-.-.", "-..", ".",
    "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.",
    "--.-", ".-.", "...", "-", "..-",
    "...-", ".--", "-..-", "-.--", "--.."
};

void send_string(const char *tName, const char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            vTaskDelay(SEP_DUR / portTICK_PERIOD_MS);
            continue;
        }

        const char *m = morse[str[i] - 'a'];
        ESP_LOGI(tName, "Trying: %d\n", str[i] - 'a');
        ESP_LOGI(tName, "Got morse char: %s %c\n", m, str[i]);
        for (int j = 0; m[j] != '\0'; j++) {
            gpio_set_level(RED_LED, 1);
            if (m[j] == '.') {
                vTaskDelay(DOT_DUR / portTICK_PERIOD_MS);
            } else {
                vTaskDelay(DASH_DUR / portTICK_PERIOD_MS);
            }

            gpio_set_level(RED_LED, 0);
            vTaskDelay(OFF_DUR / portTICK_PERIOD_MS);
        }
 
        vTaskDelay(CH_END_DUR / portTICK_PERIOD_MS);
    }

    vTaskDelay(END_DUR / portTICK_PERIOD_MS);
}

void app_main(void)
{
    char *tName = pcTaskGetName(NULL);
    ESP_LOGI(tName, "Hello, world! Started.\n");

    gpio_reset_pin(RED_LED);
    gpio_reset_pin(BUILTIN_LED);
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUILTIN_LED, GPIO_MODE_OUTPUT);

    while (1) {
        send_string(tName, "sos");
    }
}
