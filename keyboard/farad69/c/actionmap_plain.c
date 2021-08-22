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
#define AC_FN0  ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)
#define AC_FN1	ACTION_MODS_TAP_KEY(MOD_RSFT, KC_DOT)
#define AC_FN2	ACTION_LAYER_TAP(1, KC_SPC)
#define AC_FN3	ACTION_MODS_TAP_KEY(MOD_RCTL, KC_DEL)
#define AC_FN4	ACTION_FUNCTION(0)

/* 
 * @brief 按键映射
 * 下面的按键映射与按键布局的第一组实际布局对应
 */
const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS] = { 
	[0] = ACTIONMAP_ROT(
		ESC, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, MINS, EQL, NLCK, BSPC,
		TAB, Q, W, E, R, T, Y, U, I, O, P, BSLS, P7, P8, P9,
		FN0, A, S, D, F, G, H, J, K, L, ENT, P4, P5, P6, 
		LSFT, Z, X, C, V, B, N, M, COMM, FN1, UP, P1, P2, P3, 
		LCTL, LGUI, LALT, FN2, SPC, FN3, LEFT, DOWN, RGHT, P0, PDOT,
		VOLU, VOLD,  ESC),
	[1] = ACTIONMAP(
		GRV, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, PSCR, TRNS,
		FN4, BTN1, MS_U, BTN2, TRNS, TRNS, TRNS, TRNS, TRNS, LBRC, RBRC, TRNS, TRNS, INS, DEL,
		TRNS, MS_L, MS_D, MS_R, TRNS, TRNS, TRNS, TRNS, TRNS, SCLN, QUOT, MPRV, MPLY, MNXT, 
		TRNS, MYCM, CALC, TRNS, TRNS, TRNS, TRNS, TRNS, SLSH, TRNS, TRNS, VOLU, VOLD, MUTE, 
		TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, PGUP, PGDN),
};
