/* Copyright 2016-2017 Yang Liu     modify by Geno
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
#include "rgblight.h"
#include "app_timer.h"
#include "config.h"
#include "data_storage.h"
#include "keyboard_evt.h"
#include "led_tables.h"
#include "nrf_gpio.h"
#include "progmem.h"
#include "timer.h"
#include "wait.h"
#include <math.h>

#include <ble_keyboard.h>

#ifndef WS2812_LIMIT_VAL
#define WS2812_LIMIT_VAL 255
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
const uint8_t RGBLED_RAINBOW_SWIRL_INTERVALS[]
    = { 100, 50, 20 };
__attribute__((weak))
const uint8_t RGBLED_SNAKE_INTERVALS[]
    = { 100, 50, 20 };
__attribute__((weak))
const uint8_t RGBLED_KNIGHT_INTERVALS[]
    = { 127, 63, 31 };
__attribute__((weak))
const uint16_t RGBLED_GRADIENT_RANGES[]
    = { 360, 240, 180, 120, 90 };
__attribute__((weak))
const uint16_t RGBLED_RGBTEST_INTERVALS[]
    = { 1024 };

ws2812_config_t ws2812_config;

LED_TYPE led[WS2812_NUM];
bool ws2812_timer_enabled = false;

void base_sethsv(uint16_t hue, uint8_t sat, uint8_t val, LED_TYPE* led1)
{
    uint8_t r = 0, g = 0, b = 0, base, color;

    if (val > WS2812_LIMIT_VAL) {
        val = WS2812_LIMIT_VAL; // limit the val
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
    r = pgm_read_byte(&CIE1931_CURVE[r]);
    g = pgm_read_byte(&CIE1931_CURVE[g]);
    b = pgm_read_byte(&CIE1931_CURVE[b]);

    base_setrgb(r, g, b, led1);
}

void base_setrgb(uint8_t r, uint8_t g, uint8_t b, LED_TYPE* led1)
{
    (*led1).r = r;
    (*led1).g = g;
    (*led1).b = b;
}

//注册switch需要的存储区
CONFIG_SECTION(ws2812_rgb, 4);

/**@brief 读取ws2812存储数据.
 *
 */
uint32_t eeconfig_read_rgblight(void)
{
    return ws2812_rgb.data[3] | (ws2812_rgb.data[2] << 8)
        | (ws2812_rgb.data[1] << 16) | (ws2812_rgb.data[0] << 24);
}
/**@brief 写入ws2812存储数据.
 *
 */
void eeconfig_update_rgblight(uint32_t val)
{
    if ((ws2812_rgb.data[3] | (ws2812_rgb.data[2] << 8)
            | (ws2812_rgb.data[1] << 16) | (ws2812_rgb.data[0] << 24))
        != val) {
        ws2812_rgb.data[3] = (val % 0x100);
        ws2812_rgb.data[2] = (val >> 8);
        ws2812_rgb.data[1] = (val >> 16);
        ws2812_rgb.data[0] = (val >> 24);
        storage_write((1 << STORAGE_CONFIG));
    }
}

void eeconfig_update_ws2812_default(void)
{
#ifdef WS2812_ENABLE
    ws2812_config.enable = 1;
#else
    ws2812_config.enable = 0;
#endif
    ws2812_config.mode = 1;
    ws2812_config.hue = 0;
    ws2812_config.sat = 255;
    ws2812_config.val = WS2812_LIMIT_VAL;
    eeconfig_update_rgblight(ws2812_config.raw);
}

void eeconfig_debug_rgblight(void)
{
}

//开启RGB电源
void rgb_pwr_on(void)
{
#ifdef WS2812_PWR_PIN
    nrf_gpio_pin_write(WS2812_PWR_PIN, 0);
#endif
#ifdef WS2812_PWR_PIN_REVERSE
    nrf_gpio_pin_write(WS2812_PWR_PIN_REVERSE, 1);
#endif
}

