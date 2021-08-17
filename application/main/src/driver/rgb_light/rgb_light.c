/* Copyright 2016-2017 Yang Liu
 *           2020-2021 modify by Geno
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "rgb_light.h"
#include "app_timer.h"
#include "config.h"
#include "data_storage.h"
#include "keyboard_evt.h"
#include "low_power_pwm.h"
#include "nrf_gpio.h"
#include "power_save.h"
#include "progmem.h"
#include "timer.h"
#include "wait.h"
#include <math.h>

#include <keyboard_services.h>

#ifndef RGB_LIGHT_LIMIT_VAL
#define RGB_LIGHT_LIMIT_VAL 255
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define M_PI 3.14159265358979323846
#define M_E 2.71828182845904523536

__attribute__((weak))
const uint8_t RGBLED_BREATHING_INTERVALS[]
    = { 30, 20, 10, 5 };
__attribute__((weak))
const uint8_t RGBLED_RAINBOW_MOOD_INTERVALS[]
    = { 120, 60, 30 };
__attribute__((weak))

rgb_light_config_t rgb_light_config;
static bool rgb_light_timer_enabled = false;
static bool ble_connected = false;
static bool usb_working = false;
static uint8_t ble_channel = 0;
static bool rgb_working = false;


#define PWM_BITS 8
#define PWM_PRESCALER (8 - PWM_BITS)

static low_power_pwm_t led_r;
static low_power_pwm_t led_g;
static low_power_pwm_t led_b;

/**
 * 闪烁延迟处理事件
 */
enum rgb_light_delay_hander_event {
    INIT_LED,         //初始化
    SLEEP_LED         //休眠
};

// Gamma 对应表
const uint8_t LED_TABLE[] = {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
    3, 4, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 5, 6, 6, 6, 6, 6, 7, 7, 7,
    7, 8, 8, 8, 8, 9, 9, 9, 10, 10,
    10, 10, 11, 11, 11, 12, 12, 12, 13, 13,
    13, 14, 14, 15, 15, 15, 16, 16, 17, 17,
    17, 18, 18, 19, 19, 20, 20, 21, 21, 22,
    22, 23, 23, 24, 24, 25, 25, 26, 26, 27,
    28, 28, 29, 29, 30, 31, 31, 32, 32, 33,
    34, 34, 35, 36, 37, 37, 38, 39, 39, 40,
    41, 42, 43, 43, 44, 45, 46, 47, 47, 48,
    49, 50, 51, 52, 53, 54, 54, 55, 56, 57,
    58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
    68, 70, 71, 72, 73, 74, 75, 76, 77, 79,
    80, 81, 82, 83, 85, 86, 87, 88, 90, 91,
    92, 94, 95, 96, 98, 99, 100, 102, 103, 105,
    106, 108, 109, 110, 112, 113, 115, 116, 118, 120,
    121, 123, 124, 126, 128, 129, 131, 132, 134, 136,
    138, 139, 141, 143, 145, 146, 148, 150, 152, 154,
    155, 157, 159, 161, 163, 165, 167, 169, 171, 173,
    175, 177, 179, 181, 183, 185, 187, 189, 191, 193,
    196, 198, 200, 202, 204, 207, 209, 211, 214, 216,
    218, 220, 223, 225, 228, 230, 232, 235, 237, 240,
    242, 245, 247, 250, 252, 255,
};

static void pwm_handler(void* p_context)
{
    UNUSED_PARAMETER(p_context);
}

static inline void led_val_set(low_power_pwm_t *pwm, char val) {
    low_power_pwm_duty_set(pwm, val >> PWM_PRESCALER);
} 

/**
 * @brief 启动PWM
 * 
 */
static void rgb_light_lppwm_start()
{
    uint32_t err_code;
    err_code = low_power_pwm_start(&led_r, led_r.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_g, led_g.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_b, led_b.bit_mask);
    APP_ERROR_CHECK(err_code);
    rgb_working = true;
}
/**
 * @brief 停止PWM
 * 
 */
static void rgb_light_lppwm_stop()
{
    uint32_t err_code;
    err_code = low_power_pwm_stop(&led_r);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_stop(&led_g);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_stop(&led_g);
    APP_ERROR_CHECK(err_code);
    rgb_working = false;
}

/**
 * @brief 初始化LPPWM
 * 
 */
