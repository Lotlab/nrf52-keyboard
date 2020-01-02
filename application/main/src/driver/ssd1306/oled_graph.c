/*
Copyright (C) 2020 Jim Jiang <jim@lotlab.org>

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
#include <stdlib.h>
#include <string.h>

#include "oled_graph.h"
#include "ssd1306_oled.h"

#include "font_5x8.h"
#include "font_gohu.h"
#include "icons.h"

/**
 * @brief 清空指定行的缓存
 * 
 * @param row 
 */
void oled_clear_row(uint8_t row)
{
    memset(&ssd1306_display_buffer[row * SSD1306_COLS], 0, SSD1306_COLS);
    ssd1306_buff_dirty[row] = true;
}

/**
 * @brief 在指定行显示5x8的字符
 * 
 * @param row 行
 * @param align 文本对齐方式
 * @param offset 文本偏移
 * @param str 字符串
 */
void oled_draw_text_5x8(uint8_t row, enum text_align align, uint8_t offset, const char* str)
{
    oled_clear_row(row);
    uint8_t str_len = strlen(str);
    uint8_t total_len = str_len * (5 + 1);
    int8_t start_index = 0;

    // 根据排版计算偏移
    switch (align) {
    case TEXT_ALIGN_LEFT:
        start_index = offset;
        break;
    case TEXT_ALIGN_CENTER:
        start_index = (SSD1306_COLS - total_len) / 2;
        break;
    case TEXT_ALIGN_RIGHT:
        start_index = SSD1306_COLS - offset - total_len;
        break;
    default:
        break;
    }

    // 计算偏移
    uint16_t buff_offset = row * SSD1306_COLS + start_index;

    while (*str) {
        uint8_t* pointer = (uint8_t*)&font5x8[(0x7F - 0x20) * 5];
        if (*str >= 0x20 && *str <= 0x7F) {
            pointer = (uint8_t*)&font5x8[(*str - 0x20) * 5];
        }
        for (uint8_t i = 0; i < 5; i++) {
            ssd1306_display_buffer[buff_offset + i] = pointer[i];
        }
        buff_offset += 6;
        str++;
    }
}

/**
 * @brief 在指定行显示16x8的字符
 * 
 * @param row 行
 * @param align 对齐方式 
 * @param offset 偏移
 * @param str 字符串
 */
void oled_draw_text_16(uint8_t row, enum text_align align, uint8_t offset, const char* str)
{
    oled_clear_row(row);
    oled_clear_row(row + 1);

    uint8_t str_len = strlen(str);
    uint8_t total_len = str_len * (8 + 1);
    int8_t start_index = 0;

    // 根据排版计算偏移
    switch (align) {
    case TEXT_ALIGN_LEFT:
        start_index = offset;
        break;
    case TEXT_ALIGN_CENTER:
        start_index = (SSD1306_COLS - total_len) / 2;
        break;
    case TEXT_ALIGN_RIGHT:
        start_index = SSD1306_COLS - offset - total_len;
        break;
    default:
        break;
    }

    // 计算偏移
    uint16_t buff_offset = row * SSD1306_COLS + start_index;

    while (*str) {
        uint8_t* pointer = (uint8_t*)GohuFont.chars[0].image->data;
        if (*str >= 0x20 && *str <= 0x7E) {
            pointer = (uint8_t*)GohuFont.chars[(*str - 0x20)].image->data;
        }
        for (uint8_t i = 0; i < 8; i++) {
            ssd1306_display_buffer[buff_offset + i] = pointer[i];
            ssd1306_display_buffer[buff_offset + i + SSD1306_COLS] = pointer[i + 8];
        }
        buff_offset += 9;
        str++;
    }
}

/**
 * @brief 在指定位置上画一个图标
 * 
 * @param row 行
 * @param col 列
 * @param icon 图标
 */
void oled_draw_icon(uint8_t row, uint8_t col, const tImage* icon)
{
    uint8_t bit = (icon->height - 1) / 8 + 1;
    for (uint8_t c = col, i = 0; c < SSD1306_COLS && i < icon->width; i++, c++) {
        for (uint8_t r = row, j = 0; r < SSD1306_ROWS && j < bit; j++, r++) {
            ssd1306_display_buffer[r * SSD1306_COLS + c] = icon->data[j * icon->width + i];
        }
    }
}

/**
 * @brief 绘制状态栏图标
 * 
 * @param row 行
 * @param batt 电量状态
 * @param charging 是否充电
 * @param conn 当前连接状态
 * @param passkey 是否请求Passkey
 * @param led 键盘LED状态
 */
void oled_draw_icons(uint8_t row, uint8_t batt, bool charging, enum connection_type conn, bool passkey, uint8_t led)
{
    oled_clear_row(row);
    oled_clear_row(row + 1);

    uint8_t index = SSD1306_COLS;

    // 使用特殊方法绘制电池图标
    index -= Icons[ICON_BATT_0]->width;
    uint8_t batt_lvl = batt / (100 / (ICON_BATT_END - ICON_BATT_START));
    for (uint8_t i = 0; i < Icons[ICON_BATT_0]->width; i++) {
        if (i > ICON_BATT_START + batt_lvl) {
            ssd1306_display_buffer[row * SSD1306_COLS + i + index] = Icons[ICON_BATT_0]->data[i];
            ssd1306_display_buffer[(row + 1) * SSD1306_COLS + i + index] = Icons[ICON_BATT_0]->data[i + Icons[ICON_BATT_0]->width];
        } else {
            ssd1306_display_buffer[row * SSD1306_COLS + i + index] = Icons[ICON_BATT_100]->data[i];
            ssd1306_display_buffer[(row + 1) * SSD1306_COLS + i + index] = Icons[ICON_BATT_100]->data[i + Icons[ICON_BATT_0]->width];
        }
    }

    // 充电图标
    if (charging) {
        index -= Icons[ICON_CHARGE]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_CHARGE]);
    }

    // 连接类型图标
    switch (conn) {
    case CONN_TYPE_BLE:
        index -= Icons[ICON_BLUETOOTH]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_BLUETOOTH]);
        break;
    case CONN_TYPE_USB:
        index -= Icons[ICON_USB]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_USB]);
        break;
    case CONN_TYPE_GAZELL:
        index -= Icons[ICON_GAZELL]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_GAZELL]);
        break;
    default:
        break;
    }

    // Passkey
    if (passkey) {
        index -= Icons[ICON_PASSKEY]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_PASSKEY]);
    }

    // 各个LED
    if (led & 0x04) {
        index -= Icons[ICON_SCROLLLOCK]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_SCROLLLOCK]);
    }

    if (led & 0x02) {
        index -= Icons[ICON_CAPSLOCK]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_CAPSLOCK]);
    }

    if (led & 0x01) {
        index -= Icons[ICON_NUMLOCK]->width + ICON_SPACING;
        oled_draw_icon(row, index, Icons[ICON_NUMLOCK]);
    }
}
