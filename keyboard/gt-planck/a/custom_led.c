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

#include "app_timer.h"
#include "config.h"

#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "usb_comm.h"


bool led_block = false;

APP_TIMER_DEF(ble_led_blink_timer);
bool blink_status = true; //当前蓝牙广播LED闪烁状态

/** 
 * 蓝牙广播状态闪烁LED
 **/
void ble_led_blink_timer_handler(void* context)
{
    if (!usb_working()) {
#ifdef LED_CAPS
            LED_WRITE(LED_CAPS, blink_status);
            blink_status = !blink_status;
#endif
    }
}

/**
 * @brief 蓝牙广播状态开启闪烁灯
 * 
 */
static void ble_blink_led_on()
{
    blink_status = true;
    ble_led_blink_timer_handler(NULL); //即刻执行闪烁
    app_timer_start(ble_led_blink_timer, APP_TIMER_TICKS(500), NULL);
}
/**
 * @brief 蓝牙关闭广播后关闭闪烁灯
 * 
 */
static void ble_blink_led_off()
{
    blink_status = false;
    app_timer_stop(ble_led_blink_timer);
}

static void custom_led_evt_handler(enum user_event event, void* arg)
{
    switch (event) {
    case USER_EVT_STAGE:
        switch ((uint32_t)arg) {
        case KBD_STATE_PRE_INIT: // 开机闪烁LED
#ifdef LED_CAPS
            nrf_gpio_cfg_output(LED_CAPS);
            LED_WRITE(LED_CAPS, 1);
            nrf_delay_ms(20);
            app_timer_create(&ble_led_blink_timer, APP_TIMER_MODE_REPEATED, ble_led_blink_timer_handler);
#endif
            break;
        case KBD_STATE_SLEEP: // 关机闪烁LED
#ifdef LED_CAPS
            LED_WRITE(LED_CAPS, 1);
#endif
            break;
        default:
            break;
        }
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙状态事件
        if (((uint32_t)arg) == BLE_STATE_FAST_ADV) {
            ble_blink_led_on();
        } else {
            ble_blink_led_off();
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(custom_led_evt_handler);