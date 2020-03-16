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

#include <string.h>

#include "CH554_SDCC.h"
#include "compiler.h"
#include "descriptor.h"
#include "endpoints.h"

#define THIS_ENDP0_SIZE DEFAULT_ENDP0_SIZE
#define REMOTE_WAKE true

/**
 * @brief 端点0/4缓冲区。
 *
 * 0x00-0x3F 为端点0的IN与OUT缓冲区
 * 0x40-0x7F 端点4 OUT
 * 0x80-0xBF 端点4 IN
 *
 */
volatile uint8_t __XDATA Ep0Buffer[MAX_PACKET_SIZE * 3];
/**
 * @brief 端点1缓冲区，用于键盘报文
 *
 * 地址0x88-0xC7为端点1OUT缓冲区 （实际使用1byte)
 * 地址0xC8-0xCF为端点1IN缓冲区 (8byte)
 *
 */
volatile uint8_t __XDATA Ep1Buffer[MAX_PACKET_SIZE * 2]; //端点1 IN缓冲区,必须是偶地址
/**
 * @brief 端点2IN缓冲区，用于System包和Consumer包的发送
 *
 */
volatile uint8_t __XDATA Ep2Buffer[MAX_PACKET_SIZE];
/**
 * @brief 端点3IN&OUT缓冲区，用于传递配置
 *
 */
volatile uint8_t __XDATA Ep3Buffer[MAX_PACKET_SIZE * 2]; //端点3 IN缓冲区,必须是偶地址

static uint8_t DataInLen, UsbConfig, UsbAddr;
static uint8_t* pDataIn;

// USB 状态
union UsbState usb_state = {
    .protocol = true, // HID规范要求默认的Protocol是Report
};

static uint8_t keyboard_idle = 0;

#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

static uint8_t ClassRequestHandler(PUSB_SETUP_REQ packet);

/**
 * 重置端点0的状态。
 * SETUP/OUT ACK
 * IN NAK
 */
#define EP0_RESET() EP_SET(0, 0, UEP_R_RES_ACK | UEP_T_RES_NAK)
/**
 * 端点0响应状态包的Status，ACK
 */
#define EP0_DATA1_ACK() EP_SET(0, (bUEP_R_TOG | bUEP_T_TOG), (UEP_R_RES_ACK | UEP_T_RES_ACK))

/**
 * @brief 为SETUP请求响应STALL
 * 
 */
#define SETUP_STALL() EP_SET(0, bUEP_R_TOG | bUEP_T_TOG, UEP_R_RES_STALL | UEP_T_RES_STALL)

void nop()
{
}

