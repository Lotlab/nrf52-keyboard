#include <stdint.h>
#include <string.h>

#include "hid_configuration.h"
#include "nordic_common.h"
#include "usb_comm.h"
#include "util.h"

#include "action.h"
#include "data_storage.h"
#include "keymap.h"

#define HID_PROTOCOL 4
#define MAX_HID_PACKET_SIZE 56

#ifndef BUILD_TIME
#define BUILD_TIME 0
#endif

#ifndef VERSION
#define VERSION 00000000
#endif

#define APP_VERSION CONCAT_2(0x, VERSION)

const uint32_t keyboard_function_table =
#ifdef BOOTMAGIC_ENABLE
    (1 << 0) +
#endif
#ifdef MOUSEKEY_ENABLE
    (1 << 1) +
#endif
#ifdef EXTRAKEY_ENABLE
    (1 << 2) +
#endif
#ifdef NKRO_ENABLE
    (1 << 3) +
#endif
#ifdef KEYMAP_STORAGE
    (1 << 8) +
#endif
#ifdef ACTIONMAP_ENABLE
    (1 << 9) +
#endif
#ifdef MACRO_STORAGE
    (1 << 10) +
#endif
#ifdef CONFIG_STORAGE
    (1 << 11) +
#endif
#ifdef RGBLIGHT_ENABLE
    (1 << 16) +
#endif
#ifdef RGBMATRIX_ENABLE
    (1 << 17) +
#endif
    0;

#ifndef CONF_VENDOR_ID
#define CONF_VENDOR_ID VENDOR_ID
#endif

#ifndef CONF_PRODUCT_ID
#define CONF_PRODUCT_ID PRODUCT_ID
#endif

#ifdef HAS_USB

HID_CONFIG_DEF();
#ifdef CONFIG_STORAGE
static struct hid_config_section* hid_config_get(uint8_t id)
{
    for (uint8_t i = 0; i < HID_CONFIG_COUNT; i++) {
        if (HID_CONFIG_GET(i)->index == id)
            return HID_CONFIG_GET(i);
    }
    return 0;
}
#endif

/**
 * @brief 响应HID成功命令
 * 
 * @param len 额外数据长度
 * @param data 额外数据
 */
void hid_response_success(uint8_t len, uint8_t* data)
{
    uint8_t buff[63];
    buff[0] = 0x00;
    buff[1] = len;
    memcpy(&buff[2], data, len);
    uart_send_conf(len + 2, buff);
}

/**
 * @brief 响应HID命令
 * 
 * @param response 响应状态
 */
void hid_response_generic(enum hid_response response)
{
    uart_send_conf(1, &response);
}

/**
 * @brief 发送键盘信息
 * 
 */
static void send_information()
{
    const uint8_t info[] = {
        UINT16_SEQ(CONF_VENDOR_ID), // VENDOR
        UINT16_SEQ(CONF_PRODUCT_ID), // PRODUCT
        DEVICE_VER & 0xFF, // HWVER
        HID_PROTOCOL, // PROTOCOL_VER
        UINT32_SEQ(APP_VERSION), // FIRMWARE_VER
        UINT32_SEQ(BUILD_TIME), // BUILD_DATE
        UINT32_SEQ(keyboard_function_table), // FUNCTION_TABLE
    };
    hid_response_success(sizeof(info), (uint8_t*)info);
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
    keypos_t pos = {
        .col = col,
        .row = row
    };
#ifndef ACTIONMAP_ENABLE
    uint8_t code[] = { keymap_key_to_keycode(layer, pos), 0 };
    hid_response_success(2, code);
#else
    action_t action = action_for_key(layer, pos);
    uint8_t code[] = { UINT16_SEQ(action.code) };
    hid_response_success(2, code);
#endif
}
/**
 * @brief 获取单个FN键值
 * 
 * @param id FN的ID
 */
