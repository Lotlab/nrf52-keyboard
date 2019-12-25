/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

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

#include "3led_status.h"
#include "config.h"

#include "../keyboard/keyboard_led.h"
#include "../keyboard/power_save.h"
#include "nrf.h"
#include "nrf_gpio.h"

enum status_led {
    BIT_LED_BLE,
    BIT_LED_CHARGING,
    BIT_LED_USB
};

/** 
 * 初始化状态LED
 **/
void status_led_init()
{
#ifdef LED_STATUS_BLE
    nrf_gpio_cfg_output(LED_STATUS_BLE);
#endif
#ifdef LED_STATUS_CHARGING
    nrf_gpio_cfg_output(LED_STATUS_CHARGING);
#endif
#ifdef LED_STATUS_USB
    nrf_gpio_cfg_output(LED_STATUS_USB);
#endif
}

/** 
 * 内部设置状态LED的值
 **/
static void status_led_set_internal(uint8_t val)
{
#ifdef LED_STATUS_BLE
    LED_WRITE(LED_STATUS_BLE, val & (1 << BIT_LED_BLE));
#endif
#ifdef LED_STATUS_CHARGING
    LED_WRITE(LED_STATUS_CHARGING, val & (1 << BIT_LED_CHARGING));
#endif
#ifdef LED_STATUS_USB
    LED_WRITE(LED_STATUS_USB, val & (1 << BIT_LED_USB));
#endif
}

// 暂存的LED的值
static uint8_t saved_status_led_val;

/** 
 * 关闭状态LED的灯光
 **/
void status_led_off()
{
    status_led_set_internal(0);
}

/** 
 * 启用状态LED的灯光
 **/
void status_led_on()
{
    status_led_set_internal(saved_status_led_val);
}

/** 
 * 通知所有灯光启用
 **/
static void set_led_on()
{
    status_led_on();
    power_save_reset();
}

/** 
 * 设置蓝牙状态灯的状态
 **/
void status_led_ble(bool state)
{
    if (state) {
        saved_status_led_val |= (1 << BIT_LED_BLE);
    } else {
        saved_status_led_val &= ~(1 << BIT_LED_BLE);
    }
    set_led_on();
}

/** 
 * 设置充电状态灯的状态
 **/
void status_led_charging(bool state)
{
    if (state) {
        saved_status_led_val |= (1 << BIT_LED_CHARGING);
    } else {
        saved_status_led_val &= ~(1 << BIT_LED_CHARGING);
    }
    set_led_on();
}

/** 
 * 设置USB状态灯的状态
 **/
void status_led_usb(bool state)
{
    if (state) {
        saved_status_led_val |= (1 << BIT_LED_USB);
    } else {
        saved_status_led_val &= ~(1 << BIT_LED_USB);
    }
    set_led_on();
}

/** 
 * 默认的基于事件的状态路由
 **/
void status_led_evt_handler(enum user_ble_event arg)
{
    switch (arg) {
    case USER_EVT_POST_INIT:
        status_led_init();
        break;
    case USER_LED_ON:
        status_led_on();
        break;
    case USER_LED_OFF:
    case USER_LED_DEINIT:
        status_led_off();
        break;
    default:
        break;
    }

    switch (arg) {
    case USER_USB_DISCONNECT:
        status_led_charging(false);
    case USER_USB_CHARGE:
        status_led_usb(false);
        break;
    case USER_USB_CONNECTED:
        status_led_usb(true);
        break;
    case USER_BAT_CHARGING:
        status_led_charging(true);
        break;
    case USER_BAT_FULL:
        status_led_charging(false);
        break;
    case USER_BLE_DISCONNECT:
        status_led_ble(false);
        break;
    case USER_BLE_CONNECTED:
        status_led_ble(true);
        break;
    default:
        break;
    }
}
