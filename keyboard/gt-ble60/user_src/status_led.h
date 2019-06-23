#ifndef __STATUS_LED_H_
#define __STATUS_LED_H_

#include <stdint.h>
#include <stdbool.h>

enum led_bit_usage
{
    // 自定义灯
    LED_BIT_BLE,
    LED_BIT_USB,
    LED_BIT_CHARGING,
};
/**
 * @brief 初始化LED设置
 * 
 */
void status_led_init(void);
/**
 * @brief 设置系统状态LED灯的状态值
 * 
 * @param bit 位置
 * @param state 状态
 */
void status_led_set_val(enum led_bit_usage bit, bool state);
/**
 * @brief 设置系统状态LED灯,并启用自动熄灭
 * 
 */
void status_led_set();
/**
 * @brief 开启系统状态LED灯
 * 
 */
void status_led_on(void);
/**
 * @brief 熄灭系统状态LED灯
 * 
 */
void status_led_off(void);
/**
 * @brief 设置是否启用省电模式
 * 
 * @param powersave 
 */
void status_led_powersave(bool powersave);
/**
 * @brief 启用LED状态灯显示
 */
void status_led_display();
/**
 * @brief 释放LED
 * 
 */
void status_led_deinit(void);

#endif //__STATUS_LED_H_