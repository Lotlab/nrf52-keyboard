#pragma once

#include "host.h"
#include "host_driver.h"

/**
 * @brief TMK使用的driver
 * 
 */
extern host_driver_t driver;

/**
 * 待发送按键是否为空
 */
bool keys_queue_empty(void);

/**
 * @brief 发送数据包
 * 
 * @param index 数据包类型
 * @param len 长度
 * @param keys 按键
 */
void send_packet(uint8_t index, uint8_t len, uint8_t* keys);

/**
 * @brief 发送数据的类型
 * 
 */
enum packet_type {
    PACKET_KEYBOARD = 0,
    PACKET_MOUSE = REPORT_ID_MOUSE,
    PACKET_SYSTEM = REPORT_ID_SYSTEM,
    PACKET_CONSUMER = REPORT_ID_CONSUMER,
    PACKET_CONF = 0x3F,
    PACKET_NKRO = 0x80,
};

/**
 * @brief 内部多模的driver
 * 
 */
struct host_driver {
    /**
     * @brief 获取当前键盘LED
     * 
     */
    uint8_t (*keyboard_leds)(void);
    /**
     * @brief 发送按键数据包
     * 
     */
    void (*send_packet)(enum packet_type type, uint8_t len, uint8_t* data);
    /**
     * @brief 队列是否为空
     * 
     */
    bool (*queue_empty)(void);
    /**
     * @brief 当前此驱动是否工作中
     * 
     */
    bool (*driver_working)(void);
    /**
     * @brief 最大传输长度（不包括packet_type）
     * 
     */
    uint8_t mtu;
};

#include "nrf_section_iter.h"
#include <stdlib.h>

/**
 * @brief 注册一个键盘驱动
 * priority是优先级，越大越靠后
 * 
 */
#define KEYBOARD_HOST_DRIVER(_priority, _pt) NRF_SECTION_SET_ITEM_REGISTER(host_driver, _priority, struct host_driver* _##_pt) = &_pt
