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
#include "nrf_delay.h"

static void off_timer_start();

#ifdef LED_RGB
#include "low_power_pwm.h"

static low_power_pwm_t led_r;
static low_power_pwm_t led_g;
static low_power_pwm_t led_b;
static uint32_t saved_color;

static void pwm_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);
}

// Gamma 对应表
const uint8_t gamma[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 4, 4, 4, 5, 5,
    6, 6, 6, 7, 7, 8, 8, 9,
    9, 10, 11, 11, 12, 12, 13, 14,
    14, 15, 16, 17, 17, 18, 19, 20,
    20, 21, 22, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 41, 42, 43, 44,
    45, 47, 48, 49, 51, 52, 53, 55,
    56, 58, 59, 60, 62, 63, 65, 67,
    68, 70, 71, 73, 75, 76, 78, 80,
    81, 83, 85, 87, 88, 90, 92, 94,
    96, 98, 100, 102, 104, 106, 108, 110,
    112, 114, 116, 118, 120, 122, 124, 127};

/**
 * @brief 设置RGB灯光值
 * 
 * @param color 
 */
static void keyboard_led_rgb_set_internal(uint32_t color)
{
    uint8_t r, g, b;
    r = (color & 0xFF0000) >> 17;
    g = (color & 0xFF00) >> 9;
    b = (color & 0xFF) >> 1;

    r = gamma[r];
    g = gamma[g];
    b = gamma[b];

    // 红色灯光的亮度在同样电流下是其他两个的一半，故需要校正颜色
    low_power_pwm_duty_set(&led_r, r * 2);
    low_power_pwm_duty_set(&led_g, g);
    low_power_pwm_duty_set(&led_b, b);
}

/**
 * @brief 设置RGB灯光值并启用自动熄灭
 * 
 * @param color 
 */
void keyboard_led_rgb_set(uint32_t color)
{
    saved_color = color;
    keyboard_led_rgb_set_internal(color);
    off_timer_start();
}

/**
 * @brief 初始化RGB灯光
 * 
 */
static void keyboard_led_rgb_init()
{
    uint32_t err_code;
    low_power_pwm_config_t lppwm_config = LOW_POWER_PWM_DEFAULT_CONFIG(0);

    APP_TIMER_DEF(lpp_timer_0);
    lppwm_config.bit_mask = 1 << LED_RGB_R;
    lppwm_config.p_timer_id = &lpp_timer_0;
    err_code = low_power_pwm_init(&led_r, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_r, 0);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    lppwm_config.bit_mask = 1 << LED_RGB_G;
    lppwm_config.p_timer_id = &lpp_timer_1;
    err_code = low_power_pwm_init(&led_g, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_g, 0);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    lppwm_config.bit_mask = 1 << LED_RGB_B;
    lppwm_config.p_timer_id = &lpp_timer_2;
    err_code = low_power_pwm_init(&led_b, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_b, 0);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start(&led_r, led_r.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_g, led_g.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_b, led_b.bit_mask);
    APP_ERROR_CHECK(err_code);
}
/**
 * @brief 关闭 RGB LED 的显示
 * 
 */
static void keyboard_led_rgb_off()
{
    low_power_pwm_stop(&led_r);
    low_power_pwm_stop(&led_g);
    low_power_pwm_stop(&led_b);

    nrf_gpio_pin_set(LED_RGB_R);
    nrf_gpio_pin_set(LED_RGB_G);
    nrf_gpio_pin_set(LED_RGB_B);
}

/**
 * @brief 直接设置 RGB LED 的值
 * 
 * @remark 在关闭timer后，无法使用原有的PWM调色，仅能使用此方法设置LED颜色
 * 
 * @param bit 
 */
void keyboard_led_rgb_direct(uint8_t bit)
{
    nrf_gpio_pin_write(LED_RGB_R, !(bit & 0x04));
    nrf_gpio_pin_write(LED_RGB_G, !(bit & 0x02));
    nrf_gpio_pin_write(LED_RGB_B, !(bit & 0x01));
}
#endif

/**
 * @brief 设置键盘状态LED灯的值
 * 
 * @param keyboard_led_val   LED状态值
 */
static void keyboard_led_set_internal(uint8_t keyboard_led_val)
{
#ifdef LED_NUM
    LED_WRITE(LED_NUM, keyboard_led_val & (1 << 0));
#endif
#ifdef LED_CAPS
    LED_WRITE(LED_CAPS, keyboard_led_val & (1 << 1));
#endif
#ifdef LED_SCLK
    LED_WRITE(LED_SCLK, keyboard_led_val & (1 << 2));
#endif
}

static uint8_t saved_keyboard_led_val;

/**
 * @brief 设置键盘状态LED灯的值并启用自动熄灭
 * 
 * @param keyboard_led_val   LED状态值
 */
void keyboard_led_set(uint8_t keyboard_led_val)
{
    saved_keyboard_led_val = keyboard_led_val;
    keyboard_led_set_internal(keyboard_led_val);
    off_timer_start();
}

#ifdef LED_STATUS

/**
 * @brief 设置系统状态LED灯的状态
 * 
 * @param system_led_val   LED状态值
 */
static void system_led_set_internal(uint8_t system_led_val)
{
//更改系统LED的状态
#ifdef LED_BLE
    LED_WRITE(LED_BLE, system_led_val & (1 << LED_BIT_BLE));
#endif
#ifdef LED_USB
    LED_WRITE(LED_USB, system_led_val & (1 << LED_BIT_USB));
#endif
#ifdef LED_CHARGING
    LED_WRITE(LED_CHARGING, system_led_val & (1 << LED_BIT_CHARGING));
#endif
}

/**
 * @brief 开启系统状态LED灯
 * 
 */
