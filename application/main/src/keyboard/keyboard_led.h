#pragma once

#include <stdint.h>
#include "../config/keyboard_config.h"

/**
 * @brief 设置LED灯光值
 * 
 * @param led_val 
 */
void keyboard_led_set(uint8_t led_val);
/**
 * @brief 初始化LED设置
 * 
 */
void keyboard_led_init(void);
/**
 * @brief 禁用LED，准备进入睡眠模式
 * 
 */
void keyboard_led_deinit();
