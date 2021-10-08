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

#define XXX KC_NO  // maKes the switch matrix easier to read

// This layout is supported by alicego  revisions
#define LAYOUT_ALICEGO( \
            K01, K02, K03, K04, K05, K06, K07,      K08, K09, K0A, K0B, K0C, K0D,      K0E,  \
      K10,  K11,  K12, K13, K14, K15, K16,        K17, K18, K19, K1A, K1B, K1C, K1D,   K1E,\
     K20,   K21,   K22, K23, K24, K25, K26,        K27, K28, K29, K2A, K2B, K2C,    K2D,\
    K30,    K31,    K32, K33, K34, K35, K36,     K37, K38, K39, K3A,  K3B,  K3C,    K3D,     \
   K40,    K41,   K42,   K43,      K45, KVU,  KVD,    K48,      K4A, K4B,      K4C, K4D, K4E  \
   ) { \
    { XXX, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, XXX }, \
    { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, XXX }, \
    { K40, K41, K42, K43, XXX, K45, KVU, KVD, K48, XXX, K4A, K4B, K4C, K4D, K4E }  \
}

// This layout is supported by alicego  revisions
// 注意K1E和K4E的矩阵位置（布线位置）
#define LAYOUT_ALICEGO1( \
            K00, K01, K02, K03, K04, K05, K06, K07,      K08, K09, K0A, K0B, K0C,      K0D, \
      K10,  K11,  K12, K13, K14, K15, K16,        K17, K18, K19, K1A, K1B, K1C, K1D,   K1E,\
     K20,   K21,   K22, K23, K24, K25, K26,        K27, K28, K29, K2A, K2B, K2C,    K2D,\
    K30,    K31,    K32, K33, K34, K35, K36,     K37, K38, K39, K3A,  K3B,  K3C,    K3D,     \
   K40,    K41,   K42,   K43,      K45, KVU,  KVD,    K48,      K4A, K4B,      K4C, K4D, K4E  \
   ) { \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D }, \
    { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D }, \
    { K40, K41, K42, K43, K1E, K45, KVU, KVD, K48, K4E, K4A, K4B, K4C, K4D }  \
}

// This layout is supported by alicego_dull  revisions
// 注意K1E和K4E的矩阵位置（布线位置）,节省两个引脚
#define LAYOUT_ALICEGO_DULL( \
            K00, K01, K02, K03, K04, K05, K06, K07,      K08, K09, K0A, K0B, K0C,      K0D, \
      K10,  K11,  K12, K13, K14, K15, K16,        K17, K18, K19, K1A, K1B, K1C, K1D,   K1E,\
     K20,   K21,   K22, K23, K24, K25, K26,        K27, K28, K29, K2A, K2B, K2C,    K2D,\
    K30,    K31,    K32, K33, K34, K35, K36,     K37, K38, K39, K3A,  K3B,  K3C,    K3D,     \
   K40,    K41,   K42,   K43,      K45,   KVU,  KVD,  K48,      K4A, K4B,      K4C, K4D, K4E \
   ) { \
	{ K00, K01, K02, K03, K04, K05, K06 }, \
	{ K10, K11, K12, K13, K14, K15, K16 }, \
	{ K20, K21, K22, K23, K24, K25, K26 }, \
	{ K30, K31, K32, K33, K34, K35, K36 }, \
	{ K40, K41, K42, K43, K1E, K45, KVU }, \
	{ K0D, K0C, K0B, K0A, K09, K08, K07, },\
	{ K1D, K1C, K1B, K1A, K19, K18, K17, },\
	{ K2D, K2C, K2B, K2A, K29, K28, K27, },\
	{ K3D, K3C, K3B, K3A, K39, K38, K37, },\
	{ K4D, K4C, K4B, K4A, K4E, K48, KVD, } \
}

// Layout exposing all Keys (adding the ISO-style split left shift, which is supported starting rev3)
// This layout is supported by bmek revisions
#define LAYOUT_BMEK(\
       K00, K01, K02, K03, K04, K05, K06, K07,     K08, K09, K0A, K0B, K0C, K0D, K0E, K1E,\
      K10,  K11,  K12, K13, K14, K15, K16,        K17, K18, K19, K1A, K1B, K1C, K1D,   K2E,\
     K20,   K21,   K22, K23, K24, K25, K26,        K27, K28, K29, K2A, K2B,     K2C,    K2D,\
    K30,  K31, K41, K32,   K33, K34, K35, K36,   K37, K38, K39, K3A,  K3B, K3C,  K3D,    K3E,\
                    K42,   K43,      K45,             K48,      K4A,  K4B                     \
   ) { \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E }, \
    { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E }, \
    { XXX, K41, K42, K43, XXX, K45, XXX, XXX, K48, XXX, K4A, K4B, XXX, XXX, XXX }  \
}
