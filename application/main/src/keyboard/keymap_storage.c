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

#include "keymap_storage.h"
#include "fds.h"
#include "keymap.h"
#include "keymap_common.h"
#include "nrf.h"
#include "app_scheduler.h"
#include <string.h>

#define layer_size (MATRIX_ROWS * MATRIX_COLS)
#define checksum_offset 0x13
#define fn_offset (checksum_offset + 2)
#define layer_offset (fn_offset + 0x40)
#define layer_end (layer_offset + layer_size * 8)

#define KEYMAP_SIZE ((((layer_end - 1) / 60) + 1) * 60)
#define KEYMAP_SIZE_WORD (KEYMAP_SIZE / 4)
#define FILE_ID 0x0514 /* The ID of the file to write the records into. */
#define RECORD_KEY 0x0514 /* A key for the first record. */

#ifdef KEYMAP_STORAGE
__ALIGN(4)
static uint8_t keymap_block[KEYMAP_SIZE] = { 0 };

static bool storage_keymap_valid = false;

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    if (layer >= 8 || key.col >= MATRIX_COLS || key.row >= MATRIX_ROWS)
        return KC_NO;
    if (storage_keymap_valid)
        return keymap_block[layer_offset + layer * layer_size + key.row * MATRIX_COLS + key.col];
    else
        return keymaps[layer][key.row][key.col];
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    if (storage_keymap_valid) {
        uint8_t index = fn_offset + FN_INDEX(keycode) * 2;
        uint16_t action = ((uint16_t)keymap_block[index + 1] << 8) + keymap_block[index];
        return (action_t)action;
    } else
        return fn_actions[FN_INDEX(keycode)];
}

static fds_record_t record;
static fds_record_desc_t record_desc;

static void keymap_valid(void)
{
    uint32_t checksum = 0xFEED;
    for (uint16_t i = fn_offset; i < layer_end; i += 2) {
        checksum += (keymap_block[i]) + (keymap_block[i + 1] << 8);
        checksum &= 0xFFFF;
    }
    uint16_t real_sum = (keymap_block[checksum_offset]) + (keymap_block[checksum_offset + 1] << 8);
    storage_keymap_valid = (real_sum == checksum);
}

static void keymap_read(void)
{
    fds_find_token_t ftok;
    fds_flash_record_t flash_record;
    /* It is required to zero the token before first use. */
    memset(&ftok, 0x00, sizeof(fds_find_token_t));

    if (fds_record_find(FILE_ID, RECORD_KEY, &record_desc, &ftok) == FDS_SUCCESS) {
        fds_record_open(&record_desc, &flash_record);

        if (flash_record.p_header->length_words == KEYMAP_SIZE_WORD) {
            memcpy(keymap_block, flash_record.p_data, KEYMAP_SIZE);
            fds_record_close(&record_desc);
        } else {
            fds_record_close(&record_desc);
            ret_code_t code = fds_record_update(&record_desc, &record);
            APP_ERROR_CHECK(code);
        }
    } else {
        ret_code_t code = fds_record_write(&record_desc, &record);
        APP_ERROR_CHECK(code);
    }
}

static void keymap_update(void)
{
    // record_desc was create by fds_record_find or fds_record_write
    ret_code_t err_code = fds_record_update(&record_desc, &record);

    // space full, gc
    if (err_code == FDS_ERR_NO_SPACE_IN_FLASH)
    {
        err_code = fds_gc();
        fds_record_update(&record_desc, &record);
    }
}

/**
 * @brief 初始化Keymap
 * 
 */
void keymap_init(void)
{
    record.file_id = FILE_ID;
    record.key = RECORD_KEY;
    record.data.p_data = &keymap_block;
    record.data.length_words = KEYMAP_SIZE_WORD;

    keymap_read();
    keymap_valid();
}

/**
 * @brief 确认Keymap写入
 * 
 */
static void keymap_write(void *p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(p_event_data);
    UNUSED_PARAMETER(event_size);
    keymap_valid();
    keymap_update();
}

/**
 * @brief 写入Keymap
 * 
 * @param block 当前块
 * @param size 块大小
 * @param data 数据
 */
bool keymap_set(uint8_t block, uint8_t size, uint8_t* data)
{
    if (block * size < KEYMAP_SIZE) {
        memcpy(&keymap_block[block * size], data, size);
        if ((block + 1) * size >= KEYMAP_SIZE) {
            app_sched_event_put(NULL, 0, &keymap_write);
            return true;
        }
        return false;
    }
    return true;
}

#else
void keymap_init(void) {}
bool keymap_set(uint8_t block, uint8_t size, uint8_t* data) {}
#endif