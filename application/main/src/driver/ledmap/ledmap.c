/*
Copyright (C) 2020 Jim Jiang <jim@lotlab.org>

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

#include "ledmap.h"

#include "app_timer.h"
#include "config.h"

#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "power_save.h"
#include "usb_comm.h"

APP_TIMER_DEF(led_blink_timer);

static struct led_define leds[] = LEDMAP_LEDS;
static struct events_led events[] = LEDMAP_EVENTS;

static bool power_save = false;
static uint8_t led_val = 0;

static uint8_t led_saved_evt[ARRAY_SIZE(leds)] = { 0 };
static uint8_t led_blink_counter[ARRAY_SIZE(leds)] = { 0 };

#if 0
/**
 * @brief 底层设置LED状态
 * 
 * @param led 指定的LED
 * @param on 
 */
static void led_set_ll(uint8_t led, bool on)
{
    if (led >= ARRAY_SIZE(leds))
        return;

    nrf_gpio_pin_write(leds[led].pin, !leds[led].dir ^ on);
}
#endif

/**
 * @brief 底层设置LED的状态
 * 
 * @param mask 
 */
static void leds_set_ll(uint8_t mask)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(leds); i++) {
        nrf_gpio_pin_write(leds[i].pin, !leds[i].dir ^ ((mask & (1 << i)) > 0));
    }
}

/**
 * @brief 显示缓存的LED状态
 * 
 */
static void leds_update()
{
    leds_set_ll(led_val);
}

/**
 * @brief 关闭所有LED
 * 
 */
static void leds_off()
{
    leds_set_ll(0);
}

/**
 * @brief 设置缓存的LED状态，但不更新显示
 * 
 * @param led 
 * @param val 
 */
static void led_set(uint8_t led, bool val)
{
    if (val)
        led_val |= (1 << led);
    else
        led_val &= ~(1 << led);
}

/**
 * @brief 反转缓存的LED状态，但不更新显示
 * 
 * @param led 
 */
static void led_reverse(uint8_t led)
{
    led_set(led, !(led_val & (1 << led)));
}

static void ledmap_handler(enum user_event event, uint8_t param)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(events); i++) {
        if (events[i].event == event && events[i].param == param) {
            for (uint8_t j = 0; j < ARRAY_SIZE(leds); j++) {
                if ((1 << j) & events[i].led_mask) {
                    led_saved_evt[j] = events[i].action;
                    switch (events[i].action) {
                    case TRIG_LED_ON:
                        led_set(j, true);
                        break;
                    case TRIG_LED_OFF:
                        led_set(j, false);
                        break;
                    case TRIG_LED_REVERSE:
                        led_reverse(j);
                        break;
                    default:
                        break;
                    }

                    leds_update();
                    power_save_reset();
                }
            }
        }
    }
}

static void led_blink_timer_handler(void* context)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(leds); i++) {
        if (led_saved_evt[i] >> 6 == LED_TRIG_BLINK) {
            uint8_t times = led_saved_evt[i] & 0x07;
            uint8_t duration = (led_saved_evt[i] >> 3) & 0x07;

            // 判断是否达到次数。如果达到则重置计数器并关闭灯管
            if (times && (led_blink_counter[i] >> 4) >= times) {
                led_saved_evt[i] = TRIG_LED_OFF;
                led_blink_counter[i] = 0;
                continue;
            }

            // 长度到了，处理闪亮
            if ((led_blink_counter[i] & 0x0F) == 0) {
                led_blink_counter[i] &= 0xF0; // 重置长度计数器
                led_blink_counter[i] += duration;

                // 仅在指定次数并且当前灯是亮的情况下增加计数器
                if (times && led_val & (1 << i)) {
                    led_blink_counter[i] += 0x10;
                }
                led_reverse(i);

                // 显示当前状态
                if (!power_save) {
                    leds_update();
                }
            }
            led_blink_counter[i]--;
        }
    }
}

static void leds_init()
{
    for (uint8_t i = 0; i < ARRAY_SIZE(leds); i++) {
        nrf_gpio_cfg_output(leds[i].pin);
    }
    app_timer_create(&led_blink_timer, APP_TIMER_MODE_REPEATED, led_blink_timer_handler);
}

static void leds_deinit()
{
    for (uint8_t i = 0; i < ARRAY_SIZE(leds); i++) {
        nrf_gpio_cfg_default(leds[i].pin);
    }
}

static void ledmap_evt_handler(enum user_event event, void* arg)
{
    uint32_t param = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (param) {
        case KBD_STATE_POST_INIT: // 初始化LED
            leds_init();
            leds_set_ll(0xFF);
            break;
        case KBD_STATE_INITED: // 启动闪亮计时器
            app_timer_start(led_blink_timer, APP_TIMER_TICKS(200), NULL);
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            leds_deinit();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE:
        switch (param) {
        case PWR_SAVE_ENTER: // 进入省电模式
            leds_off();
            power_save = true;
            break;
        case PWR_SAVE_EXIT: // 退出省电模式
            leds_update();
            power_save = false;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    ledmap_handler(event, param);
}

EVENT_HANDLER(ledmap_evt_handler);
