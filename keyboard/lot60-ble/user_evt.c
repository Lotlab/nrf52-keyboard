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

#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "ble_keyboard.h"
#include "keyboard_led.h"
#include "nrf_delay.h"
#include "config.h"

#ifdef LED_RGB
#include "led_rgb.h"
enum keyboard_status {
    kbd_ble,
    kbd_charge,
    kbd_usb
};

static enum keyboard_status status = 0;
static bool charging_full = false;
static bool ble_connected = false;

/**
 * @brief 按状态更改 LED 样式
 * 
 */
static void led_status_change() {
    switch (status)
    {
    case kbd_ble:
        keyboard_led_rgb_set(ble_connected ? 0x66ccff : 0xFFFFFF);
        break;
    case kbd_charge:
        keyboard_led_rgb_set(charging_full ? 0x00FF00: 0xFF8000);
        break;
    case kbd_usb:
        keyboard_led_rgb_set(0x0099FF);
    default:
        break;
    }
}

// 这里可以放置用户自定义的处理程序，例如设置灯光等。
void custom_event_handler(enum user_ble_event arg) {
    // 将事件传递给RGB灯光设置
    switch (arg)
    {
    case USER_EVT_POST_INIT:
        keyboard_led_rgb_init();
        break;
    case USER_LED_ON:
        keyboard_led_rgb_switch(true);
        break;
    case USER_LED_OFF:
        keyboard_led_rgb_switch(false);
        break;
    case USER_LED_DEINIT:
        keyboard_led_rgb_deinit();
        break;
    default:
        break;
    }

    switch (arg) {
    case USER_EVT_INITED:
        led_status_change();
        break;
    case USER_USB_DISCONNECT:
        status = kbd_ble;
        led_status_change();
        break;
    case USER_USB_CHARGE:
        status = kbd_charge;
        led_status_change();
        break;
    case USER_USB_CONNECTED:
        status = kbd_usb;
        led_status_change();
        break;
    case USER_BAT_CHARGING:
        charging_full = false;
        led_status_change();
        break;
    case USER_BAT_FULL:
        charging_full = true;
        led_status_change();
        break;
    case USER_BLE_DISCONNECT:
        ble_connected = false;
        led_status_change();
        break;
    case USER_BLE_CONNECTED:
        ble_connected = true;
        keyboard_led_rgb_set(0x66ccff);
        break;
    case USER_BLE_PASSKEY_REQUIRE:
        keyboard_led_rgb_set(0xFFFF00);
        break;
    case USER_BLE_PASSKEY_SEND:
        keyboard_led_rgb_set(0xFF0080);
        break;
    case USER_EVT_SLEEP_AUTO:
    case USER_EVT_SLEEP_MANUAL:
        keyboard_led_rgb_direct(0b00000101);
        nrf_delay_ms(200);
        break;
    default:
        break;
    }
}
#endif