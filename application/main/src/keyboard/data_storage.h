#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#ifdef ACTIONMAP_ENABLE
#define SINGLE_KEY_SIZE 2
#else
#define SINGLE_KEY_SIZE 1
#endif

#define MAX_LAYER 8
#define MAX_FN_KEYS 32
#define MAX_MACRO_SIZE 256
#define FILE_ID 0x0514

#define KEYMAP_ROW_SIZE (MATRIX_COLS * SINGLE_KEY_SIZE)
#define KEYMAP_LAYER_SIZE (MATRIX_ROWS * KEYMAP_ROW_SIZE)

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

#ifdef CONFIG_STORAGE
extern uint8_t config_block[];
#endif
