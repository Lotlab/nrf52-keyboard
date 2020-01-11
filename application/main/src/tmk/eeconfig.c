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
#include "data_storage.h"
#include "util.h"
#include <string.h>

CONFIG_SECTION(eeconfig, 6);

static bool eeconfig_inited;

static void eeconfig_set_default()
{
    UINT16_WRITE(eeconfig.data, 0, EECONFIG_MAGIC_NUMBER);
    eeconfig.data[2] = 0;
    eeconfig.data[3] = 0;
    eeconfig.data[4] = 0;
    eeconfig.data[5] = 0;
#ifdef BACKLIGHT_ENABLE
    eeconfig.data[6] = 0;
#endif
}

/**
 * @brief 初始化，或重置为默认值
 * 
 */
void eeconfig_init(void)
{
    if (!eeconfig_inited) {
        // eeconfig 已经在data storage 里面读取了，不需要再次操作
        eeconfig_inited = true;
    } else {
        eeconfig_set_default();
        storage_write((1 << STORAGE_CONFIG));
    }
}

void eeconfig_enable(void)
{
    UINT16_WRITE(eeconfig.data, 0, EECONFIG_MAGIC_NUMBER);
}

void eeconfig_disable(void)
{
    UINT16_WRITE(eeconfig.data, 0, 0);
}

bool eeconfig_is_enabled(void)
{
    return eeconfig_inited && UINT16_READ(eeconfig.data, 0) == EECONFIG_MAGIC_NUMBER;
}

uint8_t eeconfig_read_debug(void)
{
    return eeconfig.data[2];
}
void eeconfig_write_debug(uint8_t val)
{
    if (eeconfig.data[2] != val) {
        eeconfig.data[2] = val;
        storage_write((1 << STORAGE_CONFIG));
    }
}

uint8_t eeconfig_read_default_layer(void)
{
    return eeconfig.data[3];
}
void eeconfig_write_default_layer(uint8_t val)
{
    if (eeconfig.data[3] != val) {
        eeconfig.data[3] = val;
        storage_write((1 << STORAGE_CONFIG));
    }
}

uint8_t eeconfig_read_keymap(void)
{
    return eeconfig.data[4];
}
void eeconfig_write_keymap(uint8_t val)
{
    if (eeconfig.data[4] != val) {
        eeconfig.data[4] = val;
        storage_write((1 << STORAGE_CONFIG));
    }
}

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight(void)
{
    return eeconfig.data[6];
}
void eeconfig_write_backlight(uint8_t val)
{
    if (eeconfig.data[6] != val) {
        eeconfig.data[6] = val;
        storage_write((1 << STORAGE_CONFIG));
    }
}
#endif