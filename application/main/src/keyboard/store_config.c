/*
Copyright 2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "store_config.h"
#include "data_storage.h"
#include "hid_configuration.h"
#include "keyboard_config.h"
#include "util.h"

#ifdef CONFIG_STORAGE

CONFIG_SECTION(config_slow_scan, 2);
HID_CONFIG(0x10, config_slow_scan);

/**
 * @brief 获取慢速扫描进入时间
 * 
 * @return uint16_t 
 */
uint16_t get_slow_scan_timeout(void)
{
    uint16_t data = UINT16_READ(config_slow_scan.data, 0);
    return data > 0 ? data : SLEEP_SLOW_TIMEOUT;
}

CONFIG_SECTION(config_sleep_timeout, 2);
HID_CONFIG(0x11, config_sleep_timeout);

/**
 * @brief 获取自动关机时间
 * 
 * @return uint16_t 
 */
uint16_t get_sleep_timeout(void)
{
    uint16_t data = UINT16_READ(config_sleep_timeout.data, 0);
    return data > 0 ? data : SLEEP_OFF_TIMEOUT;
}

CONFIG_SECTION(config_powersave_timeout, 2);
HID_CONFIG(0x12, config_powersave_timeout);

/**
 * @brief 获取LED省电模式进入时间
 * 
 * @return uint16_t 
 */
uint16_t get_led_powersave_timeout(void)
{
    uint16_t data = UINT16_READ(config_powersave_timeout.data, 0);
    return data > 0 ? data : LED_AUTOOFF_TIME;
}

#endif
