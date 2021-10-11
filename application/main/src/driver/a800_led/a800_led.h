#pragma once

#include <stdbool.h>
#include <stdint.h>


// A800 LED驱动PIN脚配置
#ifndef A800_LED_DATA
#define A800_LED_DATA 5    // LED数据pin定义(74HC595 DS)
#endif
#ifndef A800_LED_CLK
#define A800_LED_CLK 3     // LED时钟pin定义(74HC595 SHCP)
#endif
#ifndef A800_LED_ON
#define A800_LED_ON 4      // LED使能pin定义(74HC595 STCP)
#endif

// LED通道配置
#define LED_CHANNEL_NUM 5
#define CAP_CHANNEL 0
#define NUM_CHANNEL 1
#define G_CHANNEL 2
#define R_CHANNEL 3
#define B_CHANNEL 4
#define CAP_MASK (1 << CAP_CHANNEL)
#define NUM_MASK (1 << NUM_CHANNEL)
#define R_MASK (1 << R_CHANNEL)
#define G_MASK (1 << G_CHANNEL)
#define B_MASK (1 << B_CHANNEL)
// BLE, USB, CHARGING状态指示灯复用RGB指示灯
#define BLE_CHANNEL B_CHANNEL
#define USB_CHANNEL R_CHANNEL
#define CHARGING_CHANNEL NUM_CHANNEL
#define BLE_MASK (1 << BLE_CHANNEL)
#define USB_MASK (1 << USB_CHANNEL)
#define CHARGING_MASK (1 << CHARGING_CHANNEL)

// LED指示灯闪烁控制
#define A800_LED_BLINK_INTERVAL 100    // 闪烁速度(ms)
#define MAKE_BLINK_MULT(CHANNEL, VAL) ((VAL & 0x1f) << (CHANNEL * LED_CHANNEL_NUM))
#define GET_BLINK_MULT(CHANNEL, MULT) ((MULT >> (CHANNEL * LED_CHANNEL_NUM)) & 0x1f)

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
 * 开启或关闭led参数对应二进制位的灯
 **/
void a800_led_set(bool state, uint8_t mask);

/**
 * 设置所有状态灯的状态
 **/
void a800_led_all(bool state);

/**
 * @brief 开启闪烁灯
 * 
 */
void a800_led_blink_on(uint8_t blink_mask_, uint32_t blink_mult_);

/**
 * @brief 关闭闪烁灯
 * 
 */
void a800_led_blink_off(uint8_t blink_mask_);

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