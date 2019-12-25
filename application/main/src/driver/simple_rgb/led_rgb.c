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
#include "led_rgb.h"
#include "power_save.h"
#include "low_power_pwm.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "config.h"
#include <stdint.h>

static low_power_pwm_t led_r;
static low_power_pwm_t led_g;
static low_power_pwm_t led_b;
static uint32_t saved_color;

static void pwm_handler(void* p_context)
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
    112, 114, 116, 118, 120, 122, 124, 127
};

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
    power_save_reset();
}

/**
 * @brief 初始化RGB灯光
 * 
 */
void keyboard_led_rgb_init()
{
    uint32_t err_code;
    low_power_pwm_config_t lppwm_config = LOW_POWER_PWM_DEFAULT_CONFIG(0);

#ifdef LED_RGB_CC
    lppwm_config.active_high = true;
#endif

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
void keyboard_led_rgb_deinit()
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

/**
 * @brief 切换 RGB LED 的显示状态
 */
void keyboard_led_rgb_switch(bool on)
{
    keyboard_led_rgb_set_internal(on ? saved_color : 0);
}
