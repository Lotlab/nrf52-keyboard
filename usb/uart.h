#ifndef __UART__DRIVER__
#define __UART__DRIVER__

#include <stdint.h>

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

void uart_send(packet_type type, uint8_t *data, uint8_t len);
void uart_recv(void);
void uart_init(void);
void uart_check(void);

typedef enum
{
    STATE_IDLE, // 接收完毕
    STATE_DATA, // 正在接收数据
} uart_state;

extern uart_state uart_rx_state;

#endif // __UART__DRIVER__
