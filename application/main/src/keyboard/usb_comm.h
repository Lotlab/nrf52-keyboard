#if !defined(__USB_COMM_H__)
#define __USB_COMM_H__

#include <stdbool.h>
#include <stdint.h>
#include "../config/keyboard_config.h"

/**
 * @brief 当前USB是否正常连接到主机
 * 
 * @return true 
 * @return false 
 */
bool usb_working(void);

/**
 * @brief 通过USB发送数据包
 * 
 * @param index 类型Index。参见BLE相关部分的参数
 * @param len 数据包长度
 * @param pattern 数据包
 */
void usb_send(uint8_t index, uint8_t len, uint8_t* pattern);

/**
 * @brief USB初始化
 * 
 */
void usb_comm_init(void);

/**
 * @brief USB准备休眠
 * 
 */
void usb_comm_sleep_prepare(void);

/**
 * @brief USB准备启动计时器
 * 
 */
void usb_comm_timer_start(void);

/**
 * @brief 在USB和蓝牙设备之间切换
 * 
 */
void usb_comm_switch(void);

/**
 * @brief 当前USB的键盘状态灯
 */
extern uint8_t keyboard_led_val_usb;

#endif // __USB_COMM_H__