static void rgb_light_lppwm_init()
{
    uint32_t err_code;
    low_power_pwm_config_t lppwm_config = {
#ifdef LED_RGB_CC
        .active_high = true,
#else
        .active_high = false,
#endif
        .period = (1 << PWM_BITS) - 1,
        .p_port = LOW_POWER_PWM_CONFIG_PORT,
    };

    //RED LED
    APP_TIMER_DEF(lpp_timer_0);
    lppwm_config.bit_mask = 1 << LED_RGB_R;
    lppwm_config.p_timer_id = &lpp_timer_0;
    err_code = low_power_pwm_init(&led_r, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_r, 0);
    APP_ERROR_CHECK(err_code);
    //Green LED
    APP_TIMER_DEF(lpp_timer_1);
    lppwm_config.bit_mask = 1 << LED_RGB_G;
    lppwm_config.p_timer_id = &lpp_timer_1;
    err_code = low_power_pwm_init(&led_g, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_g, 0);
    APP_ERROR_CHECK(err_code);
    //Blue LED
    APP_TIMER_DEF(lpp_timer_2);
    lppwm_config.bit_mask = 1 << LED_RGB_B;
    lppwm_config.p_timer_id = &lpp_timer_2;
    err_code = low_power_pwm_init(&led_b, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_b, 0);
    APP_ERROR_CHECK(err_code);

    rgb_light_lppwm_start();
}
/**
 * @brief 关闭LPPWM
 * 
 */
