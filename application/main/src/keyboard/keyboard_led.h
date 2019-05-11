#ifndef __KEYBOARD_LED_H_
#define __KEYBOARD_LED_H_

#include <stdint.h>
#include "../config/keyboard_config.h"

#ifdef LED_RGB
void keyboard_led_rgb_set(uint8_t r, uint8_t g, uint8_t b);
#endif

void keyboard_led_set(uint8_t led_val);
void keyboard_led_init(void);

#endif //__KEYBOARD_LED_H_