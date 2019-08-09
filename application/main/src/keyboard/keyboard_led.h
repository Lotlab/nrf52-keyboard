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
 * @brief 设置是否启用省电模式
 * 
 * @param powersave 
 */
void keyboard_led_powersave(bool powersave);

/**
 * @brief 禁用LED，准备进入睡眠模式
 * 
 */
void keyboard_led_deinit();

/**
 * @brief 启用自动熄灭的计时器
 */
void off_timer_start();
