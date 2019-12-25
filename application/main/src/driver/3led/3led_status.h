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
 * 默认的基于事件的状态路由
 * 如果覆盖了custom_event_handler，可以在自定义的事件中调用这个方法，从而避免再次编写对应的处理逻辑
 **/
void status_led_evt_handler(enum user_ble_event arg);
