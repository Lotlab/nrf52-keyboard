#pragma once
#include <stdbool.h>

/**
 * @brief 设置RGB灯光值
 * 
 * @param color 
 */
void keyboard_led_rgb_set(uint32_t color);
void keyboard_led_rgb_direct(uint8_t bit);

/**
 * @brief 关闭 RGB LED 的显示
 * 
 */
void keyboard_led_rgb_deinit(void);

/**
 * @brief 初始化RGB灯光
 * 
 */
void keyboard_led_rgb_init(void);

/**
 * @brief 切换 RGB LED 的显示状态
 * 
 * @param on 
 */
void keyboard_led_rgb_switch(bool on);
