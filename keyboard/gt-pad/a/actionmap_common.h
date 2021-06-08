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
	K00, K01, K02, K03,    \
	K10, K11, K12, K13,    \
	K20, K21, K22,         \
	K30, K31, K32, K33,    \
	K40, K42,              \
	R0P, R0N, K44\
) { \
	{ AC_##K00, AC_##K01, AC_##K02, AC_##K03,   AC_NO }, \
	{ AC_##K10, AC_##K11, AC_##K12, AC_##K13,   AC_NO }, \
	{ AC_##K20, AC_##K21, AC_##K22, AC_NO,      AC_NO }, \
	{ AC_##K30, AC_##K31, AC_##K32, AC_##K33,   AC_NO }, \
	{ AC_##K40, AC_##R0P, AC_##K42, AC_##R0N,   AC_##K44   }, \
}

