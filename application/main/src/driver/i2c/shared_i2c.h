#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "nrfx_twi.h"

nrfx_twi_t* shared_i2c_init(uint8_t sda, uint8_t scl);
void shared_i2c_write(nrfx_twi_t* channel, uint8_t addr, uint8_t len, const uint8_t* data, uint8_t cmd, bool sequence);
