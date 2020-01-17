/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <string.h>

#include "data_storage.h"

#include "app_scheduler.h"
#include "fds.h"
#include "nrf.h"
#include "queue.h"
#include "util.h"

// keymap
#include "keymap.h"
#include "keymap_common.h"

// actionmap
#include "actionmap.h"

// macro
#include "action_macro.h"

#define GET_WORD(_i) ((_i - 1) / 4 + 1)

#define REGISTER_FDS_BLOCK(_name, _size_word, _record_key) \
    __ALIGN(4)                                             \
    uint8_t _name##_block[(_size_word * 4)] = { 0 };       \
    static fds_record_desc_t _name##_record_desc = { 0 };  \
    static fds_record_t _name##_record = {                 \
        .file_id = FILE_ID,                                \
        .key = _record_key,                                \
        .data = {                                          \
            .p_data = &_name##_block,                      \
            .length_words = _size_word,                    \
        }                                                  \
    };

#ifdef KEYMAP_STORAGE
#define KEYMAP_SIZE_WORD GET_WORD(KEYMAP_LAYER_SIZE* MAX_LAYER)
#define KEYMAP_RECORD_KEY 0x0514

REGISTER_FDS_BLOCK(keymap, KEYMAP_SIZE_WORD, KEYMAP_RECORD_KEY)

static bool keymap_valid = true;
static void check_keymap()
{
    bool flag = false;
    for (uint16_t i = 0; i < KEYMAP_LAYER_SIZE * MAX_LAYER; i++) {
        flag |= (keymap_block[i] != 0);
        if (flag)
            break;
    }
    keymap_valid = flag;
}
#define USE_KEYMAP keymap_valid
#ifndef ACTIONMAP_ENABLE
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    if (layer >= KEYMAP_LAYER_SIZE || key.col >= MATRIX_COLS || key.row >= MATRIX_ROWS)
        return KC_NO;
    if (USE_KEYMAP)
        return keymap_block[layer * KEYMAP_LAYER_SIZE + key.row * MATRIX_COLS + key.col];
    else
        return keymaps[layer][key.row][key.col];
}
#else
extern const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS];

action_t action_for_key(uint8_t layer, keypos_t key)
{
    if (layer >= KEYMAP_LAYER_SIZE || key.col >= MATRIX_COLS || key.row >= MATRIX_ROWS) {
        action_t action = AC_NO;
        return action;
    }
    if (USE_KEYMAP) {
        action_t action;
        uint16_t index = layer * KEYMAP_LAYER_SIZE + key.row * KEYMAP_ROW_SIZE + key.col * 2;
        action.code = UINT16_READ(keymap_block, index);
        return action;
    } else {
        return actionmaps[layer][key.row][key.col];
    }
}
#endif

#ifndef ACTIONMAP_ENABLE
#define FN_BLOCK_SIZE_WORD GET_WORD(MAX_FN_KEYS * 2)
#define FN_RECORD_KEY 0x0495

REGISTER_FDS_BLOCK(fn, FN_BLOCK_SIZE_WORD, FN_RECORD_KEY)

action_t keymap_fn_to_action(uint8_t keycode)
{
    if (USE_KEYMAP) {
        uint8_t index = FN_INDEX(keycode) * 2;
        uint16_t action = UINT16_READ(fn_block, index);
        return (action_t)action;
    } else
        return fn_actions[FN_INDEX(keycode)];
}
#endif
#endif

#ifdef MACRO_STORAGE
#define MACRO_BLOCK_SIZE_WORD GET_WORD(MAX_MACRO_SIZE)
#define MACRO_RECORD_KEY 0x0496

REGISTER_FDS_BLOCK(macro, MACRO_BLOCK_SIZE_WORD, MACRO_RECORD_KEY)

const macro_t* action_get_macro(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (!record->event.pressed)
        return MACRO_NONE;
    if (id == 0)
        return macro_block;

    uint16_t index = 0;
    while (index < MAX_MACRO_SIZE) {
        uint8_t code = macro_block[index++];
        if (code == END && --id == 0) {
            // 到达目标位置
            return &macro_block[index];
        }
        if (code == KEY_DOWN || code == KEY_UP || code == WAIT || code == INTERVAL)
            index++; // 2 byte command
    }

    return MACRO_NONE;
}
#endif

