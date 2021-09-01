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
    switch (event) {
    case USER_EVT_INTERNAL:
        switch ((uint32_t)arg) {
        case INTERNAL_EVT_BATTERY_INFO_REFRESH:
            battery_level_update(battery_info.percentage);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(battery_service_event_handler);