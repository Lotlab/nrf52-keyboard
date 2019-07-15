/*
Copyright (C) 2019 Geno Kolar <geno@live.com>

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

#include "user_func.h"
#include "app_timer.h"
#include "ble_services.h"
#include "config.h"
#include "keyboard_led.h"
#include "keyboard_matrix.h"
#include "nrf_gpio.h"
#include "nrf_pwr_mgmt.h"
#include "status_led.h"
#include "usb_comm.h"
#include <stdint.h>
#include <string.h>

/**
* @brief 释放键盘阵列
 * 
 */
void matrix_uninit(void)
{
    for (uint_fast8_t i = MATRIX_ROWS; i--;) {
        nrf_gpio_cfg_default((uint32_t)row_pin_array[i]);
    }
    for (uint_fast8_t i = MATRIX_COLS; i--;) {
        nrf_gpio_cfg_default((uint32_t)column_pin_array[i]);
    }
}

/**
 * @brief 使键盘进入关机状态
 * 
 * @param reason
 */
void systemoff(void)
{
    app_timer_stop_all();
    ble_user_event(USER_EVT_SLEEP_AUTO);
    keyboard_led_deinit();
#ifdef HAS_USB
    usb_comm_sleep_prepare();
#endif
    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    ret_code_t err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

/**
* @brief 初始化POWER_ON按钮
 * 
 */
void buttons_init(void)
{
#ifdef POWER_BUTTON
    nrf_gpio_cfg_sense_input(POWER_BUTTON,
                             NRF_GPIO_PIN_PULLUP,
                             NRF_GPIO_PIN_SENSE_LOW);
#endif
}