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

#include "nrf_log.h"
#include "ble_bas_service.h"
#include <string.h>

#include "ble_bas.h"
#include "ble_config.h"
#include "keyboard_battery.h"
#include "keyboard_evt.h"

#include "../config/keyboard_config.h"

BLE_BAS_DEF(m_bas); /**< Structure used to identify the battery service. */

/**@brief Function for performing a battery measurement, and update the Battery Level characteristic in the Battery Service.
 */
static void battery_level_update(uint8_t battery_level)
{
    ret_code_t err_code;

    err_code = ble_bas_battery_level_update(&m_bas, battery_level, BLE_CONN_HANDLE_ALL);
    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY) && (err_code != NRF_ERROR_RESOURCES) && (err_code != NRF_ERROR_FORBIDDEN) && (err_code != NRF_ERROR_INVALID_STATE) && (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)) {
        APP_ERROR_HANDLER(err_code);
    }
}

/**@brief Function for initializing Battery Service.
 */
void battery_service_init(void)
{
    ret_code_t err_code;
    ble_bas_init_t bas_init_obj;

    memset(&bas_init_obj, 0, sizeof(bas_init_obj));

    bas_init_obj.evt_handler = NULL;
    bas_init_obj.support_notification = true;
    bas_init_obj.p_report_ref = NULL;
    bas_init_obj.initial_batt_level = 100;

    bas_init_obj.bl_rd_sec = SEC_CURRENT;
    bas_init_obj.bl_cccd_wr_sec = SEC_CURRENT;
    bas_init_obj.bl_report_rd_sec = SEC_CURRENT;

    err_code = ble_bas_init(&m_bas, &bas_init_obj);
    APP_ERROR_CHECK(err_code);
}

static void battery_service_event_handler(enum user_event event, void* arg)
{
    // 电池电量曲线
    static uint16_t level[] = {2900, 3650, 3700, 3740, 3760, 3795, 3840, 3910, 3980, 4070, 4150};

    if (info->voltage <= level[0])
    {
        info->percentage = 0;
        goto EXIT;
    }

    for (uint8_t i = 1; i < sizeof(level) / sizeof(uint16_t); ++i)
    {
        if (info->voltage < level[i])
        {
            info->percentage = 10 * i;
            info->percentage -= (char)((10 * (level[i] - info->voltage)) / (level[i] - level[i - 1]));
            goto EXIT;
        }
    }

    info->percentage = 100;

EXIT:
    NRF_LOG_INFO("batt voltage=%dmv percentage=%d%%", info->voltage, info->percentage);
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
        
        // 电量矫正
        battery_info.voltage = battery_info.voltage * 4000 / 3873;

        calculate_battery_persentage(&battery_info);
        battery_level_update(battery_info.percentage);
    }
}

EVENT_HANDLER(battery_service_event_handler);
