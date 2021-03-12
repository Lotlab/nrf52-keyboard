#pragma once

#define PRODUCT "Lot60.H" /* 硬件名称，用于蓝牙显示 */
#define DEVICE_VER 0x0004 /* 硬件版本 */

#define LED_CAPS 30
#define LED_RGB_R 8
#define LED_RGB_G 7
#define LED_RGB_B 6
#define LED_STATUS_BLE LED_RGB_B
#define LED_STATUS_CHARGING LED_RGB_G
#define LED_STATUS_USB LED_RGB_R

#define UART_RXD 9
#define UART_TXD 10

#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0

static const uint8_t row_pin_array[MATRIX_ROWS] = { 29, 28, 27, 25, 26 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 31, 3, 4, 5, 22, 12, 13, 14, 15, 16, 17, 18, 19, 20 };

#define ROW_IN // 二极管方向是从COL->ROW

#define SWD_DAT_IO T2
#define SWD_DAT_MASK bT2
#define SWD_DAT_PORT P1
#define SWD_CLK_IO T2EX
#define SWD_CLK_MASK bT2EX
#define SWD_CLK_PORT P1

// Bootloader指示灯
#define LED_DFU_INIT LED_RGB_B
#define LED_DFU_START LED_RGB_G
#define LED_DFU_FINISH LED_RGB_R
// #define LED_DFU_POSITIVE

// 设置为内部RC
#define NRFX_CLOCK_CONFIG_LF_SRC 0
#define CLOCK_CONFIG_LF_SRC 0
#define NRF_SDH_CLOCK_LF_SRC 0
#define NRF_SDH_CLOCK_LF_ACCURACY 1
