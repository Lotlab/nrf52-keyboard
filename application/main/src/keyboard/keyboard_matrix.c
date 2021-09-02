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
#include "keyboard_services.h"
#include "debug.h"
#include "keyboard_matrix.h"
#include "matrix.h"
#include "print.h"
#include "util.h"
#include "wait.h"

#ifndef DEBOUNCE
#define DEBOUNCE 1
#endif
// 实际的消抖次数
#define DEBOUNCE_RELOAD ((DEBOUNCE + KEYBOARD_SCAN_INTERVAL - 1) / KEYBOARD_SCAN_INTERVAL)

static uint8_t debouncing = DEBOUNCE_RELOAD;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

#ifdef LESS_IO
static matrix_row_t read_cols(uint8_t row);
#else
static matrix_row_t read_cols(void);
#endif
static void select_row(uint8_t row);
static void unselect_rows(uint8_t row);

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
    for (uint_fast8_t i = MATRIX_COLS; i--;) {
#ifdef ROW_IN
        nrf_gpio_cfg_input((uint32_t)column_pin_array[i], NRF_GPIO_PIN_PULLUP);
#else
        nrf_gpio_cfg_input((uint32_t)column_pin_array[i], NRF_GPIO_PIN_PULLDOWN);
#endif
    }
}
/** read all rows */
#ifdef LESS_IO
static matrix_row_t read_cols(uint8_t row)
{
    uint16_t result = 0;

    for (uint_fast8_t c = 0; c < MATRIX_COLS; c++) {

        if (c == row)
             continue;

        if (READ_COL((uint32_t)column_pin_array[c]))
            result |= 1 << c;
    }

    return result;
}
#else
static matrix_row_t read_cols(void)
{
    matrix_row_t result = 0;

    for (uint_fast8_t c = 0; c < MATRIX_COLS; c++) {
        if (READ_COL((uint32_t)column_pin_array[c]))
            result |= 1 << c;
    }

    return result;
}
#endif

static void select_row(uint8_t row)
{
    if ((uint32_t)row_pin_array[row] > 31 )
        return;
nrf_gpio_cfg(
            (uint32_t)row_pin_array[row],
            NRF_GPIO_PIN_DIR_OUTPUT,
            NRF_GPIO_PIN_INPUT_DISCONNECT,
            NRF_GPIO_PIN_NOPULL,
#ifdef LESS_IO
            NRF_GPIO_PIN_H0D1,
#else
#ifdef ROW_IN
            NRF_GPIO_PIN_H0H1,
#else
            NRF_GPIO_PIN_S0S1,
#endif
#endif
            NRF_GPIO_PIN_NOSENSE);
#ifdef ROW_IN
    nrf_gpio_pin_clear((uint32_t)row_pin_array[row]);
#else
    nrf_gpio_pin_set((uint32_t)row_pin_array[row]);
#endif
}

static void unselect_rows(uint8_t row)
{
    if ((uint32_t)row_pin_array[row] > 31 )
        return;
#ifdef LESS_IO
#ifdef ROW_IN
    nrf_gpio_cfg_input((uint32_t)row_pin_array[row], NRF_GPIO_PIN_PULLUP);
#else
    nrf_gpio_cfg_input((uint32_t)row_pin_array[row], NRF_GPIO_PIN_PULLDOWN);
#endif
#else
#ifdef ROW_IN
    nrf_gpio_pin_set((uint32_t)row_pin_array[row]);
#else
    nrf_gpio_pin_clear((uint32_t)row_pin_array[row]);
#endif
#endif
}

static inline void delay_us(void)
{
#ifdef __GNUC__
#define __nop() __asm("NOP")
#endif

#ifndef MATRIX_SCAN_DELAY_CYCLE
#define MATRIX_SCAN_DELAY_CYCLE 36
#endif
    for (int i = 0; i < MATRIX_SCAN_DELAY_CYCLE; i++) {
        __nop(); //64mhz, 64cycle = 1us
    }
}

uint8_t matrix_scan(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
        delay_us(); // wait stable
#ifdef LESS_IO
        matrix_row_t cols = read_cols(i);
#else
        matrix_row_t cols = read_cols();
#endif
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                dprint("bounce!: ");
                debug_hex(debouncing);
                dprint("\n");
            }
            debouncing = DEBOUNCE_RELOAD;
        }
        unselect_rows(i);
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

// 外部按键
#ifdef MATRIX_FORIGN_KEY
static bool matrix_oneshot_send[MATRIX_ROWS];
static matrix_row_t matrix_forign_oneshot[MATRIX_ROWS];
static matrix_row_t matrix_forign[MATRIX_ROWS];

/**
 * @brief 为matrix添加一个按键按下事件（会自动清空）
 * 
 * @param row 行号
 * @param col 列号
 */
void matrix_forign_add_oneshot(uint8_t row, uint8_t col)
{
    if (row >= MATRIX_ROWS)
        return;
    if (col >= sizeof(matrix_row_t) * 8)
        return;

    matrix_forign_oneshot[row] |= (1 << col);
}

/**
 * @brief 为matrix添加一个按键事件
 * 
 * @param row 行号
 * @param col 列号
 * @param press 按下
 */
void matrix_forign_set(uint8_t row, uint8_t col, bool press)
{
    if (row >= MATRIX_ROWS)
        return;
    if (col >= sizeof(matrix_row_t) * 8)
        return;
    if (press)
        matrix_forign[row] |= (1 << col);
    else
        matrix_forign[row] &= ~(1 << col);
}

inline matrix_row_t matrix_get_row(uint8_t row)
{
    matrix_row_t val = matrix[row] | matrix_forign[row];

    // 发送一次后在下次发送空包，防止多次连击
    if (matrix_oneshot_send[row]) {
        val |= matrix_forign_oneshot[row];
        matrix_forign_oneshot[row] = 0; // 清空单个按键
    }
    matrix_oneshot_send[row] = !matrix_oneshot_send[row];
    return val;
}
#else
inline matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}
#endif

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

/**
 * @brief 禁用所有阵列针脚
 * 
 */
void matrix_deinit(void)
{
#ifndef LESS_IO
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_default(column_pin_array[i]);
    }
#endif
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_default(row_pin_array[i]);
    }
}

/**
 * @brief 阵列准备睡眠
 * 
 */
void matrix_wakeup_prepare(void)
{
// 这里监听所有按键作为唤醒按键，所以真正的唤醒判断应该在main的初始化过程中
#ifdef LESS_IO
    matrix_deinit();
#else
#ifdef ROW_IN
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_output(column_pin_array[i]);
        nrf_gpio_pin_set(column_pin_array[i]);
    }
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_sense_input(row_pin_array[i], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }
#else
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_sense_input(column_pin_array[i], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_output(row_pin_array[i]);
        nrf_gpio_pin_set(row_pin_array[i]);
    }
#endif
#endif
#ifdef ROTARY_BUTTON
    nrf_gpio_cfg_sense_input(ROTARY_BUTTON,
        NRF_GPIO_PIN_PULLUP,
        NRF_GPIO_PIN_SENSE_LOW);
#endif
}
