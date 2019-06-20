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
#include  "../ble/ble_services.h"

static bool usb_connected = false;

__attribute__((weak)) void custom_event_handler(enum user_ble_event arg){}

__attribute__((weak)) void user_event_handler(enum user_ble_event arg)
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
        usb_connected = true;
        break;
    case USER_USB_DISCONNECT:
        // 断开USB后，切换至省电模式节省电量
        keyboard_led_powersave(true);
        ble_keyboard_powersave(true);
        usb_connected = false;
        break;
    case USER_BLE_IDLE:
        // 长时间没有连接，若处于蓝牙模式则睡眠
        if (!usb_connected) {
            sleep(SLEEP_NO_CONNECTION);
        } else {
            advertising_slow();
        }
    default:
        break;
    }

    custom_event_handler(arg);
}