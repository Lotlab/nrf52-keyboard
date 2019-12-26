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

#include "keyboard_led.h"

#include "../main.h"
#include "keyboard_evt.h"
#include "nrf_gpio.h"
#include "power_save.h"

/**
 * @brief 设置LED灯的值
 * 
 * @param led_val 
 */
static void keyboard_led_set_internal(uint8_t led_val)
{
#ifdef LED_NUM
    LED_WRITE(LED_NUM, led_val & (1 << 0));
#endif
#ifdef LED_CAPS
    LED_WRITE(LED_CAPS, led_val & (1 << 1));
#endif
#ifdef LED_SCLK
    LED_WRITE(LED_SCLK, led_val & (1 << 2));
#endif
}

static uint8_t saved_led_val;

/**
 * @brief 设置LED灯的值并启用自动熄灭
 * 
 * @param led_val 
 */
void keyboard_led_set(uint8_t led_val)
{
    saved_led_val = led_val;
    keyboard_led_set_internal(led_val);
    power_save_reset();
}

/**
 * @brief 将LED熄灭
 * 
 */
static void led_off(void)
{
    keyboard_led_set_internal(0);
}

/**
 * @brief 将LED熄灭以准备休眠
 * 
 */
void keyboard_led_deinit(void)
{
    led_off();
}

/**
 * @brief 将LED点亮
 * 
 */
static void led_on(void)
{
    keyboard_led_set_internal(saved_led_val);
}

/**
 * @brief 初始化LED各项设置
 * 
 */
void keyboard_led_init()
{
#ifdef LED_NUM
    nrf_gpio_cfg_output(LED_NUM);
#endif
#ifdef LED_CAPS
    nrf_gpio_cfg_output(LED_CAPS);
#endif
#ifdef LED_SCLK
    nrf_gpio_cfg_output(LED_SCLK);
#endif
}

static void led_event_handler(enum user_event event, void* arg)
{
    switch (event) {
    case USER_EVT_POWERSAVE:
        switch ((uint32_t)arg) {
        case PWR_SAVE_ENTER:
            led_off();
            break;
        case PWR_SAVE_EXIT:
            led_on();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(led_event_handler);
