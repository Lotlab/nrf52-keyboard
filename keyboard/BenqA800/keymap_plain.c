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

/* keymap for Benq A800 */
const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: qwerty */
    KEYMAP( 
        NO,   ESC, F12,  NO,   H,  F6,   G,SLSH,QUOT,  F5,  F4,  P0,PDOT,  NO,  NO,LALT,  NO,  NO,MPRV,
        LSFT, TAB,CAPS,  F3,   Y,  F7,   T,RBRC,LBRC,BSPC,  P4,  P5,  P6,PPLS,  NO,VOLU,  NO,  NO,MNXT,
        NO,   GRV,  F1,  F2,  F9,  F8,   5, EQL,MINS,   6, INS, DEL,PGUP,HOME,  NO,MUTE,LCTL,  NO,WHOM,
        NO,     1, F11,PSCR, F10,   9,   4,   8,   0,   7,   2,   3,PGDN, END,  NO,  NO,  NO,  NO,MAIL,
        RSFT,   Q,   W,  NO,   U,   O,   R,   I,   P,  NO,  P7,  P8,  P9,   E,VOLD,  NO,  NO,  NO,WFAV,
        NO,     A,   S,   D,   J,   L,   F,   K,SCLN,  NO,  P1,  P2,  P3,PENT,LGUI,  NO,  NO,  NO,  NO,
        NO,     Z,   X,   C,   M, DOT,   V,COMM,  NO, ENT,NLCK,RGHT,PAST,LEFT,  NO,  NO,RCTL,  NO,  NO,
        NO,    NO,  NO, SPC,   N,  NO,   B,  NO,BSLS,  UP,PSLS,DOWN,PMNS, APP,  NO,RALT,  NO,  NO,  NO)
};
const action_t fn_actions[] = {
    /* Poker Layout */
    // ACTION_LAYER_MOMENTARY(1),  // to Fn overlay
    ACTION_FUNCTION(KEYBOARD_CONTROL), // sleep
    ACTION_FUNCTION(SWITCH_DEVICE) // switch between usb and ble
};
