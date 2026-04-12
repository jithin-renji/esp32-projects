#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"

#define TAG                             "servo_ctrl"

#define SERVO_MIN_PULSEWIDTH_US         500
#define SERVO_MAX_PULSEWIDTH_US         2500
#define SERVO_MIN_DEGREE                -90
#define SERVO_MAX_DEGREE                90

#define SERVO_PULSE_PIN                 4
#define SERVO_PULSE_PIN2                18
#define SERVO_PULSE_PIN3                21
#define SERVO_TIMEBASE_RES_HZ           1000000
#define SERVO_TIMEBASE_PERIOD           20000

#define START_ANGLE                     90
#define END_ANGLE                       -90

static const int pins[3] = {SERVO_PULSE_PIN, SERVO_PULSE_PIN2, SERVO_PULSE_PIN3};

static inline uint32_t compare_angle(int angle)
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void app_main(void)
{
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = SERVO_TIMEBASE_RES_HZ,
        .period_ticks = SERVO_TIMEBASE_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP
    };

    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_oper_handle_t opers[3] = {};
    mcpwm_operator_config_t operator_config = {
        .group_id = 0
    };

    mcpwm_cmpr_handle_t comparators[3] = {};
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true
    };

    mcpwm_gen_handle_t generators[3] = {};
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &opers[i]));

        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(opers[i], timer));

        ESP_ERROR_CHECK(mcpwm_new_comparator(opers[i], &comparator_config, &comparators[i]));
        mcpwm_generator_config_t generator_config = {
            .gen_gpio_num = pins[i]
        };

        ESP_ERROR_CHECK(mcpwm_new_generator(opers[i], &generator_config, &generators[i]));

        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators[i],
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generators[i],
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators[i], MCPWM_GEN_ACTION_LOW)));
    }


    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    for (int i = 0; i < 3; i++) {
        ESP_LOGI(TAG, "Setting initial angle %d", i);
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], compare_angle(START_ANGLE)));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "ALL ANGLES SET TO %d", START_ANGLE);
    vTaskDelay(pdMS_TO_TICKS(1000));

    int angle = START_ANGLE;
    int step = -5;
    int i = 0;
    int rep = 0;
    while (1) {
        ESP_LOGI(TAG, "%d Angle of rotation: %d", i, angle);
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], compare_angle(angle)));
        vTaskDelay(pdMS_TO_TICKS(20));

        if ((angle + step) > START_ANGLE || (angle + step) < END_ANGLE) {
            step *= -1;
            vTaskDelay(pdMS_TO_TICKS(1000));
            if (rep != 1) {
                rep++;
            } else {
                i = (i + 1) % 3;
                rep = 0;
            }
        }


        angle += step;
    }
}