//关闭RGB电源
void rgb_pwr_off(void)
{
#ifdef WS2812_PWR_PIN
    nrf_gpio_pin_write(WS2812_PWR_PIN, 1);
#endif
#ifdef WS2812_PWR_PIN_REVERSE
    nrf_gpio_pin_write(WS2812_PWR_PIN_REVERSE, 0);
#endif
}

void ws2812_init(void)
{
    ws2812_config.raw = eeconfig_read_rgblight();
    wait_ms(2);
    if (!ws2812_config.mode) {  //如果ws2812_config.mode为0，则还未写入初始化数据，写入初始化数据
        eeconfig_update_ws2812_default();
        ws2812_config.raw = eeconfig_read_rgblight();
    }

#ifdef WS2812_ANIMATIONS
    ws2812_timer_init(); // setup the timer
#endif
//初始化RGB power控制脚
#ifdef WS2812_PWR_PIN
    nrf_gpio_cfg_output(WS2812_PWR_PIN);
#endif
#ifdef WS2812_PWR_PIN_REVERSE
    nrf_gpio_cfg_output(WS2812_PWR_PIN_REVERSE);
#endif
    if (ws2812_config.enable) {
        ws2812_mode_noeeprom(ws2812_config.mode);
        rgb_pwr_on();
    } else {
        rgb_pwr_off();
    }
}

void ws2812_update_dword(uint32_t dword)
{
    ws2812_config.raw = dword;
    eeconfig_update_rgblight(ws2812_config.raw);
    if (ws2812_config.enable)
        ws2812_mode(ws2812_config.mode);
    else {
#ifdef WS2812_ANIMATIONS
        ws2812_timer_disable();
#endif
        ws2812_set();
    }
}

void ws2812_increase(void)
{
    uint8_t mode = 0;
    if (ws2812_config.mode < WS2812_MODES) {
        mode = ws2812_config.mode + 1;
    }
    ws2812_mode(mode);
}
void ws2812_decrease(void)
{
    uint8_t mode = 0;
    // Mode will never be < 1. If it ever is, eeprom needs to be initialized.
    if (ws2812_config.mode > 1) {
        mode = ws2812_config.mode - 1;
    }
    ws2812_mode(mode);
}
void ws2812_step(void)
{
    uint8_t mode = 0;
    mode = ws2812_config.mode + 1;
    if (mode > WS2812_MODES) {
        mode = 1;
    }
    ws2812_mode(mode);
}
void ws2812_step_reverse(void)
{
    uint8_t mode = 0;
    mode = ws2812_config.mode - 1;
    if (mode < 1) {
        mode = WS2812_MODES;
    }
    ws2812_mode(mode);
}

uint32_t ws2812_get_mode(void)
{
    if (!ws2812_config.enable) {
        return false;
    }

    return ws2812_config.mode;
}

void ws2812_mode_eeprom_helper(uint8_t mode, bool write_to_eeprom)
{
    if (!ws2812_config.enable) {
        return;
    }
    if (mode < 1) {
        ws2812_config.mode = 1;
    } else if (mode > WS2812_MODES) {
        ws2812_config.mode = WS2812_MODES;
    } else {
        ws2812_config.mode = mode;
    }
    if (write_to_eeprom) {
        eeconfig_update_rgblight(ws2812_config.raw);
    } else {
    }
    if (ws2812_config.mode == 1) {
#ifdef WS2812_ANIMATIONS
        ws2812_timer_disable();
#endif
    } else if ((ws2812_config.mode >= 2 && ws2812_config.mode <= 24) || ws2812_config.mode == 35 || ws2812_config.mode == 36) {
// MODE 2-5, breathing
// MODE 6-8, rainbow mood
// MODE 9-14, rainbow swirl
// MODE 15-20, snake
// MODE 21-23, knight
// MODE 24, xmas
// MODE 35  RGB test
// MODE 36, alterating

#ifdef WS2812_ANIMATIONS
        ws2812_timer_enable();
#endif
    } else if (ws2812_config.mode >= 25 && ws2812_config.mode <= 34) {
// MODE 25-34, static gradient

#ifdef WS2812_ANIMATIONS
        ws2812_timer_disable();
#endif
    }
    ws2812_sethsv_noeeprom(ws2812_config.hue, ws2812_config.sat, ws2812_config.val);
}

