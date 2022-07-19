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

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#include "ble_keyboard.h"
#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "nrf_delay.h"
#include "simple_rgb/led_rgb.h"

#define COLOR_IDLE 0xFFFFFF // 白色: 空闲状态
#define COLOR_BLE 0x66FFFF // 青色: 蓝牙连接
#define COLOR_CHARGING 0xFF8000 // 橙色: 充电中
#define COLOR_FULL 0x00FF00 // 绿色: 充电完毕
#define COLOR_USB 0x0099ff // 蓝色: USB 已连接
#define COLOR_PASSKEY_REQ 0xFFFF00 // 黄色: 请求输入配对码
#define COLOR_PASSKEY_SEND 0xFF0080 // 粉红色: 配对码输入完毕
#define COLOR_SLEEP 0xFF00FF // 紫红色: 睡眠

#ifdef PIN_CHARGING
static bool is_charging = false;
static bool charging_full = false;
#endif
static bool ble_connected = false;
static bool usb_working = false;

/**
 * @brief 按状态更改 LED 样式
 * 
 */
static void led_status_change()
{
    if (usb_working) {
        keyboard_led_rgb_set(COLOR_USB);
#ifdef PIN_CHARGING
    } else if (is_charging) {
        keyboard_led_rgb_set(charging_full ? COLOR_FULL : COLOR_CHARGING);
#endif
    } else {
        keyboard_led_rgb_set(ble_connected ? COLOR_BLE : COLOR_IDLE);
    }
}

static bool leds_inited = false;

void rgb_led_event_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            keyboard_led_rgb_init();
            leds_inited = true;
            break;
        case KBD_STATE_INITED: // 初始化完毕
            led_status_change();
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            if (leds_inited) {
                nrf_delay_ms(20); // 等待睡眠指示灯显示完毕
                keyboard_led_rgb_deinit();
            }
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE:
        switch (arg2) {
        case PWR_SAVE_ENTER: // 进入省电模式
            keyboard_led_rgb_switch(false);
            break;
        case PWR_SAVE_EXIT: // 退出省电模式
            keyboard_led_rgb_switch(true);
            break;
        default:
            break;
        }
        break;
#ifdef PIN_CHARGING
    case USER_EVT_CHARGE: // 充电事件
        switch (arg2) {
        case BATT_NOT_CHARGING:
            is_charging = false;
            break;
        case BATT_CHARGING:
            is_charging = true;
            charging_full = false;
            break;
        case BATT_CHARGED:
            is_charging = true;
            charging_full = true;
        default:
            break;
        }
        led_status_change();
        break;
#endif
    case USER_EVT_USB: // USB事件
        switch (arg2) {
        case USB_WORKING:
            usb_working = true;
            break;
        default:
            usb_working = false;
            break;
        }
        led_status_change();
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙事件
        switch (arg2) {
        case BLE_STATE_CONNECTED:
            ble_connected = true;
            led_status_change();
            break;
        case BLE_STATE_DISCONNECT:
            ble_connected = false;
            led_status_change();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_BLE_DEVICE_SWITCH: // 设备切换事件
        led_status_change(); // 暂时先把灯亮起来
        break;
    case USER_EVT_BLE_PASSKEY_STATE: // 请求Passkey
        switch (arg2) {
        case PASSKEY_STATE_REQUIRE:
            keyboard_led_rgb_set(COLOR_PASSKEY_REQ);
            break;
        case PASSKEY_STATE_SEND:
            keyboard_led_rgb_set(COLOR_PASSKEY_SEND);
            break;
        default:
            break;
        }
        break;
    case USER_EVT_SLEEP: // 睡眠指示
        keyboard_led_rgb_direct(COLOR_SLEEP);
        break;
    default:
        break;
    }
}

EVENT_HANDLER(rgb_led_event_handler);