void system_led_on(void)
{
//开启LED状态灯
#ifdef LED_BLE
    LED_WRITE(LED_BLE, 1);
#endif
#ifdef LED_USB
    LED_WRITE(LED_USB, 1);
#endif
#ifdef LED_CHARGING
    LED_WRITE(LED_CHARGING, 1);
#endif
}

uint8_t saved_system_led_val;

/**
 * @brief 设置系统状态LED灯的状态值
 * 
 * @param bit 位置
 * @param state 状态
 */
void system_led_set_val(enum led_bit_usage bit, bool state)
{
    uint8_t system_led_val = saved_system_led_val; //读取系统LED的状态值
    // 更改系统LED的状态的值
    if (state)
    {
        system_led_val |= 1 << bit;
    }
    else
    {
        system_led_val &= ~(1 << bit);
    }
    saved_system_led_val = system_led_val; //存储系统LED的状态值
}

/**
 * @brief 设置系统状态LED灯,并启用自动熄灭
 * 
 */
void system_led_set()
{
    system_led_set_internal(saved_system_led_val);
    off_timer_start();
}
#endif

/**
 * @brief 将所有LED、RGB熄灭
 * 
 */
static void led_off(void)
{
    keyboard_led_set_internal(0);
#ifdef LED_STATUS
    system_led_set_internal(0);
#endif
#ifdef LED_RGB
    keyboard_led_rgb_set_internal(0);
#endif
}

/**
 * @brief 将所有LED、RGB熄灭、并停用RGB。
 * 
 */
void keyboard_led_off(void)
{
    led_off();
#ifdef LED_RGB
    keyboard_led_rgb_off();
#endif
}

#if LED_AUTOOFF_TIME > 0

static bool counting;
static bool led_autooff = true;
APP_TIMER_DEF(led_off_timer);
APP_TIMER_DEF(led_blink_timer);

/**
 * @brief 重新启用LED、RGB，并恢复原本状态
 * 
 */
static void led_on(void)
{
    keyboard_led_set_internal(saved_keyboard_led_val);
#ifdef LED_STATUS
    system_led_set_internal(saved_system_led_val);
#endif
#ifdef LED_RGB
    keyboard_led_rgb_set_internal(saved_color);
#endif
}

/**
 * @brief LED自动关闭的handler
 * 
 * @param context 
 */
static void led_off_timer_handler(void *context)
{
    led_off();
    counting = false;
}

/**
 * @brief LED闪烁并提示状态handler
 * 
 * @param context 
 */
static void led_blink_timer_handler(void *context)
{
    led_off();
    led_on();
    off_timer_start();
}

/**
 * @brief 启动自动关闭计时器
 * 
 */
static void off_timer_start()
{
    if (led_autooff)
    {
        if (counting)
            app_timer_stop(led_off_timer);
        led_on();
        app_timer_start(led_off_timer, APP_TIMER_TICKS(LED_AUTOOFF_TIME), NULL);
        counting = true;
    }
}

/**
 * @brief 初始自动关灯计时器
 * 
 */
static void off_timer_init(void)
{
    app_timer_create(&led_off_timer, APP_TIMER_MODE_SINGLE_SHOT, led_off_timer_handler);
    app_timer_create(&led_blink_timer, APP_TIMER_MODE_SINGLE_SHOT, led_blink_timer_handler);
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

/**
 * @brief 启用LED状态灯显示
 */
void led_display()
{
    keyboard_led_set_internal(0x07);
#ifdef LED_STATUS
    system_led_set_internal(0x07);
#endif
#ifdef LED_RGB
    keyboard_led_rgb_set_internal(saved_color);
#endif
    app_timer_start(led_blink_timer, APP_TIMER_TICKS(200), NULL);
}

#else

static void off_timer_init()
{
}

static void off_timer_start()
{
}

void keyboard_led_powersave(bool powersave)
{
}

void led_display()
{
}
#endif

/**
 * @brief 初始化LED各项设置
 * 
 */
void keyboard_led_init()
{
#ifdef LED_RGB
    keyboard_led_rgb_init();
#endif
#ifdef LED_NUM
    nrf_gpio_cfg_output(LED_NUM);
#endif
#ifdef LED_CAPS
    nrf_gpio_cfg_output(LED_CAPS);
#endif
#ifdef LED_SCLK
    nrf_gpio_cfg_output(LED_SCLK);
#endif

#ifdef LED_STATUS
#ifdef LED_BLE
    nrf_gpio_cfg_output(LED_BLE);
#endif
#ifdef LED_USB
    nrf_gpio_cfg_output(LED_USB);
#endif
#ifdef LED_CHARGING
    nrf_gpio_cfg_output(LED_CHARGING);
#endif
#endif
    off_timer_init();
}

/**
 * @brief 释放LED
 * 
 */
void led_uninit(void)
{
#ifdef LED_RGB
    nrf_gpio_cfg_default(LED_RGB_R);
    nrf_gpio_cfg_default(LED_RGB_G);
    nrf_gpio_cfg_default(LED_RGB_B);
#endif
#ifdef LED_NUM
    nrf_gpio_cfg_default(LED_NUM);
#endif
#ifdef LED_CAPS
    nrf_gpio_cfg_default(LED_CAPS);
#endif
#ifdef LED_SCLK
    nrf_gpio_cfg_default(LED_SCLK);
#endif

#ifdef LED_STATUS
#ifdef LED_BLE
    nrf_gpio_cfg_default(LED_BLE);
#endif
#ifdef LED_USB
    nrf_gpio_cfg_default(LED_USB);
#endif
#ifdef LED_CHARGING
    nrf_gpio_cfg_default(LED_CHARGING);
#endif
#endif
}