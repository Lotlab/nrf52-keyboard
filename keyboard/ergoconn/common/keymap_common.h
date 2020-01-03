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

/**
 * @brief 右手的按键阵列
 * 
 */
#define KEYMAP_RIGHT( \
    K00, K01, K02, K03, K04, K05, K06,\
    K10, K11, K12, K13, K14, K15, K16,\
         K21, K22, K23, K24, K25, K26,\
    K30, K31, K32, K33, K34, K35, K36,\
              K42, K43, K44, K45, K46,\
    K50, \
    K70, K71\
) { \
   {K00  , K01  , K02  , K03  , K04  , K05  , K06  }, \
   {K10  , K11  , K12  , K13  , K14  , K15  , K16  }, \
   {KC_NO, K21  , K22  , K23  , K24  , K25  , K26  }, \
   {K30  , K31  , K32  , K33  , K34  , K35  , K36  }, \
   {KC_NO, KC_NO, K42  , K43  , K44  , K45  , K46  }, \
   {KC_NO, K50  , KC_NO, K71  , K70  , KC_NO, KC_NO}, \
}

/**
 * @brief 右手的按键阵列
 * 
 */
#define KEYMAP_RIGHT_ENC( \
    K00, K01, K02, K03, K04, K05, K06,\
    K10, K11, K12, K13, K14, K15, K16,\
         K21, K22, K23, K24, K25, K26,\
    K30, K31, K32, K33, K34, K35, K36,\
              K42, K43, K44, K45, K46,\
    K50, R0P, R0N, \
    K70, K71\
) { \
   {K00  , K01  , K02  , K03  , K04  , K05  , K06  }, \
   {K10  , K11  , K12  , K13  , K14  , K15  , K16  }, \
   {KC_NO, K21  , K22  , K23  , K24  , K25  , K26  }, \
   {K30  , K31  , K32  , K33  , K34  , K35  , K36  }, \
   {KC_NO, KC_NO, K42  , K43  , K44  , K45  , K46  }, \
   {KC_NO, K50  , KC_NO, K71  , K70  , R0P  , R0N  }, \
}

/**
 * @brief 右手分裂一个空位
 * 
 */
#define KEYMAP_RIGHT_SP( \
    K00, K01, K02, K03, K04, K05, K06,\
    K10, K11, K12, K13, K14, K15, K16,\
         K21, K22, K23, K24, K25, K26,\
    K30, K31, K32, K33, K34, K35, K36,\
              K42, K43, K44, K45, K46,\
    K50, \
    K60, \
    K70, K71\
) { \
   {K00  , K01  , K02  , K03  , K04  , K05  , K06  }, \
   {K10  , K11  , K12  , K13  , K14  , K15  , K16  }, \
   {KC_NO, K21  , K22  , K23  , K24  , K25  , K26  }, \
   {K30  , K31  , K32  , K33  , K34  , K35  , K36  }, \
   {K60  , KC_NO, K42  , K43  , K44  , K45  , K46  }, \
   {KC_NO, K50  , KC_NO, K71  , K70  , KC_NO, KC_NO}, \
}

/**
 * @brief 左手的按键阵列
 * 
 */
#define KEYMAP_LEFT( \
    K00, K01, K02, K03, K04, K05, K06,\
    K10, K11, K12, K13, K14, K15, K16,\
    K20, K21, K22, K23, K24, K25,     \
    K30, K31, K32, K33, K34, K35, K36,\
    K40, K41, K42, K43, K44,          \
                                  K56,\
                             K75, K76\
) { \
   {K06  , K05  , K04  , K03  , K02  , K01  , K00  }, \
   {K16  , K15  , K14  , K13  , K12  , K11  , K10  }, \
   {KC_NO, K25  , K24  , K23  , K22  , K21  , K20  }, \
   {K36  , K35  , K34  , K33  , K32  , K31  , K30  }, \
   {KC_NO, KC_NO, K44  , K43  , K42  , K41  , K40  }, \
   {KC_NO, K56  , KC_NO, K75  , K76  , KC_NO, KC_NO}, \
}

/**
 * @brief 左手的按键阵列
 * 
 */
#define KEYMAP_LEFT_ENC( \
    K00, K01, K02, K03, K04, K05, K06,\
    K10, K11, K12, K13, K14, K15, K16,\
    K20, K21, K22, K23, K24, K25,     \
    K30, K31, K32, K33, K34, K35, K36,\
    K40, K41, K42, K43, K44,          \
                        R0N, R0P, K56,\
                             K75, K76\
) { \
   {K06  , K05  , K04  , K03  , K02  , K01  , K00  }, \
   {K16  , K15  , K14  , K13  , K12  , K11  , K10  }, \
   {KC_NO, K25  , K24  , K23  , K22  , K21  , K20  }, \
   {K36  , K35  , K34  , K33  , K32  , K31  , K30  }, \
   {KC_NO, KC_NO, K44  , K43  , K42  , K41  , K40  }, \
   {KC_NO, K56  , KC_NO, K75  , K76  , R0N  , R0P  }, \
}

/**
 * @brief 左手分裂一个空位
 * 
 */
#define KEYMAP_LEFT_SP( \
    K00, K01, K02, K03, K04, K05, K06,\
    K10, K11, K12, K13, K14, K15, K16,\
    K20, K21, K22, K23, K24, K25,     \
    K30, K31, K32, K33, K34, K35, K36,\
    K40, K41, K42, K43, K44,          \
                                  K56,\
                                  K66,\
                             K75, K76\
) { \
   {K06  , K05  , K04  , K03  , K02  , K01  , K00  }, \
   {K16  , K15  , K14  , K13  , K12  , K11  , K10  }, \
   {KC_NO, K25  , K24  , K23  , K22  , K21  , K20  }, \
   {K36  , K35  , K34  , K33  , K32  , K31  , K30  }, \
   {K66  , KC_NO, K44  , K43  , K42  , K41  , K40  }, \
   {KC_NO, K56  , KC_NO, K75  , K76  , KC_NO, KC_NO}, \
}