static void get_single_fn(uint8_t id)
{
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
    uint8_t data[2];
    uint8_t len = storage_read_data(STORAGE_FN, id * 2, 2, data);
    if (len != 2)
        return hid_response_generic(HID_RESP_PARAMETER_ERROR);
    hid_response_success(2, data);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

static void response_storage(enum storage_type type, uint16_t offset, uint16_t len)
{
    if (len > MAX_HID_PACKET_SIZE)
        return hid_response_generic(HID_RESP_PARAMETER_ERROR);

    uint8_t data[MAX_HID_PACKET_SIZE];

    len = storage_read_data(type, offset, len, data);
    hid_response_success(len, data);
}

/**
 * @brief 获取所有按键键值
 * 
 * @param offset 按键偏移量
 */
static void get_all_keys(uint16_t offset)
{
    response_storage(STORAGE_KEYMAP, offset, MAX_HID_PACKET_SIZE);
}

/**
 * @brief 获取所有Fn键键值
 * 
 * @param offset 按键偏移量
 */
static void get_all_fns(uint8_t offset)
{
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
    response_storage(STORAGE_FN, offset, MAX_HID_PACKET_SIZE);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

/**
 * @brief 获取指定配置项目的值
 * 
 * @param offset 
 * @param len 
 */
static void get_single_config(uint8_t id)
{
#ifdef CONFIG_STORAGE
    struct hid_config_section* item = hid_config_get(id);
    if (item == 0) {
        hid_response_generic(HID_RESP_PARAMETER_ERROR);
    } else {
        hid_response_success(item->section->len, item->section->data);
    }
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

/**
 * @brief 获取所有宏的值
 * 
 * @param offset 
 */
static void get_all_macro(uint16_t offset)
{
#ifdef MACRO_STORAGE
    response_storage(STORAGE_MACRO, offset, MAX_HID_PACKET_SIZE);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

/**
 * @brief 将数据写入存储并返回hid应答。
 * 
 * 若数据写入大小小于要写入的大小，则返回HID_RESP_WRITE_OVERFLOW
 * 
 * @param type 写入类型
 * @param offset 写入偏移
 * @param data 数据指针
 * @param len 数据长度
 */
static void set_storage(enum storage_type type, uint16_t offset, uint8_t* data, uint16_t len)
{
    if (storage_write_data(type, offset, len, data) < len)
        hid_response_generic(HID_RESP_WRITE_OVERFLOW);
    else
        hid_response_generic(HID_RESP_SUCCESS);
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
#ifdef KEYMAP_STORAGE
    uint16_t index = layer * KEYMAP_LAYER_SIZE + row * KEYMAP_ROW_SIZE + col * SINGLE_KEY_SIZE;
    uint8_t data[2] = { UINT16_SEQ(keycode) };
    set_storage(STORAGE_KEYMAP, index, data, SINGLE_KEY_SIZE);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

/**
 * @brief 设置单个FN的键值
 * 
 * @param id FN的ID
 * @param keycode 键值
 */
static void set_single_fn(uint8_t id, uint16_t keycode)
{
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
    uint8_t data[2] = { UINT16_SEQ(keycode) };
    set_storage(STORAGE_FN, id * 2, data, 2);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
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
#ifdef KEYMAP_STORAGE
    set_storage(STORAGE_KEYMAP, id * MAX_HID_PACKET_SIZE, data, len);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
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
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
    set_storage(STORAGE_FN, id * MAX_HID_PACKET_SIZE, data, len);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

/**
 * @brief 设置单个配置数据
 * 
 * @param offset 偏移
 * @param len 长度
 * @param data 数据
 */
static void set_single_config(uint8_t id, uint8_t len, uint8_t* data)
{
#ifdef CONFIG_STORAGE
    struct hid_config_section* item = hid_config_get(id);
    if (item == 0) {
        hid_response_generic(HID_RESP_PARAMETER_ERROR);
    } else {
        if (item->section->len != len) {
            hid_response_generic(HID_RESP_WRITE_OVERFLOW);
        } else {
            memcpy(item->section->data, data, len);
            hid_response_generic(HID_RESP_SUCCESS);
        }
    }
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
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
#ifdef MACRO_STORAGE
    set_storage(STORAGE_MACRO, id * MAX_HID_PACKET_SIZE, data, len);
#else
    hid_response_generic(HID_RESP_UNDEFINED);
#endif
}

/**
 * @brief 写入数据
 * 
 * @param type 数据类型
 */
static void write_data(uint8_t type)
{
    storage_write(type);
    hid_response_generic(HID_RESP_SUCCESS);
}

/**
 * @brief 读取数据（放弃当前更改）
 * 
 * @param type 读取类型
 */
static void read_data(uint8_t type)
{
    storage_read(type);
    hid_response_generic(HID_RESP_SUCCESS);
}

/**
 * @brief 重置数据
 * 
 * @param type 重置类型
 */
static void reset_data(uint8_t type)
{
    storage_delete(type);
    read_data(type);
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
            get_all_keys(UINT16_READ(data, 0));
        break;
    case HID_CMD_GET_ALL_FNS:
        if (len != 1)
            hid_response_generic(HID_RESP_PARAMETER_ERROR);
        else
            get_all_fns(data[0]);
        break;
    case HID_CMD_GET_SINGLE_CONFIG:
        if (len != 1)
            hid_response_generic(HID_RESP_PARAMETER_ERROR);
        else
            get_single_config(data[0]);
        break;
    case HID_CMD_GET_ALL_MACRO:
        if (len != 2)
            hid_response_generic(HID_RESP_PARAMETER_ERROR);
        else
            get_all_macro(UINT16_READ(data, 0));
        break;
    case HID_CMD_SET_SINGLE_KEY:
        if (len != 5)
            hid_response_generic(HID_RESP_PARAMETER_ERROR);
        else
            set_single_key(data[0], data[1], data[2], UINT16_READ(data, 3));
        break;
    case HID_CMD_SET_SINGLE_FN:
        if (len != 3)
            hid_response_generic(HID_RESP_PARAMETER_ERROR);
        else
            set_single_fn(data[0], UINT16_READ(data, 1));
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
    case HID_CMD_SET_ALL_MACRO:
        set_all_macro(data[0], len - 1, &data[1]);
        break;
    case HID_CMD_READ_CONFIG:
        if (len != 1)
            hid_response_generic(HID_RESP_PARAMETER_ERROR);
        else
            read_data(data[0]);
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
            reset_data(data[0]);
        break;
    default:
        hid_response_generic(HID_RESP_UNDEFINED);
        break;
    }
}

#endif