void ws2812_mode(uint8_t mode)
{
    ws2812_mode_eeprom_helper(mode, true);
}

void ws2812_mode_noeeprom(uint8_t mode)
{
    ws2812_mode_eeprom_helper(mode, false);
}

void ws2812_toggle(void)
{
    if (ws2812_config.enable) {
        ws2812_disable();
    } else {
        ws2812_enable();
    }
}

void ws2812_toggle_noeeprom(void)
{
    if (ws2812_config.enable) {
        ws2812_disable_noeeprom();
    } else {
        ws2812_enable_noeeprom();
    }
}

void ws2812_enable(void)
{
    rgb_pwr_on();
    ws2812_config.enable = 1;
    // No need to update EEPROM here. ws2812_mode() will do that, actually
    //eeconfig_update_rgblight(ws2812_config.raw);
    ws2812_mode(ws2812_config.mode);
}

void ws2812_enable_noeeprom(void)
{
    rgb_pwr_on();
    ws2812_config.enable = 1;
    ws2812_mode_noeeprom(ws2812_config.mode);
}

void ws2812_set_val(uint8_t value)
{
    LED_TYPE tmp_led;
    base_sethsv(ws2812_config.hue, ws2812_config.sat, value, &tmp_led);
    ws2812_setrgb(tmp_led.r, tmp_led.g, tmp_led.b);
}

void ws2812_disable(void)
{
#ifdef WS2812_ANIMATIONS
    ws2812_timer_disable();
#endif
#if !defined(WS2812_PWR_PIN) && !defined(WS2812_PWR_PIN_REVERSE) //如果无电源控制则关机前逐步降低亮度
    ws2812_set_val(120);
    wait_ms(1);
    ws2812_set_val(60);
    wait_ms(1);
#endif
    ws2812_config.enable = 0;
    eeconfig_update_rgblight(ws2812_config.raw);
    ws2812_set();
    wait_ms(2);
    rgb_pwr_off();
}

void ws2812_disable_noeeprom(void)
{
#ifdef WS2812_ANIMATIONS
    ws2812_timer_disable();
#endif
#if !defined(WS2812_PWR_PIN) && !defined(WS2812_PWR_PIN_REVERSE) //如果无电源控制则关机前逐步降低亮度
    ws2812_set_val(120);
    wait_ms(1);
    ws2812_set_val(60);
    wait_ms(1);
#endif
    ws2812_config.enable = 0;
    ws2812_set();
    wait_ms(2);
    rgb_pwr_off();
}

// Deals with the messy details of incrementing an integer
uint8_t increment(uint8_t value, uint8_t step, uint8_t min, uint8_t max)
{
    int16_t new_value = value;
    new_value += step;
    return MIN(MAX(new_value, min), max);
}

uint8_t decrement(uint8_t value, uint8_t step, uint8_t min, uint8_t max)
{
    int16_t new_value = value;
    new_value -= step;
    return MIN(MAX(new_value, min), max);
}

