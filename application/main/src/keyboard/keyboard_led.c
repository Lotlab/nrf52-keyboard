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

#include "app_timer.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "../main.h"

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
    off_timer_start();
}

/**
 * @brief 将LED熄灭
 * 
 */
static void led_off(void)
{
    keyboard_led_set_internal(0);
    ble_user_event(USER_LED_OFF);
}

/**
 * @brief 将LED熄灭
 * 
 */
void keyboard_led_deinit(void)
{
    led_off();
    ble_user_event(USER_LED_DEINIT);
}

#if LED_AUTOOFF_TIME > 0

static bool counting;
static bool led_autooff = true;
APP_TIMER_DEF(led_off_timer);

/**
 * @brief 将LED点亮
 * 
 */
static void led_on(void)
{
    keyboard_led_set_internal(saved_led_val);
    ble_user_event(USER_LED_ON);
}

/**
 * @brief LED自动关闭的handler
 * 
 * @param context 
 */
static void led_off_timer_handler(void* context)
{
    led_off();
    counting = false;
}

/**
 * @brief 启动自动关闭计时器
 * 
 */
void off_timer_start()
{
    if (led_autooff) {
        if (counting)
            app_timer_stop(led_off_timer);
        led_on();
        app_timer_start(led_off_timer, APP_TIMER_TICKS(LED_AUTOOFF_TIME), NULL);
        counting = true;
    }
}

/**
 * @brief 初始计时器
 * 
 */
static void off_timer_init(void)
{
    app_timer_create(&led_off_timer, APP_TIMER_MODE_SINGLE_SHOT, led_off_timer_handler);
}

/**
 * @brief 设置省电模式状态
 * 
 * @param powersave 
 */
void keyboard_led_powersave(bool powersave)
{
    if (counting)
        app_timer_stop(led_off_timer);

    led_autooff = powersave;
    if (powersave)
        off_timer_start();
    else
        led_on();
}

#else

static void off_timer_init()
{
}

void off_timer_start()
{
}

void keyboard_led_powersave(bool powersave)
{
}

#endif

/**
 * @brief 初始化LED各项设置
 * 
 */
void keyboard_led_init()
{
    off_timer_init();

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
