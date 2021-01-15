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

static const struct ledmap_led ledmap_leds[];
static const struct ledmap_event ledmap_events[];

static bool power_save = false;
static uint8_t led_val = 0;

static struct ledmap_evt_action led_saved_evt[4][ARRAY_SIZE(ledmap_leds)] = { 0 };
static uint8_t led_blink_counter[ARRAY_SIZE(ledmap_leds)] = { 0 };

#if 0
/**
 * @brief 底层设置LED状态
 * 
 * @param led 指定的LED
 * @param on 
 */
static void led_set_ll(uint8_t led, bool on)
{
    if (led >= ARRAY_SIZE(ledmap_leds))
        return;

    nrf_gpio_pin_write(ledmap_leds[led].pin, !ledmap_leds[led].dir ^ on);
}
#endif

/**
 * @brief 底层设置LED的状态
 * 
 * @param mask 
 */
static void leds_set_ll(uint8_t mask)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(ledmap_leds); i++) {
        nrf_gpio_pin_write(ledmap_leds[i].pin, !ledmap_leds[i].dir ^ ((mask & (1 << i)) > 0));
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

/**
 * @brief 获取指定的LED当前的动作
 * 
 * @param led 
 * @return struct ledmap_evt_action* 
 */
static struct ledmap_evt_action* led_get_action(uint8_t led)
{
    uint8_t j = 4;
    while (j--) {
        if (led_saved_evt[j][led].action != TRIG_NO_ACTION) {
            return &led_saved_evt[j][led];
        }
    }
    return &led_saved_evt[0][led];
}

/**
 * @brief 处理一些可以立即设置的灯光
 * 
 */
static void led_actions_handler()
{
    for (uint8_t i = 0; i < ARRAY_SIZE(ledmap_leds); i++) {
        struct ledmap_evt_action* action = led_get_action(i);

        switch (action->action) {
        case TRIG_NO_ACTION:
        case TRIG_LED_OFF:
            led_set(i, false);
            break;
        case TRIG_LED_ON:
            led_set(i, true);
            break;
        case TRIG_LED_REVERSE:
            led_reverse(i);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief LEDMAP 事件处理
 * 
 * @param evt 
 * @param param 
 */
static void ledmap_handler(enum user_event evt, uint8_t param)
{
    bool led_changed = false;
    for (uint8_t i = 0; i < ARRAY_SIZE(ledmap_events); i++) {
        struct ledmap_event event = ledmap_events[i];
        if (event.event != evt || event.param != param)
            continue;

        for (uint8_t j = 0; j < ARRAY_SIZE(ledmap_leds); j++) {
            if ((1 << j) & event.led_mask) {
                led_saved_evt[event.action.priority][j] = event.action;
                led_blink_counter[j] = 0;
                led_changed = true;
            }
        }
    }

    // 更新灯光并推出省电模式
    if (led_changed) {
        led_actions_handler();
        leds_update();
        power_save_reset();
    }
}

static void led_blink_timer_handler(void* context)
{
    bool state_change = false;
    for (uint8_t i = 0; i < ARRAY_SIZE(ledmap_leds); i++) {
        struct ledmap_evt_action* action = led_get_action(i);

        if (action->act_code == LED_TRIG_BLINK) {
            uint8_t times = action->param & 0x03;
            uint8_t duration = (action->param >> 2) & 0x03;

            // 判断是否达到次数。如果达到则重置计数器并关闭灯光
            if (times && (led_blink_counter[i] >> 4) >= times) {
                action->action = TRIG_NO_ACTION;
                led_blink_counter[i] = 0;
                // 更新灯光
                led_actions_handler();
                state_change = true;
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
                state_change = true;
            }
            led_blink_counter[i]--;
        }
    }
    if (state_change && !power_save) {
        leds_update();
    }
}

static void leds_init()
{
    for (uint8_t i = 0; i < ARRAY_SIZE(ledmap_leds); i++) {
        nrf_gpio_cfg_output(ledmap_leds[i].pin);
    }
    app_timer_create(&led_blink_timer, APP_TIMER_MODE_REPEATED, led_blink_timer_handler);
}

static void leds_deinit()
{
    for (uint8_t i = 0; i < ARRAY_SIZE(ledmap_leds); i++) {
        nrf_gpio_cfg_default(ledmap_leds[i].pin);
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
