/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <string.h>

#include "action.h"
#include "action_util.h"
#include "keyboard_battery.h"

#include "../config/keyboard_config.h"
#include "adc_convert.h"
#include "keyboard_evt.h"

#define ADC_BUFFER_SIZE 6
struct BatteryInfo battery_info;

uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint8_t adc_buffer_index;

static void calculate_battery_persentage(struct BatteryInfo* info)
{
    if (info->voltage >= 4100)
        info->percentage = 100;
    else if (info->voltage >= 3335)
        info->percentage = 15 + (info->voltage - 3335) / 9;
    else if (info->voltage >= 2900)
        info->percentage = (info->voltage - 2900) / 29;
    else
        info->percentage = 0;
}

static void adc_result_handler(nrf_saadc_value_t value)
{
    adc_buffer[adc_buffer_index++] = value;
    if (adc_buffer_index >= ADC_BUFFER_SIZE) {
        adc_buffer_index = 0;

        uint32_t result = 0;
        for (uint8_t i = 0; i < ADC_BUFFER_SIZE; i++) {
            result += adc_buffer[i];
        }
        result /= ADC_BUFFER_SIZE;
        // RESULT = [V(P) – V(N) ] * GAIN/REFERENCE * 2 ^ (RESOLUTION - m)
        // value  = V_in / 1.2 * 1024
        // V_in   = V_bat * 2.2 / 12.2
        battery_info.voltage = result * 1200 * 122 / 1024 / 22;
        calculate_battery_persentage(&battery_info);
        trig_event_param(USER_EVT_INTERNAL, INTERNAL_EVT_BATTERY_INFO_REFRESH);
    }
}

static nrf_saadc_channel_config_t channel_config = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain = NRF_SAADC_GAIN1_2,
    .reference = NRF_SAADC_REFERENCE_INTERNAL,
    .acq_time = NRF_SAADC_ACQTIME_10US,
    .mode = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst = NRF_SAADC_BURST_DISABLED,
    .pin_p = (nrf_saadc_input_t)(BATTERY_ADC_PIN),
    .pin_n = NRF_SAADC_INPUT_DISABLED
};

static struct adc_channel_config batt_channel = {
    .adc_start = 0,
    .adc_finish = &adc_result_handler,
    .period = 2000,
    .config = &channel_config,
};

ADC_CONVERT_CHANNEL(batt_channel);

/**
 * @brief 输出电池剩余电量.
 *
 */
void print_battery_percentage()
{
    int digits[10] = { KC_0, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9 };
    char percentage = battery_info.percentage;

    if (percentage == 0) {
        type_code(KC_N);
    } else if (percentage == 100) {
        type_code(KC_F);
    } else {
        int factor = 100;
        do {
            if (percentage >= factor) {
                int index = (percentage / factor) % 10;
                int keycode = digits[index];
                type_code(keycode);
            }
        } while ((factor /= 10) >= 1);
    }
}
