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
#define PRODUCT_ID 0x0521 /* USB PID */
#define DEVICE_VER 0x0002 /* 硬件版本 */
#define MANUFACTURER "ALICEGO" /* 硬件制造商，用于蓝牙显示 */
#define PRODUCT "ALICEGO" /* 硬件名称，用于USB和蓝牙显示 */

#define MACADDR_SEPRATOR '_' /* 蓝牙名称后地址的分隔符。若不设置则不显示蓝牙名称后面的地址 */

/* USB HID report parameter */
#define KEYBOARD_EPSIZE 8 /* 键盘上传端点大小，请不要修改 */
#define NKRO_EPSIZE 28 /* 键盘NKRO端点大小，请不要修改 */

/* key matrix size */
#define MATRIX_ROWS 10 /* 硬件阵列行数 */
#define MATRIX_COLS 7 /* 硬件阵列列数 */

/* define if matrix has ghost */
// #define MATRIX_HAS_GHOST /* 按键阵列是否出现Ghost Key，若没有加二极管则需要启用这个项目 */

#define DEBOUNCE 5 /* 硬件消抖次数，设置为0则不消抖 */

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)))

// 定义Bootmagic按键
#define BOOTMAGIC_KEY_BOOT KC_U /* 开机 */
#define BOOTMAGIC_KEY_ERASE_BOND KC_E /* 删除所有绑定 */

// 键盘省电参数
#define SLEEP_SLOW_TIMEOUT 15 // 键盘闲置多久后转入慢速扫描模式 (s)
#define SLEEP_OFF_TIMEOUT 600 // 键盘闲置多久后转入自动关机 (s)
#define KEYBOARD_SCAN_INTERVAL 1 // 键盘最小时间单位TICK (ms)
#define KEYBOARD_FAST_SCAN_INTERVAL 10 // 通常模式下，多久扫描一次键盘 (ms)
#define KEYBOARD_SLOW_SCAN_INTERVAL 100 // 慢速模式下，多久扫描一次键盘 (ms)

// 功率相关
#define DYNAMIC_TX_POWER /* 启用自动发射功率调整 */
//#define HIGH_TX_POWER // 高发射功率

// LED自动熄灭时长(s)，设为0则不自动熄灭
#define LED_AUTOOFF_TIME 5

// 需要输入配对码
// #define PASSKEY_REQUIRED

// 启用多设备切换
// #define MULTI_DEVICE_SWITCH

#define BUTTONLESS_DFU /* 启用免按钮DFU */

// 启用看门狗
#define ENABLE_WATCHDOG

// 启用keymap存储
#define KEYMAP_STORAGE
#define MACRO_STORAGE /* 启用宏存储功能 */
#define CONFIG_STORAGE /* 启用配置存储功能 */

#define BUTTONLESS_DFU /* 启用免按钮DFU */

// 直接开机而跳过开机条件检测，用于调试
// #define DEBUG_SKIP_PWRON_CHECK

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

// ws2812 RGB 配置
//#define RGB_DI_PIN 28
//#define RGBLED_NUM 8
//#define RGBLIGHT_ANIMATIONS
//#define RGB_PWR_PIN_REVERSE 29 // N-mos
//#define RGB_PWR_PIN 30 // P-mos

// LED 配置
//#define LED_NUM 22
//#define LED_CAPS 23
// #define LED_SCLK 23
//#define LED_POSITIVE // LED上拉驱动


// 独立硬件按钮
#define POWER_BUTTON 25

// USB UART 传输配置
#define HAS_USB // 启用与CH554的通信支持
#define UART_RXD 29 // UART_RX口IO
#define UART_TXD 30 // UART_TX口IO
// #define UART_DET 19 // UART 检测引脚，若此脚被拉低，则说明USB正在工作。若不配置则使用RX口作为检测引脚
#define UART_BAUDRATE NRF_UART_BAUDRATE_115200 // 通信波特率，请不要修改

// OLED 屏幕配置
#define SSD1306_SDA 3
#define SSD1306_SCL 4
#define SSD1306_ROTATE_180 // 旋转屏幕

// 旋钮配置
#define ROTARY_ENCODER_A 26
#define ROTARY_ENCODER_B 27

// 旋钮正向按钮映射
#define ROTARY_ENCODER_POS 9,6
// 旋钮负向按钮映射
#define ROTARY_ENCODER_NEG 4,6

// 电量检测配置
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0 // 电量检测引脚

// 充电检测配置
#define PIN_CHARGING !UCC1 // CH554的充电检测。当UCC1拉低时表示正在充电
#define PIN_STANDBY !UCC2 // CH554的充电检测。当UCC2拉低时表示充电完成。若不配置则只使用PIN_CHARGING作为是否充电的检测标志


// 按键阵列配置,目前仍剩余：24, 28, 31
static const uint8_t row_pin_array[MATRIX_ROWS] = { 5, 6, 7, 8, 9, 11, 12, 13, 14, 15 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 23, 20, 19, 22, 18, 17, 16 };

#define ROW_IN // 键盘阵列的二极管方向是从COL->ROW
