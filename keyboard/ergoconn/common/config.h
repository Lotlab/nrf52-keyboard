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

/* USB和蓝牙的显示参数 */
#define VENDOR_ID 0x1209 /* USB VID */
#define PRODUCT_ID 0x0514 /* USB PID */
#define CONF_VENDOR_ID 0x0514 /* 配置项目内显示的VendorID */
#if defined(ERGOCONN_HAND_RIGHT)
#define CONF_PRODUCT_ID 0x1918 /* 配置项目内显示的ProductID */
#elif defined(ERGOCONN_HAND_LEFT)
#define CONF_PRODUCT_ID 0x1919 /* 配置项目内显示的ProductID */
#endif
#define DEVICE_VER 0x0001 /* 硬件版本 */
#define MANUFACTURER "Lotlab" /* 硬件制造商，用于蓝牙显示 */
#define PRODUCT "ErgoConn" /* 硬件名称，用于USB和蓝牙显示 */
#define MACADDR_SEPRATOR '_' /* 蓝牙名称后地址的分隔符。若不设置则不显示蓝牙名称后面的地址 */

/* USB HID report parameter */
#define KEYBOARD_EPSIZE 8 /* 键盘上传端点大小，请不要修改 */
#define NKRO_EPSIZE 28 /* 键盘NKRO端点大小，请不要修改 */

// 定义Bootmagic按键
#define BOOTMAGIC_KEY_BOOT KC_U /* 开机 */
#define BOOTMAGIC_KEY_ERASE_BOND KC_E /* 删除所有绑定 */

// 键盘省电参数
#define SLEEP_SLOW_TIMEOUT 15 // 键盘闲置多久后转入慢速扫描模式 (s)
#define SLEEP_OFF_TIMEOUT 600 // 键盘闲置多久后转入自动关机 (s)
#define KEYBOARD_SCAN_INTERVAL 1 // 键盘最小时间单位TICK (ms)
#define KEYBOARD_FAST_SCAN_INTERVAL 10 // 通常模式下，多久扫描一次键盘 (ms)
#define KEYBOARD_SLOW_SCAN_INTERVAL 100 // 慢速模式下，多久扫描一次键盘 (ms)
#define LED_AUTOOFF_TIME 5 /* LED自动熄灭时长(s)，设为0则不自动熄灭 */

// 键盘额外功能
#define DYNAMIC_TX_POWER /* 启用自动发射功率调整 */
// #define PASSKEY_REQUIRED /* 需要输入配对码 */
#define ENABLE_WATCHDOG /* 启用看门狗 */
#define KEYMAP_STORAGE /* 启用keymap存储 */
// #define MACRO_BLOCKING_MODE /* 在宏播放时禁用其他按键输入 */
#define MACRO_STORAGE /* 启用宏存储功能 */
#define CONFIG_STORAGE /* 启用配置存储功能 */
#define BUTTONLESS_DFU /* 启用免按钮DFU */

#define DEBUG_SKIP_PWRON_CHECK /*  直接开机而跳过开机条件检测，用于调试 */

/* TMK固件内置功能 */
/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)))

// USB UART 传输配置
#define HAS_USB // 启用与CH554的通信支持
#define UART_RXD 27 // UART_RX口IO
#define UART_TXD 26 // UART_TX口IO
#define UART_BAUDRATE NRF_UART_BAUDRATE_115200 // 通信波特率，请不要修改

// 电量检测配置
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0 // 电量检测引脚

// 充电检测配置
#define PIN_CHARGING !PWM2_ // CH554的充电检测。当UCC1拉低时表示正在充电
#define PIN_STANDBY !PWM1_ // CH554的充电检测。当UCC2拉低时表示充电完成。若不配置则只使用PIN_CHARGING作为是否充电的检测标志

// 按键阵列配置
#define MATRIX_ROWS 6 /* 硬件阵列行数 */
#define MATRIX_COLS 7 /* 硬件阵列列数 */
#ifdef HARDWARE_REV_0
static const uint8_t row_pin_array[MATRIX_ROWS] = { 17, 16, 15, 14, 13, 18 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 30, 29, 28, 25, 24, 23, 22 };
#endif
#ifdef HARDWARE_REV_1
static const uint8_t row_pin_array[MATRIX_ROWS] = { 11, 12, 13, 14, 17, 18 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 30, 29, 28, 25, 24, 23, 22 };
#endif
// #define ROW_IN // 键盘阵列的二极管方向是从COL->ROW

/* define if matrix has ghost */
// #define MATRIX_HAS_GHOST /* 按键阵列是否出现Ghost Key，若没有加二极管则需要启用这个项目 */

#define DEBOUNCE 5 /* 硬件消抖次数，设置为0则不消抖 */

// OLED 屏幕配置
#ifdef HARDWARE_REV_0
#define SSD1306_SDA 9
#define SSD1306_SCL 10
#endif
#ifdef HARDWARE_REV_1
#define SSD1306_SDA 15
#define SSD1306_SCL 16
#endif
// 旋钮配置
#define ROTARY_ENCODER_A 19
#define ROTARY_ENCODER_B 20

#if defined(ERGOCONN_HAND_RIGHT)
// 旋钮正向按钮映射
#define ROTARY_ENCODER_POS 5,5
// 旋钮负向按钮映射
#define ROTARY_ENCODER_NEG 5,6
#elif defined(ERGOCONN_HAND_LEFT)
#define SSD1306_ROTATE_180 // 旋转屏幕
// 旋钮正向按钮映射
#define ROTARY_ENCODER_POS 5,6
// 旋钮负向按钮映射
#define ROTARY_ENCODER_NEG 5,5
#else
// #error "请指定左手还是右手"
#endif

#define NRF_BL_DFU_ENTER_METHOD_PINRESET 1