static void rgb_light_lppwm_deinit()
{
    nrf_gpio_cfg_input(LED_RGB_R, NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_input(LED_RGB_G, NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_input(LED_RGB_B, NRF_GPIO_PIN_PULLDOWN);
}

//闪烁计时器
APP_TIMER_DEF(inc_flash_timer);

static void inc_flash_handler(void* p_context)
{
    uint8_t handler_name = (uint32_t)p_context;
    switch (handler_name) {
    case SLEEP_LED:
        rgb_light_lppwm_stop();
        rgb_light_lppwm_deinit();
        break;
    case INIT_LED:
        light_setrgb(0, 0, 0);
        rgb_light_lppwm_stop();
        break;
    default:
        break;
    }
}

static void inc_flash_timer_init()
{
    app_timer_create(&inc_flash_timer, APP_TIMER_MODE_SINGLE_SHOT, inc_flash_handler);
}

static void led_status_change()
{
    if (rgb_light_config.ind) {
        if (usb_working) {
            rgb_light_setrgb_green();
        } else {
            if (ble_connected) {
                rgb_light_setrgb_blue();
            } else {
                switch (ble_channel) {
                case 0:
                    light_setrgb(0xFF, 0x10, 0x20); //粉色
                    break;
                case 1:
                    light_setrgb(0xFF, 0x60, 0x00); //黄色
                    break;
                case 2:
                    light_setrgb(0xFF, 0x00, 0x00); //红色
                    break;
                default:
                    light_setrgb(0, 0, 0);
                    break;
                }
            }
        }
        power_save_reset();
    }
}

void light_sethsv(uint16_t hue, uint8_t sat, uint8_t val)
{
    uint8_t r = 0, g = 0, b = 0, base, color;

    if (val > RGB_LIGHT_LIMIT_VAL) {
        val = RGB_LIGHT_LIMIT_VAL; // limit the val
    }

    if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
        r = val;
        g = val;
        b = val;
    } else {
        base = ((255 - sat) * val) >> 8;
        color = (val - base) * (hue % 60) / 60;

        switch (hue / 60) {
        case 0:
            r = val;
            g = base + color;
            b = base;
            break;
        case 1:
            r = val - color;
            g = val;
            b = base;
            break;
        case 2:
            r = base;
            g = val;
            b = base + color;
            break;
        case 3:
            r = base;
            g = val - color;
            b = val;
            break;
        case 4:
            r = base + color;
            g = base;
            b = val;
            break;
        case 5:
            r = val;
            g = base;
            b = val - color;
            break;
        }
    }
    r = pgm_read_byte(&LED_TABLE[r]);
    g = pgm_read_byte(&LED_TABLE[g]);
    b = pgm_read_byte(&LED_TABLE[b]);

    light_setrgb(r, g, b);
}

void light_setrgb(uint8_t r, uint8_t g, uint8_t b)
{
    // 可通过灯阻校准颜色，如：R-220R  B-510R  G-330R
    led_val_set(&led_r, r);
    led_val_set(&led_g, g);
    led_val_set(&led_b, b);
}

//注册rgb_light需要的存储区
CONFIG_SECTION(rgb_light, 4);

/**@brief 读取rgb_light存储数据.
 *
 */
uint32_t eeconfig_read_rgb_light(void)
{
    return rgb_light.data[3] | (rgb_light.data[2] << 8)
        | (rgb_light.data[1] << 16) | (rgb_light.data[0] << 24);
}
/**@brief 写入rgb_light存储数据.
 *
 */
void eeconfig_update_rgb_light(uint32_t val)
{
    if ((rgb_light.data[3] | (rgb_light.data[2] << 8)
            | (rgb_light.data[1] << 16) | (rgb_light.data[0] << 24))
        != val) {
        rgb_light.data[3] = (val % 0x100);
        rgb_light.data[2] = (val >> 8);
        rgb_light.data[1] = (val >> 16);
        rgb_light.data[0] = (val >> 24);
        storage_write((1 << STORAGE_CONFIG));
    }
}

void eeconfig_update_rgb_light_default(void)
{
#ifdef RGB_LIGHT_ENABLE
    rgb_light_config.enable = 1;
#else
    rgb_light_config.enable = 0;
#endif
    rgb_light_config.ind = 1;
    rgb_light_config.mode = 1;
    rgb_light_config.hue = 0;
    rgb_light_config.sat = 255;
    rgb_light_config.val = RGB_LIGHT_LIMIT_VAL;
    eeconfig_update_rgb_light(rgb_light_config.raw);
}

void rgb_light_init(void)
{
    rgb_light_config.raw = eeconfig_read_rgb_light();
    if (!rgb_light_config.mode) { //如果rgb_light_config.mode为0，则还未写入初始化数据，写入初始化数据
        eeconfig_update_rgb_light_default();
        rgb_light_config.raw = eeconfig_read_rgb_light();
    }
    rgb_light_lppwm_init();
    inc_flash_timer_init();
#ifdef RGB_LIGHT_ANIMATIONS
        rgb_light_timer_init(); // setup the timer
#endif
    if (rgb_light_config.enable) {
        rgb_light_mode_noeeprom(rgb_light_config.mode);
    }
}

void rgb_light_increase(void)
{
    uint8_t mode = 0;
    if (rgb_light_config.mode < RGB_LIGHT_MODES) {
        mode = rgb_light_config.mode + 1;
    }
    rgb_light_mode(mode);
}
void rgb_light_decrease(void)
{
    uint8_t mode = 0;
    // Mode will never be < 1. If it ever is, eeprom needs to be initialized.
    if (rgb_light_config.mode > 1) {
        mode = rgb_light_config.mode - 1;
    }
    rgb_light_mode(mode);
}
void rgb_light_step(void)
{
    uint8_t mode = 0;
    mode = rgb_light_config.mode + 1;
    if (mode > RGB_LIGHT_MODES) {
        mode = 1;
    }
    rgb_light_mode(mode);
}
void rgb_light_step_reverse(void)
{
    uint8_t mode = 0;
    mode = rgb_light_config.mode - 1;
    if (mode < 1) {
        mode = RGB_LIGHT_MODES;
    }
    rgb_light_mode(mode);
}

uint32_t rgb_light_get_mode(void)
{
    if (!rgb_light_config.enable) {
        return false;
    }

    return rgb_light_config.mode;
}

void rgb_light_mode_eeprom_helper(uint8_t mode, bool write_to_eeprom)
{
    if (!rgb_light_config.enable || rgb_light_config.ind) {
        return;
    }
    if (mode < 1) {
        rgb_light_config.mode = 1;
    } else if (mode > RGB_LIGHT_MODES) {
        rgb_light_config.mode = RGB_LIGHT_MODES;
    } else {
        rgb_light_config.mode = mode;
    }
    if (write_to_eeprom) {
        eeconfig_update_rgb_light(rgb_light_config.raw);
    }
    if (rgb_light_config.mode == 1) {
#ifdef RGB_LIGHT_ANIMATIONS
        rgb_light_timer_disable();
#endif
    } else if (rgb_light_config.mode >= 2 && rgb_light_config.mode <= 8) {
// MODE 2-5, breathing
// MODE 6-8, rainbow mood

#ifdef RGB_LIGHT_ANIMATIONS
        rgb_light_timer_enable();
#endif
    }
    rgb_light_sethsv_noeeprom(rgb_light_config.hue, rgb_light_config.sat, rgb_light_config.val);
}

void rgb_light_mode(uint8_t mode)
{
    rgb_light_mode_eeprom_helper(mode, true);
}

void rgb_light_mode_noeeprom(uint8_t mode)
{
    rgb_light_mode_eeprom_helper(mode, false);
}

void rgb_light_toggle(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    if (rgb_light_config.enable) {
        rgb_light_disable();
    } else {
        rgb_light_enable();
    }
}

//指示灯模式和轴灯模式切换
void rgb_indicator_toggle(void)
{
    rgb_light_config.ind = !rgb_light_config.ind;
    eeconfig_update_rgb_light(rgb_light_config.raw);
    if (rgb_light_config.ind) {  //指示灯模式
#ifdef RGB_LIGHT_ANIMATIONS
      rgb_light_timer_disable();  //指示灯模式不需要动效，关闭动效计时器
#endif
      if (!rgb_working) {
        rgb_light_lppwm_start(); //如果切换前轴灯处于关闭状态（llpwm处于关闭），先启用lppwm
      }
      led_status_change();
    } else {  //轴灯模式
      if (rgb_light_config.enable) {
        if (!rgb_working) {  //如果切换前处于省电模式（llpwm处于关闭），先启用lppwm
          rgb_light_lppwm_start();
        }
        rgb_light_mode_eeprom_helper(rgb_light_config.mode, false);
      } else {
        if (rgb_working) {  //如果llpwm开启才需要关闭
          light_setrgb(0, 0, 0);
          rgb_light_lppwm_stop();
        }
      }
    }
}

void rgb_light_toggle_noeeprom(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    if (rgb_light_config.enable) {
        rgb_light_disable_noeeprom();
    } else {
        rgb_light_enable_noeeprom();
    }
}

void rgb_light_enable(void)
{
    rgb_light_config.enable = 1;
    // No need to update EEPROM here. rgb_light_mode() will do that, actually
    //eeconfig_update_rgb_light(rgb_light_config.raw);
    rgb_light_mode(rgb_light_config.mode);
    rgb_light_lppwm_start();
}

void rgb_light_enable_noeeprom(void)
{
    rgb_light_config.enable = 1;
    rgb_light_mode_noeeprom(rgb_light_config.mode);
    rgb_light_lppwm_start();
}

void rgb_light_disable(void)
{
#ifdef RGB_LIGHT_ANIMATIONS
    rgb_light_timer_disable();
#endif
    light_setrgb(0, 0, 0);
    rgb_light_lppwm_stop();
    rgb_light_config.enable = 0;
    eeconfig_update_rgb_light(rgb_light_config.raw);
}

void rgb_light_disable_noeeprom(void)
{
#ifdef RGB_LIGHT_ANIMATIONS
    rgb_light_timer_disable();
#endif
    light_setrgb(0, 0, 0);
    rgb_light_lppwm_stop();
    rgb_light_config.enable = 0;
}

void rgb_light_increase_hue(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    uint16_t hue;
    hue = (rgb_light_config.hue + RGB_LIGHT_HUE_STEP) % 360;
    rgb_light_sethsv(hue, rgb_light_config.sat, rgb_light_config.val);
}
void rgb_light_decrease_hue(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    uint16_t hue;
    if (rgb_light_config.hue - RGB_LIGHT_HUE_STEP < 0) {
        hue = (rgb_light_config.hue + 360 - RGB_LIGHT_HUE_STEP) % 360;
    } else {
        hue = (rgb_light_config.hue - RGB_LIGHT_HUE_STEP) % 360;
    }
    rgb_light_sethsv(hue, rgb_light_config.sat, rgb_light_config.val);
}
void rgb_light_increase_sat(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    uint8_t sat;
    if (rgb_light_config.sat + RGB_LIGHT_SAT_STEP > 255) {
        sat = 255;
    } else {
        sat = rgb_light_config.sat + RGB_LIGHT_SAT_STEP;
    }
    rgb_light_sethsv(rgb_light_config.hue, sat, rgb_light_config.val);
}
void rgb_light_decrease_sat(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    uint8_t sat;
    if (rgb_light_config.sat - RGB_LIGHT_SAT_STEP < 0) {
        sat = 0;
    } else {
        sat = rgb_light_config.sat - RGB_LIGHT_SAT_STEP;
    }
    rgb_light_sethsv(rgb_light_config.hue, sat, rgb_light_config.val);
}
void rgb_light_increase_val(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    uint8_t val;
    if (rgb_light_config.val + RGB_LIGHT_VAL_STEP > RGB_LIGHT_LIMIT_VAL) {
        val = RGB_LIGHT_LIMIT_VAL;
    } else {
        val = rgb_light_config.val + RGB_LIGHT_VAL_STEP;
    }
    rgb_light_sethsv(rgb_light_config.hue, rgb_light_config.sat, val);
}
void rgb_light_decrease_val(void)
{
    if (rgb_light_config.ind) {
        return;
    }
    uint8_t val;
    if (rgb_light_config.val - RGB_LIGHT_VAL_STEP < 0) {
        val = 0;
    } else {
        val = rgb_light_config.val - RGB_LIGHT_VAL_STEP;
    }
    rgb_light_sethsv(rgb_light_config.hue, rgb_light_config.sat, val);
}

void rgb_light_sethsv_noeeprom_old(uint16_t hue, uint8_t sat, uint8_t val)
{
    if (rgb_light_config.enable && !rgb_light_config.ind) {
        light_sethsv(hue, sat, val);
    }
}

void rgb_light_sethsv_eeprom_helper(uint16_t hue, uint8_t sat, uint8_t val, bool write_to_eeprom)
{
    if (rgb_light_config.enable && !rgb_light_config.ind) {
        if (rgb_light_config.mode == 1) {
            // same static color
            light_sethsv(hue, sat, val);
        } else {
            // all LEDs in same color
            if (rgb_light_config.mode >= 2 && rgb_light_config.mode <= 5) {
                // breathing mode, ignore the change of val, use in memory value instead
                val = rgb_light_config.val;
            } else if (rgb_light_config.mode >= 6 && rgb_light_config.mode <= 8) {
                // rainbow mood and rainbow swirl, ignore the change of hue
                hue = rgb_light_config.hue;
            }
        }
        rgb_light_config.hue = hue;
        rgb_light_config.sat = sat;
        rgb_light_config.val = val;
        if (write_to_eeprom) {
            eeconfig_update_rgb_light(rgb_light_config.raw);
        }
    }
}

void rgb_light_sethsv(uint16_t hue, uint8_t sat, uint8_t val)
{
    rgb_light_sethsv_eeprom_helper(hue, sat, val, true);
}

void rgb_light_sethsv_noeeprom(uint16_t hue, uint8_t sat, uint8_t val)
{
    rgb_light_sethsv_eeprom_helper(hue, sat, val, false);
}

uint16_t rgb_light_get_hue(void)
{
    return rgb_light_config.hue;
}

uint8_t rgb_light_get_sat(void)
{
    return rgb_light_config.sat;
}

uint8_t rgb_light_get_val(void)
{
    return rgb_light_config.val;
}


#ifdef RGB_LIGHT_ANIMATIONS

APP_TIMER_DEF(rgb_light_timer);

static void rgb_light_animations_handler(void* p_context)
{
    rgb_light_task();
}

// Animation timer -- AVR Timer3
void rgb_light_timer_init(void)
{
    app_timer_create(&rgb_light_timer, APP_TIMER_MODE_REPEATED, rgb_light_animations_handler);
}
void rgb_light_timer_enable(void)
{
    app_timer_start(rgb_light_timer, APP_TIMER_TICKS(16), NULL);
    rgb_light_timer_enabled = true;
}
void rgb_light_timer_disable(void)
{
    app_timer_stop(rgb_light_timer);
    rgb_light_timer_enabled = false;
}
void rgb_light_timer_toggle(void)
{
    rgb_light_timer_enabled ^= rgb_light_timer_enabled;
}

void rgb_light_task(void)
{
    if (rgb_light_timer_enabled && !rgb_light_config.ind) {
        // mode = 1, static light, do nothing here
        if (rgb_light_config.mode >= 2 && rgb_light_config.mode <= 5) {
            // mode = 2 to 5, breathing mode
            rgb_light_effect_breathing(rgb_light_config.mode - 2);
        } else if (rgb_light_config.mode >= 6 && rgb_light_config.mode <= 8) {
            // mode = 6 to 8, rainbow mood mod
            rgb_light_effect_rainbow_mood(rgb_light_config.mode - 6);
        }
    }
}

// Effects
void rgb_light_effect_breathing(uint8_t interval)
{
    static uint8_t pos = 0;
    static uint16_t last_timer = 0;
    float val;

    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_BREATHING_INTERVALS[interval])) {
        return;
    }
    last_timer = timer_read();

    // http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    val = (exp(sin((pos / 255.0) * M_PI)) - RGB_LIGHT_EFFECT_BREATHE_CENTER / M_E) * (RGB_LIGHT_EFFECT_BREATHE_MAX / (M_E - 1 / M_E));
    rgb_light_sethsv_noeeprom_old(rgb_light_config.hue, rgb_light_config.sat, val);
    pos = (pos + 1) % 256;
}
void rgb_light_effect_rainbow_mood(uint8_t interval)
{
    static uint16_t current_hue = 0;
    static uint16_t last_timer = 0;

    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_RAINBOW_MOOD_INTERVALS[interval])) {
        return;
    }
    last_timer = timer_read();
    rgb_light_sethsv_noeeprom_old(current_hue, rgb_light_config.sat, rgb_light_config.val);
    current_hue = (current_hue + 1) % 360;
}

