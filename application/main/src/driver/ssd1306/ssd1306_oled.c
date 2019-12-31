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

#include "ssd1306_oled.h"
#include "events.h"
#include "keyboard_evt.h"
#include "sdk_config.h"
#include <stdint.h>
#include <string.h>

#include "app_error.h"
#include "nrf_drv_twi.h"

#define SSD1306_SDA 9
#define SSD1306_SCL 10
// #define SSD1306_ADDR 0x3C
#define SSD1306_ADDR 0x3C

const uint8_t ssd1306_init_commands[] = {
    0xAE, /* display off */
    0x00, /* set lower column address */
    0x10, /* set higher column address */
    0x00, /* set display start line */
    0xB0, /* set page address */
    0x81, /* contract control */
    0xff, /* 128 */
    0xA1, /* set segment remap */
    0xA6, /* normal / reverse */
    0xA8, /* multiplex ratio */
    0x1F, /* duty = 1/32 */
    0xC8, /* Com scan direction */
    0xD3, /* set display offset */
    0x00,
    0xD5, /* set osc division */
    0x80,
    0xD9, /* set pre-charge period */
    0x1f,
    0xDA, /* set COM pins */
    0x00,
    0xdb, /* set vcomh */
    0x40,
    0x8d, /* set charge pump enable */
    0x14,
    0xAF, /* display ON */
    // 0xA4, /* Entire on */
};

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);
/*
static void ssd1306_write_cmd(uint8_t cmd)
{
    uint8_t buff[2];
    buff[0] = 0x00; // command
    buff[1] = cmd;

    nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, buff, 2, false);
}*/
static void ssd1306_write_cmds(uint8_t len, uint8_t* data)
{
    uint8_t buff[65];
    buff[0] = 0x00; // command
    memcpy(&buff[1], data, len);

    nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, buff, len + 1, false);
}

static void ssd1306_write_data(uint8_t len, uint8_t* data)
{
    uint8_t buff[64];
    buff[0] = 0x40; // data
    memcpy(&buff[1], data, len);
    nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, buff, len + 1, false);
}

static void ssd1306_oled_init()
{
    /*
    for (uint8_t i = 0; i < sizeof(ssd1306_init_commands); i++) {
        ssd1306_write_cmd(ssd1306_init_commands[i]);
    }*/
    ssd1306_write_cmds(sizeof(ssd1306_init_commands), (uint8_t *)ssd1306_init_commands);
}

static void ssd1306_twi_init()
{
    const nrf_drv_twi_config_t ssd1306_config = {
        .scl = SSD1306_SCL,
        .sda = SSD1306_SDA,
        .frequency = NRF_DRV_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW,
        .clear_bus_init = false
    };

    uint32_t err_code = nrf_drv_twi_init(&m_twi, &ssd1306_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

static void ssd1306_test()
{
    const uint8_t data[16] = { 0x55 };
    ssd1306_write_data(16, (uint8_t*)data);
}

static void ssd1306_event_handler(enum user_event event, void* arg)
{
    uint8_t param = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (param) {
        case KBD_STATE_POST_INIT:
            ssd1306_twi_init();
            ssd1306_oled_init();
            break;
        case KBD_STATE_INITED:
            ssd1306_test();
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

EVENT_HANDLER(ssd1306_event_handler);
