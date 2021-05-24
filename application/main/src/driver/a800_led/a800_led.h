#pragma once

#include "../main.h"
#include <stdint.h>

/**
 * 初始化状态LED
 **/
void a800_led_init(void);

/**
 * 释放LED
 **/
void a800_led_deinit(void);

/**
 * 关闭状态LED的显示
 **/
void a800_led_off(void);

/**
 * 启用状态LED的显示
 **/
void a800_led_on(void);

/**
 * 设置所有状态灯的状态
 **/
void a800_led_all(bool state);

/** 
 * 设置CAP状态灯的状态
 **/
void a800_led_cap(bool state);

/** 
 * 设置NUM状态灯的状态
 **/
void a800_led_num(bool state);

/** 
 * 设置RGB状态灯R分量状态
 **/
void a800_led_r(bool state);

/** 
 * 设置RGB状态灯G分量状态
 **/
void a800_led_g(bool state);

/** 
 * 设置RGB状态灯B分量状态
 **/
void a800_led_b(bool state);

/** 
 * 设置蓝牙状态灯的状态
 **/
void a800_led_ble(bool state);

/** 
 * 设置充电状态灯的状态
 **/
void a800_led_charging(bool state);

/** 
 * 设置USB状态灯的状态
 **/
void a800_led_usb(bool state);

// /**
//  * 读取状态灯开关存储值
//  */
// uint8_t leds_turn_off_read(void);

// /**
//  * 写入状态灯开关存储值
//  */
// void leds_turn_off_write(uint8_t val);

// /** 
//  * LED状态灯开关
//  **/
// void leds_switch();