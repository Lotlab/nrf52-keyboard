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

#include "keyboard_host_driver.h"
#include <stdint.h>

#include "../config/keyboard_config.h"
#include "report.h"

NRF_SECTION_SET_DEF(host_driver, struct host_driver*, 3);

/**
 * @brief 获取当前正在工作的host driver
 * 
 * @return struct host_driver* 
 */
static struct host_driver* get_working_driver()
{
    nrf_section_iter_t iter;
    for (nrf_section_iter_init(&iter, &host_driver);
         nrf_section_iter_get(&iter) != NULL;
         nrf_section_iter_next(&iter)) {
        struct host_driver* p_item = *(struct host_driver**)nrf_section_iter_get(&iter);
        if (p_item->driver_working())
            return p_item;
    }
    return 0;
}

uint8_t keyboard_idle = 0; // 没有用到的变量
uint8_t keyboard_protocol = 0;

uint8_t keyboard_leds()
{
    struct host_driver* driver = get_working_driver();
    if (driver != NULL)
        return driver->keyboard_leds();
    else
        return 0;
}

/**
 * @brief 发送按键包
 * 
 * @param index 类型Index。0: generic, 1: mouse, 2: system, 3: consumer, 0x80: nkro
 * @param len 长度
 * @param keys 按键
 */
void send_packet(uint8_t index, uint8_t len, uint8_t* keys)
{
    struct host_driver* driver = get_working_driver();
    if (driver != NULL)
        return driver->send_packet(index, len, keys);
}

void send_keyboard(report_keyboard_t* report)
{
#if defined(NKRO_ENABLE)
    if (keyboard_protocol && keyboard_nkro) {
        send_packet(0x80, NKRO_EPSIZE, report->raw);
    } else
#endif
    {
        send_packet(0, 8, report->raw);
    }
}

void send_mouse(report_mouse_t* report)
{
    send_packet(REPORT_ID_MOUSE, sizeof(report_mouse_t), (uint8_t*)report);
}

void send_system(uint16_t data)
{
    send_packet(REPORT_ID_SYSTEM, 2, (uint8_t*)&data);
}

void send_consumer(uint16_t data)
{
    send_packet(REPORT_ID_CONSUMER, 2, (uint8_t*)&data);
}

host_driver_t driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

/**
 * 待发送按键是否为空
 */
bool keys_queue_empty()
{
    struct host_driver* driver = get_working_driver();
    if (driver != NULL)
        return driver->queue_empty();
    return true;
}