void ws2812_increase_hue(void)
{
    uint16_t hue;
    hue = (ws2812_config.hue + WS2812_HUE_STEP) % 360;
    ws2812_sethsv(hue, ws2812_config.sat, ws2812_config.val);
}
void ws2812_decrease_hue(void)
{
    uint16_t hue;
    if (ws2812_config.hue - WS2812_HUE_STEP < 0) {
        hue = (ws2812_config.hue + 360 - WS2812_HUE_STEP) % 360;
    } else {
        hue = (ws2812_config.hue - WS2812_HUE_STEP) % 360;
    }
    ws2812_sethsv(hue, ws2812_config.sat, ws2812_config.val);
}
void ws2812_increase_sat(void)
{
    uint8_t sat;
    if (ws2812_config.sat + WS2812_SAT_STEP > 255) {
        sat = 255;
    } else {
        sat = ws2812_config.sat + WS2812_SAT_STEP;
    }
    ws2812_sethsv(ws2812_config.hue, sat, ws2812_config.val);
}
void ws2812_decrease_sat(void)
{
    uint8_t sat;
    if (ws2812_config.sat - WS2812_SAT_STEP < 0) {
        sat = 0;
    } else {
        sat = ws2812_config.sat - WS2812_SAT_STEP;
    }
    ws2812_sethsv(ws2812_config.hue, sat, ws2812_config.val);
}
void ws2812_increase_val(void)
{
    uint8_t val;
    if (ws2812_config.val + WS2812_VAL_STEP > WS2812_LIMIT_VAL) {
        val = WS2812_LIMIT_VAL;
    } else {
        val = ws2812_config.val + WS2812_VAL_STEP;
    }
    ws2812_sethsv(ws2812_config.hue, ws2812_config.sat, val);
}
void ws2812_decrease_val(void)
{
    uint8_t val;
    if (ws2812_config.val - WS2812_VAL_STEP < 0) {
        val = 0;
    } else {
        val = ws2812_config.val - WS2812_VAL_STEP;
    }
    ws2812_sethsv(ws2812_config.hue, ws2812_config.sat, val);
}

void ws2812_sethsv_noeeprom_old(uint16_t hue, uint8_t sat, uint8_t val)
{
    if (ws2812_config.enable) {
        LED_TYPE tmp_led;
        base_sethsv(hue, sat, val, &tmp_led);
        ws2812_setrgb(tmp_led.r, tmp_led.g, tmp_led.b);
    }
}

void ws2812_sethsv_eeprom_helper(uint16_t hue, uint8_t sat, uint8_t val, bool write_to_eeprom)
{
    if (ws2812_config.enable) {
        if (ws2812_config.mode == 1) {
            // same static color
            LED_TYPE tmp_led;
            base_sethsv(hue, sat, val, &tmp_led);
            ws2812_setrgb(tmp_led.r, tmp_led.g, tmp_led.b);
        } else {
            // all LEDs in same color
            if (ws2812_config.mode >= 2 && ws2812_config.mode <= 5) {
                // breathing mode, ignore the change of val, use in memory value instead
                val = ws2812_config.val;
            } else if (ws2812_config.mode >= 6 && ws2812_config.mode <= 14) {
                // rainbow mood and rainbow swirl, ignore the change of hue
                hue = ws2812_config.hue;
            } else if (ws2812_config.mode >= 25 && ws2812_config.mode <= 34) {
                // static gradient
                uint16_t _hue;
                int8_t direction = ((ws2812_config.mode - 25) % 2) ? -1 : 1;
                uint16_t range = pgm_read_word(&RGBLED_GRADIENT_RANGES[(ws2812_config.mode - 25) / 2]);
                for (uint8_t i = 0; i < WS2812_NUM; i++) {
                    _hue = (range / WS2812_NUM * i * direction + hue + 360) % 360;
                    base_sethsv(_hue, sat, val, (LED_TYPE*)&led[i]);
                }
                ws2812_set();
            }
        }
        ws2812_config.hue = hue;
        ws2812_config.sat = sat;
        ws2812_config.val = val;
        if (write_to_eeprom) {
            eeconfig_update_rgblight(ws2812_config.raw);
        } else {
        }
    }
}

void ws2812_sethsv(uint16_t hue, uint8_t sat, uint8_t val)
{
    ws2812_sethsv_eeprom_helper(hue, sat, val, true);
}

void ws2812_sethsv_noeeprom(uint16_t hue, uint8_t sat, uint8_t val)
{
    ws2812_sethsv_eeprom_helper(hue, sat, val, false);
}

uint16_t ws2812_get_hue(void)
{
    return ws2812_config.hue;
}

uint8_t ws2812_get_sat(void)
{
    return ws2812_config.sat;
}

uint8_t ws2812_get_val(void)
{
    return ws2812_config.val;
}

