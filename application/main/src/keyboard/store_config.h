#pragma once
#include "config.h"
#include "data_storage.h"
#include <stdint.h>

#ifdef CONFIG_STORAGE
uint16_t get_slow_scan_timeout(void);
uint16_t get_sleep_timeout(void);
uint16_t get_led_powersave_timeout(void);
#else
#define get_slow_scan_timeout() (SLEEP_SLOW_TIMEOUT)
#define get_sleep_timeout() (SLEEP_OFF_TIMEOUT)
#define get_led_powersave_timeout() (LED_AUTOOFF_TIME)
#endif
