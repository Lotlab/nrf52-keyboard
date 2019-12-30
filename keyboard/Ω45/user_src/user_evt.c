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

#include <stdbool.h>
#include <stdint.h>

#include "ble_keyboard.h"
#include "config.h"
#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "main.h"
#include "nrf_delay.h"
#include "status_led.h"
#include "usb_comm.h"
#include "user_func.h"

enum keyboard_status {
    kbd_ble,
    kbd_charge,
    kbd_usb
};

static enum keyboard_status status = 0;
static bool charging_full = false;
static bool ble_connected = false;

/**
 * @brief 按系统状态更改系统LED、RGB 的颜色
 * 
 */
static void led_status_change()
{
    switch (status) {
    case kbd_ble:
#ifdef LED_BLE
        if (ble_connected) {
            status_led_set_val(LED_BIT_BLE, 1);
        } else {
            status_led_set_val(LED_BIT_BLE, 0);
        }
#endif
#ifdef LED_CHARGING
        status_led_set_val(LED_BIT_CHARGING, 0);
#endif
#ifdef HAS_USB
#ifdef LED_USB
        status_led_set_val(LED_BIT_USB, 0);
#endif
#endif
        status_led_set();
        break;
    case kbd_charge:
#ifdef LED_CHARGING
        if (charging_full) {
            status_led_set_val(LED_BIT_CHARGING, 0);
        } else {
            status_led_set_val(LED_BIT_CHARGING, 1);
        }

#endif
#ifdef LED_BLE
        if (ble_connected) {
            status_led_set_val(LED_BIT_BLE, 1);
        } else {
            status_led_set_val(LED_BIT_BLE, 0);
        }
#endif
#ifdef HAS_USB
#ifdef LED_USB
        if (usb_working()) {
            status_led_set_val(LED_BIT_USB, 1);
        } else {
            status_led_set_val(LED_BIT_USB, 0);
        }
#endif
#endif
        status_led_set();
        break;
    case kbd_usb:
#ifdef LED_USB
        status_led_set_val(LED_BIT_USB, 1);
#endif
#ifdef LED_CHARGING
        if (charging_full) {
            status_led_set_val(LED_BIT_CHARGING, 0);
        } else {
            status_led_set_val(LED_BIT_CHARGING, 1);
        }

#endif
#ifdef LED_BLE
        if (ble_connected) {
            status_led_set_val(LED_BIT_BLE, 1);
        } else {
            status_led_set_val(LED_BIT_BLE, 0);
        }
#endif
        status_led_set();
        break;
    default:
        break;
    }
}

// 这里可以放置用户自定义的处理程序，例如设置灯光等。
static void custom_event_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            status_led_init();
            buttons_init();
            break;
        case KBD_STATE_INITED: // 初始化完毕
            led_status_change();
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            status_led_deinit();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE:
        switch (arg2) {
        case PWR_SAVE_ENTER: // 进入省电模式
            status_led_off();
            break;
        case PWR_SAVE_EXIT: // 退出省电模式
            status_led_on();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_CHARGE: // 充电事件
        charging_full = (arg2 != BATT_CHARGING);
        led_status_change();
        break;
    case USER_EVT_USB: // USB事件
        switch (arg2) {
        case USB_WORKING:
            status = kbd_usb;
            break;
        case USB_NOT_WORKING:
        case USB_NO_HOST: // no_host状态也能说明正在充电
            status = kbd_charge;
            break;
        case USB_NOT_CONNECT:
            status = kbd_ble;
            break;
        default:
            break;
        }
        led_status_change();
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙事件
        ble_connected = (arg2 == BLE_STATE_CONNECTED);
        led_status_change();
        break;
    case USER_EVT_SLEEP: // 睡眠指示
        matrix_uninit(); // 释放键盘阵列针脚
        status_led_on();
        nrf_delay_ms(200);
        break;
    default:
        break;
    }
}

EVENT_HANDLER(custom_event_handler);
