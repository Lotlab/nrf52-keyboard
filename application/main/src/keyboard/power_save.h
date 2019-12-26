#pragma once

#include "config.h"
#include <stdbool.h>
#include "../main.h"

#if LED_AUTOOFF_TIME > 0
void power_save_set_mode(bool on);
void power_save_reset();
#else
#define power_save_set_mode(X) 
#define power_save_reset() 
#endif