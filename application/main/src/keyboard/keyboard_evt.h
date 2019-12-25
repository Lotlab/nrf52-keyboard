#pragma once
#include <stdint.h>
#include "../main.h"

#include <stdlib.h>
#include "nrf_section.h"

typedef void (*UserEventHandler)(uint8_t, void*);
#define EVENT_HANDLER(_func) NRF_SECTION_ITEM_REGISTER(modules_init, const UserEventHandler _pt_##_func) = &_func
