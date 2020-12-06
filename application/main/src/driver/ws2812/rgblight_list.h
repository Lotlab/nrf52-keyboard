/* Copyright 2018 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef WS2812_LIST_H
#define WS2812_LIST_H

/*                            SET RGB List                            */
#define ws2812_setrgb_white()       ws2812_setrgb (0xFF, 0xFF, 0xFF)
#define ws2812_setrgb_red()         ws2812_setrgb (0xFF, 0x00, 0x00)
#define ws2812_setrgb_coral()       ws2812_setrgb (0xFF, 0x7C, 0x4D)
#define ws2812_setrgb_orange()      ws2812_setrgb (0xFF, 0x80, 0x00)
#define ws2812_setrgb_goldenrod()   ws2812_setrgb (0xD9, 0xA5, 0x21)
#define ws2812_setrgb_gold()        ws2812_setrgb (0xFF, 0xD9, 0x00)
#define ws2812_setrgb_yellow()      ws2812_setrgb (0xFF, 0xFF, 0x00)
#define ws2812_setrgb_chartreuse()  ws2812_setrgb (0x80, 0xFF, 0x00)
#define ws2812_setrgb_green()       ws2812_setrgb (0x00, 0xFF, 0x00)
#define ws2812_setrgb_springgreen() ws2812_setrgb (0x00, 0xFF, 0x80)
#define ws2812_setrgb_turquoise()   ws2812_setrgb (0x47, 0x6E, 0x6A)
#define ws2812_setrgb_teal()        ws2812_setrgb (0x00, 0x80, 0x80)
#define ws2812_setrgb_cyan()        ws2812_setrgb (0x00, 0xFF, 0xFF)
#define ws2812_setrgb_azure()       ws2812_setrgb (0x99, 0xf5, 0xFF)
#define ws2812_setrgb_blue()        ws2812_setrgb (0x00, 0x00, 0xFF)
#define ws2812_setrgb_purple()      ws2812_setrgb (0x7A, 0x00, 0xFF)
#define ws2812_setrgb_magenta()     ws2812_setrgb (0xFF, 0x00, 0xFF)
#define ws2812_setrgb_pink()        ws2812_setrgb (0xFF, 0x80, 0xBF)

/*                            SET RGB List                            */
#define ws2812_setrgb_white_at(at)       ws2812_setrgb_at (0xFF, 0xFF, 0xFF, at)
#define ws2812_setrgb_red_at(at)         ws2812_setrgb_at (0xFF, 0x00, 0x00, at)
#define ws2812_setrgb_coral_at(at)       ws2812_setrgb_at (0xFF, 0x7C, 0x4D, at)
#define ws2812_setrgb_orange_at(at)      ws2812_setrgb_at (0xFF, 0x80, 0x00, at)
#define ws2812_setrgb_goldenrod_at(at)   ws2812_setrgb_at (0xD9, 0xA5, 0x21, at)
#define ws2812_setrgb_gold_at(at)        ws2812_setrgb_at (0xFF, 0xD9, 0x00, at)
#define ws2812_setrgb_yellow_at(at)      ws2812_setrgb_at (0xFF, 0xFF, 0x00, at)
#define ws2812_setrgb_chartreuse_at(at)  ws2812_setrgb_at (0x80, 0xFF, 0x00, at)
#define ws2812_setrgb_green_at(at)       ws2812_setrgb_at (0x00, 0xFF, 0x00, at)
#define ws2812_setrgb_springgreen_at(at) ws2812_setrgb_at (0x00, 0xFF, 0x80, at)
#define ws2812_setrgb_turquoise_at(at)   ws2812_setrgb_at (0x47, 0x6E, 0x6A, at)
#define ws2812_setrgb_teal_at(at)        ws2812_setrgb_at (0x00, 0x80, 0x80, at)
#define ws2812_setrgb_cyan_at(at)        ws2812_setrgb_at (0x00, 0xFF, 0xFF, at)
#define ws2812_setrgb_azure_at(at)       ws2812_setrgb_at (0x99, 0xf5, 0xFF, at)
#define ws2812_setrgb_blue_at(at)        ws2812_setrgb_at (0x00, 0x00, 0xFF, at)
#define ws2812_setrgb_purple_at(at)      ws2812_setrgb_at (0x7A, 0x00, 0xFF, at)
#define ws2812_setrgb_magenta_at(at)     ws2812_setrgb_at (0xFF, 0x00, 0xFF, at)
#define ws2812_setrgb_pink_at(at)        ws2812_setrgb_at (0xFF, 0x80, 0xBF, at)

