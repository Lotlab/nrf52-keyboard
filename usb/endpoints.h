#pragma once

#include <stdbool.h>
#include <stdint.h>

extern uint8_t __XDATA_AT(0x00) Ep0Buffer[];
extern uint8_t __XDATA_AT(0xc0) Ep1Buffer[];
extern uint8_t __XDATA_AT(0x140) Ep2Buffer[];
extern uint8_t __XDATA_AT(0x180) Ep3Buffer[];

enum UsbSetupState {
    SETUP_IDLE,     // 当前处于空闲状态
    SETUP_DATA_IN,  // 即将传出数据 (DATA IN)
    SETUP_DATA_OUT, // 即将传入数据 (DATA OUT)
    SETUP_STATE_IN, // 传入后返回状态 (DATA IN)
    SETUP_STATE_OUT // 传出后返回状态 (DATA OUT)
};

union UsbState
{
    uint8_t raw;
    struct
    {
        bool is_ready: 1; // 设备已配置
        bool is_busy: 1; // 数据等待发送
        bool is_sleep: 1; // 设备已挂起
        bool remote_wake: 1; // 支持远程唤醒
        bool protocol: 1; // 当前HID报文类型
        enum UsbSetupState setup_state: 3;
    };
};

extern union UsbState usb_state;

extern void USBDeviceInit();

extern void EP0_OUT();
extern void EP0_IN();
extern void EP0_SETUP();

extern void EP1_IN();
extern void EP1_OUT();

extern void EP2_IN();
extern void EP2_OUT();

extern void EP3_IN();
extern void EP3_OUT();

extern void EP4_IN();
extern void EP4_OUT();

extern void nop();

#define USB_OUT_EP0 EP0_OUT
#define USB_OUT_EP1 EP1_OUT
#define USB_OUT_EP2 nop
#define USB_OUT_EP3 EP3_OUT
#define USB_OUT_EP4 EP4_OUT

#define USB_IN_EP0 EP0_IN
#define USB_IN_EP1 EP1_IN
#define USB_IN_EP2 EP2_IN
#define USB_IN_EP3 EP3_IN
#define USB_IN_EP4 EP4_IN

#define USB_SETUP_EP0 EP0_SETUP
#define USB_SETUP_EP1 nop
#define USB_SETUP_EP2 nop
#define USB_SETUP_EP3 nop
#define USB_SETUP_EP4 nop

#define USB_SOF_EP0 nop
#define USB_SOF_EP1 nop
#define USB_SOF_EP2 nop
#define USB_SOF_EP3 nop
#define USB_SOF_EP4 nop