void EP0_OUT()
{
    switch (usb_state.setup_state) {
    case SETUP_IDLE:
        /* code */
        break;
    case SETUP_STATE_OUT:
        // 重置端点状态，等待下次传输
        EP0_RESET();
        usb_state.setup_state = SETUP_IDLE;
        break;
    case SETUP_DATA_OUT:
        // 似乎没有下传的数据
        break;
    default:
        // ERROR
        EP0_RESET();
        usb_state.setup_state = SETUP_IDLE;
        break;
    }
}
void EP0_IN()
{
    switch (usb_state.setup_state) {
    case SETUP_IDLE:
        /* code */
        break;

    case SETUP_DATA_IN:
        if (DataInLen == 0) {
            usb_state.setup_state = SETUP_STATE_OUT;
        }
        uint8_t len = DataInLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : DataInLen; //本次传输长度
        memcpy(Ep0Buffer, pDataIn, len); //加载上传数据
        DataInLen -= len;
        pDataIn += len;
        UEP0_T_LEN = len;
        UEP0_CTRL ^= bUEP_T_TOG; //同步标志位翻转
        break;

    case SETUP_STATE_IN:
        // 延迟设置USB设备的地址
        if (UsbAddr) {
            USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | UsbAddr;
            UsbAddr = 0;
        }
        // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
        UEP0_T_LEN = 0;
        EP0_RESET();
        usb_state.setup_state = SETUP_IDLE;
        break;
    default:
        // ERROR
        EP0_RESET();
        usb_state.setup_state = SETUP_IDLE;
        break;
    }
}
void EP0_SETUP()
{
    if (USB_RX_LEN != (sizeof(USB_SETUP_REQ))) {
        SETUP_STALL();
        return;
    }
    uint8_t datalen = 0;
    DataInLen = UsbSetupBuf->wLength > 0xFF ? 0xFF : UsbSetupBuf->wLength; // 限制总长度
    if (UsbSetupBuf->bmRequestType.Type == 0) //标准请求
    {
        switch (UsbSetupBuf->bRequest) //请求码
        {
        case USB_GET_DESCRIPTOR:
            datalen = GetUsbDescriptor(UsbSetupBuf->wValueH, UsbSetupBuf->wValueL, UsbSetupBuf->wIndexL, &pDataIn);
            usb_state.setup_state = SETUP_DATA_IN;
            break;

        case USB_SET_ADDRESS:
            UsbAddr = UsbSetupBuf->wValueL; //暂存USB设备地址
            usb_state.setup_state = SETUP_STATE_IN;
            break;

        case USB_GET_CONFIGURATION:
            Ep0Buffer[0] = UsbConfig;
            pDataIn = Ep0Buffer;
            datalen = 1;

            usb_state.setup_state = SETUP_DATA_IN;
            break;

        case USB_SET_CONFIGURATION:
            UsbConfig = UsbSetupBuf->wValueL;
            // USB枚举完毕
            usb_state.is_ready = UsbConfig > 0;
            usb_state.setup_state = SETUP_STATE_IN;
            break;

        case USB_GET_INTERFACE:
            Ep0Buffer[0] = 0x00;
            pDataIn = Ep0Buffer;
            datalen = 1;

            usb_state.setup_state = SETUP_DATA_IN;
            break;

        case USB_CLEAR_FEATURE: {
            switch (UsbSetupBuf->bmRequestType.Recipient) {
            case USB_REQ_TO_ENDPOINT: {
                switch (UsbSetupBuf->wIndexL) {
                case 0x84:
                    EP_IN_NAK_TOG(4);
                    break;
                case 0x83:
                    EP_IN_NAK_TOG(3);
                    break;
                case 0x82:
                    EP_IN_NAK_TOG(2);
                    break;
                case 0x81:
                    EP_IN_NAK_TOG(1);
                    break;
                case 0x04:
                    EP_OUT_ACK_TOG(4);
                    break;
                case 0x03:
                    EP_OUT_ACK_TOG(3);
                    break;
                case 0x02:
                    EP_OUT_ACK_TOG(2);
                    break;
                case 0x01:
                    EP_OUT_ACK_TOG(1);
                    break;
                default:
                    SETUP_STALL();
                    return;
                }
                break;
            }
#if REMOTE_WAKE
            case USB_REQ_TO_DEVICE:
                if (UsbSetupBuf->wValue != 0x01) {
                    // 操作失败
                    SETUP_STALL();
                    return;
                }
                // 设置唤醒使能标志
                usb_state.remote_wake = false;
                break;
#endif
            default: //unsupport
                SETUP_STALL();
                return;
            }
            usb_state.setup_state = SETUP_STATE_IN;
            break;
        }
        case USB_SET_FEATURE: /* Set Feature */
        {
            switch (UsbSetupBuf->bmRequestType.Recipient) {
            // 接口
            case USB_REQ_TO_ENDPOINT: {
                // 接口的Value始终为0
                if (UsbSetupBuf->wValue != 0) {
                    SETUP_STALL();
                    return;
                }
                // Zero, Interface endpoint
                switch (UsbSetupBuf->wIndex) {
                case 0x84:
                    EP_IN_STALL_TOG(4);
                    break;
                case 0x83:
                    EP_IN_STALL_TOG(3);
                    break;
                case 0x82:
                    EP_IN_STALL_TOG(2);
                    break;
                case 0x81:
                    EP_IN_STALL_TOG(1);
                    break;
                case 0x02:
                    EP_OUT_STALL_TOG(2);
                    break;
                default:
                    SETUP_STALL();
                    return;
                }
                break;
            }
#if REMOTE_WAKE
            case USB_REQ_TO_DEVICE: {
                if (UsbSetupBuf->wValue != 0x01) {
                    SETUP_STALL();
                    return;
                }
                // 设置唤醒使能标志
                usb_state.remote_wake = true;
                break;
            }
#endif
            default:
                SETUP_STALL();
                return;
            }
            usb_state.setup_state = SETUP_STATE_IN;
            break;
        }
        case USB_GET_STATUS:
            Ep0Buffer[0] = 0x00 | (usb_state.remote_wake ? 0x02 : 0x00);
            Ep0Buffer[1] = 0x00;
            datalen = 2;
            usb_state.setup_state = SETUP_DATA_IN;
            break;
        default:
            SETUP_STALL();
            return;
        }
    } else if (UsbSetupBuf->bmRequestType.Type == 1) {
        //HID类请求
        datalen = ClassRequestHandler(UsbSetupBuf);
        if (datalen == 0xFF) {
            SETUP_STALL();
            return;
        }
        if (datalen > 0) {
            usb_state.setup_state = SETUP_STATE_IN;
        } else {
            pDataIn = Ep0Buffer;
            usb_state.setup_state = SETUP_DATA_IN;
        }
    } else {
        SETUP_STALL();
        return;
    }

    switch (usb_state.setup_state) {
    case SETUP_STATE_IN:
        UEP0_T_LEN = 0; //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
        EP0_DATA1_ACK(); // 响应SETUP包的STATUS
        // usb_state.setup_state = SETUP_IDLE;
        break;
    case SETUP_DATA_IN:
        DataInLen = DataInLen > datalen ? datalen : DataInLen;
        uint8_t len = DataInLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : DataInLen; //本次传输长度
        memcpy(Ep0Buffer, pDataIn, len); //加载上传数据
        DataInLen -= len;
        pDataIn += len;

        UEP0_T_LEN = len;
        EP0_DATA1_ACK(); // 使用DATA1响应数据

        if (DataInLen == 0)
            usb_state.setup_state = SETUP_STATE_OUT;
        break;
    default:
        break;
    }
}

