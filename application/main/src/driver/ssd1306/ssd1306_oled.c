/*
Copyright (C) 2019-2020 Jim Jiang <jim@lotlab.org>

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
#include <stdint.h>
#include <string.h>

#include "app_error.h"
#include "app_scheduler.h"
#include "nrf_delay.h"

#include "keyboard_battery.h"
#include "events.h"
#include "keyboard_evt.h"
#include "passkey.h"
#include "power_save.h"
#include "queue.h"

#include "i2c/shared_i2c.h"

#include "oled_graph.h"

#define SSD1306_ADDR 0x3C
static nrfx_twi_t* twi_channel;

/**
 * @brief SSD1306的显示屏初始化命令
 * 
 */
const uint8_t ssd1306_init_commands[] = {
    SSD1306_DISPLAYOFF, /* display off */
    SSD1306_SETLOWCOLUMN, /* set lower column address */
    SSD1306_SETHIGHCOLUMN, /* set higher column address */
    SSD1306_SETSTARTLINE, /* set display start line */
    SSD1306_PAGESTARTADDR, /* set page address */
    SSD1306_SETCONTRAST, /* contract control */
    0xFF, /* 128 */
#ifdef SSD1306_ROTATE_180
    SSD1306_SEGREMAP_RESET, /* set segment remap */
#else
    SSD1306_SEGREMAP_INVERSE, /* set segment remap */
#endif
    SSD1306_NORMALDISPLAY, /* normal / reverse */
    SSD1306_SETMULTIPLEX, /* multiplex ratio */
#if SSD1306_LCDHEIGHT == 64
    0x3F, /* duty = 1/32 */
#else
    0x1F, /* duty = 1/32 */
#endif
#ifdef SSD1306_ROTATE_180
    SSD1306_COMSCANINC, /* Com scan direction */
#else
    SSD1306_COMSCANDEC,
#endif
    SSD1306_SETDISPLAYOFFSET, /* set display offset */
    0x00,
    SSD1306_SETDISPLAYCLOCKDIV, /* set osc division */
    0x80,
    SSD1306_SETPRECHARGE, /* set pre-charge period */
    0xF1,
    SSD1306_SETCOMPINS, /* set COM pins */
#if SSD1306_LCDHEIGHT == 64
    0x12, /* duty = 1/32 */
#else
    0x02, /* duty = 1/32 */
#endif
    SSD1306_SETVCOMDETECT, /* set vcomh */
    0x40,
    SSD1306_CHARGEPUMP, /* set charge pump enable */
    0x14,
    SSD1306_COLUMNADDR,
    0x00,
    0x7F,
    SSD1306_PAGEADDR,
    0x00,
#if SSD1306_LCDHEIGHT == 64
    0x07,
#else
    0x03,
#endif
    SSD1306_DISPLAYON, /* display ON */
};

uint8_t ssd1306_display_buffer[128 * SSD1306_LCDHEIGHT / 8] = SSD1306_INIT_BUFF;

/**
 * @brief 发送命令或数据
 * 
 * @param is_cmd 此条是命令
 * @param len 长度
 * @param data 数据指针。若发送长度小于等于4，则会尝试复制数据，否则会直接使用指针。在长度大于4时，请保证指针非局部变量。
 */
static void ssd1306_write(bool is_cmd, uint8_t len, const uint8_t* data)
{
    shared_i2c_write(twi_channel, SSD1306_ADDR, len, data, is_cmd ? 0x00 : 0x40, false);
}

/**
 * @brief 初始化OLED屏幕
 * 
 */
static void ssd1306_oled_init()
{
    ssd1306_write(true, sizeof(ssd1306_init_commands), (uint8_t*)ssd1306_init_commands);
}

/**
 * @brief 初始化TWI驱动
 * 
 */
static void ssd1306_twi_init()
{
    twi_channel = shared_i2c_init(SSD1306_SDA, SSD1306_SCL);
    if (twi_channel == NULL)
        APP_ERROR_HANDLER(1);
}

/**
 * @brief 显示指定行的Buff
 * 
 * @param row 行号，0-3
 * @param col_start 列开始Index，0-127
 * @param len 显示长度
 */
void ssd1306_show_buff(uint8_t row, uint8_t col_start, uint8_t len)
{
    if (row >= SSD1306_ROWS)
        return;
    if (col_start + len > SSD1306_COLS)
        len = SSD1306_COLS - col_start;

    uint8_t commands[] = { 0xB0 + row, 0x00 + (col_start & 0xF), 0x10 + ((col_start >> 4) & 0xF) };
    ssd1306_write(true, sizeof(commands), commands);
    ssd1306_write(false, len, &ssd1306_display_buffer[row * 128 + col_start]);
}

