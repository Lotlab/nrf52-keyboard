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

#include "uart.h"
#include "CH554_SDCC.h"
#include "usb_comm.h"
#include "system.h"
#include <stdbool.h>

#define CHARGING !UCC1
#define STANDBY !UCC2

uart_state uart_rx_state;
static uint8_t len, pos;
static uint8_t __xdata recv_buff[64];
bool usb_evt = false;

static bool uart_check_flag, uart_arrive_flag, last_success;

static void uart_tx(uint8_t c)
{
    SBUF1 = c;
    while (U1TI == 0);
    U1TI = 0;
}

static uint8_t uart_rx()
{
    while (U1RI == 0);
    U1RI = 0;
    return SBUF1;
}

static uint8_t checksum()
{
    uint8_t sum = 0x00;

    for (int i = 0; i < len - 1; i++)
        sum += recv_buff[i];
    return sum == recv_buff[len - 1];
}

void uart_init()
{
    U1SM0 = 0;  // 8Bit
    U1SMOD = 1; // fast mode
    U1REN = 1;  //串口0接收使能
    SBAUD1 = 256 - FREQ_SYS / 16 / 57600;
    IE_UART1 = 1; //启用串口中断
}

static void uart_data_parser(void)
{
    uint8_t command = recv_buff[0];
    if (command & 0x80) {
        uint8_t index = (command & 0x30) >> 4;
        uint8_t kplen = (command & 0x0F);
        if (checksum()) {
            if (index == 0) {
                KeyboardGenericUpload(&recv_buff[1], kplen);
                last_success = true;
            } else if (index == 1 || index == 2) {
                recv_buff[0] = index + 1;
                KeyboardExtraUpload(recv_buff, 3);
                last_success = true;
            }
        } else {
        }
    }
}

static void uart_send_status() {
    uint8_t data = 0x10;
    if (!CHARGING && STANDBY)
        data |= 0x02;
    if (usb_evt)
        data |= 0x04;
    if (last_success)
        data |= 0x01;
    uart_tx(data);
}

static uint8_t __xdata send_buff[64];
static uint8_t send_len = 0;

void uart_check()
{
    if (uart_check_flag)
    {
        if (uart_rx_state == STATE_DATA)
        {
            // 接收超时强制退出
            uart_rx_state = STATE_IDLE;
        }
        else if ((uart_rx_state == STATE_IDLE) && uart_arrive_flag)
        {
            uart_arrive_flag = false;
            uart_data_parser();
        }

        if (uart_rx_state == STATE_IDLE) {
            if (send_len > 0) {
                for (uint8_t i = 0; i<send_len; i++) {
                    uart_tx(send_buff[i]);
                }
                send_len=0;
            } else {
                // 发送定期Query状态包
                if (last_success) {
                    uart_send_status();
                    last_success = false;
                } else {
                    uart_send_status();
                }
            }
        }
    }
    uart_check_flag = true;
}

void uart_recv(void)
{
    /**
     * Packet Format:
     *     Len  Type   Data[0] ... Data[Len-2]
     * Variable:
     *     len  buf[0] buf[1]  ... Buf[Len-1]
     */
    uint8_t data = uart_rx();

    switch (uart_rx_state)
    {
    case STATE_IDLE:
        if (data >= 0x80) {
            len = (data & 0x0F) + 2; // 实际大小加上1byte 的头和1byte的 Checksum
            pos = 0;
            recv_buff[pos++] = data;
            uart_rx_state = STATE_DATA;
        }
        else if (data >= 0x10) {
            ResponseConfigurePacket(&data, 1);
        }
        break;

    case STATE_DATA:
        recv_buff[pos++] = data;
        if (pos >= len)
        {
            uart_rx_state = STATE_IDLE;
            uart_arrive_flag = true;
        }
        break;
    }
    uart_check_flag = false;
}

void uart_send_led(uint8_t val) {
    send_buff[0] = 0x40 + (val & 0x3F);
    send_len = 1;
}

void uart_send_keymap(uint8_t* data, uint8_t len) {
    data[0] = (data[0] & 0x7F) + 0x80;
    for (uint8_t i = 0; i<len; i++)
        send_buff[i] = data[i];

    send_buff[len - 1] += 0x80; // fix checksum
    send_len = len;
}
