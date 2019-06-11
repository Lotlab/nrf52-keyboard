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

#include "interrupt.h"
#include "CH554_SDCC.h"
#include "endpoints.h"
// #include <stdio.h>

const __code void (*EndpointPacketOutHandler[5])(void) = {
    USB_OUT_EP0,
    USB_OUT_EP1,
    USB_OUT_EP2,
    USB_OUT_EP3,
    USB_OUT_EP4
};

const __code void (*EndpointPacketInHandler[5])(void) = {
    USB_IN_EP0,
    USB_IN_EP1,
    USB_IN_EP2,
    USB_IN_EP3,
    USB_IN_EP4
};

const __code void (*EndpointPacketSetupHandler[5])(void) = {
    USB_SETUP_EP0,
    USB_SETUP_EP1,
    USB_SETUP_EP2,
    USB_SETUP_EP3,
    USB_SETUP_EP4
};

const __code void (*EndpointPacketSofHandler[5])(void) = {
    USB_SOF_EP0,
    USB_SOF_EP1,
    USB_SOF_EP2,
    USB_SOF_EP3,
    USB_SOF_EP4
};

/** \brief USB 传输完成中断处理
 */
static void UsbTransfurEventHandler()
{

    uint8_t ep = USB_INT_ST & MASK_UIS_ENDP;

    switch (USB_INT_ST & MASK_UIS_TOKEN) {
    case UIS_TOKEN_IN:
        (*EndpointPacketInHandler[ep])();
        break;
    case UIS_TOKEN_OUT:
        (*EndpointPacketOutHandler[ep])();
        break;
    case UIS_TOKEN_SETUP:
        (*EndpointPacketSetupHandler[ep])();
        break;
    case UIS_TOKEN_SOF:
        (*EndpointPacketSofHandler[ep])();
        break;
    default:
        break;
    }
    UIF_TRANSFER = 0; //写0清空中断
}

/** \brief USB 总线复位事件中断处理
 *
 */
static void UsbBusResetEventHandler()
{
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP3_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
    USB_DEV_AD = 0x00;
    UIF_SUSPEND = 0;
    UIF_TRANSFER = 0;
    UIF_BUS_RST = 0; //清中断标志
}

/** \brief USB 总线挂起或唤醒事件处理
 *
 * \return void
 *
 */
static void UsbBusSuspendEventHandler()
{
    UIF_SUSPEND = 0;
    UsbSuspendEvt(USB_MIS_ST & bUMS_SUSPEND); //挂起
}

/** \brief USB 中断服务函数
 *
 * \return void
 *
 */
void UsbIsr()
{
    if (UIF_TRANSFER) {
        //USB传输完成标志
        UsbTransfurEventHandler();
    } else if (UIF_BUS_RST) {
        //设备模式USB总线复位中断
        UsbBusResetEventHandler();
    } else if (UIF_SUSPEND) {
        //USB总线挂起/唤醒完成
        UsbBusSuspendEventHandler();
    } else {
        //意外的中断，产生中断必然会设置此标志位
        USB_INT_FG = 0xFF; //清中断标志
    }
}
