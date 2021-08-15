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
#include <stdbool.h>
#include <stdint.h>

#include "../main.h"
#include "bootmagic.h"
#include "config.h"
#include "data_storage.h"
#include "hook.h"
#include "keycode.h"
#include "nrf_delay.h"
#include "sleep_reason.h"
#include "usb_comm.h"

#include "matrix.h"

#ifndef BOOTMAGIC_KEY_BOOT_TRAP
#define BOOTMAGIC_KEY_BOOT_TRAP KC_Y
#endif
#ifndef BOOTMAGIC_KEY_BOOT_TRAP2
#define BOOTMAGIC_KEY_BOOT_TRAP2 KC_J
#endif
#ifndef BOOTMAGIC_KEY_BOOT_TRAP3
#define BOOTMAGIC_KEY_BOOT_TRAP3 KC_N
#endif
#ifndef BOOTMAGIC_KEY_BOOT_TRAP4
#define BOOTMAGIC_KEY_BOOT_TRAP4 KC_I
#endif

static bool scan_key(uint16_t code)
{
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        matrix_row_t matrix_row = matrix_get_row(r);
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            if (matrix_row & ((matrix_row_t)1 << c)) {
                // read key from Layer 0
                action_t action = action_for_key(0, (keypos_t){ .row = r, .col = c });
                if (action.code == code || ((action.kind.id == ACT_LMODS || action.kind.id == ACT_RMODS || action.kind.id == ACT_LMODS_TAP || action.kind.id == ACT_RMODS_TAP || action.kind.id == ACT_LAYER_TAP || action.kind.id == ACT_LAYER_TAP_EXT) && action.key.code == code)) {
                    return true;
                }
            }
        }
    }
    return false;
}

static bool bootcheck_scan_key(uint16_t code)
{
    if (!scan_key(BOOTMAGIC_KEY_SALT))
        return false;
    return scan_key(code);
}

static bool keypress_check()
{
    uint8_t scan = 200;
    while (scan--) {
        matrix_scan();
        nrf_delay_ms(1);
    }

#ifdef BOOTMAGIC_KEY_ERASE_BOND
    // 检查取消绑定的按钮
    erase_bonds = bootcheck_scan_key(BOOTMAGIC_KEY_ERASE_BOND);
#endif

#ifdef BOOTMAGIC_KEY_BOOT
    // 检查开机按钮
    return bootcheck_scan_key(BOOTMAGIC_KEY_BOOT)
        && !bootcheck_scan_key(BOOTMAGIC_KEY_BOOT_TRAP)
        && !bootcheck_scan_key(BOOTMAGIC_KEY_BOOT_TRAP2)
        && !bootcheck_scan_key(BOOTMAGIC_KEY_BOOT_TRAP3)
        && !bootcheck_scan_key(BOOTMAGIC_KEY_BOOT_TRAP4);
#else
    return true;
#endif
}

//注册bootcheck_flag需要的存储区
CONFIG_SECTION(bootcheck_flag, 1);

/**
 * @brief 读取是否启用bootcheck
 * 
 * @return true 启用
 * @return false 不启用
 */
bool bootcheck_flag_get(void)
{
    return bootcheck_flag.data[0];
}

/**
 * @brief 切换是否启用bootcheck
 * 
 */
void bootcheck_flag_toggle(void)
{
    if (bootcheck_flag.data[0]) {
        bootcheck_flag.data[0] = 0x00;
    } else {
        bootcheck_flag.data[0] = 0x01;
    }
    storage_write((1 << STORAGE_CONFIG));
}

__attribute__((weak)) void boot_check()
{
    if (bootcheck_flag_get()) {
        bool sleep_flag = true;
#ifdef DEBUG_SKIP_PWRON_CHECK
        // debug状态下自动开机
        sleep_flag = false;
#endif

        // 自动休眠则不需要需要使用BOOT按键开机
        if (sleep_reason_get())
            sleep_flag = false;

        // 如果以上条件均不满足则尝试检测开机按键
        if (sleep_flag) {
            sleep_flag = !keypress_check();
#ifdef HAS_USB
            // 若连接至主机则自动开机
            if (usb_working())
                sleep_flag &= false;
#endif
        }

        if (sleep_flag) {
            sleep(SLEEP_NOT_PWRON);
        }
    }
}
