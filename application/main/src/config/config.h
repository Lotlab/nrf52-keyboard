/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_H
#define CONFIG_H


/* USB Device descriptor parameter */
#define VENDOR_ID       0x1209
#define PRODUCT_ID      0x0514
#define DEVICE_VER      0x0001
#define MANUFACTURER    "Lotlab"
#define PRODUCT         "BLE60-Rev.C"

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 14

/* define if matrix has ghost */
//#define MATRIX_HAS_GHOST

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE    5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)

// 定义Bootmagic按键
#define BOOTMAGIC_KEY_BOOT              KC_U /* boot! */
#define BOOTMAGIC_KEY_ERASE_BOND        KC_E /* erase bond info */

// 键盘省电参数
#define SLEEP_SLOW_TIMEOUT 15               // 键盘闲置多久后转入慢速扫描模式 (s)
#define SLEEP_OFF_TIMEOUT 600               // 键盘闲置多久后转入自动关机 (s)
#define KEYBOARD_SCAN_INTERVAL 1            // 唤醒定时器
#define KEYBOARD_FAST_SCAN_INTERVAL 10      // 通常模式下，多久扫描一次键盘 (ms)
#define KEYBOARD_SLOW_SCAN_INTERVAL 100     // 慢速模式下，多久扫描一次键盘 (ms)

// LED自动熄灭时长(ms)，设为0则不自动熄灭
#define LED_AUTOOFF_TIME 1000

// 需要输入配对码
#define PASSKEY_REQUIRED

// 启用看门狗
#define ENABLE_WATCHDOG

// 启用keymap存储
#define KEYMAP_STORAGE

// 直接开机而跳过开机条件检测，用于调试
// #define DEBUG_SKIP_PWRON_CHECK

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

#endif
