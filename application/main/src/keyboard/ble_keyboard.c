#include "ble_keyboard.h" // self
#include "../config/keyboard_config.h"
#include "app_timer.h" // nordic
#include "keyboard.h" // tmk
#include <stdint.h>
#include "keyboard_host_driver.h"

#include "hook.h"
#include "custom_hook.h"

APP_TIMER_DEF(m_keyboard_scan_timer); /**< keyboard scan timer. */
APP_TIMER_DEF(m_keyboard_sleep_timer); /**< keyboard sleep timer. */

#define SCAN_INTERVAL_FAST APP_TIMER_TICKS(KEYBOARD_FAST_SCAN_INTERVAL) /**< Keyboard scan interval (ticks). */
#define SCAN_INTERVAL_SLOW APP_TIMER_TICKS(KEYBOARD_SLOW_SCAN_INTERVAL) /**< Keyboard slow scan interval (ticks). */
#define TICK_INTERVAL APP_TIMER_TICKS(1000) /**< 键盘Tick计时器 */

static uint32_t sleep_counter;

/**
 * @brief 切换扫描模式
 * 
 * @param slow 切换至慢速扫描
 */
static void keyboard_switch_scan_mode(bool slow)
{
    uint32_t err_code;
    err_code = app_timer_stop(m_keyboard_scan_timer);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_keyboard_scan_timer, slow ? SCAN_INTERVAL_SLOW : SCAN_INTERVAL_FAST, NULL);
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
    keyboard_task();
}

/**
 * @brief 睡眠定时器超时处理器
 * 
 * @param p_context 
 */
static void keyboard_sleep_handler(void* p_context)
{
    UNUSED_PARAMETER(p_context);
    sleep_counter++;

    if (sleep_counter == SLEEP_SLOW_TIMEOUT) {
        keyboard_switch_scan_mode(true);
    } else if (sleep_counter == SLEEP_OFF_TIMEOUT) {
        // todo: sleep.
    }
}

/**
 * @brief 重置睡眠定时器
 * 
 */
static void keyboard_sleep_counter_reset(void)
{
    if (sleep_counter >= SLEEP_SLOW_TIMEOUT) {
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
 * @brief 键盘按键按下的Hook
 */
void hook_key_change()
{
#ifdef SLOW_MODE_EARLY_EXIT
    keyboard_sleep_counter_reset();
#endif
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

    // init keyboard sleep counter timer
    err_code = app_timer_create(&m_keyboard_sleep_timer,
        APP_TIMER_MODE_REPEATED,
        keyboard_sleep_handler);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 启动键盘计时器
 * 
 */
void ble_keyboard_timer_start(void)
{
    ret_code_t err_code = app_timer_start(m_keyboard_scan_timer, SCAN_INTERVAL_FAST, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_keyboard_sleep_timer, TICK_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 初始化键盘所需的各项东西
 * 
 */
void ble_keyboard_init(void)
{
    keyboard_setup(); // 初始化各按键阵列
    // - martix_setup();
    // led_init(); // 初始化led
    // keymap_init(); // 初始化自定义keymap
    // uart_init(); // 初始化外部USB通信协议
    keyboard_init();
    // - timer_init();
    // - matrix_init();
    host_set_driver(&driver); // 设置 host driver
    keyboard_timer_init(); // 初始化计时器
}
