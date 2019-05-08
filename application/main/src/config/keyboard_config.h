#ifndef __KEYBOARD_CONFIG_H__
#define __KEYBOARD_CONFIG_H__

#include <stdint.h>
#include "config.h"

#define __STATIC_INLINE static inline
#include "nrf_drv_saadc.h"
#include "nrf_saadc.h"

// LED 配置
#define LED_CAPS 21
#define LED_EXT1 22
#define LED_EXT2 23
#define LED_EXT3 24

// SPI 传输配置
#define SPI_CLK 17
#define SPI_MISO 18
#define SPI_MOSI 19

// 电量检测配置
#define BATTERY_ADC_PIN NRF_SAADC_INPUT_AIN0

static const uint8_t row_pin_array[MATRIX_ROWS] = { 25, 28, 29, 30, 26 };
static const uint8_t column_pin_array[MATRIX_COLS] = { 16, 15, 14, 13, 12, 11, 27, 3, 4, 5, 6, 7, 8, 9 };

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