void ws2812_setrgb(uint8_t r, uint8_t g, uint8_t b)
{
    if (!ws2812_config.enable) {
        return;
    }

    for (uint8_t i = 0; i < WS2812_NUM; i++) {
        led[i].r = r;
        led[i].g = g;
        led[i].b = b;
    }
    ws2812_set();
}

void ws2812_setrgb_at(uint8_t r, uint8_t g, uint8_t b, uint8_t index)
{
    if (!ws2812_config.enable || index >= WS2812_NUM) {
        return;
    }

    led[index].r = r;
    led[index].g = g;
    led[index].b = b;
    ws2812_set();
}

void ws2812_sethsv_at(uint16_t hue, uint8_t sat, uint8_t val, uint8_t index)
{
    if (!ws2812_config.enable) {
        return;
    }

    LED_TYPE tmp_led;
    base_sethsv(hue, sat, val, &tmp_led);
    ws2812_setrgb_at(tmp_led.r, tmp_led.g, tmp_led.b, index);
}

#ifndef WS2812_CUSTOM_DRIVER
void ws2812_set(void)
{
    if (ws2812_config.enable) {
#ifdef RGBW
        ws2812_setleds_rgbw(led, WS2812_NUM);
#else
        ws2812_setleds(led, WS2812_NUM);
#endif
    } else {
        for (uint8_t i = 0; i < WS2812_NUM; i++) {
            led[i].r = 0;
            led[i].g = 0;
            led[i].b = 0;
        }
#ifdef RGBW
        ws2812_setleds_rgbw(led, WS2812_NUM);
#else
        ws2812_setleds(led, WS2812_NUM);
#endif
    }
}
#endif

//休眠关机时调用关闭RGB
void ws2812_sleep_prepare(void)
{
    // 禁用RGB LED
    ws2812_disable_noeeprom();
    wait_ms(1);
    nrf_gpio_cfg_default(WS2812_DI_PIN);
#ifdef WS2812_PWR_PIN
    nrf_gpio_cfg_default(WS2812_PWR_PIN);
#endif
#ifdef WS2812_PWR_PIN_REVERSE
    nrf_gpio_cfg_default(WS2812_PWR_PIN_REVERSE);
#endif
}

#ifdef WS2812_ANIMATIONS

APP_TIMER_DEF(ws2812_timer);

static void WS2812_ANIMATIONS_handler(void* p_context)
{
    ws2812_task();
}

// Animation timer -- AVR Timer3
void ws2812_timer_init(void)
{
    app_timer_create(&ws2812_timer, APP_TIMER_MODE_REPEATED, WS2812_ANIMATIONS_handler);
}
void ws2812_timer_enable(void)
{
    app_timer_start(ws2812_timer, APP_TIMER_TICKS(16), NULL);
    ws2812_timer_enabled = true;
}
void ws2812_timer_disable(void)
{
    app_timer_stop(ws2812_timer);
    ws2812_timer_enabled = false;
}
void ws2812_timer_toggle(void)
{
    ws2812_timer_enabled ^= ws2812_timer_enabled;
}

void ws2812_show_solid_color(uint8_t r, uint8_t g, uint8_t b)
{
    ws2812_enable();
    ws2812_mode(1);
    ws2812_setrgb(r, g, b);
}

void ws2812_task(void)
{
    if (ws2812_timer_enabled) {
        // mode = 1, static light, do nothing here
        if (ws2812_config.mode >= 2 && ws2812_config.mode <= 5) {
            // mode = 2 to 5, breathing mode
            ws2812_effect_breathing(ws2812_config.mode - 2);
        } else if (ws2812_config.mode >= 6 && ws2812_config.mode <= 8) {
            // mode = 6 to 8, rainbow mood mod
            ws2812_effect_rainbow_mood(ws2812_config.mode - 6);
        } else if (ws2812_config.mode >= 9 && ws2812_config.mode <= 14) {
            // mode = 9 to 14, rainbow swirl mode
            ws2812_effect_rainbow_swirl(ws2812_config.mode - 9);
        } else if (ws2812_config.mode >= 15 && ws2812_config.mode <= 20) {
            // mode = 15 to 20, snake mode
            ws2812_effect_snake(ws2812_config.mode - 15);
        } else if (ws2812_config.mode >= 21 && ws2812_config.mode <= 23) {
            // mode = 21 to 23, knight mode
            ws2812_effect_knight(ws2812_config.mode - 21);
        } else if (ws2812_config.mode == 24) {
            // mode = 24, christmas mode
            ws2812_effect_christmas();
        } else if (ws2812_config.mode == 35) {
            // mode = 35, RGB test
            ws2812_effect_rgbtest();
        } else if (ws2812_config.mode == 36) {
            ws2812_effect_alternating();
        }
    }
}

