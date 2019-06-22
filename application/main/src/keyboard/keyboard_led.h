#ifndef __KEYBOARD_LED_H_
#define __KEYBOARD_LED_H_

#include <stdint.h>
#include "../config/keyboard_config.h"

enum led_bit_usage
{
    // 自定义灯
    LED_BIT_BLE,
    LED_BIT_USB,
    LED_BIT_CHARGING,
};

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
 * @brief 设置键盘状态LED灯的值并启用自动熄灭
 * 
 * @param keyboard_led_val   LED状态值
 */
void keyboard_led_set(uint8_t keyboard_led_val);
#ifdef LED_STATUS
/**
 * @brief 设置系统状态LED灯的状态值
 * 
 * @param bit 位置
 * @param state 状态
 */
void system_led_set_val(enum led_bit_usage bit, bool state);
/**
 * @brief 设置系统状态LED灯,并启用自动熄灭
 * 
 */
void system_led_set();
/**
 * @brief 开启系统状态LED灯
 * 
 */
void system_led_on(void);
#endif

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
 * @brief 启用LED状态灯显示
 */
void led_display();
/**
 * @brief 关闭LED，准备进入睡眠模式
 * 
 */
void keyboard_led_off();
/**
 * @brief 释放LED
 * 
 */
void led_uninit(void);

#endif //__KEYBOARD_LED_H_