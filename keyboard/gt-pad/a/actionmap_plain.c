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
#define AC_FN0	ACTION_LAYER_TAP(2, KC_NLCK)
#define AC_FN1	ACTION_FUNCTION_OPT(SWITCH_DEVICE, SWITCH_DEVICE_BLE_0)
#define AC_FN2	ACTION_FUNCTION_OPT(SWITCH_DEVICE, SWITCH_DEVICE_BLE_1)
#define AC_FN3	ACTION_FUNCTION_OPT(SWITCH_DEVICE, SWITCH_DEVICE_BLE_2)
#define AC_FN4	ACTION_FUNCTION_OPT(SWITCH_DEVICE, SWITCH_DEVICE_BLE_READV)
#define AC_FN5	ACTION_FUNCTION_OPT(SWITCH_DEVICE, SWITCH_DEVICE_BLE_REBOND)
#define AC_FN6	ACTION_FUNCTION_OPT(RGB_LIGHT_CONTROL, RGB_LIGHT_STEP)
#define AC_FN7  ACTION_FUNCTION_OPT(RGB_LIGHT_CONTROL, RGB_LIGHT_TOGGLE)
#define AC_FN8  ACTION_FUNCTION_OPT(RGB_LIGHT_CONTROL, RGB_LIGHT_TML)
#define AC_FN9	ACTION_FUNCTION_OPT(RGB_LIGHT_CONTROL, RGB_LIGHT_IHUE)
#define AC_FN10	ACTION_FUNCTION_OPT(RGB_LIGHT_CONTROL, RGB_LIGHT_DHUE)
#define AC_FN11	ACTION_LAYER_TAP(1, KC_PDOT)

/* 
 * @brief 按键映射
 * 下面的按键映射与按键布局的第一组实际布局对应
 */


const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS] = { 
	[0] = ACTIONMAP_ENC(
		FN0,  PSLS,  PAST, PMNS,\
		P7,   P8,    P9,   PPLS, \
		P4,   P5,    P6,  \
		P1,   P2,    P3,   PENT, \
		P0,   FN11, \
		VOLU, VOLD,  FN0 ),

	[1] = ACTIONMAP_ENC(
		TRNS,  FN1,   FN2,  FN3,\
		FN6,   FN7,   FN8,  FN4, \
		MYCM,  MAIL,  MSEL,  \
		MPRV,  MPLY,  MNXT, FN5, \
		BSPC,  CALC, \
		FN9,  FN10, TRNS),

	[2] = ACTIONMAP_ENC(  //Android电视遥控器
		NO,    NO,    VOLU, VOLD,\
		NO,    UP,    NO,   HOME, \
		LEFT,  ENTER, RIGHT,  \
		NO,    DOWN,  NO,   ESC, \
		APP,   NO, \
		TRNS,  TRNS,  TRNS ),
};
