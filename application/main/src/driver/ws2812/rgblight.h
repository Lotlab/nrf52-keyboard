/* Copyright 2017 Yang Liu
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
#ifndef WS2812_H
#define WS2812_H

#ifdef CONFIG_H_FILE
    #include CONFIG_H_FILE
#endif

#ifdef WS2812_ANIMATIONS
	#define WS2812_MODES 36
#else
	#define WS2812_MODES 1
#endif

#ifndef WS2812_EFFECT_BREATHE_CENTER
#define WS2812_EFFECT_BREATHE_CENTER 1.85  // 1-2.7
#endif

#ifndef WS2812_EFFECT_BREATHE_MAX
#define WS2812_EFFECT_BREATHE_MAX 255   // 0-255
#endif

#ifndef WS2812_EFFECT_SNAKE_LENGTH
#define WS2812_EFFECT_SNAKE_LENGTH 4
#endif

#ifndef WS2812_EFFECT_KNIGHT_LENGTH
#define WS2812_EFFECT_KNIGHT_LENGTH 3
#endif

#ifndef WS2812_EFFECT_KNIGHT_OFFSET
#define WS2812_EFFECT_KNIGHT_OFFSET 0
#endif

#ifndef WS2812_EFFECT_KNIGHT_LED_NUM
#define WS2812_EFFECT_KNIGHT_LED_NUM WS2812_NUM
#endif

#ifndef WS2812_EFFECT_CHRISTMAS_INTERVAL
#define WS2812_EFFECT_CHRISTMAS_INTERVAL 1000
#endif

#ifndef WS2812_EFFECT_CHRISTMAS_STEP
#define WS2812_EFFECT_CHRISTMAS_STEP 2
#endif

#ifndef WS2812_HUE_STEP
#define WS2812_HUE_STEP 6
#endif
#ifndef WS2812_SAT_STEP
#define WS2812_SAT_STEP 17
#endif
#ifndef WS2812_VAL_STEP
#define WS2812_VAL_STEP 17
#endif

#define RGBLED_TIMER_TOP F_CPU/(256*64)
// #define RGBLED_TIMER_TOP 0xFF10

#include <stdint.h>
#include <stdbool.h>
#include "eeconfig.h"
#ifndef WS2812_CUSTOM_DRIVER
#include "ws2812.h"
#endif
#include "ws2812_types.h"
#include "ws2812_list.h"

extern LED_TYPE led[WS2812_NUM];

extern const uint8_t RGBLED_BREATHING_INTERVALS[4] ;
extern const uint8_t RGBLED_RAINBOW_MOOD_INTERVALS[3] ;
extern const uint8_t RGBLED_RAINBOW_SWIRL_INTERVALS[3] ;
extern const uint8_t RGBLED_SNAKE_INTERVALS[3] ;
extern const uint8_t RGBLED_KNIGHT_INTERVALS[3] ;
extern const uint16_t RGBLED_RGBTEST_INTERVALS[1] ;

typedef union {
  uint32_t raw;
  struct {
    bool     enable  :1;
    uint8_t  mode    :6;
    uint16_t hue     :9;
    uint8_t  sat     :8;
    uint8_t  val     :8;
  };
} ws2812_config_t;

void ws2812_init(void);
void ws2812_increase(void);
void ws2812_decrease(void);
void ws2812_toggle(void);
void WS2812_ENABLE(void);
void ws2812_disable(void);
void ws2812_step(void);
void ws2812_step_reverse(void);
uint32_t ws2812_get_mode(void);
void ws2812_mode(uint8_t mode);
void ws2812_set(void);
void ws2812_sleep_prepare(void);
void ws2812_update_dword(uint32_t dword);
void ws2812_increase_hue(void);
void ws2812_decrease_hue(void);
void ws2812_increase_sat(void);
void ws2812_decrease_sat(void);
void ws2812_increase_val(void);
void ws2812_decrease_val(void);
void ws2812_sethsv(uint16_t hue, uint8_t sat, uint8_t val);
uint16_t ws2812_get_hue(void);
uint8_t ws2812_get_sat(void);
uint8_t ws2812_get_val(void);
void ws2812_setrgb(uint8_t r, uint8_t g, uint8_t b);
void ws2812_setrgb_at(uint8_t r, uint8_t g, uint8_t b, uint8_t index);
void ws2812_sethsv_at(uint16_t hue, uint8_t sat, uint8_t val, uint8_t index);

void eeconfig_update_rgblight(uint32_t val);
void eeconfig_update_ws2812_default(void);
void eeconfig_debug_rgblight(void);

void rgb_matrix_increase(void);
void rgb_matrix_decrease(void);

void sethsv(uint16_t hue, uint8_t sat, uint8_t val, LED_TYPE *led1);
void setrgb(uint8_t r, uint8_t g, uint8_t b, LED_TYPE *led1);

void ws2812_sethsv_noeeprom(uint16_t hue, uint8_t sat, uint8_t val);
void ws2812_mode_noeeprom(uint8_t mode);
void ws2812_toggle_noeeprom(void);
void WS2812_ENABLE_noeeprom(void);
void ws2812_disable_noeeprom(void);

void ws2812_sethsv_eeprom_helper(uint16_t hue, uint8_t sat, uint8_t val, bool write_to_eeprom);
void ws2812_mode_eeprom_helper(uint8_t mode, bool write_to_eeprom);


#define EZ_RGB(val) ws2812_show_solid_color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF)
void ws2812_show_solid_color(uint8_t r, uint8_t g, uint8_t b);

void ws2812_task(void);

void ws2812_timer_init(void);
void ws2812_timer_enable(void);
void ws2812_timer_disable(void);
void ws2812_timer_toggle(void);
void ws2812_effect_breathing(uint8_t interval);
void ws2812_effect_rainbow_mood(uint8_t interval);
void ws2812_effect_rainbow_swirl(uint8_t interval);
void ws2812_effect_snake(uint8_t interval);
void ws2812_effect_knight(uint8_t interval);
void ws2812_effect_christmas(void);
void ws2812_effect_rgbtest(void);
void ws2812_effect_alternating(void);

#endif
