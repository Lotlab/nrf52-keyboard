#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    // uart rx
    PACKET_PING = 0x00,
    PACKET_LED,
    PACKET_CHARGING,
    PACKET_KEYMAP,
    PACKET_USB_STATE,

    // uart tx
    PACKET_KEYBOARD = 0x80,
    PACKET_SYSTEM,
    PACKET_COMSUMER,
    PACKET_GET_STATE,

    // uart other
    PACKET_FAIL = 0xc0,
    PACKET_ACK,
} packet_type;

void uart_recv(void);
void uart_init(void);
void uart_check(void);
void uart_send_led(uint8_t val);
void uart_send_keymap(uint8_t* data, uint8_t len);

typedef enum
{
    STATE_IDLE, // 接收完毕
    STATE_DATA, // 正在接收数据
} uart_state;

extern uart_state uart_rx_state;

extern bool usb_evt;
