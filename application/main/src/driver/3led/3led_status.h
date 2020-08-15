#pragma once

#include "../main.h"
#include <stdint.h>

/**
 * 初始化状态LED
 **/
void status_led_init(void);

/**
 * 关闭状态LED的显示
 **/
void status_led_off(void);

/**
 * 启用状态LED的显示
 **/
void status_led_on(void);

/** 
 * 设置蓝牙状态灯的状态
 **/
void status_led_ble(bool state);

/** 
 * 设置充电状态灯的状态
 **/
void status_led_charging(bool state);

/** 
 * 设置USB状态灯的状态
 **/
void status_led_usb(bool state);

/**
 * 读取状态灯开关存储值
 */
uint8_t leds_turn_off_read(void);

/**
 * 写入状态灯开关存储值
 */
void leds_turn_off_write(uint8_t val);
/** 
 * LED状态灯开关
 **/
void leds_switch();