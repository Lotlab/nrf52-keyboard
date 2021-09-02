#pragma once

#include <stdbool.h>

void keyboard_services_init(void);
void keyboard_timer_start(void);
void keyboard_powersave(bool save);
void keyboard_debounce(void);
