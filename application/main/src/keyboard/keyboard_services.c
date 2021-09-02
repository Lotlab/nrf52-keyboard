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

#include "keyboard_services.h" // self
#include "../config/keyboard_config.h"
#include "app_timer.h" // nordic
#include "keyboard.h" // tmk
#include "keyboard_host_driver.h"
#include <stdint.h>

#include "../main.h"
#include "data_storage.h"
#include "hook.h"
#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "macro_player.h"
#include "store_config.h"
#include "usb_comm.h"

#include "nrf_drv_wdt.h"

APP_TIMER_DEF(m_keyboard_scan_timer); /**< keyboard scan timer. */
APP_TIMER_DEF(m_keyboard_debounce_timer); /**< keyboard debounce timer. */
APP_TIMER_DEF(m_keyboard_sleep_timer); /**< keyboard sleep timer. */

#define FAST_SCAN_INTERVAL APP_TIMER_TICKS(KEYBOARD_FAST_SCAN_INTERVAL)
#define SLOW_SCAN_INTERVAL APP_TIMER_TICKS(KEYBOARD_SLOW_SCAN_INTERVAL)
#define DEBOUNCE_INTERVAL APP_TIMER_TICKS(KEYBOARD_SCAN_INTERVAL)
#define TICK_INTERVAL APP_TIMER_TICKS(1000) /**< 键盘Tick计时器 */

static uint32_t sleep_counter;
static bool powersave = true;

/**
 * @brief 切换扫描模式
 * 
 * @param slow 切换至慢速扫描
 */
static void keyboard_switch_scan_mode(bool slow)
{
    ret_code_t err_code = app_timer_stop(m_keyboard_scan_timer);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_keyboard_scan_timer, slow ? SLOW_SCAN_INTERVAL : FAST_SCAN_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 键盘扫描定时器
 * 
 * @param p_context 
 */
static void keyboard_scan_handler(void* p_context)
{
    UNUSED_PARAMETER(p_context);
    // 处理按键扫描
#ifdef MACRO_BLOCKING_MODE
    // 仅在没有更多宏的情况下继续处理按键
    if (macro_queue_empty())
#endif
    {
        keyboard_task();
    }
}

/**
 * @brief 睡眠定时器超时处理器
 * 
 * @param p_context 
 */
static void keyboard_sleep_handler(void* p_context)
{
    UNUSED_PARAMETER(p_context);

#ifdef ENABLE_WATCHDOG
    // feed watch dog
    nrf_drv_wdt_feed();
#endif

    if (powersave)
        sleep_counter++;

    if (sleep_counter == get_slow_scan_timeout()) {
        keyboard_switch_scan_mode(true);
    } else if (sleep_counter == get_sleep_timeout()) {
        sleep(SLEEP_TIMEOUT);
    }

    // trig TICK
    trig_event(USER_EVT_TICK, 0);
}

/**
 * @brief 重置睡眠定时器
 * 
 */
static void keyboard_sleep_counter_reset(void)
{
    if (sleep_counter >= get_slow_scan_timeout()) {
        keyboard_switch_scan_mode(false);
    }
    sleep_counter = 0;
}

/**
 * @brief 键盘扫描结果改变的Hook
 * 
 * @param event 
 */
void hook_matrix_change(keyevent_t event)
{
    keyboard_sleep_counter_reset();
}

/**
 * @brief 初始化键盘计时器
 * 
 */
static void keyboard_timer_init(void)
{
    // init keyboard scan timer
    ret_code_t err_code = app_timer_create(&m_keyboard_scan_timer,
        APP_TIMER_MODE_REPEATED,
        keyboard_scan_handler);
    APP_ERROR_CHECK(err_code);

    // debounce timer
    err_code = app_timer_create(&m_keyboard_debounce_timer,
        APP_TIMER_MODE_SINGLE_SHOT,
        keyboard_scan_handler);
    APP_ERROR_CHECK(err_code);

    // init keyboard sleep counter timer
    err_code = app_timer_create(&m_keyboard_sleep_timer,
        APP_TIMER_MODE_REPEATED,
        keyboard_sleep_handler);
    APP_ERROR_CHECK(err_code);
}

#ifdef ENABLE_WATCHDOG
nrf_drv_wdt_channel_id m_channel_id;

static void wdt_event_handler(void)
{
    APP_ERROR_HANDLER(0xFEED0514);
}

/**
 * @brief 初始化看门狗
 * 
 */
static void keyboard_wdt_init(void)
{
    ret_code_t err_code;

    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}
#endif

void keyboard_debounce(void)
{
    // 启用消抖定时器，使用快速扫描间隔扫描
    ret_code_t err_code = app_timer_start(m_keyboard_debounce_timer, DEBOUNCE_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 启动键盘计时器
 * 
 */
void keyboard_timer_start(void)
{
    ret_code_t err_code = app_timer_start(m_keyboard_scan_timer, FAST_SCAN_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_keyboard_sleep_timer, TICK_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 设置省电模式
 * 
 * @param save 
 */
void keyboard_powersave(bool save)
{
    if (save != powersave) {
        powersave = save;
        if (!powersave) {
            keyboard_sleep_counter_reset();
        }
    }
}

/**
 * @brief 初始化键盘所需的各项东西
 * 
 */
void keyboard_services_init(void)
{
    keyboard_setup(); // 初始化各按键阵列
    // - martix_setup();
    keyboard_led_init(); // 初始化LED
#ifdef HAS_USB
    usb_comm_init(); // 初始化USB通讯
#endif
    keyboard_init(); // 初始化键盘所需的其他东西，包括按键阵列和Bootmagic
    // - timer_init();
    // - matrix_init();
    host_set_driver(&driver); // 设置 host driver
    keyboard_timer_init(); // 初始化计时器
    macro_play_timer_init(); // 初始化宏计数器
#ifdef ENABLE_WATCHDOG
    keyboard_wdt_init(); // 初始化看门狗
#endif
}
