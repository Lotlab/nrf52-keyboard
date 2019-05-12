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
static packet_type send_type;
bool usb_evt = false;

static bool uart_check_flag, uart_arrive_flag, last_success, sending;

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

    for (int i = 1; i < len - 1; i++)
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
        uint8_t len = (command & 0x0F);
        if (checksum()) {
            if (index == 0) {
                KeyboardGenericUpload(&recv_buff[1], len - 2);
                last_success = true;
            } else if (index == 1 || index == 2) {
                recv_buff[0] = index + 1;
                KeyboardExtraUpload(recv_buff, 3);
                last_success = true;
            }
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

        // 发送定期Query状态包
        if (!sending) {
            if (last_success) {
                uart_send_status();
                last_success = false;
            } else {
                uart_send_status();
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
            len = data & 0x0F + 2; // 实际大小加上1byte 的头和1byte的 Checksum
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

void uart_send(uint8_t *data, uint8_t len)
{
    sending = true;
    while (len--)
        uart_tx(*(data++));

    sending = false;
}

void uart_send_led(uint8_t val) {
    uint8_t data = 0x40 + val & 0x3F;
    uart_tx(data);
}

void uart_send_keymap(uint8_t* data, uint8_t len) {
    data[0] = data[0] & 0x7F + 0x80;
    uart_send(data, len);
}
