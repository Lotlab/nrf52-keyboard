/*
 Copyright (C)  2021,2022 Geno <geno@live.com>

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
 * 键盘矩阵配置文件
 * 
 * 定义各个按键的位置，方便编写Keymap/Actionmap
 */
#pragma once

#include <stdint.h>
#include "actionmap.h"
#include "config.h"

extern const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS];

/* 
 * @brief 按键阵列
 * 第一组为实际布局
 * 第二组为按键实际矩阵
 */
#define ACTIONMAP_ENC( \
	K01, K02, K03, K04, K05, K06, K07, K10, K12, K13, K14, K15, \
	K16, K17, K20, K21, K23, K24, K25, K26, K27, K30, K31, K32, \
	K34, K35, K36, K37, K40, K41, K42, K43, K45, K46, K47, K50, \
	K52, K53, K54, K56, K57,    K60,   K61, K62, K63, K64, K65, \
	R0P, R0N, K68 \
) { \
	{ AC_##R0P, AC_##K01, AC_##K02, AC_##K03, AC_##K04, AC_##K05, AC_##K06, AC_##K07, AC_NO   }, \
	{ AC_##K10, AC_##R0N, AC_##K12, AC_##K13, AC_##K14, AC_##K15, AC_##K16, AC_##K17, AC_NO   }, \
	{ AC_##K20, AC_##K21, AC_NO   , AC_##K23, AC_##K24, AC_##K25, AC_##K26, AC_##K27, AC_NO   }, \
	{ AC_##K30, AC_##K31, AC_##K32, AC_NO   , AC_##K34, AC_##K35, AC_##K36, AC_##K37, AC_NO   }, \
	{ AC_##K40, AC_##K41, AC_##K42, AC_##K43, AC_NO   , AC_##K45, AC_##K46, AC_##K47, AC_NO   }, \
	{ AC_##K50, AC_NO   , AC_##K52, AC_##K53, AC_##K54, AC_NO   , AC_##K56, AC_##K57, AC_NO   }, \
	{ AC_##K60, AC_##K61, AC_##K62, AC_##K63, AC_##K64, AC_##K65, AC_NO   , AC_NO   , AC_##K68}, \
	{ AC_NO   , AC_NO   , AC_NO   , AC_NO   , AC_NO   , AC_NO   , AC_NO   , AC_NO   , AC_NO   }, \
}

