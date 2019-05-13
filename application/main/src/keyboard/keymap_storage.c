#include "keymap_storage.h"
#include "../config/keymap_common.h"
#include "fds.h"
#include "keymap.h"
#include "nrf.h"
#include <string.h>

#define KEYMAP_SIZE 660
#define KEYMAP_SIZE_WORD (KEYMAP_SIZE / 4)
#define FILE_ID 0x0514 /* The ID of the file to write the records into. */
#define RECORD_KEY 0x0514 /* A key for the first record. */

#define layer_size (MATRIX_ROWS * MATRIX_COLS)
#define checksum_offset 0x13
#define fn_offset (checksum_offset + 2)
#define layer_offset (fn_offset + 0x40)
#define layer_end (layer_offset + 14 * 5 * 8)

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

    bool data_valid = false;

    if (fds_record_find(FILE_ID, RECORD_KEY, &record_desc, &ftok) == FDS_SUCCESS) {
        fds_record_open(&record_desc, &flash_record);

        if (flash_record.p_header->length_words == KEYMAP_SIZE_WORD) {
            memcpy(keymap_block, flash_record.p_data, KEYMAP_SIZE);
            fds_record_close(&record_desc);
            data_valid = true;
        } else {
            fds_record_close(&record_desc);
            fds_record_delete(&record_desc);
        }
    }
    if (!data_valid) {
        record.file_id = FILE_ID;
        record.key = RECORD_KEY;
        record.data.p_data = &keymap_block;
        record.data.length_words = KEYMAP_SIZE_WORD;

        fds_record_write(&record_desc, &record);
    }
}

static void keymap_update(void)
{
    record.file_id = FILE_ID;
    record.key = RECORD_KEY;
    record.data.p_data = &keymap_block;
    record.data.length_words = KEYMAP_SIZE_WORD;

    // record_desc was create by fds_record_find or fds_record_write
    fds_record_update(&record_desc, &record);
}

/**
 * @brief 初始化Keymap
 * 
 */
void keymap_init(void)
{
    keymap_read();
    keymap_valid();
}

/**
 * @brief 写入Keymap
 * 
 * @param block 当前块
 * @param size 块大小
 * @param data 数据
 */
void keymap_set(uint8_t block, uint8_t size, uint8_t* data)
{
    memcpy(&keymap_block[block * size], data, size);
}

/**
 * @brief 确认Keymap写入
 * 
 */
void keymap_write(void)
{
    keymap_valid();
    keymap_update();
}