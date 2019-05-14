#ifndef __KEYBOARD_LED_H_
#define __KEYBOARD_LED_H_

#include <stdint.h>
#include "../config/keyboard_config.h"

#ifdef LED_RGB
/**
 * @brief 设置RGB灯光值
 * 
 * @param color 
 */
void keyboard_led_rgb_set(uint32_t color);
void keyboard_led_rgb_direct(uint8_t bit);
#endif

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
 * @brief 关闭LED，准备进入睡眠模式
 * 
 */
void keyboard_led_off();

#endif //__KEYBOARD_LED_H_