#ifndef __KEYBOARD_LED_H_
#define __KEYBOARD_LED_H_

#include <stdint.h>

void keyboard_led_set(uint8_t r, uint8_t g, uint8_t b);
void keyboard_led_init(void);

#endif //__KEYBOARD_LED_H_