#pragma once

#include <stdint.h>

/* Device */
#define VENDOR_ID 0xDE29
#define PRODUCT_ID 0xD707
#define CONF_VENDOR_ID 0x9A29
#define CONF_PRODUCT_ID 0x9707
#define DEVICE_VER 0x0001
#define MANUFACTURER "Leo Deng"
#define PRODUCT "Magnet"
#define DEVICE_BLE_APPEARANCE BLE_APPEARANCE_HID_KEYBOARD
#define MACADDR_SEPRATOR '_'

/* Key Matrix */
// MUX: RS2522XS16; 1 enable pin, 2 channel pins, 2 analog output pins, 8 analog input pins
#define MUX_COUNT 1
static const uint8_t enable_pin_array[MUX_COUNT] = { 6 }; //mux enable, digital output
#define MUX_CHANNELS 2
static const uint8_t channel_pin_array[MUX_CHANNELS] = { 7, 8 }; // mux channel, digital output
#define MATRIX_ROWS 4 // Used mux channels, max = 2 ^ MUX_CHANNELS
#define MATRIX_COLS 2 // Total mux sig pins, MUX_COUNT * MUX_OUTPUT
static const uint8_t row_pin_array[MATRIX_ROWS] = { 0 }; // fake
static const uint8_t column_pin_array[MATRIX_COLS] = { 4, 5 }; // mux output, analog input
#define DEBOUNCE 5
#define MATRIX_SCAN_DELAY_CYCLE 36
#define WAKE_UP_PIN 12

/* Magnet Switch Value Range */
#define MAGNET_THRESHOLD_TOP 486 // stay still
#define MAGNET_THRESHOLD_LIGHT 430
#define MAGNET_THRESHOLD_DEFAULT 380
#define MAGNET_THRESHOLD_HEAVY 330
#define MAGNET_THRESHOLD_BOTTOM 270 // fully push down

/* Command Key */
#define IS_COMMAND() (keyboard_report->mods == (MOD_BIT(KC_LEFT) | MOD_BIT(KC_RGHT)))

/* Power Saving */
#define SLEEP_SLOW_TIMEOUT 15 // 键盘闲置多久后转入慢速扫描模式 (s)
#define SLEEP_OFF_TIMEOUT 600 // 键盘闲置多久后转入自动关机 (s)
#define KEYBOARD_SCAN_INTERVAL 1 // 键盘最小时间单位TICK (ms)
#define KEYBOARD_FAST_SCAN_INTERVAL 10 // 通常模式下，多久扫描一次键盘 (ms)
#define KEYBOARD_SLOW_SCAN_INTERVAL 100 // 慢速模式下，多久扫描一次键盘 (ms)
#define LED_AUTOOFF_TIME 60 // LED自动熄灭时长(s)，设为0则不自动熄灭
#define DYNAMIC_TX_POWER /* 启用自动发射功率调整 */
// #define HIGH_TX_POWER /* 更改发射功率到+4dBm */

/* Extra Features */
#define ENABLE_WATCHDOG /* 启用看门狗 */
#define KEYMAP_STORAGE /* 启用keymap存储 */
#define MACRO_STORAGE /* 启用宏存储功能 */
#define CONFIG_STORAGE /* 启用配置存储功能 */
#define BUTTONLESS_DFU /* 启用免按钮DFU */

/* USB HID */
#define KEYBOARD_EPSIZE 8 /* 键盘上传端点大小，请不要修改 */
#define NKRO_EPSIZE 28 /* 键盘NKRO端点大小，请不要修改 */

/* USB UART */
#define HAS_USB // 启用与CH554的通信支持
#define UART_RXD 26 // UART_RX IO
#define UART_TXD 27 // UART_TX IO
#define UART_BAUDRATE NRF_UART_BAUDRATE_115200 // 通信波特率，请不要修改

/* Battery */
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0 // 电量检测引脚
#define PIN_CHARGING !UCC1 // CH554的充电检测。当UCC1拉低时表示正在充电
#define PIN_STANDBY !UCC2 // CH554的充电检测。当UCC2拉低时表示充电完成。若不配置则只使用PIN_CHARGING作为是否充电的检测标志