#ifdef CONFIG_STORAGE
#define CONFIG_BLOCK_SIZE_WORD 4
#define CONFIG_BLOCK_SIZE_BYTE CONFIG_BLOCK_SIZE_WORD * 4
#define CONFIG_RECORD_KEY 0x0497

REGISTER_FDS_BLOCK(config, CONFIG_BLOCK_SIZE_WORD, CONFIG_RECORD_KEY)
CONFIG_SECTION_DEF();

/**
 * @brief 存储初始化，分配指针
 * 
 */
static void config_storage_init()
{
    // 分配指针
    uint8_t cnt = 0;
    for (int i = 0; i < CONFIG_SECTION_COUNT; i++) {
        struct config_section* item = CONFIG_SECTION_GET(i);
        if (cnt + item->len <= CONFIG_BLOCK_SIZE_BYTE) {
            item->data = &config_block[cnt];
            cnt += item->len;
        } else {
            item->data = 0;
        }
    }
}
#endif

struct fds_update_op {
    fds_record_t const* record;
    fds_record_desc_t* record_desc;
};

QUEUE(struct fds_update_op, gc_queue, 5);

/**
 * @brief FDS操作回调
 * 
 * @param p_evt 
 */
static void storage_callback(fds_evt_t const* p_evt)
{
    // GC完毕事件
    if (p_evt->id == FDS_EVT_GC && p_evt->result == FDS_SUCCESS) {
        while (!gc_queue_empty()) {
            struct fds_update_op* item = gc_queue_peek();
            ret_code_t err_code = fds_record_update(item->record_desc, item->record);
            if (err_code == FDS_SUCCESS) {
                gc_queue_pop();
            } else {
                // 没有空间了，尝试GC
                if (err_code == FDS_ERR_NO_SPACE_IN_FLASH) {
                    fds_gc();
                    break;
                }
                // 操作队列没有空间了，等会再说
                if (err_code == FDS_ERR_NO_SPACE_IN_QUEUES) {
                    break;
                    // todo: 等有空间再重新调用？
                }
            }
        }
    }
}

/**
 * @brief 初始化FDS回调
 * 
 */
static void storage_callback_init()
{
    ret_code_t code = fds_register(&storage_callback);
    APP_ERROR_CHECK(code);
}

/**
 * @brief 内部更新FDS的数据
 * 
 * @param record FDS 记录
 * @param record_desc FDS 记录描述。必须先调用 storage_read_inner 或其他函数生成此描述后才能调用此函数。
 */
static void storage_update_inner(fds_record_t const* record, fds_record_desc_t* record_desc)
{
    ret_code_t err_code = fds_record_update(record_desc, record);
    if (err_code == FDS_ERR_NO_SPACE_IN_FLASH) {
        // 没有空间则GC
        err_code = fds_gc();
        struct fds_update_op item = {
            .record = record,
            .record_desc = record_desc,
        };
        // 将操作入队列，等待下一次执行
        gc_queue_push(item);
    }
}

/**
 * @brief 内部读取FDS的数据，如果数据不存在则尝试新建一个
 * 
 * @param record FDS记录
 * @param record_desc FDS记录描述
 */
static void storage_read_inner(fds_record_t const* record, fds_record_desc_t* record_desc)
{
    fds_find_token_t ftok = { 0 };
    fds_flash_record_t flash_record = { 0 };

    // 查找对应记录
    if (fds_record_find(record->file_id, record->key, record_desc, &ftok) == FDS_SUCCESS) {
        fds_record_open(record_desc, &flash_record);

        if (flash_record.p_header->length_words == record->data.length_words) {
            // 大小正常，读取数据
            memcpy((uint8_t*)record->data.p_data, flash_record.p_data, record->data.length_words * 4);
            fds_record_close(record_desc);
        } else {
            // 大小不正常，尝试更新数据
            fds_record_close(record_desc);
            storage_update_inner(record, record_desc);
        }
    } else {
        // 记录不存在，尝试新建一个
        ret_code_t code = fds_record_write(record_desc, record);
        APP_ERROR_CHECK(code);
    }
}

/**
 * @brief 读取存储的记录并写到内存（重置内存中的数据）
 * 
 * @param mask 要读取的记录mask。1: keymap, 2: fn, 4: macro, 8: config
 */
