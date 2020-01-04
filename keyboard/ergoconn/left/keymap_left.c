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
    KEYMAP_LEFT_ENC(
        KC_ESC, KC_1,   KC_2,   KC_3,   KC_4,   KC_5,  KC_MINS, \
        KC_TAB, KC_Q,   KC_W,   KC_E,   KC_R,   KC_T,  KC_NO  , \
        KC_CAPS,KC_A,   KC_S,   KC_D,   KC_F,   KC_G,           \
        KC_LSHIFT,KC_Z, KC_X,   KC_C,   KC_V,   KC_B,  KC_LBRC, \
        KC_LCTL,KC_LGUI,KC_LALT,KC_SPACE,KC_FN0,                \
                                       KC_VOLD,KC_VOLU,KC_MUTE, \
                                              KC_NO, KC_SPACE),
    /* 1: fn */
    KEYMAP_LEFT(
        KC_TRNS, KC_FN1,  KC_FN2,  KC_FN3,  KC_FN4, KC_TRNS, KC_TRNS, \
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          \
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                   \
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

