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
    KEYMAP_ANSI(
        ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSPC, \
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,BSLS, \
        CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     ENT,  \
        LSFT,     Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     FN4,  \
        LCTL,LGUI,LALT,          SPC,                     FN0, FN1, FN2, FN3),
    /* 1: qwerty */
	    KEYMAP_ANSI(
        ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSPC, \
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,BSLS, \
        CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     ENT,  \
        LSFT,     Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RSFT, \
        LCTL,LGUI,LALT,          SPC,                     FN0, RALT, FN8, RCTL),
    /* 2: Poker Fn */
    KEYMAP_ANSI(
        GRV, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, DEL,  \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,PGUP,PGDN,INS,  \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,HOME,END,      TRNS, \
        TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,VOLD,VOLU,MUTE,     UP,   \
        TRNS,TRNS,TRNS,          TRNS,                    TRNS,LEFT,DOWN,RGHT),
	  /* 3: system key */
	  KEYMAP_ANSI(
        FN7, MYCM,MAIL,CALC,MSEL,TRNS,TRNS,TRNS,MPLY,MPRV,MNXT,MRWD,MFFD,FN5, \
        FN6, TRNS,TRNS,FN9 ,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,PGUP,PGDN,INS,  \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,HOME,END,      FN10, \
        TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,VOLD,VOLU,MUTE,     TRNS, \
        TRNS,TRNS,TRNS,          TRNS,                    TRNS,TRNS,TRNS,TRNS),
};
const action_t PROGMEM fn_actions[] = {
    /* Poker Layout */
    [0] = ACTION_LAYER_TAP_KEY(2, KC_DELETE),
    [1] = ACTION_MODS_TAP_KEY(MOD_RALT, KC_LEFT),
    [2] = ACTION_LAYER_TAP_KEY(3, KC_DOWN),
    [3] = ACTION_MODS_TAP_KEY(MOD_RCTL, KC_RIGHT),
    [4] = ACTION_MODS_TAP_KEY(MOD_RSFT, KC_UP),
    [5] = ACTION_FUNCTION(AF_POWER_SLEEP),
    [6] = ACTION_FUNCTION(AF_SWITCH_MODE),
    [7] = ACTION_FUNCTION(AF_POWER_OFF),
    [8] = ACTION_LAYER_TAP_KEY(3, KC_APP),
    [9] = ACTION_FUNCTION(AF_DELETE_BOND),
    [10] = ACTION_FUNCTION(AF_LED_DISPLAY),
};