// Effects
void ws2812_effect_breathing(uint8_t interval)
{
    static uint8_t pos = 0;
    static uint16_t last_timer = 0;
    float val;

    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_BREATHING_INTERVALS[interval])) {
        return;
    }
    last_timer = timer_read();

    // http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
    val = (exp(sin((pos / 255.0) * M_PI)) - WS2812_EFFECT_BREATHE_CENTER / M_E) * (WS2812_EFFECT_BREATHE_MAX / (M_E - 1 / M_E));
    ws2812_sethsv_noeeprom_old(ws2812_config.hue, ws2812_config.sat, val);
    pos = (pos + 1) % 256;
}
void ws2812_effect_rainbow_mood(uint8_t interval)
{
    static uint16_t current_hue = 0;
    static uint16_t last_timer = 0;

    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_RAINBOW_MOOD_INTERVALS[interval])) {
        return;
    }
    last_timer = timer_read();
    ws2812_sethsv_noeeprom_old(current_hue, ws2812_config.sat, ws2812_config.val);
    current_hue = (current_hue + 1) % 360;
}
void ws2812_effect_rainbow_swirl(uint8_t interval)
{
    static uint16_t current_hue = 0;
    static uint16_t last_timer = 0;
    uint16_t hue;
    uint8_t i;
    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_RAINBOW_SWIRL_INTERVALS[interval / 2])) {
        return;
    }
    last_timer = timer_read();
    for (i = 0; i < WS2812_NUM; i++) {
        hue = (360 / WS2812_NUM * i + current_hue) % 360;
        base_sethsv(hue, ws2812_config.sat, ws2812_config.val, (LED_TYPE*)&led[i]);
    }
    ws2812_set();

    if (interval % 2) {
        current_hue = (current_hue + 1) % 360;
    } else {
        if (current_hue - 1 < 0) {
            current_hue = 359;
        } else {
            current_hue = current_hue - 1;
        }
    }
}
void ws2812_effect_snake(uint8_t interval)
{
    static uint8_t pos = 0;
    static uint16_t last_timer = 0;
    uint8_t i, j;
    int8_t k;
    int8_t increment = 1;
    if (interval % 2) {
        increment = -1;
    }
    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_SNAKE_INTERVALS[interval / 2])) {
        return;
    }
    last_timer = timer_read();
    for (i = 0; i < WS2812_NUM; i++) {
        led[i].r = 0;
        led[i].g = 0;
        led[i].b = 0;
        for (j = 0; j < WS2812_EFFECT_SNAKE_LENGTH; j++) {
            k = pos + j * increment;
            if (k < 0) {
                k = k + WS2812_NUM;
            }
            if (i == k) {
                base_sethsv(ws2812_config.hue, ws2812_config.sat, (uint8_t)(ws2812_config.val * (WS2812_EFFECT_SNAKE_LENGTH - j) / WS2812_EFFECT_SNAKE_LENGTH), (LED_TYPE*)&led[i]);
            }
        }
    }
    ws2812_set();
    if (increment == 1) {
        if (pos - 1 < 0) {
            pos = WS2812_NUM - 1;
        } else {
            pos -= 1;
        }
    } else {
        pos = (pos + 1) % WS2812_NUM;
    }
}
void ws2812_effect_knight(uint8_t interval)
{
    static uint16_t last_timer = 0;
    if (timer_elapsed(last_timer) < pgm_read_byte(&RGBLED_KNIGHT_INTERVALS[interval])) {
        return;
    }
    last_timer = timer_read();

    static int8_t low_bound = 0;
    static int8_t high_bound = WS2812_EFFECT_KNIGHT_LENGTH - 1;
    static int8_t increment = 1;
    uint8_t i, cur;

    // Set all the LEDs to 0
    for (i = 0; i < WS2812_NUM; i++) {
        led[i].r = 0;
        led[i].g = 0;
        led[i].b = 0;
    }
    // Determine which LEDs should be lit up
    for (i = 0; i < WS2812_EFFECT_KNIGHT_LED_NUM; i++) {
        cur = (i + WS2812_EFFECT_KNIGHT_OFFSET) % WS2812_NUM;

        if (i >= low_bound && i <= high_bound) {
            base_sethsv(ws2812_config.hue, ws2812_config.sat, ws2812_config.val, (LED_TYPE*)&led[cur]);
        } else {
            led[cur].r = 0;
            led[cur].g = 0;
            led[cur].b = 0;
        }
    }
    ws2812_set();

    // Move from low_bound to high_bound changing the direction we increment each
    // time a boundary is hit.
    low_bound += increment;
    high_bound += increment;

    if (high_bound <= 0 || low_bound >= WS2812_EFFECT_KNIGHT_LED_NUM - 1) {
        increment = -increment;
    }
}

