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
#include "../ble/ble_services.h"
#include "usb_comm.h"
#include "config.h"

enum keyboard_status
{
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
    switch (status)
    {
    case kbd_ble:
#ifdef LED_RGB
        keyboard_led_rgb_set(ble_connected ? 0x66ccff : 0xFFFFFF);
#endif
#ifdef LED_STATUS
#ifdef LED_BLE
        if(ble_connected)
        {
            system_led_set_val(LED_BIT_BLE, 1);
        }
        else
        {
            system_led_set_val(LED_BIT_BLE, 0);
        }
#endif
#ifdef LED_CHARGING
        system_led_set_val(LED_BIT_CHARGING, 0);
#endif
#ifdef HAS_USB
#ifdef LED_USB
        system_led_set_val(LED_BIT_USB, 0);
#endif
#endif
        system_led_set();
#endif
        break;
    case kbd_charge:
#ifdef LED_RGB
        keyboard_led_rgb_set(charging_full ? 0x00FF00 : 0xFF8000);
#endif
#ifdef LED_STATUS
#ifdef LED_CHARGING
        if(charging_full)
        {
            system_led_set_val(LED_BIT_CHARGING, 0);
        }
        else
        {
            system_led_set_val(LED_BIT_CHARGING, 1);
        }

#endif
#ifdef LED_BLE
        if(ble_connected)
        {
            system_led_set_val(LED_BIT_BLE, 1);
        }
        else
        {
            system_led_set_val(LED_BIT_BLE, 0);
        }
#endif
#ifdef HAS_USB
#ifdef LED_USB
    if (usb_working()) {
        system_led_set_val(LED_BIT_USB, 1);
    } else
    {
        system_led_set_val(LED_BIT_USB, 0);
    }
#endif
#endif
        system_led_set();
#endif
        break;
    case kbd_usb:
#ifdef LED_RGB
        keyboard_led_rgb_set(0x0099FF);
#endif
#ifdef LED_STATUS
#ifdef LED_USB
        system_led_set_val(LED_BIT_USB, 1);
#endif
#ifdef LED_CHARGING
        if(charging_full)
        {
            system_led_set_val(LED_BIT_CHARGING, 0);
        }
        else
        {
            system_led_set_val(LED_BIT_CHARGING, 1);
        }

#endif
#ifdef LED_BLE
        if(ble_connected)
        {
            system_led_set_val(LED_BIT_BLE, 1);
        }
        else
        {
            system_led_set_val(LED_BIT_BLE, 0);
        }
#endif
        system_led_set();
#endif
        break;
    default:
        break;
    }
}

// 这里可以放置用户自定义的处理程序，例如设置灯光等。
void custom_event_handler(enum user_ble_event arg) {
    switch (arg)
    {
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
        led_status_change();
        break;
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
#ifdef LED_STATUS
        system_led_on();
        nrf_delay_ms(200);
#endif
        break;
    default:
        break;
    }
}
