#include "uart.h"
#include "CH554_SDCC.h"
#include "usb_comm.h"
#include "system.h"
#include <stdbool.h>

#define CHARGING UCC1
#define STANDBY UCC2

uart_state uart_rx_state;
static uint8_t len, pos;
static uint8_t __xdata recv_buff[64];
static packet_type send_type;

static bool uart_check_flag, uart_arrive_flag;

static void uart_tx(uint8_t c)
{
    SBUF1 = c;
    while (U1TI == 0)
        ;
    U1TI = 0;
}

static uint8_t uart_rx()
{
    while (U1RI == 0)
        ;
    U1RI = 0;
    return SBUF1;
}

static void uart_ack()
{
    uart_send(PACKET_ACK, 0, 0);
}
static void uart_fail()
{
    uart_send(PACKET_FAIL, 0, 0);
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
    switch ((packet_type)recv_buff[0])
    {
    case PACKET_ACK:
    case PACKET_FAIL:
        if (send_type == PACKET_KEYMAP)
        {
            ResponseConfigurePacket(recv_buff, 1);
        }
        break;
    case PACKET_KEYBOARD:
        if (checksum())
        {
            KeyboardGenericUpload(&recv_buff[1], len - 2);
            uart_ack();
        }
        else
        {
            uart_fail();
        }

        break;
    case PACKET_SYSTEM:
        // 使用type位置暂存一下ID
        recv_buff[0] = 2;
        KeyboardExtraUpload(recv_buff, 3);
        uart_ack();
        break;
    case PACKET_COMSUMER:
        recv_buff[0] = 3;
        KeyboardExtraUpload(recv_buff, 3);
        uart_ack();
        break;
    case PACKET_GET_STATE:
        recv_buff[0] = CHARGING;
        uart_send(PACKET_CHARGING, recv_buff, 1);
        break;
    }
}

static bool length_check(void)
{
    switch ((packet_type)recv_buff[0])
    {
    case PACKET_KEYBOARD:
        return len == 10;
    case PACKET_SYSTEM:
    case PACKET_COMSUMER:
        return len == 4;
    case PACKET_GET_STATE:
    case PACKET_FAIL:
    case PACKET_ACK:
        return len == 1;
    default:
        return false;
    }
}

void uart_check()
{
    if (uart_check_flag)
    {
        if (uart_rx_state == STATE_DATA)
        {
            // 接收超时强制退出并请求重发
            uart_rx_state = STATE_IDLE;
            // uart_fail();
        }
        else if ((uart_rx_state == STATE_IDLE) && uart_arrive_flag)
        {
            uart_arrive_flag = false;
            if (length_check())
            {
                uart_data_parser();
            }
            else
            {
                uart_fail();
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
    switch (uart_rx_state)
    {
    case STATE_IDLE:
        len = uart_rx();
        pos = 0;
        if (len > 0) // Len=0 意味着出错了，别管它
            uart_rx_state = STATE_DATA;
        break;

    case STATE_DATA:
        recv_buff[pos++] = uart_rx();
        if (pos >= len)
        {
            uart_rx_state = STATE_IDLE;
            uart_arrive_flag = true;
        }
        break;
    }
    uart_check_flag = false;
}

void uart_send(packet_type type, uint8_t *data, uint8_t len)
{
    send_type = type;
    uart_tx(len + 1);
    uart_tx(type);
    while (len--)
    {
        uart_tx(*(data++));
    }
}
