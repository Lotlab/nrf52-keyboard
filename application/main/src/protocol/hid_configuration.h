#pragma once
#include "data_storage.h"
#include "nrf_section.h"
#include <stdint.h>

enum hid_command {
    // 获取键盘信息
    HID_CMD_GET_INFORMATION = 0x20,
    // 获取单个按键键值
    HID_CMD_GET_SINGLE_KEY,
    // 获取单个Fn的功能
    HID_CMD_GET_SINGLE_FN,
    // 获取所有键值
    HID_CMD_GET_ALL_KEYS,
    // 获取所有Fn功能
    HID_CMD_GET_ALL_FNS,
    // 获取指定的配置项目的值
    HID_CMD_GET_SINGLE_CONFIG,
    // 获取所有配置项目的值
    HID_CMD_GET_ALL_CONFIG,
    // 获取所有宏的值
    HID_CMD_GET_ALL_MACRO,

    // 设置单个按键键值
    HID_CMD_SET_SINGLE_KEY = 0x31,
    // 设置单个Fn功能
    HID_CMD_SET_SINGLE_FN,
    // 设置所有键值
    HID_CMD_SET_ALL_KEYS,
    // 设置所有Fn值
    HID_CMD_SET_ALL_FNS,
    // 设置指定的配置项目的值
    HID_CMD_SET_SINGLE_CONFIG,
    // 设置所有配置项目的值
    HID_CMD_SET_ALL_CONFIG,
    // 设置所有宏的值
    HID_CMD_SET_ALL_MACRO,

    // 将数据写入存储
    HID_CMD_WRITE_CONFIG = 0x3E,
    // 重置键盘
    HID_CMD_RESET_CONFIG = 0x3F,
};

enum hid_response {
    // 操作成功
    HID_RESP_SUCCESS,
    // 此操作未定义
    HID_RESP_UNDEFINED,
    // 参数错误
    HID_RESP_PARAMETER_ERROR,
    // 内部错误
    HID_RESP_INTERNAL_ERROR,
    // 写入数据超出限制
    HID_RESP_WRITE_OVERFLOW,
    // UART通信校验错误
    HID_RESP_UART_CHECKSUM_ERROR,
};

void hid_on_recv(uint8_t command, uint8_t len, uint8_t* data);
void hid_response_success(uint8_t len, uint8_t* data);
void hid_response_generic(enum hid_response response);

/**
 * @brief 配置记录区域
 * 
 */
struct hid_config_section {
    /**
     * @brief 数据索引
     * 
     */
    uint8_t index;
    /**
     * @brief 数据指针
     * 
     */
    struct config_section* section;
};

/**
 * @brief 定义HID配置项目区域
 * 
 */
#define HID_CONFIG_DEF() NRF_SECTION_DEF(hid_config, struct config_section)
/**
 * @brief 定义一个HID配置项目
 * 
 */
#define HID_CONFIG(code, config_section) NRF_SECTION_ITEM_REGISTER(hid_config, struct hid_config_section) \
                                 __hid_conf_##code = { .index = code, .section = &config_section };
#define HID_CONFIG_COUNT NRF_SECTION_ITEM_COUNT(hid_config, struct hid_config_section)
#define HID_CONFIG_GET(i) (NRF_SECTION_ITEM_GET(hid_config, struct hid_config_section, i))
