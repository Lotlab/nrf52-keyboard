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

#include "../main.h"
#include "ble_keyboard.h"
#include "keyboard_led.h"
#include "sleep_reason.h"
#include "nrf_delay.h"
#include  "../ble/ble_services.h"

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
#ifdef LED_RGB
        keyboard_led_rgb_set(ble_connected ? 0x66ccff : 0xFFFFFF);
#endif
        break;
    case kbd_charge:
#ifdef LED_RGB
        keyboard_led_rgb_set(charging_full ? 0x00FF00: 0xFF8000);
#endif
        break;
    case kbd_usb:
#ifdef LED_RGB
        keyboard_led_rgb_set(0x0099FF);
#endif
    default:
        break;
    }
}

void user_event_handler(enum user_ble_event arg)
{
    // 处理各项事件，启用对应的处理程序
    switch (arg) {
    case USER_EVT_SLEEP_AUTO:
        // 自动休眠时，设置休眠原因便于下次免按键启动
        sleep_reason_set(true);
        break;
    case USER_EVT_SLEEP_MANUAL:
        // 手动休眠时，设置下次必须按键启动
        sleep_reason_set(false);
        break;
    case USER_USB_CHARGE:
        // 接入USB后，切换至非省电模式防止自动休眠
        keyboard_led_powersave(false);
        ble_keyboard_powersave(false);
        break;
    case USER_USB_DISCONNECT:
        // 断开USB后，切换至省电模式节省电量
        keyboard_led_powersave(true);
        ble_keyboard_powersave(true);
        break;
    case USER_BLE_IDLE:
        // 长时间没有连接，若处于蓝牙模式则睡眠
        if (status == kbd_ble) {
            sleep(SLEEP_NO_CONNECTION);
        } else {
            advertising_slow();
        }
    default:
        break;
    }

    // 这里可以放置用户自定义的处理程序，例如设置灯光等。
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
#ifdef LED_RGB
        keyboard_led_rgb_set(0x66ccff);
        break;
#endif
    case USER_BLE_PASSKEY_REQUIRE:
#ifdef LED_RGB
        keyboard_led_rgb_set(0xFFFF00);
        break;
#endif
    case USER_BLE_PASSKEY_SEND:
#ifdef LED_RGB
        keyboard_led_rgb_set(0xFF0080);
        break;
#endif
    case USER_EVT_SLEEP_AUTO:
    case USER_EVT_SLEEP_MANUAL:
#ifdef LED_RGB
        keyboard_led_rgb_direct(0b00000101);
        nrf_delay_ms(200);
#endif
        break;
    default:
        break;
    }
}