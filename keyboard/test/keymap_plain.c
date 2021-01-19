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
	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_MS_WH_DOWN, KC_MS_WH_UP ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),

	KEYMAP_ENC(
		KC_ESC, KC_Z, \
		KC_X, KC_C, \
		KC_VOLD, KC_VOLU ),
};

const action_t fn_actions[] = {

};