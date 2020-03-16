#pragma once

#include <stdbool.h>
#include <stdint.h>

extern volatile uint8_t __XDATA Ep0Buffer[];
extern volatile uint8_t __XDATA Ep1Buffer[];
extern volatile uint8_t __XDATA Ep2Buffer[];
extern volatile uint8_t __XDATA Ep3Buffer[];

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

#ifndef ONBOARD_CMSIS_DAP
#define EP4_OUT nop
#endif

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

/**
 * 设置指定Endpoint的状态
 * 
 * @param num: Endpoint号
 * @param data: 指定DATA0或DATA1
 * @param resp: 默认应答
 **/
#define EP_SET(num, data, resp) (UEP##num##_CTRL = ((data) | (resp)) & 0xFF)

/**
 * @brief 设置指定Endpoint IN的响应
 * 
 * @param num: Endpoint号
 * @param resp: 默认应答
 */
#define EP_IN_RESP(num, resp) (UEP##num##_CTRL = UEP##num##_CTRL & (~MASK_UEP_T_RES) | (resp))

/**
 * @brief 设置指定Endpoint OUT的响应
 * 
 * @param num: Endpoint号
 * @param resp: 默认应答
 */
#define EP_OUT_RESP(num, resp) (UEP##num##_CTRL = UEP##num##_CTRL & (~MASK_UEP_R_RES) | ((resp) << 2))

/**
 * 设置端点IN NAK
 */
#define EP_IN_NAK(num) (UEP##num##_CTRL = UEP##num##_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK)

/**
 * 设置端点OUT NAK
 */
#define EP_OUT_NAK(num) (UEP##num##_CTRL = UEP##num##_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_NAK)

/**
 * 端点 IN 后续处理
 * 清空发送长度，并设置NAK
 */
#define EP_IN_FINISH(num) \
    UEP##num##_T_LEN = 0; \
    EP_IN_NAK(num)

/**
 * 设置端点 IN NAK 响应并反转 DATA
 */
#define EP_IN_NAK_TOG(num) (UEP##num##_CTRL = UEP##num##_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK)
/**
 * 设置端点 OUT ACK 响应并反转 DATA
 */
#define EP_OUT_ACK_TOG(num) (UEP##num##_CTRL = UEP##num##_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK)
/**
 * 设置端点 IN STALL 响应并反转 DATA
 */
#define EP_IN_STALL_TOG(num) (UEP##num##_CTRL = UEP##num##_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL)
/**
 * 设置端点 OUT STALL 响应并反转 DATA
 */
#define EP_OUT_STALL_TOG(num) (UEP##num##_CTRL = UEP##num##_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL)
/**
 * 设置端点 OUT NAK 响应并反转 DATA
 */
#define EP_OUT_NAK_TOG(num) (UEP##num##_CTRL = UEP##num##_CTRL & (~bUEP_R_TOG) | UEP_R_RES_NAK)
