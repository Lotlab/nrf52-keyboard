/*
 * light weight WS2812 lib include
 *
 * Version 2.3  - Nev 29th 2015
 * Author: Tim (cpldcpu@gmail.com)
 *
 * Please do not change this file! All configuration is handled in "ws2812_config.h"
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

#pragma once

//#include "ws2812_config.h"
//#include "i2cmaster.h"

#include <stdint.h>
#include "color.h"

#ifdef CONFIG_H_FILE
    #include CONFIG_H_FILE
#endif


/* User Interface
 *
 * Input:
 *         ledarray:           An array of GRB data describing the LED colors
 *         number_of_leds:     The number of LEDs to write
 *         pinmask (optional): Bitmask describing the output bin. e.g. _BV(PB0)
 *
 * The functions will perform the following actions:
 *         - Set the data-out pin as output
 *         - Send out the LED data
 *         - Wait 50�ｿｽs to reset the LEDs
 */

void ws2812_setleds     (LED_TYPE *ledarray, uint16_t number_of_leds);
void ws2812_setleds_pin (LED_TYPE *ledarray, uint16_t number_of_leds,uint8_t pinmask);
void ws2812_setleds_rgbw(LED_TYPE *ledarray, uint16_t number_of_leds);

/*
 * Old interface / Internal functions
 *
 * The functions take a byte-array and send to the data output as WS2812 bitstream.
 * The length is the number of bytes to send - three per LED.
 */

void ws2812_sendarray     (uint8_t *array,uint16_t length);
void ws2812_sendarray_mask(uint8_t *array,uint16_t length, uint8_t pinmask);

//初始化RGB power控制脚
void ws2812_pwr_init(void);
//释放RGB power控制脚
void ws2812_pwr_deinit(void);
//开启RGB电源
void ws2812_pwr_on(void);
//关闭RGB电源
void ws2812_pwr_off(void);

/*
 * Internal defines
 */
#ifndef CONCAT
#define CONCAT(a, b)            a ## b
#endif
#ifndef CONCAT_EXP
#define CONCAT_EXP(a, b)   CONCAT(a, b)
#endif

// #define ws2812_PORTREG  CONCAT_EXP(PORT,ws2812_port)
// #define ws2812_DDRREG   CONCAT_EXP(DDR,ws2812_port)

