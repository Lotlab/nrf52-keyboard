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
#ifndef RGB_LIGHT_H
#define RGB_LIGHT_H

#ifdef CONFIG_H_FILE
    #include CONFIG_H_FILE
#endif

#ifdef RGB_LIGHT_ANIMATIONS
	#define RGB_LIGHT_MODES 8
#else
	#define RGB_LIGHT_MODES 1
#endif

#ifndef RGB_LIGHT_EFFECT_BREATHE_CENTER
#define RGB_LIGHT_EFFECT_BREATHE_CENTER 2  // 1-2.7
#endif

#ifndef RGB_LIGHT_EFFECT_BREATHE_MAX
#define RGB_LIGHT_EFFECT_BREATHE_MAX 255   // 0-255
#endif

#ifndef RGB_LIGHT_HUE_STEP
#define RGB_LIGHT_HUE_STEP 6
#endif
#ifndef RGB_LIGHT_SAT_STEP
#define RGB_LIGHT_SAT_STEP 17
#endif
#ifndef RGB_LIGHT_VAL_STEP
#define RGB_LIGHT_VAL_STEP 17
#endif

#define RGBLED_TIMER_TOP F_CPU/(256*64)
// #define RGBLED_TIMER_TOP 0xFF10

#include <stdint.h>
#include <stdbool.h>
#include "eeconfig.h"

extern const uint8_t RGBLED_BREATHING_INTERVALS[4] ;
extern const uint8_t RGBLED_RAINBOW_MOOD_INTERVALS[3] ;

typedef union {
  uint32_t raw;
  struct {
    bool     enable   :1;  //是否启用
    bool     ind      :1;  //是否指示灯模式
    bool     reserved :1;  //保留
    uint8_t  mode     :4;  //灯效模式
    uint16_t hue      :9;
    uint8_t  sat      :8;
    uint8_t  val      :8;
  };
} rgb_light_config_t;

void rgb_light_init(void);
void rgb_light_increase(void);
void rgb_light_decrease(void);
void rgb_light_toggle(void);
void rgb_indicator_toggle(void);
void rgb_light_enable(void);
void rgb_light_disable(void);
void rgb_light_step(void);
void rgb_light_step_reverse(void);
uint32_t rgb_light_get_mode(void);
void rgb_light_mode(uint8_t mode);
void rgb_light_sleep_prepare(void);
void rgb_light_update_dword(uint32_t dword);
void rgb_light_increase_hue(void);
void rgb_light_decrease_hue(void);
void rgb_light_increase_sat(void);
void rgb_light_decrease_sat(void);
void rgb_light_increase_val(void);
void rgb_light_decrease_val(void);
void rgb_light_sethsv(uint16_t hue, uint8_t sat, uint8_t val);
uint16_t rgb_light_get_hue(void);
uint8_t rgb_light_get_sat(void);
uint8_t rgb_light_get_val(void);
void rgb_light_setrgb(uint8_t r, uint8_t g, uint8_t b);
void rgb_light_setrgb_at(uint8_t r, uint8_t g, uint8_t b, uint8_t index);
void rgb_light_sethsv_at(uint16_t hue, uint8_t sat, uint8_t val, uint8_t index);

void eeconfig_update_rgb_light(uint32_t val);
void eeconfig_update_rgb_light_default(void);
void eeconfig_debug_rgb_light(void);

void light_sethsv(uint16_t hue, uint8_t sat, uint8_t val);
void light_setrgb(uint8_t r, uint8_t g, uint8_t b);

void rgb_light_sethsv_noeeprom(uint16_t hue, uint8_t sat, uint8_t val);
void rgb_light_mode_noeeprom(uint8_t mode);
void rgb_light_toggle_noeeprom(void);
void rgb_light_enable_noeeprom(void);
void rgb_light_disable_noeeprom(void);

void rgb_light_sethsv_eeprom_helper(uint16_t hue, uint8_t sat, uint8_t val, bool write_to_eeprom);
void rgb_light_mode_eeprom_helper(uint8_t mode, bool write_to_eeprom);


void rgb_light_task(void);

void rgb_light_timer_init(void);
void rgb_light_timer_enable(void);
void rgb_light_timer_disable(void);
void rgb_light_timer_toggle(void);
void rgb_light_effect_breathing(uint8_t interval);
void rgb_light_effect_rainbow_mood(uint8_t interval);

/*                            SET RGB List                            */
#define rgb_light_setrgb_white()       light_setrgb (0xFF, 0xFF, 0xFF)
#define rgb_light_setrgb_red()         light_setrgb (0xFF, 0x00, 0x00)
#define rgb_light_setrgb_coral()       light_setrgb (0xFF, 0x7C, 0x4D)
#define rgb_light_setrgb_orange()      light_setrgb (0xFF, 0x80, 0x00)
#define rgb_light_setrgb_goldenrod()   light_setrgb (0xD9, 0xA5, 0x21)
#define rgb_light_setrgb_gold()        light_setrgb (0xFF, 0xD9, 0x00)
#define rgb_light_setrgb_yellow()      light_setrgb (0xFF, 0xFF, 0x00)
#define rgb_light_setrgb_chartreuse()  light_setrgb (0x80, 0xFF, 0x00)
#define rgb_light_setrgb_green()       light_setrgb (0x00, 0xFF, 0x00)
#define rgb_light_setrgb_springgreen() light_setrgb (0x00, 0xFF, 0x80)
#define rgb_light_setrgb_turquoise()   light_setrgb (0x47, 0x6E, 0x6A)
#define rgb_light_setrgb_teal()        light_setrgb (0x00, 0x80, 0x80)
#define rgb_light_setrgb_cyan()        light_setrgb (0x00, 0xFF, 0xFF)
#define rgb_light_setrgb_azure()       light_setrgb (0x99, 0xf5, 0xFF)
#define rgb_light_setrgb_blue()        light_setrgb (0x00, 0x00, 0xFF)
#define rgb_light_setrgb_purple()      light_setrgb (0x7A, 0x00, 0xFF)
#define rgb_light_setrgb_magenta()     light_setrgb (0xFF, 0x00, 0xFF)
#define rgb_light_setrgb_pink()        light_setrgb (0xFF, 0x80, 0xBF)

#endif
