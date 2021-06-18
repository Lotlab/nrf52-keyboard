/*
 Copyright (C) 2021,2022 Geno <geno@live.com>

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

#include "actionmap.h"
#include "action_code.h"
#include "actionmap_common.h"
#include "keyboard_fn.h"
#include "user_fn.h"

/* 
 * 定义需使用的FN按键
 */
#define AC_FN0     ACTION_LAYER_TAP(1, KC_SPC)
#define AC_FN1     ACTION_LAYER_TAP(2, KC_DEL)
#define AC_FN2     ACTION_LAYER_TAP(3, KC_APP)

/* 
 * @brief 按键映射
 * 下面的按键映射与按键布局的第一组实际布局对应
 */
const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS] = { 
	[0] = ACTIONMAP(
		ESC, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, BSPC, 
		TAB, Q, W, E, R, T, Y, U, I, O, P, LBRC, 
		CAPS, A, S, D, F, G, H, J, K, L, SCLN, ENT, 
		LSFT, Z, X, C, V, B, N, M, COMM, DOT, SLSH, RSFT, 
		LCTL, LGUI, LALT, FN0, SPC, FN1, RALT, FN2, RCTL, RBRC),

	[1] = ACTIONMAP(
		GRV, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, BSLS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, MINS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, QUOT, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, UP, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, LEFT, DOWN, RGHT, EQL),

	[2] = ACTIONMAP(
		TRNS, F11, F12, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, VOLD, VOLU, MUTE, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS),

	[3] = ACTIONMAP(
		TRNS, MYCM, MAIL, CALC, MSEL, TRNS, TRNS, TRNS, MPLY, MPRV, MNXT, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS)
};
