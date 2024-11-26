#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "keyboard_config.h"
#include "adc_convert.h"
#include "ble_keyboard.h"
#include "debug.h"
#include "keyboard_matrix.h"
#include "matrix.h"
#include "print.h"
#include "util.h"
#include "wait.h"

// 行程触发阈值
// TODO 先这样，动态调节的功能后面再加
static uint16_t magnet_threshold = MAGNET_THRESHOLD_DEFAULT;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];

void init_adc_col_pin(uint8_t pin) {
  uint8_t channel_index;
  nrf_saadc_input_t analog_input_pin;
  // NRF_SAADC_INPUT_AIN0 (P0.02) has been used to detect battery voltage
  switch (pin) {
    case 3:
      channel_index = 1;
      analog_input_pin = NRF_SAADC_INPUT_AIN1;
      break;
    case 4:
      channel_index = 2;
      analog_input_pin = NRF_SAADC_INPUT_AIN2;
      break;
    case 5:
      channel_index = 3;
      analog_input_pin = NRF_SAADC_INPUT_AIN3;
      break;
    case 28:
      channel_index = 4;
      analog_input_pin = NRF_SAADC_INPUT_AIN4;
      break;
    case 29:
      channel_index = 5;
      analog_input_pin = NRF_SAADC_INPUT_AIN5;
      break;
    case 30:
      channel_index = 6;
      analog_input_pin = NRF_SAADC_INPUT_AIN6;
      break;
    case 31:
      channel_index = 7;
      analog_input_pin = NRF_SAADC_INPUT_AIN7;
      break;
    default:
      channel_index = -1; // 理论上不可能走到这个分支?
      analog_input_pin = NRF_SAADC_INPUT_DISABLED;
  }

  nrf_saadc_channel_config_t channel_config = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain = NRF_SAADC_GAIN1_2,
    .reference = NRF_SAADC_REFERENCE_INTERNAL,
    .acq_time = NRF_SAADC_ACQTIME_10US,
    .mode = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst = NRF_SAADC_BURST_DISABLED,
    .pin_p = (nrf_saadc_input_t)(analog_input_pin),
    .pin_n = NRF_SAADC_INPUT_DISABLED
  };

  ret_code_t err_code;
  err_code = nrfx_saadc_channel_init(channel_index, &channel_config);
  APP_ERROR_CHECK(err_code);
}

/* 初始化键盘阵列 */
void matrix_init(void) {
  // 拉低mux使能引脚，启用mux
  for (uint_fast8_t i = 0; i < MUX_COUNT; i++) {
    nrf_gpio_cfg_output(enable_pin_array[i]);
    nrf_gpio_pin_clear(enable_pin_array[i]);
  }

  // 全部拉高，先选中最后一个通道
  for (uint_fast8_t i = 0; i < MUX_CHANNELS; i++) {
    nrf_gpio_cfg_output(channel_pin_array[i]);
    nrf_gpio_pin_set(channel_pin_array[i]);
  }

  // 初始化模拟引脚
  for (uint_fast8_t i = 0; i < MATRIX_COLS; i++) {
    init_adc_col_pin(column_pin_array[i]);
  }

  // 正常使用时禁用唤醒引脚
  nrf_gpio_cfg_default(WAKE_UP_PIN);
}

/* Read all mux sig pins */
matrix_row_t read_cols(void) {
  matrix_row_t result = 0;

  for (uint_fast8_t c = 0; c < MATRIX_COLS; c++) {
    nrf_saadc_value_t adc_val = adc_read_sync(column_pin_array[c]);

    if (adc_val < magnet_threshold) {
      // Pressed
      result |= (1 << c);
    } else {
      // Released
      result &= ~(1 << c);
    }
  }

  return result;
}

/* Select mux channel */
void select_row(uint8_t channel) {
  for (uint_fast8_t i = 0; i < MUX_CHANNELS; i++) {
    uint8_t state = (channel >> i) & 1;
    if (state) {
      // set high
      nrf_gpio_cfg_output(channel_pin_array[i]);
      nrf_gpio_pin_set(channel_pin_array[i]);
    } else {
      // set low
      nrf_gpio_cfg_output(channel_pin_array[i]);
      nrf_gpio_pin_clear(channel_pin_array[i]);
    }
  }
}

static inline void delay_us(void) {
#ifdef __GNUC__
#define __nop() __asm("NOP")
#endif

#ifndef MATRIX_SCAN_DELAY_CYCLE
#define MATRIX_SCAN_DELAY_CYCLE 36
#endif
  for (int i = 0; i < MATRIX_SCAN_DELAY_CYCLE; i++) {
    __nop(); // 64mhz, 64 cycles = 1us
  }
}

uint8_t matrix_scan(void) {
  for (uint_fast8_t i = 0; i < MATRIX_ROWS; i++) {
    select_row(i);
    delay_us(); // wait stable
    matrix_row_t cols = read_cols();
    matrix[i] = cols;
  }

  return 1;
}

bool matrix_is_modified(void) {
  return true;
}

/* 禁用所有阵列针脚 */
void matrix_deinit(void) {
  for (uint_fast8_t i = 0; i < MUX_COUNT; i++) {
    nrf_gpio_cfg_default(enable_pin_array[i]);
  }
  for (uint_fast8_t i = 0; i < MUX_CHANNELS; i++) {
    nrf_gpio_cfg_default(channel_pin_array[i]);
  }
  for (uint_fast8_t i = 0; i < MATRIX_COLS; i++) {
    nrf_gpio_cfg_default(column_pin_array[i]);
  }
}

/* 阵列准备睡眠 */
void matrix_wakeup_prepare(void) {
  // 拉高mux使能引脚，禁用mux
  for (uint_fast8_t i = 0; i < MUX_COUNT; i++) {
    nrf_gpio_cfg_output(enable_pin_array[i]);
    nrf_gpio_pin_set(enable_pin_array[i]);
  }

  // 禁用其他引脚
  for (uint_fast8_t i = 0; i < MUX_CHANNELS; i++) {
    nrf_gpio_cfg_default(channel_pin_array[i]);
  }
  for (uint_fast8_t i = 0; i < MATRIX_COLS; i++) {
    nrf_gpio_cfg_default(column_pin_array[i]);
  }

  // 只保留一个唤醒引脚，上拉即唤醒。
  // TODO 考虑用振动开关，或者TTP223电容触摸
  nrf_gpio_cfg_sense_input(WAKE_UP_PIN, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
}
