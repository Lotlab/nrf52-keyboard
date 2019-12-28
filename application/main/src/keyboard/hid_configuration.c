#include "hid_configuration.h"
#include "nordic_common.h"
#include "usb_comm.h"
#include <stdint.h>
#include <string.h>

#define HID_PROTOCOL 4

#ifndef BUILD_TIME
#define BUILD_TIME 0
#endif

#ifndef VERSION
#define VERSION 00000000
#endif

#define APP_VERSION CONCAT_2(0x, VERSION)
#define UINT16(_data, _index) (_data[_index] + (uint16_t)(data[_index + 1] << 8))

/**
 * @brief 发送键盘信息
 * 
 */
static void send_information()
{
    const uint8_t info[] = {
        // VENDOR
        CONF_VENDOR_ID >> 8,
        CONF_VENDOR_ID & 0xFF,
        // PRODUCT
        CONF_PRODUCT_ID >> 8,
        CONF_PRODUCT_ID & 0xFF,
        // HWVER
        DEVICE_VER & 0xFF,
        // PROTOCOL_VER
        HID_PROTOCOL,
        // FIRMWARE_VER
        (APP_VERSION >> 24) & 0xFF,
        (APP_VERSION >> 16) & 0xFF,
        (APP_VERSION >> 8) & 0xFF,
        (APP_VERSION >> 0) & 0xFF,
        // BUILD_DATE
        (BUILD_TIME >> 24) & 0xFF,
        (BUILD_TIME >> 16) & 0xFF,
        (BUILD_TIME >> 8) & 0xFF,
        (BUILD_TIME >> 0) & 0xFF,
        // FUNCTION_TABLE
        (keyboard_function_table >> 24) & 0xFF,
        (keyboard_function_table >> 16) & 0xFF,
        (keyboard_function_table >> 8) & 0xFF,
        (keyboard_function_table >> 0) & 0xFF,
    };
    hid_response_success(sizeof(info), info);
}

/**
 * @brief 获取单个按键键值
 * 
 * @param layer 按键层
 * @param row 按键所在行
 * @param col 按键所在列 
 */
static void get_single_key(uint8_t layer, uint8_t row, uint8_t col)
{
    // todo
}
/**
 * @brief 获取单个FN键值
 * 
 * @param id FN的ID
 */
static void get_single_fn(uint8_t id)
{
    // todo
}

/**
 * @brief 获取所有按键键值
 * 
 * @param offset 按键偏移量
 */
static void get_all_keys(uint16_t offset)
{
    // todo
}

/**
 * @brief 获取所有Fn键键值
 * 
 * @param offset 按键偏移量
 */
static void get_all_fns(uint8_t offset)
{
    // todo
}

/**
 * @brief 获取指定配置项目的值
 * 
 * @param offset 
 * @param len 
 */
static void get_single_config(uint8_t offset, uint8_t len)
{
    // todo
}

/**
 * @brief 获取所有配置项目的值
 * 
 */
static void get_all_config()
{
    // todo
}

/**
 * @brief 获取所有宏的值
 * 
 * @param offset 
 */
static void get_all_macro(uint16_t offset)
{
    // todo
}

/**
 * @brief 设置单个键的键值
 * 
 * @param layer 层数
 * @param row 行
 * @param col 列
 * @param keycode 键值 
 */
static void set_single_key(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    // todo
}

/**
 * @brief 设置单个FN的键值
 * 
 * @param id FN的ID
 * @param keycode 键值
 */
static void set_single_fn(uint8_t id, uint16_t keycode)
{
    // todo
}

/**
 * @brief 设置所有键值
 * 
 * @param id 分包ID
 * @param len 长度
 * @param data 数据
 */
static void set_all_keys(uint8_t id, uint8_t len, uint8_t* data)
{
    // todo
}

/**
 * @brief 设置所有FN数据
 * 
 * @param id 分包ID
 * @param len 长度
 * @param data 数据
 */
static void set_all_fns(uint8_t id, uint8_t len, uint8_t* data)
{
    // todo
}

