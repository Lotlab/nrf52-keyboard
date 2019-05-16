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

#include "sleep_reason.h"
#include "nrf.h"
#include "nrf_soc.h"

#define BIT_MASK 0x02

/**
 * @brief 读取睡眠原因
 * 
 * @return true 自动睡眠
 * @return false 手动睡眠或首次开机
 */
bool sleep_reason_get(void)
{
    uint32_t data;
    sd_power_gpregret_get(0, &data);
    return data & BIT_MASK;
}

/**
 * @brief 设置睡眠原因
 * 
 * @param val 是否为自动睡眠
 */
void sleep_reason_set(bool val)
{
    uint32_t data;
    sd_power_gpregret_get(0, &data);

    if (val) {
        data |= BIT_MASK;
    } else {
        data -= (data & BIT_MASK);
    }
    sd_power_gpregret_clr(0, 0xffffffff);
    sd_power_gpregret_set(0, data);
}