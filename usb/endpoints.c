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

/**
 * @brief 端点0缓冲区。
 *
 * 地址0x00-0x08 为端点0的IN与OUT缓冲区
 *
 */
uint8_t __XDATA_AT(0x00) Ep0Buffer[THIS_ENDP0_SIZE];
/**
 * @brief 端点1缓冲区，用于键盘报文
 *
 * 地址0x88-0xC7为端点1OUT缓冲区 （实际使用1byte)
 * 地址0xC8-0xCF为端点1IN缓冲区 (8byte)
 *
 */
uint8_t __XDATA_AT(0x0A) Ep1Buffer[MAX_PACKET_SIZE * 2]; //端点1 IN缓冲区,必须是偶地址
/**
 * @brief 端点2IN缓冲区，用于System包和Consumer包的发送
 *
 */
uint8_t __XDATA_AT(0x90) Ep2Buffer[MAX_PACKET_SIZE];
/**
 * @brief 端点3IN&OUT缓冲区，用于传递配置
 *
 */
uint8_t __XDATA_AT(0xB0) Ep3Buffer[MAX_PACKET_SIZE * 2]; //端点3 IN缓冲区,必须是偶地址

bool usb_ready = false;
bool usb_busy = false;
static uint8_t SetupReq, SetupLen, Count, UsbConfig;
static uint8_t* pDescr;

// 键盘报文类型。0为Boot，1为Report
uint8_t keyboard_protocol = 1; // HID规范要求默认的Protocol是Report
static uint8_t keyboard_idle = 0;

static USB_SETUP_REQ SetupReqBuf; //暂存Setup包
#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

static uint8_t ClassRequestHandler(PUSB_SETUP_REQ packet);

/**
 * 设置指定Endpoint的状态
 * 
 * @param num: Endpoint号
 * @param data: 指定DATA0或DATA1
 * @param resp: 默认应答
 **/
#define EP_SET(num, data, resp) (UEP##num##_CTRL = (data) | (resp))
#define EP_IN_RESP(num, resp) (UEP##num##_CTRL = UEP##num##_CTRL & (~MASK_UEP_T_RES) | (resp))
#define EP_OUT_RESP(num, resp) (UEP##num##_CTRL = UEP##num##_CTRL & (~MASK_UEP_R_RES) | ((resp) << 2))
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
 * 设置端点IN NAK
 */
#define EP_IN_NAK(num) (UEP##num##_CTRL = UEP##num##_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK)

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

