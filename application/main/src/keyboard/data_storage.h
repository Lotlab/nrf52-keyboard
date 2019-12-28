#pragma once
#include <stdint.h>
#include <stdbool.h>

enum storage_type {
    STORAGE_KEYMAP,
    STORAGE_FN,
    STORAGE_MACRO,
    STORAGE_CONFIG
};

void storage_read(uint8_t type);
void storage_init(void);
bool storage_write(uint8_t type);
uint16_t storage_read_data(uint8_t type, uint16_t offset, uint16_t len, uint8_t* data);
uint16_t storage_write_data(uint8_t type, uint16_t offset, uint16_t len, uint8_t* data);
