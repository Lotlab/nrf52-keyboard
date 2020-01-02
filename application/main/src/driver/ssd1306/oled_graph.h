#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "image_header.h"

#define ICON_SPACING 4

enum text_align {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT,
};

enum connection_type {
    CONN_TYPE_NONE,
    CONN_TYPE_BLE,
    CONN_TYPE_USB,
    CONN_TYPE_GAZELL,
};

void oled_clear_row(uint8_t row);
void oled_draw_text_5x8(uint8_t row, enum text_align align, uint8_t offset, const char* str);
void oled_draw_text_16(uint8_t row, enum text_align align, uint8_t offset, const char* str);
void oled_draw_icons(uint8_t row, uint8_t batt, bool charging, enum connection_type conn, bool passkey, uint8_t led);

