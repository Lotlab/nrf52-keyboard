#ifndef __KEYBOARD_CONFIG_H__
#define __KEYBOARD_CONFIG_H__

#include <stdint.h>
#include "config.h"

#define __STATIC_INLINE static inline
#include "nrf_drv_saadc.h"
#include "nrf_saadc.h"

// LED 配置
#define LED_CAPS 21

// 启用 RGB
#define LED_RGB
#define LED_RGB_R 22
#define LED_RGB_G 23
#define LED_RGB_B 24

// UART 传输配置
#define HAS_USB
#define UART_RXD 17
#define UART_TXD 18
// #define UART_DET 19
#define UART_BAUDRATE 57600

// 电量检测配置
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0

static const uint8_t row_pin_array[MATRIX_ROWS] = { 25, 28, 29, 30, 26 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 16, 15, 14, 13, 12, 11, 27, 3, 4, 5, 6, 7, 8, 9 };

#define ROW_IN // 二极管方向是从COL->ROW

#define LED_POSITIVE

#ifdef LED_POSITIVE
#define LED_SET(x) nrf_gpio_pin_set(x)
#define LED_CLEAR(x) nrf_gpio_pin_clear(x)
#define LED_WRITE(x, b) nrf_gpio_pin_write(x, b)
#else
#define LED_SET(x) nrf_gpio_pin_clear(x)
#define LED_CLEAR(x) nrf_gpio_pin_set(x)
#define LED_WRITE(x, b) nrf_gpio_pin_write(x, !(b))
#endif

#endif //__KEYBOARD_CONFIG_H__
