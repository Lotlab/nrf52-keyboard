/*
Copyright (C) 2020 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "image_header.h"

static const uint8_t image_data_Batt_0[58] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙█████████████████████████∙∙∙
    // ███████████████████████████∙∙
    // ███████████████████████████∙∙
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████
    // ███████████████████████████∙∙
    // ███████████████████████████∙∙
    // ∙█████████████████████████∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0xfc, 0xfe, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e,
    0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e,
    0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e,
    0x0e, 0x3e, 0xfc, 0xf0, 0xf0, 0x3f, 0x7f, 0x7f,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x7c, 0x3f,
    0x0f, 0x0f
};
const tImage Batt_0 = { image_data_Batt_0, 29, 16,
    8 };

static const uint8_t image_data_Batt_100[58] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙█████████████████████████∙∙∙
    // ███████████████████████████∙∙
    // ███████████████████████████∙∙
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████
    // ███∙████████████████████∙████
    // ███∙████████████████████∙∙███
    // ███∙████████████████████∙∙███
    // ███∙████████████████████∙∙███
    // ███∙████████████████████∙∙███
    // ███∙████████████████████∙████
    // ███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████
    // ███████████████████████████∙∙
    // ███████████████████████████∙∙
    // ∙█████████████████████████∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0xfc, 0xfe, 0xfe, 0x0e, 0xee, 0xee, 0xee, 0xee,
    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
    0x0e, 0x3e, 0xfc, 0xf0, 0xf0, 0x3f, 0x7f, 0x7f,
    0x70, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77, 0x77, 0x70, 0x7c, 0x3f,
    0x0f, 0x0f
};

const tImage Batt_100 = { image_data_Batt_100, 29, 16,
    8 };

static const uint8_t image_data_Charge[18] = {
    // ∙∙∙∙∙∙∙∙∙
    // ∙∙█∙∙∙█∙∙
    // ∙∙█∙∙∙█∙∙
    // ∙∙█∙∙∙█∙∙
    // ∙∙█∙∙∙█∙∙
    // █████████
    // ∙███████∙
    // ∙███████∙
    // ∙███████∙
    // ∙███████∙
    // ∙∙█████∙∙
    // ∙∙∙∙█∙∙∙∙
    // ∙∙∙∙█∙∙∙∙
    // ∙∙∙∙█∙∙∙∙
    // ∙∙∙∙∙███∙
    // ∙∙∙∙∙∙∙∙∙
    0x20, 0xe0, 0xfe, 0xe0, 0xe0, 0xe0, 0xfe, 0xe0,
    0x20, 0x00, 0x03, 0x07, 0x07, 0x3f, 0x47, 0x47,
    0x43, 0x00
};
const tImage Charge = { image_data_Charge, 9, 16,
    8 };

static const uint8_t image_data_Passkey[24] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████∙∙∙∙
    // ∙∙∙██████∙∙∙
    // ∙∙███∙∙███∙∙
    // ∙∙██∙∙∙∙██∙∙
    // ∙∙██∙∙∙∙██∙∙
    // ∙∙██∙∙∙∙██∙∙
    // ∙██████████∙
    // ████████████
    // ████████████
    // ████████████
    // ████████████
    // ████████████
    // ████████████
    // ∙██████████∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x80, 0xf8, 0xfc, 0x8e, 0x86, 0x86, 0x8e,
    0xfc, 0xf8, 0x80, 0x00, 0x3f, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x3f
};
const tImage Passkey = { image_data_Passkey, 12, 16,
    8 };

static const uint8_t image_data_Bluetooth[16] = {
    // ∙∙∙∙∙∙∙∙
    // ∙∙∙█∙∙∙∙
    // ∙∙∙██∙∙∙
    // █∙∙███∙∙
    // ██∙█∙██∙
    // ∙███∙∙██
    // ∙∙██∙██∙
    // ∙∙∙███∙∙
    // ∙∙∙███∙∙
    // ∙∙██∙██∙
    // ∙███∙∙██
    // ██∙█∙██∙
    // █∙∙███∙∙
    // ∙∙∙██∙∙∙
    // ∙∙∙█∙∙∙∙
    // ∙∙∙∙∙∙∙∙
    0x18, 0x30, 0x60, 0xfe, 0x8c, 0xd8, 0x70, 0x20,
    0x18, 0x0c, 0x06, 0x7f, 0x31, 0x1b, 0x0e, 0x04
};
const tImage Bluetooth = { image_data_Bluetooth, 8, 16,
    8 };

static const uint8_t image_data_Gazell[28] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙████∙∙
    // ∙∙∙∙∙∙∙██████∙
    // ∙∙∙∙∙∙███∙∙███
    // ∙∙∙∙∙███∙∙∙∙██
    // ∙∙∙∙███∙∙∙∙∙██
    // ∙∙∙∙██∙██∙∙███
    // ∙∙█∙██∙███∙██∙
    // ∙██∙███∙██∙█∙∙
    // ███∙∙██∙██∙∙∙∙
    // ██∙∙∙∙∙███∙∙∙∙
    // ██∙∙∙∙███∙∙∙∙∙
    // ███∙∙███∙∙∙∙∙∙
    // ∙██████∙∙∙∙∙∙∙
    // ∙∙████∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x80, 0x00, 0xe0, 0xf0, 0x38, 0xdc,
    0xce, 0x86, 0x06, 0xce, 0xfc, 0x78, 0x1e, 0x3f,
    0x73, 0x60, 0x61, 0x73, 0x3b, 0x1c, 0x0f, 0x07,
    0x00, 0x01, 0x00, 0x00
};
const tImage Gazell = { image_data_Gazell, 14, 16,
    8 };

static const uint8_t image_data_USB[18] = {
    // ∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█∙∙∙∙
    // ∙∙∙███∙∙∙
    // ∙∙∙∙█∙∙∙∙
    // ∙∙∙∙█∙███
    // ∙█∙∙█∙███
    // ███∙█∙∙█∙
    // ∙█∙∙█∙∙█∙
    // ∙█∙∙███∙∙
    // ∙∙█∙█∙∙∙∙
    // ∙∙∙██∙∙∙∙
    // ∙∙∙∙█∙∙∙∙
    // ∙∙∙███∙∙∙
    // ∙∙∙███∙∙∙
    // ∙∙∙███∙∙∙
    // ∙∙∙∙∙∙∙∙∙
    0x40, 0xe0, 0x40, 0x04, 0xfe, 0x04, 0x30, 0xf0,
    0x30, 0x00, 0x01, 0x02, 0x74, 0x7f, 0x71, 0x01,
    0x00, 0x00
};
const tImage USB = { image_data_USB, 9, 16,
    8 };

static const uint8_t image_data_NumLock[22] = {
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙█████████∙
    // ███████████
    // █████∙∙████
    // ████∙∙∙████
    // ███∙∙∙∙████
    // ███∙∙∙∙████
    // █████∙∙████
    // █████∙∙████
    // █████∙∙████
    // █████∙∙████
    // ██∙∙∙∙∙∙∙██
    // ██∙∙∙∙∙∙∙██
    // ███████████
    // ∙█████████∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    0xfc, 0xfe, 0xfe, 0x9e, 0x8e, 0x06, 0x06, 0xfe,
    0xfe, 0xfe, 0xfc, 0x3f, 0x7f, 0x67, 0x67, 0x67,
    0x60, 0x60, 0x67, 0x67, 0x7f, 0x3f
};
const tImage NumLock = { image_data_NumLock, 11, 16,
    8 };

static const uint8_t image_data_CapsLock[22] = {
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙█████████∙
    // ███████████
    // ████∙∙∙████
    // ███∙∙∙∙∙███
    // ███∙∙█∙∙███
    // ██∙∙∙█∙∙∙██
    // ██∙∙███∙∙██
    // ██∙∙███∙∙██
    // ██∙∙∙∙∙∙∙██
    // ██∙∙∙∙∙∙∙██
    // ██∙∙███∙∙██
    // ██∙∙███∙∙██
    // ███████████
    // ∙█████████∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    0xfc, 0xfe, 0x3e, 0x0e, 0x86, 0xe6, 0x86, 0x0e,
    0x3e, 0xfe, 0xfc, 0x3f, 0x7f, 0x60, 0x60, 0x79,
    0x79, 0x79, 0x60, 0x60, 0x7f, 0x3f
};
const tImage CapsLock = { image_data_CapsLock, 11, 16,
    8 };

static const uint8_t image_data_ScrollLock[22] = {
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙█████████∙
    // ███████████
    // █████∙█████
    // █████∙█████
    // █████∙█████
    // █████∙█████
    // █████∙█████
    // ██∙██∙██∙██
    // ██∙∙█∙█∙∙██
    // ███∙∙∙∙∙███
    // ████∙∙∙████
    // ██∙∙∙∙∙∙∙██
    // ███████████
    // ∙█████████∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    0xfc, 0xfe, 0xfe, 0xfe, 0xfe, 0x06, 0xfe, 0xfe,
    0xfe, 0xfe, 0xfc, 0x3f, 0x7f, 0x6c, 0x69, 0x63,
    0x60, 0x63, 0x69, 0x6c, 0x7f, 0x3f
};
const tImage ScrollLock = { image_data_ScrollLock, 11, 16,
    8 };

const tImage* Icons[] = {
    &Batt_0,
    &Batt_100,
    &Charge,
    &Passkey,
    &Bluetooth,
    &Gazell,
    &USB,
    &NumLock,
    &CapsLock,
    &ScrollLock
};

/**
 * @brief 图标的Index
 * 
 */
enum icon_index {
    ICON_BATT_0,
    ICON_BATT_100,
    ICON_CHARGE,
    ICON_PASSKEY,
    ICON_BLUETOOTH,
    ICON_GAZELL,
    ICON_USB,
    ICON_NUMLOCK,
    ICON_CAPSLOCK,
    ICON_SCROLLLOCK,
};

#define ICON_BATT_START 4
#define ICON_BATT_END 24