void ws2812_effect_christmas(void)
{
    static uint16_t current_offset = 0;
    static uint16_t last_timer = 0;
    uint16_t hue;
    uint8_t i;
    if (timer_elapsed(last_timer) < WS2812_EFFECT_CHRISTMAS_INTERVAL) {
        return;
    }
    last_timer = timer_read();
    current_offset = (current_offset + 1) % 2;
    for (i = 0; i < WS2812_NUM; i++) {
        hue = 0 + ((i / WS2812_EFFECT_CHRISTMAS_STEP + current_offset) % 2) * 120;
        base_sethsv(hue, ws2812_config.sat, ws2812_config.val, (LED_TYPE*)&led[i]);
    }
    ws2812_set();
}

void ws2812_effect_rgbtest(void)
{
    static uint8_t pos = 0;
    static uint16_t last_timer = 0;
    static uint8_t maxval = 0;
    uint8_t g;
    uint8_t r;
    uint8_t b;

    if (timer_elapsed(last_timer) < pgm_read_word(&RGBLED_RGBTEST_INTERVALS[0])) {
        return;
    }

    if (maxval == 0) {
        LED_TYPE tmp_led;
        base_sethsv(0, 255, WS2812_LIMIT_VAL, &tmp_led);
        maxval = tmp_led.r;
    }
    last_timer = timer_read();
    g = r = b = 0;
    switch (pos) {
    case 0:
        r = maxval;
        break;
    case 1:
        g = maxval;
        break;
    case 2:
        b = maxval;
        break;
    }
    ws2812_setrgb(r, g, b);
    pos = (pos + 1) % 3;
}

void ws2812_effect_alternating(void)
{
    static uint16_t last_timer = 0;
    static uint16_t pos = 0;
    if (timer_elapsed(last_timer) < 500) {
        return;
    }
    last_timer = timer_read();

    for (int i = 0; i < WS2812_NUM; i++) {
        if (i < WS2812_NUM / 2 && pos) {
            ws2812_sethsv_at(ws2812_config.hue, ws2812_config.sat, ws2812_config.val, i);
        } else if (i >= WS2812_NUM / 2 && !pos) {
            ws2812_sethsv_at(ws2812_config.hue, ws2812_config.sat, ws2812_config.val, i);
        } else {
            ws2812_sethsv_at(ws2812_config.hue, ws2812_config.sat, 0, i);
        }
    }
    ws2812_set();
    pos = (pos + 1) % 2;
}

#endif /* WS2812_ANIMATIONS */

static void status_ws2812_evt_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            ws2812_init();
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            ws2812_sleep_prepare();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(status_ws2812_evt_handler);