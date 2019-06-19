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

#include "keyboard_host_driver.h"
#include <stdint.h>

#include "../ble/ble_hid_service.h"
#include "../config/keyboard_config.h"
#include "custom_hook.h"
#include "usb_comm.h"

// todo: impliment
uint8_t keyboard_idle = 0;
uint8_t keyboard_protocol = 0;

uint8_t keyboard_leds(void);
void send_keyboard(report_keyboard_t* report);
void send_mouse(report_mouse_t* report);
void send_system(uint16_t data);
void send_consumer(uint16_t data);

host_driver_t driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

uint8_t keyboard_leds()
{
#ifdef HAS_USB
    if (usb_working())
        return keyboard_led_val_usb;
    else
#endif
        return keyboard_led_val_ble;
}

/**
 * @brief 发送按键包
 * 
 * @param index 类型Index
 * @param len 长度
 * @param keys 按键
 */
static void send(uint8_t index, uint8_t len, uint8_t* keys)
{
#ifdef HAS_USB
    if (usb_working()) {
        usb_send(index, len, keys);
    } else
#endif
    {
        keys_send(index, len, keys);
    }
}

void send_keyboard(report_keyboard_t* report)
{
#if defined(NKRO_ENABLE) && defined(HAS_USB)
    if (keyboard_protocol && keyboard_nkro) {
        send(3, NKRO_EPSIZE, report->raw);
    } else
#endif
    {
        hook_send_keyboard(report);
        send(0, 8, report->raw);
    }
}

void send_mouse(report_mouse_t* report)
{
    // unsupport, and will not support in future.
}

void send_system(uint16_t data)
{
    send(1, 2, (uint8_t*)&data);
}

void send_consumer(uint16_t data)
{
    send(2, 2, (uint8_t*)&data);
}