/**
 * @brief 显示Buff里面的所有内容
 * 
 */
static void ssd1306_show_all()
{
    for (uint8_t i = 0; i < SSD1306_ROWS; i++) {
        ssd1306_show_buff(i, 0, SSD1306_COLS);
    }
}

/**
 * @brief 清空Buff
 * 
 */
void ssd1306_clr()
{
    memset(ssd1306_display_buffer, 0, sizeof(ssd1306_display_buffer));
}

/**
 * @brief 进入睡眠状态
 * 
 */
static void ssd1306_sleep()
{
    uint8_t cmd = SSD1306_DISPLAYOFF;
    ssd1306_write(true, 1, &cmd);
}

/**
 * @brief 退出睡眠状态 
 * 
 */
static void ssd1306_wake()
{
    uint8_t cmd = SSD1306_DISPLAYON;
    ssd1306_write(true, 1, &cmd);
}

/**
 * @brief OLED显示缓存脏标记
 * 
 */
bool ssd1306_buff_dirty[SSD1306_ROWS];

static enum connection_type conn_type = 0;
#ifdef ESB_COMM
static enum esb_keyboard_role esb_status = 0;
#endif
static bool pwr_attach = false, usb_conn = false, ble_conn = false;
static bool passkey_req = false;
static uint8_t keyboard_led = 0;
static bool status_dirty = false;

/**
 * @brief 更新标记为脏的Block
 * 
 */
void ssd1306_show_dirty_block()
{
    for (uint8_t i = 0; i < SSD1306_ROWS; i++) {
        if (ssd1306_buff_dirty[i]) {
            ssd1306_show_buff(i, 0, SSD1306_COLS);
            ssd1306_buff_dirty[i] = false;
        }
    }
}

/**
 * @brief 更新状态栏
 * 
 */
static void update_status_bar()
{
    power_save_reset();

    if (usb_conn)
        conn_type = CONN_TYPE_USB;
    else if (ble_conn)
        conn_type = CONN_TYPE_BLE;
    else
        conn_type = CONN_TYPE_NONE;

    oled_draw_icons(0, battery_info.percentage, pwr_attach, conn_type, passkey_req, keyboard_led);
    ssd1306_show_dirty_block();

    status_dirty = false;
}
#if APP_TIMER_CONFIG_USE_SCHEDULER == 1
static void status_bar_handler(void* p_event_data, uint16_t event_size)
{
    update_status_bar();
}
#endif

static void status_mark_dirty()
{
    if (!status_dirty) {
        status_dirty = true;
#if APP_TIMER_CONFIG_USE_SCHEDULER == 1
        app_sched_event_put(NULL, 0, status_bar_handler);
#else
        update_status_bar();
#endif
    }
}

static bool ssd1306_inited = false;

static void ssd1306_event_handler(enum user_event event, void* arg)
{
    uint8_t param = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (param) {
        case KBD_STATE_POST_INIT: // 初始化
            ssd1306_twi_init();
            ssd1306_oled_init();
            ssd1306_clr();
            ssd1306_inited = true;
            break;
        case KBD_STATE_INITED: // 显示Buff
            update_status_bar();
            ssd1306_show_all();
            break;
        case KBD_STATE_SLEEP: // 睡眠
            if (ssd1306_inited) {
                ssd1306_sleep();
                nrf_delay_ms(10);
            }
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE: // 处理省电模式
        switch (param) {
        case PWR_SAVE_ENTER:
            ssd1306_sleep();
            break;
        case PWR_SAVE_EXIT:
            ssd1306_wake();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_CHARGE: // 充电状态
        pwr_attach = (param != BATT_NOT_CHARGING);
        status_mark_dirty();
        break;
    case USER_EVT_USB: // USB状态
        usb_conn = (param == USB_WORKING);
        status_mark_dirty();
        break;
    case USER_EVT_BLE_PASSKEY_STATE: // 配对码状态
        passkey_req = (param != PASSKEY_STATE_SEND);
        if (param == PASSKEY_STATE_INPUT) {
            // 显示输入的配对码
            oled_draw_text_16(2, TEXT_ALIGN_CENTER, 0, (const char*)passkey);
        } else if (param == PASSKEY_STATE_SEND) {
            // 清空配对码的显示
            oled_clear_row(2);
            oled_clear_row(3);
        }
        status_mark_dirty();
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙状态
        ble_conn = (param == BLE_STATE_CONNECTED);
        status_mark_dirty();
        break;
    case USER_EVT_LED: // 键盘灯状态
        keyboard_led = param;
        status_mark_dirty();
        break;
    default:
        break;
    }
}

EVENT_HANDLER(ssd1306_event_handler);
