#pragma once

#include "config.h"

#define __STATIC_INLINE static inline
#include "nrf_drv_saadc.h"
#include "nrf_saadc.h"

#ifdef LED_POSITIVE
#define LED_SET(x) nrf_gpio_pin_set(x)
#define LED_CLEAR(x) nrf_gpio_pin_clear(x)
#define LED_WRITE(x, b) nrf_gpio_pin_write(x, b)
#else
#define LED_SET(x) nrf_gpio_pin_clear(x)
#define LED_CLEAR(x) nrf_gpio_pin_set(x)
#define LED_WRITE(x, b) nrf_gpio_pin_write(x, !(b))
#endif