void storage_read(uint8_t mask)
{
    if (mask & 0x01) {
#ifdef KEYMAP_STORAGE
        storage_read_inner(&keymap_record, &keymap_record_desc);
        check_keymap();
#endif
    }
    if (mask & 0x02) {
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
        storage_read_inner(&fn_record, &fn_record_desc);
#endif
    }
    if (mask & 0x04) {
#ifdef MACRO_STORAGE
        storage_read_inner(&macro_record, &macro_record_desc);
#endif
    }
    if (mask & 0x08) {
#ifdef CONFIG_STORAGE
        storage_read_inner(&config_record, &config_record_desc);
#endif
    }
}

/**
 * @brief 初始化存储模块并读取记录
 * 
 */
void storage_init()
{
    storage_callback_init();
#ifdef CONFIG_STORAGE
    // 初始化配置分片
    config_storage_init();
#endif
    storage_read(0xFF);
}

/**
 * @brief 写存储模块记录
 * 
 * @param mask 要写出的记录mask。1: keymap, 2: fn, 4: macro, 8: config
 * @return true 操作成功
 * @return false 含有未定义的操作
 */
bool storage_write(uint8_t mask)
{
    bool success = true;

    if (mask & 0x01) {
#ifdef KEYMAP_STORAGE
        storage_update_inner(&keymap_record, &keymap_record_desc);
#endif
    }
    if (mask & 0x02) {
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
        storage_update_inner(&fn_record, &fn_record_desc);
#endif
    }
    if (mask & 0x04) {
#ifdef MACRO_STORAGE
        storage_update_inner(&macro_record, &macro_record_desc);
#endif
    }
    if (mask & 0x08) {
#ifdef CONFIG_STORAGE
        storage_update_inner(&config_record, &config_record_desc);
#endif
    }

    return success;
}

/**
 * @brief 获取指定类型的内存区域的指针和大小
 * 
 * @param type 存储类型
 * @param pointer 目标指针
 * @return uint16_t 存储空间大小。0则表示不支持。
 */
static uint16_t storage_get_data_pointer(enum storage_type type, uint8_t** pointer)
{
    switch (type) {
#ifdef KEYMAP_STORAGE
    case STORAGE_KEYMAP:
        *pointer = keymap_block;
        return KEYMAP_SIZE_WORD * 4;
        break;
#endif
#if defined(KEYMAP_STORAGE) && !defined(ACTIONMAP_ENABLE)
    case STORAGE_FN:
        *pointer = fn_block;
        return FN_BLOCK_SIZE_WORD * 4;
        break;
#endif
#ifdef MACRO_STORAGE
    case STORAGE_MACRO:
        *pointer = macro_block;
        return MACRO_BLOCK_SIZE_WORD * 4;
        break;
#endif
#ifdef CONFIG_STORAGE
    case STORAGE_CONFIG:
        *pointer = config_block;
        return CONFIG_BLOCK_SIZE_WORD * 4;
        break;
#endif
    default:
        return 0;
    }
}

/**
 * @brief 读取内存中的数据
 * 
 * @param type 数据类型 0: keymap, 1: fn, 2: macro, 3: config
 * @param offset 数据偏移量
 * @param len 数据长度
 * @param data 目标指针
 * @return uint16_t 读取实际长度
 */
uint16_t storage_read_data(enum storage_type type, uint16_t offset, uint16_t len, uint8_t* data)
{
    uint8_t* pointer = 0;
    uint16_t size = storage_get_data_pointer(type, &pointer);

    if (pointer == 0)
        return 0;

    if (size < len + offset)
        len = size - offset;

    memcpy(data, &pointer[offset], len);
    return len;
}

/**
 * @brief 将数据写到内存中
 * 
 * @param type 数据类型 0: keymap, 1: fn, 2: macro, 3: config
 * @param offset 数据偏移量
 * @param len 数据长度
 * @param data 数据指针
 * @return uint16_t 实际写入长度
 */
uint16_t storage_write_data(enum storage_type type, uint16_t offset, uint16_t len, uint8_t* data)
{
    uint8_t* pointer = 0;
    uint16_t size = storage_get_data_pointer(type, &pointer);

    if (pointer == 0)
        return 0;

    if (size < len + offset)
        len = size - offset;

    memcpy(&pointer[offset], data, len);

    if (type == STORAGE_KEYMAP)
        check_keymap();

    return len;
}
