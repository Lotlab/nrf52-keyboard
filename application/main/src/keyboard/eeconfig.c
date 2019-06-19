/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

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
#include "eeconfig.h"
#include "fds.h"
#include "nrf.h"
#include <string.h>

#define FILE_ID 0x0114 /* The ID of the file to write the records into. */
#define RECORD_KEY 0x0514 /* A key for the first record. */

__ALIGN(4)
static uint8_t config_buffer[8] __attribute__((aligned(4))) = { EECONFIG_MAGIC_NUMBER >> 8, EECONFIG_MAGIC_NUMBER % 0x100, 0, 0, 0, 0, 0, 0 };
static bool fds_inited = false;

static fds_record_t record;
static fds_record_desc_t record_desc;

static void eeconfig_set_default()
{
    config_buffer[0] = EECONFIG_MAGIC_NUMBER >> 8;
    config_buffer[1] = EECONFIG_MAGIC_NUMBER % 0x100;
    config_buffer[2] = 0;
    config_buffer[3] = 0;
    config_buffer[4] = 0;
    config_buffer[5] = 0;
#ifdef BACKLIGHT_ENABLE
    config_buffer[6] = 0;
#endif
}

/**
 * @brief 更新内部的配置项目
 * 
 */
static void config_update()
{
    // Set up record.
    record.file_id = FILE_ID;
    record.key = RECORD_KEY;
    record.data.p_data = &config_buffer;
    record.data.length_words = 2; /* one word is four bytes. */

    // record_desc was create by fds_record_find or fds_record_write
    ret_code_t rc = fds_record_update(&record_desc, &record);
    if (rc != FDS_SUCCESS) {
        /* Handle error. */
    }
}

/**
 * @brief 创建配置项目
 * 
 */
static void config_write()
{
    // Set up record.
    record.file_id = FILE_ID;
    record.key = RECORD_KEY;
    record.data.p_data = &config_buffer;
    record.data.length_words = 2; /* one word is four bytes. */

    ret_code_t rc;
    rc = fds_record_write(&record_desc, &record);
    if (rc != FDS_SUCCESS) {
        /* Handle error. */
    }
}

/**
 * @brief 读取或创建一个配置项目
 * 
 */
static void config_read()
{
    fds_find_token_t ftok;
    fds_flash_record_t flash_record;
    /* It is required to zero the token before first use. */
    memset(&ftok, 0x00, sizeof(fds_find_token_t));

    if (fds_record_find(FILE_ID, RECORD_KEY, &record_desc, &ftok) == FDS_SUCCESS) {
        fds_record_open(&record_desc, &flash_record);
        memcpy(config_buffer, flash_record.p_data, 8);
        fds_record_close(&record_desc);
    } else {
        config_write();
    }
}

/**
 * @brief 初始化，或重置为默认值
 * 
 */
void eeconfig_init(void)
{
    if (!fds_inited) {
        config_read();
        fds_inited = true;
    } else {
        eeconfig_set_default();
        config_update();
    }
}

void eeconfig_enable(void)
{
    // todo
}

void eeconfig_disable(void)
{
    // todo
}

bool eeconfig_is_enabled(void)
{
    return fds_inited;
}

uint8_t eeconfig_read_debug(void)
{
    return config_buffer[2];
}
void eeconfig_write_debug(uint8_t val)
{
    if (config_buffer[2] != val) {
        config_buffer[2] = val;
        config_update();
    }
}

uint8_t eeconfig_read_default_layer(void)
{
    return config_buffer[3];
}
void eeconfig_write_default_layer(uint8_t val)
{
    if (config_buffer[3] != val) {
        config_buffer[3] = val;
        config_update();
    }
}

uint8_t eeconfig_read_keymap(void)
{
    return config_buffer[4];
}
void eeconfig_write_keymap(uint8_t val)
{
    if (config_buffer[4] != val) {
        config_buffer[4] = val;
        config_update();
    }
}

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight(void)
{
    return config_buffer[6];
}
void eeconfig_write_backlight(uint8_t val)
{
    if (config_buffer[6] != val) {
        config_buffer[6] = val;
        config_update();
    }
}
#endif