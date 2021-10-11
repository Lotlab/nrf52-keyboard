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
// USB的VID和PID，自行修改
#define VENDOR_ID 0x0168 /* USB VID */
#define PRODUCT_ID 0x0001 /* USB PID */
// 配置协议使用的VID和PID，自行修改。ID组合不得重复。
#define CONF_VENDOR_ID 0x0168 /* 配置协议使用的VendorID */
#define CONF_PRODUCT_ID 0x0001 /* 配置协议使用的ProductID */
#define DEVICE_VER 0x0100 /* 硬件版本 V1.0*/
#define MANUFACTURER "Benq" /* 硬件制造商，用于USB和蓝牙显示 */
#define PRODUCT "Benq A800 Keyboard" /* 硬件名称，用于USB和蓝牙显示 */
// #define MACADDR_SEPRATOR '_' /* 蓝牙名称后地址的分隔符。若不设置则不显示蓝牙名称后面的地址 */

/* USB HID report parameter */
#define KEYBOARD_EPSIZE 8 /* 键盘上传端点大小，请不要修改 */
#define NKRO_EPSIZE 28 /* 键盘NKRO端点大小，请不要修改 */

// 定义Bootmagic/Bootcheck按键
#define BOOTMAGIC_KEY_BOOT KC_U /* 开机 */
#define BOOTMAGIC_KEY_ERASE_BOND KC_E /* 删除所有绑定 */

// 键盘省电参数
#define SLEEP_SLOW_TIMEOUT 15 // 键盘闲置多久后转入慢速扫描模式 (s)
#define SLEEP_OFF_TIMEOUT 3600 // 键盘闲置多久后转入自动关机 (s)
#define KEYBOARD_SCAN_INTERVAL 1 // 键盘最小时间单位TICK (ms)
#define KEYBOARD_FAST_SCAN_INTERVAL 10 // 通常模式下，多久扫描一次键盘 (ms)
#define KEYBOARD_SLOW_SCAN_INTERVAL 100 // 慢速模式下，多久扫描一次键盘 (ms)
#define LED_AUTOOFF_TIME 5 /* LED自动熄灭时长(s)，设为0则不自动熄灭 */

// 键盘额外功能
// #define DYNAMIC_TX_POWER /* 启用自动发射功率调整 */
// #define PASSKEY_REQUIRED /* 需要输入配对码 */
#define ENABLE_WATCHDOG /* 启用看门狗 */
// #define HIGH_TX_POWER /* 更改发射功率到+4dBm */
#define MULTI_DEVICE_SWITCH  /*启用多设备切换 */
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

// A800 LED驱动PIN脚配置
#define A800_LED_DATA 5    // LED数据pin定义(74HC595 DS)
#define A800_LED_CLK 3     // LED时钟pin定义(74HC595 SHCP)
#define A800_LED_ON 4      // LED使能pin定义(74HC595 STCP)

// Bootloader 指示灯
// #define LED_DFU_INIT 6
// #define LED_DFU_START 5
// #define LED_DFU_FINISH 4
// #define LED_DFU_POSITIVE // LED上拉驱动

// 多用途 Bootloader 按钮
// #define NRF_BL_DFU_MULTI_ROLE_BTN 21

// USB UART 传输配置
#define HAS_USB // 启用与CH554的通信支持
#define UART_RXD 18 // UART_RX口IO
#define UART_TXD 19 // UART_TX口IO
// #define UART_DET 19 // UART 检测引脚，若此脚被拉低，则说明USB正在工作。若不配置则使用RX口作为检测引脚
#define UART_BAUDRATE NRF_UART_BAUDRATE_115200 // 通信波特率，请不要修改

// 电量检测配置
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN3 // 电量检测引脚

// 充电检测配置
#define PIN_CHARGING !P3_3  // CH554的充电检测。当P3.3拉低时表示正在充电。若不配置则禁用USB下发充电事件
#define PIN_STANDBY !P3_4   // CH554的充电检测。当P3.4拉低时表示充电完成。若不配置则只使用PIN_CHARGING作为是否充电的检测标志

// 按键阵列配置
#define MATRIX_ROWS 8 /* 硬件阵列行数 */
#define MATRIX_COLS 17 /* 硬件阵列列数(1列包括功能按键) */
static const uint8_t row_pin_array[MATRIX_ROWS] = { 2, 31, 30, 29, 28, 27, 26, 25 };
static const uint8_t col_pin_array[MATRIX_COLS] = { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 20, 21, 22, 23, 24 };
// #define ROW_IN // 取消ROW_IN定义表示键盘阵列的电流方向是从ROW->COL

/* define if matrix has ghost */
#define MATRIX_HAS_GHOST /* 按键阵列是否出现Ghost Key，若没有加二极管则需要启用这个项目 */

#define DEBOUNCE 5 /* 硬件消抖次数，设置为0则不消抖 */
#define MATRIX_SCAN_DELAY_CYCLE 36 /* 按键扫描等待IO稳定的延时时长 */

// 板载调试器设置
#define SWD_CLK_IO UCC2
#define SWD_CLK_MASK bUCC2
#define SWD_CLK_PORT P1
#define SWD_DAT_IO UCC1
#define SWD_DAT_MASK bUCC1
#define SWD_DAT_PORT P1
