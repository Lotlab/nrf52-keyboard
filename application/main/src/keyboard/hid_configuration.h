#pragma once

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
};

const uint32_t keyboard_function_table = 
#ifdef BOOTMAGIC_ENABLE
    1 << 0 + 
#endif
#ifdef MOUSEKEY_ENABLE
    1 << 1 + 
#endif
#ifdef EXTRAKEY_ENABLE
    1 << 2 + 
#endif
#ifdef NKRO_ENABLE
    1 << 3 + 
#endif
#ifdef KEYMAP_STORAGE
    1 << 8 + 
#endif
#ifdef ACTIONMAP_ENABLE
    1 << 9 + 
#endif
#ifdef MARCO_STORAGE
    1 << 10 + 
#endif
#ifdef CONFIG_STORAGE
    1 << 11 + 
#endif
    0;
