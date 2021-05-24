/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

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

#include "keyboard_led.h"

#include "../main.h"
#ifdef A800_LED_ENABLE
#include "a800_led.h"
#endif

/**
 * @brief 设置LED灯的值并启用自动熄灭
 * 
 * @param led_val 
 */
void keyboard_led_set(uint8_t led_val)
{
#ifdef A800_LED_ENABLE
    a800_led_num(led_val & (1 << 0));
    a800_led_cap(led_val & (1 << 1));
#endif
}

/**
 * @brief 将LED熄灭以准备休眠
 * 
 */
void keyboard_led_deinit(void)
{
}

/**
 * @brief 初始化LED各项设置
 * 
 */
void keyboard_led_init()
{
}