/**
 * @brief 设置单个配置数据
 * 
 * @param offset 偏移
 * @param len 长度
 * @param data 数据
 */
static void set_single_config(uint8_t offset, uint8_t len, uint8_t* data)
{
    // todo
}

/**
 * @brief 设置所有配置数据
 * 
 * @param len 长度
 * @param data 数据
 */
static void set_all_config(uint8_t len, uint8_t* data)
{
    // todo
}

/**
 * @brief 设置所有宏数据
 * 
 * @param id 分包
 * @param len 长度
 * @param data 数据
 */
static void set_all_macro(uint8_t id, uint8_t len, uint8_t* data)
{
    // todo
}

/**
 * @brief 写入数据
 * 
 * @param type 数据类型
 */
static void write_data(uint8_t type)
{
    // todo
}

/**
 * @brief 重置数据
 * 
 * @param type 重置类型
 */
static void reset_keyboard(uint8_t type)
{
    // todo
}

/**
 * @brief HID 命令处理函数
 * 
 * @param command HID命令 
 * @param len 额外数据长度
 * @param data 额外数据
 */
void hid_on_recv(uint8_t command, uint8_t len, uint8_t* data)
{
    if (command >= 0x80) {
        // todo: 旧版协议的支持
        hid_response_generic(HID_RESP_UNDEFINED);
    } else {
        switch (command) {
        case HID_CMD_GET_INFORMATION:
            if (len != 0)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                send_information();
            break;
        case HID_CMD_GET_SINGLE_KEY:
            if (len != 3)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_single_key(data[0], data[1], data[2]);
            break;
        case HID_CMD_GET_SINGLE_FN:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_single_fn(data[0]);
            break;
        case HID_CMD_GET_ALL_KEYS:
            if (len != 2)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_all_keys(UINT16(data, 0));
            break;
        case HID_CMD_GET_ALL_FNS:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_all_fns(data[0]);
            break;
        case HID_CMD_GET_SINGLE_CONFIG:
            if (len != 2)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_single_config(data[0], data[1]);
            break;
        case HID_CMD_GET_ALL_CONFIG:
            if (len != 0)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_all_config();
            break;
        case HID_CMD_GET_ALL_MACRO:
            if (len != 2)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                get_all_macro(UINT16(data, 0));
            break;
        case HID_CMD_SET_SINGLE_KEY:
            if (len != 5)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                set_single_key(data[0], data[1], data[2], UINT16(data, 3));
            break;
        case HID_CMD_SET_SINGLE_FN:
            if (len != 3)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                set_single_fn(data[0], UINT16(data, 1));
            break;
        case HID_CMD_SET_ALL_KEYS:
            set_all_keys(data[0], len - 1, &data[1]);
            break;
        case HID_CMD_SET_ALL_FNS:
            set_all_fns(data[0], len - 1, &data[1]);
            break;
        case HID_CMD_SET_SINGLE_CONFIG:
            set_single_config(data[0], len - 1, &data[1]);
            break;
        case HID_CMD_SET_ALL_CONFIG:
            set_all_config(len, data);
            break;
        case HID_CMD_SET_ALL_MACRO:
            set_all_macro(data[0], len - 1, &data[1]);
            break;
        case HID_CMD_WRITE_CONFIG:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                write_data(data[0]);
            break;
        case HID_CMD_RESET_CONFIG:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                reset_keyboard(data[0]);
            break;
        default:
            hid_response_generic(HID_RESP_UNDEFINED);
            break;
        }
    }
}

/**
 * @brief 响应HID成功命令
 * 
 * @param len 额外数据长度
 * @param data 额外数据
 */
static void hid_response_success(uint8_t len, uint8_t* data)
{
    uint8_t buff[63];
    buff[0] = 0x00;
    buff[1] = len;
    memcpy(&buff[2], data, len);
    uart_send_raw(len + 2, buff);
}

/**
 * @brief 响应HID命令
 * 
 * @param response 响应状态
 */
static void hid_response_generic(enum hid_response response)
{
    uart_send_raw(1, &response);
}