/*                            SET HSV List                            */
#define ws2812_sethsv_white()       ws2812_sethsv (  0,   0, 255)
#define ws2812_sethsv_red()         ws2812_sethsv (  0, 255, 255)
#define ws2812_sethsv_coral()       ws2812_sethsv ( 16, 176, 255)
#define ws2812_sethsv_orange()      ws2812_sethsv ( 39, 255, 255)
#define ws2812_sethsv_goldenrod()   ws2812_sethsv ( 43, 218, 218)
#define ws2812_sethsv_gold()        ws2812_sethsv ( 51, 255, 255)
#define ws2812_sethsv_yellow()      ws2812_sethsv ( 60, 255, 255)
#define ws2812_sethsv_chartreuse()  ws2812_sethsv ( 90, 255, 255)
#define ws2812_sethsv_green()       ws2812_sethsv (120, 255, 255)
#define ws2812_sethsv_springgreen() ws2812_sethsv (150, 255, 255)
#define ws2812_sethsv_turquoise()   ws2812_sethsv (174,  90, 112)
#define ws2812_sethsv_teal()        ws2812_sethsv (180, 255, 128)
#define ws2812_sethsv_cyan()        ws2812_sethsv (180, 255, 255)
#define ws2812_sethsv_azure()       ws2812_sethsv (186, 102, 255)
#define ws2812_sethsv_blue()        ws2812_sethsv (240, 255, 255)
#define ws2812_sethsv_purple()      ws2812_sethsv (270, 255, 255)
#define ws2812_sethsv_magenta()     ws2812_sethsv (300, 255, 255)
#define ws2812_sethsv_pink()        ws2812_sethsv (330, 128, 255)

/*                            SET HSV List                            */
/*   If you're doing layer indication, this is best, as it won't      */
/*   write to the eeprom, since it's limited (very high value).       */
/*   If you want to use modes with this (since you can), then you     */
/*   want to use ws2812_mode_noeeprom(x) instead.                   */
#define ws2812_sethsv_noeeprom_white()       ws2812_sethsv_noeeprom (  0,   0, 255)
#define ws2812_sethsv_noeeprom_red()         ws2812_sethsv_noeeprom (  0, 255, 255)
#define ws2812_sethsv_noeeprom_coral()       ws2812_sethsv_noeeprom ( 16, 176, 255)
#define ws2812_sethsv_noeeprom_orange()      ws2812_sethsv_noeeprom ( 39, 255, 255)
#define ws2812_sethsv_noeeprom_goldenrod()   ws2812_sethsv_noeeprom ( 43, 218, 218)
#define ws2812_sethsv_noeeprom_gold()        ws2812_sethsv_noeeprom ( 51, 255, 255)
#define ws2812_sethsv_noeeprom_yellow()      ws2812_sethsv_noeeprom ( 60, 255, 255)
#define ws2812_sethsv_noeeprom_chartreuse()  ws2812_sethsv_noeeprom ( 90, 255, 255)
#define ws2812_sethsv_noeeprom_green()       ws2812_sethsv_noeeprom (120, 255, 255)
#define ws2812_sethsv_noeeprom_springgreen() ws2812_sethsv_noeeprom (150, 255, 255)
#define ws2812_sethsv_noeeprom_turquoise()   ws2812_sethsv_noeeprom (174,  90, 112)
#define ws2812_sethsv_noeeprom_teal()        ws2812_sethsv_noeeprom (180, 255, 128)
#define ws2812_sethsv_noeeprom_cyan()        ws2812_sethsv_noeeprom (180, 255, 255)
#define ws2812_sethsv_noeeprom_azure()       ws2812_sethsv_noeeprom (186, 102, 255)
#define ws2812_sethsv_noeeprom_blue()        ws2812_sethsv_noeeprom (240, 255, 255)
#define ws2812_sethsv_noeeprom_purple()      ws2812_sethsv_noeeprom (270, 255, 255)
#define ws2812_sethsv_noeeprom_magenta()     ws2812_sethsv_noeeprom (300, 255, 255)
#define ws2812_sethsv_noeeprom_pink()        ws2812_sethsv_noeeprom (330, 128, 255)

/*                            SET HSV List                            */
#define ws2812_sethsv_white_at(at)       ws2812_sethsv_at (  0,   0, 255, at)
#define ws2812_sethsv_red_at(at)         ws2812_sethsv_at (  0, 255, 255, at)
#define ws2812_sethsv_coral_at(at)       ws2812_sethsv_at ( 16, 176, 255, at)
#define ws2812_sethsv_orange_at(at)      ws2812_sethsv_at ( 39, 255, 255, at)
#define ws2812_sethsv_goldenrod_at(at)   ws2812_sethsv_at ( 43, 218, 218, at)
#define ws2812_sethsv_gold_at(at)        ws2812_sethsv_at ( 51, 255, 255, at)
#define ws2812_sethsv_yellow_at(at)      ws2812_sethsv_at ( 60, 255, 255, at)
#define ws2812_sethsv_chartreuse_at(at)  ws2812_sethsv_at ( 90, 255, 255, at)
#define ws2812_sethsv_green_at(at)       ws2812_sethsv_at (120, 255, 255, at)
#define ws2812_sethsv_springgreen_at(at) ws2812_sethsv_at (150, 255, 255, at)
#define ws2812_sethsv_turquoise_at(at)   ws2812_sethsv_at (174,  90, 112, at)
#define ws2812_sethsv_teal_at(at)        ws2812_sethsv_at (180, 255, 128, at)
#define ws2812_sethsv_cyan_at(at)        ws2812_sethsv_at (180, 255, 255, at)
#define ws2812_sethsv_azure_at(at)       ws2812_sethsv_at (186, 102, 255, at)
#define ws2812_sethsv_blue_at(at)        ws2812_sethsv_at (240, 255, 255, at)
#define ws2812_sethsv_purple_at(at)      ws2812_sethsv_at (270, 255, 255, at)
#define ws2812_sethsv_magenta_at(at)     ws2812_sethsv_at (300, 255, 255, at)
#define ws2812_sethsv_pink_at(at)        ws2812_sethsv_at (330, 128, 255, at)

#endif
