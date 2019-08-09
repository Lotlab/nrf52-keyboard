#pragma once

#include <stdbool.h>

void ble_keyboard_init(void);
void ble_keyboard_timer_start(void);
void ble_keyboard_powersave(bool save);
void keyboard_debounce(void);