#endif /* RGB_LIGHT_ANIMATIONS */

static void status_rgb_light_evt_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            rgb_light_init();
            if (!rgb_light_config.ind && !rgb_light_config.enable) {
                rgb_light_lppwm_start(); 
                light_setrgb(0xFF, 0xFF, 0xFF); // 闪烁白色灯一次
                app_timer_start(inc_flash_timer, APP_TIMER_TICKS(50), (void*)(uint32_t)INIT_LED); //延迟50ms关闭RGB
            }
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            if (rgb_light_config.ind) {
                if (!rgb_working) {
                    rgb_light_lppwm_start(); //如果进入了省电状态（llpwm处于关闭），先启用lppwm
                }
                light_setrgb(0xFF, 0xFF, 0xFF); // 指示灯模式休眠时闪烁白色灯一次
                app_timer_start(inc_flash_timer, APP_TIMER_TICKS(50), (void*)(uint32_t)SLEEP_LED); //延迟50ms关闭RGB
            } else if (!rgb_light_config.enable) {
                rgb_light_lppwm_start(); 
                light_setrgb(0xFF, 0xFF, 0xFF);
                app_timer_start(inc_flash_timer, APP_TIMER_TICKS(50), (void*)(uint32_t)SLEEP_LED);

            } else {
                rgb_light_lppwm_deinit();
            }
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE:
        switch (arg2) {
        case PWR_SAVE_ENTER: // 进入省电模式
            if (rgb_light_config.ind) {
                light_setrgb(0, 0, 0);
                rgb_light_lppwm_stop();
            }
            break;
        case PWR_SAVE_EXIT: // 退出省电模式
            if (rgb_light_config.ind) {
                rgb_light_lppwm_start();
            }
            break;
        default:
            break;
        }
        break;
    case USER_EVT_USB: // USB事件
        switch (arg2) {
        case USB_WORKING:
            usb_working = true;
            led_status_change();
            break;
        default:
            usb_working = false;
            led_status_change();
            break;
        }
        break;
    case USER_EVT_BLE_DEVICE_SWITCH: // 蓝牙设备通道切换事件
        switch (arg2) {
        case BLE_DEVICE_CHANNEL0:
            ble_channel = 0;
            led_status_change();
            break;
        case BLE_DEVICE_CHANNEL1:
            ble_channel = 1;
            led_status_change();
            break;
        case BLE_DEVICE_CHANNEL2:
            ble_channel = 2;
            led_status_change();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙状态事件
        switch (arg2) {
        case BLE_STATE_CONNECTED:
            ble_connected = true;
            led_status_change();
            break;
        case BLE_STATE_DISCONNECT:
            ble_connected = false;
            led_status_change();
            break;
        case BLE_STATE_FAST_ADV:
            ble_connected = false;
            led_status_change();
        break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(status_rgb_light_evt_handler);