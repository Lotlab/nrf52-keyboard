#pragma once
#include <stdint.h>
#include <stdbool.h>

void keymap_init(void);
bool keymap_set(uint8_t block, uint8_t size, uint8_t* data);