/***/
void EP0_OUT()
{
    switch (SetupReq) {
    case USB_GET_DESCRIPTOR:
        // 重置端点状态，等待下次传输
        EP0_RESET();
        break;
    }
}
void EP0_IN()
{
    uint8_t len = 0;
    switch (SetupReq) {
    case USB_GET_DESCRIPTOR:
        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //本次传输长度
        memcpy(Ep0Buffer, pDescr, len); //加载上传数据
        SetupLen -= len;
        pDescr += len;
        UEP0_T_LEN = len;
        UEP0_CTRL ^= bUEP_T_TOG; //同步标志位翻转
        break;
    case USB_SET_ADDRESS: // 延迟设置USB设备的地址
        USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
        EP0_RESET();
        break;
    default:
        UEP0_T_LEN = 0; //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
        EP0_RESET();
        break;
    }
}
void EP0_SETUP()
{
    uint8_t len = USB_RX_LEN;
    if (len == (sizeof(USB_SETUP_REQ))) {
        SetupLen = UsbSetupBuf->wLengthL;
        if (UsbSetupBuf->wLengthH) {
            SetupLen = 0xFF; // 限制总长度
        }
        len = 0; // 默认为成功并且上传0长度
        SetupReq = UsbSetupBuf->bRequest;
        uint8_t type = UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK;
        if (type == USB_REQ_TYP_STANDARD) //标准请求
        {
            switch (SetupReq) //请求码
            {
            case USB_GET_DESCRIPTOR:
                len = GetUsbDescriptor(UsbSetupBuf->wValueH, UsbSetupBuf->wValueL, UsbSetupBuf->wIndexL, &pDescr);

                if (SetupLen > len)
                    SetupLen = len; //限制总长度
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //本次传输长度
                memcpy(Ep0Buffer, pDescr, len); //加载上传数据
                SetupLen -= len;
                pDescr += len;
                break;

            case USB_SET_ADDRESS:
                SetupLen = UsbSetupBuf->wValueL; //暂存USB设备地址
                break;

            case USB_GET_CONFIGURATION:
                Ep0Buffer[0] = UsbConfig;
                if (SetupLen >= 1)
                    len = 1;
                break;

            case USB_SET_CONFIGURATION:
                UsbConfig = UsbSetupBuf->wValueL;
                if (UsbConfig) // USB枚举完毕
                    usb_ready = true;
                break;

            case USB_GET_INTERFACE:
                Ep0Buffer[0] = 0x00;
                if (SetupLen >= 1)
                    len = 1;
                break;

            case USB_CLEAR_FEATURE: {
                switch (UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) {
                case USB_REQ_RECIP_ENDP: {
                    switch (UsbSetupBuf->wIndexL) {
                    case 0x83:
                        EP_IN_NAK_TOG(3);
                        break;
                    case 0x82:
                        EP_IN_NAK_TOG(2);
                        break;
                    case 0x81:
                        EP_IN_NAK_TOG(1);
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
                        len = 0xFF; // 不支持的端点
                        break;
                    }
                    break;
                }
                case USB_REQ_RECIP_DEVICE:
                    break;
                default: //unsupport
                    len = 0xff;
                    break;
                }
                break;
            }
            case USB_SET_FEATURE: /* Set Feature */
            {
                switch (UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) {
                // 接口
                case USB_REQ_RECIP_ENDP: {
                    // 接口的Value始终为0
                    if (!UsbSetupBuf->wValueH && !UsbSetupBuf->wValueL) {
                        // Zero, Interface endpoint
                        switch (((uint16_t)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL) {
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
                            len = 0xFF; //操作失败
                            break;
                        }
                    } else {
                        len = 0xFF; //操作失败
                    }
                    break;
                }
                // 设备
                case USB_REQ_RECIP_DEVICE: {
                    if ((((uint16_t)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01) {
                        /*
                        if( USB_SUPPORT_REM_WAKE & 0x20 )
                        {
                            // 设置唤醒使能标志
                        }
                        else
                        {
                            len = 0xFF;                                        // 操作失败
                        }
                        */
                    } else {
                        len = 0xFF; /* 操作失败 */
                    }
                    break;
                }
                default:
                    len = 0xff;
                    break;
                }
                break;
            }
            case USB_GET_STATUS:
                Ep0Buffer[0] = 0x00;
                Ep0Buffer[1] = 0x00;
                len = SetupLen > 2 ? 2 : SetupLen;
                break;
            default:
                len = 0xff; //操作失败
                break;
            }
        } else if (type == USB_REQ_TYP_CLASS) //HID类请求
        {
            len = ClassRequestHandler(UsbSetupBuf);
            if (len != 0xFF)
                len = 0; // ignore hid request
        }
    } else {
        len = 0xff; //包长度错误
    }

    if (len == 0xff) {
        SetupReq = 0xFF;
        // 内部错误，返回STALL
        EP_SET(0, bUEP_R_TOG | bUEP_T_TOG, UEP_R_RES_STALL | UEP_T_RES_STALL);
    } else if (len <= DEFAULT_ENDP0_SIZE) //上传数据或者状态阶段返回0长度包
    {
        UEP0_T_LEN = len;
        EP0_DATA1_ACK(); // 使用DATA1响应数据，或响应SETUP包的status
    } else {
        UEP0_T_LEN = 0; //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
        EP0_DATA1_ACK(); // 响应SETUP包的STATUS
    }
}

void EP1_IN()
{
    EP_IN_FINISH(1);
    usb_busy = false;
}

void EP2_IN()
{
    EP_IN_FINISH(2);
    usb_busy = false;
}

void EP3_IN()
{
    EP_IN_FINISH(3);
    usb_busy = false;
}

static uint8_t ClassRequestHandler(PUSB_SETUP_REQ packet)
{
    uint8_t request = packet->bRequest;
    uint8_t interface = packet->wIndexL;
    uint8_t recipient = UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK;

    switch (request) {
    case 0x01: //GetReport
        if (interface == 0 && recipient == USB_REQ_RECIP_INTERF) {
            memcpy(Ep0Buffer, &Ep1Buffer[64], 8);
            return 8;
        }
        break;
    case 0x02: //GetIdle
        if (interface == 0 && recipient == USB_REQ_RECIP_INTERF) {
            Ep0Buffer[0] = keyboard_idle;
            return 1;
        }
        break;
    case 0x03: //GetProtocol
        if (interface == 0 && recipient == USB_REQ_RECIP_INTERF) {
            Ep0Buffer[0] = keyboard_protocol;
            return 1;
        }
        break;
    case 0x09: //SetReport
        if (interface == 0 && recipient == USB_REQ_RECIP_INTERF) {
            Ep1Buffer[0] = Ep0Buffer[0];
            EP1_OUT();
        }
        break;
    case 0x0A: //SetIdle
        if (interface == 0 && recipient == USB_REQ_RECIP_INTERF) {
            keyboard_idle = UsbSetupBuf->wValueH;
        }
        break;
    case 0x0B: //SetProtocol
        if (interface == 0 && recipient == USB_REQ_RECIP_INTERF) {
            keyboard_protocol = UsbSetupBuf->wValueL;
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
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN); //端点0单64字节收发缓冲区, 端点4单64字节收发缓冲区
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

    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_PD_DIS; // 禁止DP/DM下拉电阻
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN; // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN; // 允许USB端口
    USB_INT_FG = 0xFF; // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}
