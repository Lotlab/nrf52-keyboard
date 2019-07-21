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
#include "user_fn.h"

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: default */
    KEYMAP(
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   BSPC, \
        CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN, ENT,  \
        LSFT,Z,   X,   C,   V,   B,   N,   M,   COMM,FN2,       RSFT,  \
        LCTL,LGUI,LALT,FN1,     SPC,           LEFT, DOWN, RGHT, FN0),
    /* 1: qwerty */
	KEYMAP(
        ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   BSPC, \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS, FN7,  \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,VOLD,VOLU,MUTE,     TRNS, \
        TRNS,TRNS,TRNS,TRNS,     TRNS,          TRNS,TRNS,TRNS,TRNS),
    /* 2: Poker Fn */
    KEYMAP(
        F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11,  F12,  \
        FN5,TRNS,TRNS,FN6 ,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  FN3,  \
        FN4,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,VOLD,VOLU,MUTE,      TRNS, \
        TRNS,TRNS,TRNS,TRNS,     TRNS,          TRNS,TRNS,TRNS,TRNS),
};
const action_t PROGMEM fn_actions[] = {
    /* Poker Layout */
    [0] = ACTION_LAYER_TAP_KEY(2, KC_DELETE),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPACE),
    [2] = ACTION_FUNCTION(AF_TRICKY_SLSH),
    [3] = ACTION_FUNCTION(AF_POWER_SLEEP),
    [4] = ACTION_FUNCTION(AF_SWITCH_MODE),
    [5] = ACTION_FUNCTION(AF_POWER_OFF),
    [6] = ACTION_FUNCTION(AF_DELETE_BOND),
    [7] = ACTION_FUNCTION(AF_LED_DISPLAY),
};

