/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * 键盘矩阵基配置文件
 * 
 * 定义各个按键的位置，方便编写Keymap
 */
#pragma once

#include <stdint.h>
#include "keymap.h"
#include "config.h"

extern const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
extern const action_t fn_actions[];

/* GH60 keymap definition macro
 * K2C, K31 and  K3C are extra keys for ISO
 */
#define KEYMAP( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,      \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C,      \
	K40, K41, K42, K43,           K46, K47, K48, K49, K4A, K4B  \
) { \
	{ K00,    K01,    K02,    K03,    K04,    K05,    K06,    K07,    K08,    K09,    K0A,    K0B,    K0C,    K0D }, \
	{ K10,    K11,    K12,    K13,    K14,    K15,    K16,    K17,    K18,    K19,    K1A,    K1B,    K1C,    K1D }, \
	{ K20,    K21,    K22,    K23,    K24,    K25,    K26,    K27,    K28,    K29,    K2A,    K2B,    K2C,    KC_NO }, \
	{ K30,    K31,    K32,    K33,    K34,    K35,    K36,    K37,    K38,    K39,    K3A,    K3B,    K3C,    KC_NO }, \
	{ K40,    K41,    K42,    K43,    KC_NO,  KC_NO,  K46,    K47,    K48,    K49,    K4A,    K4B,    KC_NO,  KC_NO }  \
}

/* ANSI variant. No extra keys for ISO */
#define KEYMAP_ANSI( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,      \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C,      \
	K40, K41, K42, K43,           K46, K47, K48, K49, K4A, K4B  \
) KEYMAP( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,      \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C,      \
	K40, K41, K42, K43,           K46, K47, K48, K49, K4A, K4B  \
)


#define KEYMAP_HHKB( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,      \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C,      \
	K40, K41, K42, K43,           K46, K47, K48, K49, K4A, K4B  \
) KEYMAP( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,      \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C,      \
	K40, K41, K42, K43,           K46, K47, K48, K49, K4A, K4B  \
)
