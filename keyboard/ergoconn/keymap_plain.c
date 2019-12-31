/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

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

#include "keymap_common.h"
#include "keyboard_fn.h"

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: qwerty */
    KEYMAP_RIGHT(
        KC_EQL, KC_6,   KC_7,   KC_8,   KC_9,   KC_0,  KC_BSPC, \
        KC_QUOT,KC_Y,   KC_U,   KC_I,   KC_O,   KC_P,  KC_BSLS, \
                KC_H,   KC_J,   KC_K,   KC_L,  KC_SCLN, KC_ENT, \
        KC_NO , KC_N,   KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, \
                       KC_FN0,KC_RALT, KC_RGUI, KC_APP, KC_RCTL, \
        KC_NO, \
        KC_FN0, KC_SPACE),
    /* 1: fn */
    KEYMAP_RIGHT(
        KC_TRNS, KC_FN1,  KC_FN2,  KC_FN3,  KC_FN4, KC_TRNS, KC_TRNS, \
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
        KC_TRNS, \
        KC_TRNS, KC_TRNS),
};
const action_t fn_actions[] = {
    /* Poker Layout */
    ACTION_LAYER_MOMENTARY(1),  // to Fn overlay
    ACTION_FUNCTION_OPT(KEYBOARD_CONTROL, CONTROL_SLEEP), // sleep
    ACTION_FUNCTION_OPT(KEYBOARD_CONTROL, CONTROL_NKRO), // nkro
    ACTION_FUNCTION_OPT(KEYBOARD_CONTROL, CONTROL_BOOTLOADER), // bootloader
    ACTION_FUNCTION(SWITCH_DEVICE) // switch between usb and ble
};