void EP1_IN()
{
    EP_IN_FINISH(1);
    usb_state.is_busy = false;
}

void EP2_IN()
{
    EP_IN_FINISH(2);
    usb_state.is_busy = false;
}

void EP3_IN()
{
    EP_IN_FINISH(3);
    usb_state.is_busy = false;
}

void EP4_IN()
{
    EP_IN_FINISH(4);
    UEP4_CTRL ^= bUEP_T_TOG;
    usb_state.is_busy = false;
}

static uint8_t ClassRequestHandler(PUSB_SETUP_REQ packet)
{
    uint8_t interface = packet->wIndexL;
    uint8_t recipient = UsbSetupBuf->bmRequestType.Recipient;

    switch (packet->bRequest) {
    case 0x01: //GetReport
        if (interface == 0 && recipient == USB_REQ_TO_INTERFACE) {
            memcpy(Ep0Buffer, &Ep1Buffer[64], 8);
            return 8;
        }
        break;
    case 0x02: //GetIdle
        if (interface == 0 && recipient == USB_REQ_TO_INTERFACE) {
            Ep0Buffer[0] = keyboard_idle;
            return 1;
        }
        break;
    case 0x03: //GetProtocol
        if (interface == 0 && recipient == USB_REQ_TO_INTERFACE) {
            Ep0Buffer[0] = usb_state.protocol ? 1 : 0;
            return 1;
        }
        break;
    case 0x09: //SetReport
        if (interface == 0 && recipient == USB_REQ_TO_INTERFACE) {
            Ep1Buffer[0] = Ep0Buffer[0];
            EP1_OUT();
        }
        break;
    case 0x0A: //SetIdle
        if (interface == 0 && recipient == USB_REQ_TO_INTERFACE) {
            keyboard_idle = UsbSetupBuf->wValueH;
        }
        break;
    case 0x0B: //SetProtocol
        if (interface == 0 && recipient == USB_REQ_TO_INTERFACE) {
            usb_state.protocol = UsbSetupBuf->wValueL > 0;
        }
        break;
    default:
        return 0xFF; /*命令不支持*/
        break;
    }
    return 0;
}

/** \brief USB设备模式配置,设备模式启动，收发端点配置，中断开启
 *
 */
void USBDeviceInit()
{
    IE_USB = 0;
    USB_CTRL = 0x00; // 先设定USB设备模式

    UEP0_DMA = (uint16_t)Ep0Buffer; //端点0数据传输地址
    UEP4_1_MOD |= bUEP4_RX_EN | bUEP4_TX_EN; //端点0单64字节收发缓冲区, 端点4单64字节收发缓冲区
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK; //OUT事务返回ACK，IN事务返回NAK

    UEP1_DMA = (uint16_t)Ep1Buffer; //端点1数据传输地址
    UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN | bUEP1_RX_EN; //端点1收发使能 64字节收发缓冲区
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK; //端点1自动翻转同步标志位，IN事务返回NAK

    UEP2_DMA = (uint16_t)Ep2Buffer; //端点2数据传输地址
    UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN; //端点2接收使能 64字节缓冲区
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK; //端点2自动翻转同步标志位，IN事务返回NAK

    UEP3_DMA = (uint16_t)Ep3Buffer; //端点3数据传输地址
    UEP2_3_MOD = UEP2_3_MOD & ~bUEP3_BUF_MOD | bUEP3_TX_EN | bUEP1_RX_EN; //端点3接收使能 64字节缓冲区
    UEP3_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK; //端点3自动翻转同步标志位，IN事务返回NAK

    UEP4_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK; //端点4自动翻转同步标志位，IN事务返回NAK

    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_PD_DIS; // 禁止DP/DM下拉电阻
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN; // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN; // 允许USB端口
    USB_INT_FG = 0xFF; // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}
