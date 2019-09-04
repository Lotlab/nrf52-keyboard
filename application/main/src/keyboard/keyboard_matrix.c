/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

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
/**
 * @brief 键盘按键扫描
 * 
 * @file matrix.c
 * @author Jim Jiang
 * @date 2018-05-13
 */
#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "../config/keyboard_config.h"
#include "ble_keyboard.h"
#include "custom_hook.h"
#include "debug.h"
#include "keyboard_matrix.h"
#include "matrix.h"
#include "print.h"
#include "util.h"
#include "wait.h"

#ifndef DEBOUNCE
#define DEBOUNCE 1
#endif

static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void select_row(uint8_t row);
static void unselect_rows(void);

#ifdef ROW_IN
#define READ_COL(pin) (!nrf_gpio_pin_read(pin))
#else
#define READ_COL(pin) nrf_gpio_pin_read(pin)
#endif

/**
 * @brief 初始化键盘阵列
 * 
 */
void matrix_init(void)
{
    for (uint_fast8_t i = MATRIX_ROWS; i--;) {
        nrf_gpio_cfg(
            (uint32_t)row_pin_array[i],
            NRF_GPIO_PIN_DIR_OUTPUT,
            NRF_GPIO_PIN_INPUT_DISCONNECT,
            NRF_GPIO_PIN_NOPULL,
#ifdef ROW_IN
            NRF_GPIO_PIN_S0D1,
#else
            NRF_GPIO_PIN_D0S1,
#endif
            NRF_GPIO_PIN_NOSENSE);
#ifdef ROW_IN
        nrf_gpio_pin_set((uint32_t)row_pin_array[i]);
#else
        nrf_gpio_pin_clear((uint32_t)row_pin_array[i]); //Set pin to low
#endif
    }
    for (uint_fast8_t i = MATRIX_COLS; i--;) {
#ifdef ROW_IN
        nrf_gpio_cfg_input((uint32_t)column_pin_array[i], NRF_GPIO_PIN_PULLUP);
#else
        nrf_gpio_cfg_input((uint32_t)column_pin_array[i], NRF_GPIO_PIN_PULLDOWN);
#endif
    }
}
/** read all rows */
static matrix_row_t read_cols(void)
{
    uint16_t result = 0;

    for (uint_fast8_t c = 0; c < MATRIX_COLS; c++) {
        if (READ_COL((uint32_t)column_pin_array[c]))
            result |= 1 << c;
    }

    return result;
}

static void select_row(uint8_t row)
{
#ifdef ROW_IN
    nrf_gpio_pin_clear((uint32_t)row_pin_array[row]);
#else
    nrf_gpio_pin_set((uint32_t)row_pin_array[row]);
#endif
}

static void unselect_rows(void)
{
    for (uint_fast8_t i = 0; i < MATRIX_ROWS; i++) {
#ifdef ROW_IN
        nrf_gpio_pin_set((uint32_t)row_pin_array[i]);
#else
        nrf_gpio_pin_clear((uint32_t)row_pin_array[i]);
#endif
    }
}

static inline void delay_us(void)
{
#ifdef __GNUC__
#define __nop() __asm("NOP")
#endif
    for (int i = 0; i < 36; i++) {
        __nop(); //64mhz, 64cycle = 1us, delay 0.56us
    }
}

uint8_t matrix_scan(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
#ifdef HYBRID_MATRIX
        init_cols();
#endif
        delay_us(); // wait stable
        matrix_row_t cols = read_cols();
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                dprint("bounce!: ");
                debug_hex(debouncing);
                dprint("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing) {
        if (--debouncing) {
            // no need to delay here manually, because we use the clock.
            keyboard_debounce();
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing)
        return false;
    return true;
}

inline matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

/**
 * @brief 阵列准备睡眠
 * 
 */
void matrix_sleep_prepare(void)
{
// 这里监听所有按键作为唤醒按键，所以真正的唤醒判断应该在main的初始化过程中
#ifdef ROW_IN
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_output((uint32_t)column_pin_array[i]);
        nrf_gpio_pin_set((uint32_t)column_pin_array[i]);
    }
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_sense_input((uint32_t)row_pin_array[i], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }
#else
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_sense_input((uint32_t)column_pin_array[i], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_output((uint32_t)row_pin_array[i]);
        nrf_gpio_pin_set((uint32_t)row_pin_array[i]);
    }
#endif
}
