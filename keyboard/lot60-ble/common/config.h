/*
Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2019 Jim Jiang <jim@lotlab.org>

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

#pragma once

#include <stdint.h>

/* USB Device descriptor parameter */
#define VENDOR_ID 0x1209 /* USB VID */
#define PRODUCT_ID 0x0514 /* USB PID */
#define CONF_VENDOR_ID 0x0514 /* 配置项目内显示的VendorID */
#define CONF_PRODUCT_ID 0x0114 /* 配置项目内显示的ProductID */
#define MANUFACTURER "Lotlab" /* 硬件制造商，用于蓝牙显示 */

#ifdef HARDWARE_REV_C
#define PRODUCT "Lot60.C" /* 硬件名称，用于蓝牙显示 */
#define DEVICE_VER 0x0000 /* 硬件版本 */
#endif

#ifdef HARDWARE_REV_E
#define PRODUCT "Lot60.E" /* 硬件名称，用于蓝牙显示 */
#define DEVICE_VER 0x0001 /* 硬件版本 */
#endif

#ifdef HARDWARE_REV_F
#define PRODUCT "Lot60.F" /* 硬件名称，用于蓝牙显示 */
#define DEVICE_VER 0x0002 /* 硬件版本 */
#endif

#ifdef HARDWARE_REV_G
#define PRODUCT "Lot60.G" /* 硬件名称，用于蓝牙显示 */
#define DEVICE_VER 0x0003 /* 硬件版本 */
#endif

#define MACADDR_SEPRATOR '_' /* 蓝牙名称后地址的分隔符。若不设置则不显示蓝牙名称后面的地址 */

/* USB HID report parameter */
#define KEYBOARD_EPSIZE 8 /* 键盘上传端点大小，请不要修改 */
#define NKRO_EPSIZE 28 /* 键盘NKRO端点大小，请不要修改 */

/* key matrix size */
#define MATRIX_ROWS 5 /* 硬件阵列行数 */
#define MATRIX_COLS 14 /* 硬件阵列列数 */

/* define if matrix has ghost */
// #define MATRIX_HAS_GHOST /* 按键阵列是否出现Ghost Key，若没有加二极管则需要启用这个项目 */

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5 /* 硬件消抖次数 */

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)))

// 定义Bootmagic按键
#define BOOTMAGIC_KEY_BOOT KC_U /* 开机按键 */
#define BOOTMAGIC_KEY_ERASE_BOND KC_E /* 删除所有绑定的按键 */

// 键盘省电参数
#define SLEEP_SLOW_TIMEOUT 15 // 键盘闲置多久后转入慢速扫描模式 (s)
#define SLEEP_OFF_TIMEOUT 600 // 键盘闲置多久后转入自动关机 (s)
#define KEYBOARD_SCAN_INTERVAL 1 // 键盘最小时间单位TICK (ms)
#define KEYBOARD_FAST_SCAN_INTERVAL 10 // 通常模式下，多久扫描一次键盘 (ms)
#define KEYBOARD_SLOW_SCAN_INTERVAL 100 // 慢速模式下，多久扫描一次键盘 (ms)

// 功率相关
// #define DYNAMIC_TX_POWER // 自动发射功率调整
#define HIGH_TX_POWER // 高发射功率

// LED自动熄灭时长(s)，设为0则不自动熄灭
#define LED_AUTOOFF_TIME 5

// 需要输入配对码
// #define PASSKEY_REQUIRED

// 启用看门狗
#define ENABLE_WATCHDOG

// 启用keymap存储
#define KEYMAP_STORAGE
#define MACRO_STORAGE /* 启用宏存储功能 */
#define CONFIG_STORAGE /* 启用配置存储功能 */

// 直接开机而跳过开机条件检测，用于调试
// #define DEBUG_SKIP_PWRON_CHECK

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

// LED 配置
#ifdef HARDWARE_REV_C
#define LED_CAPS 21
#endif

#if defined(HARDWARE_REV_E) || defined(HARDWARE_REV_F)
#define LED_CAPS 19
#endif

// RGB LED 指示配置引脚
#ifdef HARDWARE_REV_C
#define LED_RGB_R 22
#define LED_RGB_G 23
#define LED_RGB_B 24
#endif

#if defined(HARDWARE_REV_E) || defined(HARDWARE_REV_F) || defined(HARDWARE_REV_G)
#define LED_RGB_R 24
#define LED_RGB_G 23
#define LED_RGB_B 22
#endif

// 3灯指示配置引脚
#define LED_STATUS_BLE 22
#define LED_STATUS_CHARGING 23
#define LED_STATUS_USB 24

// USB UART 传输配置
#define HAS_USB
#define UART_RXD 17
#define UART_TXD 18
#ifdef HARDWARE_REV_C
#define UART_DET 19
#endif
#define UART_BAUDRATE NRF_UART_BAUDRATE_115200

// 电量检测配置
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0

// 充电检测配置
#if defined(HARDWARE_REV_C) || defined(HARDWARE_REV_E) || defined(HARDWARE_REV_F)
#define PIN_CHARGING !P3_3
#define PIN_STANDBY !P3_4
#endif

#if defined(HARDWARE_REV_G)
#define PIN_CHARGING true
#endif

// 按键阵列配置
static const uint8_t row_pin_array[MATRIX_ROWS] = { 25, 28, 29, 30, 26 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 16, 15, 14, 13, 12, 11, 27, 3, 4, 5, 6, 7, 8, 9 };

#if defined(HARDWARE_REV_C) || defined(HARDWARE_REV_F)
#define ROW_IN // 二极管方向是从COL->ROW
#endif

#define LED_POSITIVE // LED上拉驱动

#define FDS_VIRTUAL_PAGES 2 // 缩减FDS的页面数，防止出现问题

// 调试器配置
#if defined(HARDWARE_REV_G)
#define SWD_DAT_IO T2
#define SWD_DAT_MASK bT2
#define SWD_DAT_PORT P1
#define SWD_CLK_IO T2EX
#define SWD_CLK_MASK bT2EX
#define SWD_CLK_PORT P1
#endif
