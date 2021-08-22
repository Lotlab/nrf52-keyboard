/*
Copyright (C) 2019 Geno Kolar <geno@live.com>

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

#include "ble_services.h"
#include "bootloader.h"
#include "config.h"
#include "data_storage.h"
#include "eeconfig.h"
#include "keyboard_evt.h"
#include "main.h"
#include "nrf_gpio.h"
#include <stdint.h>
#include <string.h>

#ifdef POWER_BUTTON
uint8_t button_count = 0;

/**
 * @brief power button按键处理
 * 
 * @param p_context 
 */
static void button_handler(void)
{

    if (!nrf_gpio_pin_read(POWER_BUTTON) && button_count < 10) //如果BUTTON输入低电平(按下)，同时计数小于10，则仅计数
    {
        button_count++;
        return;
    } else {                                                   //如果BUTTON释放，或计数大于等于10，则根据技术进行关机、进入DFU、重置
        //1~4秒关机
        if (button_count > 1 && button_count <= 4) {
            button_count = 0;
            sleep(SLEEP_MANUALLY_NO_WAKEUP);
        }
        //5~9秒启动DFU
        if (button_count > 4 && button_count <= 9) {
            button_count = 0;
            bootloader_jump();
        }
        //10秒以上重置
        if (button_count >= 10) {
            if (button_count == 10) {
            delete_bonds();
            storage_delete(0x0F);
            storage_read(0x0F);
            }
            if (button_count >= 15) {
            NVIC_SystemReset();
            }
            button_count++;
        }
        //上述判断最大误差1秒
    }
}

/**
* @brief 初始化POWER_BUTTON按钮
 * 
 */
void buttons_init(void)
{
    nrf_gpio_cfg_sense_input(POWER_BUTTON,
        NRF_GPIO_PIN_PULLUP,
        NRF_GPIO_PIN_SENSE_LOW);
}

static void power_button_evt_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            buttons_init();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_TICK:
        button_handler();
        break;
    default:
        break;
    }
}

EVENT_HANDLER(power_button_evt_handler);

#endif
