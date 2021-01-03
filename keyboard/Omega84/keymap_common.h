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
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K69, K68, K67, K66, K65, K70, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K64, K63, K62, K61,      K60, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K79, K78, K77, K76,      K75, \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K74, K73, K72,           K71, \
	K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K89, K88,           K87, K86, \
	K50, K51, K52,                K53,           K58, K59, K85,      K84, K83, K82  \
) { \
	{ KC_##K00, KC_##K01, KC_##K02, KC_##K03, KC_##K04, KC_##K05, KC_##K06, KC_##K07, KC_##K08, KC_##K09 }, \
	{ KC_##K10, KC_##K11, KC_##K12, KC_##K13, KC_##K14, KC_##K15, KC_##K16, KC_##K17, KC_##K18, KC_##K19 }, \
	{ KC_##K20, KC_##K21, KC_##K22, KC_##K23, KC_##K24, KC_##K25, KC_##K26, KC_##K27, KC_##K28, KC_##K29 }, \
	{ KC_##K30, KC_##K31, KC_##K32, KC_##K33, KC_##K34, KC_##K35, KC_##K36, KC_##K37, KC_##K38, KC_##K39 }, \
	{ KC_##K40, KC_##K41, KC_##K42, KC_##K43, KC_##K44, KC_##K45, KC_##K46, KC_##K47, KC_##K48, KC_##K49 }, \
	{ KC_##K50, KC_##K51, KC_##K52, KC_##K53, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##K58, KC_##K59 }, \
	{ KC_##K60, KC_##K61, KC_##K62, KC_##K63, KC_##K64, KC_##K65, KC_##K66, KC_##K67, KC_##K68, KC_##K69 }, \
	{ KC_##K70, KC_##K71, KC_##K72, KC_##K73, KC_##K74, KC_##K75, KC_##K76, KC_##K77, KC_##K78, KC_##K79 }, \
	{ KC_NO,    KC_NO,    KC_##K82, KC_##K83, KC_##K84, KC_##K85, KC_##K86, KC_##K87, KC_##K88, KC_##K89 }  \
}


/* ANSI variant. No extra keys for ISO */
#define KEYMAP_ANSI( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K69, K68, K67, K66, K65, K70, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K64, K63, K62, K61,      K60, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K79, K78, K77, K76,      K75, \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K74, K73, K72,           K71, \
	K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K89, K88,           K87, K86, \
	K50, K51, K52,                K53,           K58, K59, K85,      K84, K83, K82  \
) KEYMAP( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K69, K68, K67, K66, K65, K70, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K64, K63, K62, K61,      K60, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K79, K78, K77, K76,      K75, \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K74, K73, K72,           K71, \
	K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K89, K88,           K87, K86, \
	K50, K51, K52,                K53,           K58, K59, K85,      K84, K83, K82  \
)


#define KEYMAP_HHKB( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K69, K68, K67, K66, K65, K70, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K64, K63, K62, K61,      K60, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K79, K78, K77, K76,      K75, \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K74, K73, K72,           K71, \
	K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K89, K88,           K87, K86, \
	K50, K51, K52,                K53,           K58, K59, K85,      K84, K83, K82  \
) KEYMAP( \
	K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K69, K68, K67, K66, K65, K70, \
	K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K64, K63, K62, K61,      K60, \
	K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K79, K78, K77, K76,      K75, \
	K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K74, K73, K72,           K71, \
	K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K89, K88,           K87, K86, \
	K50, K51, K52,                K53,           K58, K59, K85,      K84, K83, K82  \
)
