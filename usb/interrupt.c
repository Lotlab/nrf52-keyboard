#include "interrupt.h"
#include "endpoints.h"
#include "CH554_SDCC.h"
// #include <stdio.h>

const __code void (*EndpointPacketOutHandler[5])(void) = {
    USB_OUT_EP0,
    USB_OUT_EP1,
    USB_OUT_EP2,
    USB_OUT_EP3,
    USB_OUT_EP4};

const __code void (*EndpointPacketInHandler[5])(void) = {
    USB_IN_EP0,
    USB_IN_EP1,
    USB_IN_EP2,
    USB_IN_EP3,
    USB_IN_EP4};

const __code void (*EndpointPacketSetupHandler[5])(void) = {
    USB_SETUP_EP0,
    USB_SETUP_EP1,
    USB_SETUP_EP2,
    USB_SETUP_EP3,
    USB_SETUP_EP4};

const __code void (*EndpointPacketSofHandler[5])(void) = {
    USB_SOF_EP0,
    USB_SOF_EP1,
    USB_SOF_EP2,
    USB_SOF_EP3,
    USB_SOF_EP4};

/** \brief USB 传输完成中断处理
 */
static void UsbTransfurEventHandler()
{
    if (UIF_TRANSFER) //USB传输完成标志
    {
        uint8_t ep = USB_INT_ST & MASK_UIS_ENDP;

        switch (USB_INT_ST & MASK_UIS_TOKEN)
        {
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
}

/** \brief USB 总线复位事件中断处理
 *
 */
static void UsbBusResetEventHandler()
{
    if (UIF_BUS_RST) //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0; //清中断标志
    }
}

/** \brief USB 总线挂起或唤醒事件处理
 *
 * \return void
 *
 */
static void UsbBusSuspendEventHandler()
{
    if (UIF_SUSPEND) //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if (USB_MIS_ST & bUMS_SUSPEND) //挂起
        {
            UsbSuspendEvt();
        }
    }
    else //意外的中断，产生中断必然会设置此标志位
    {
        USB_INT_FG = 0xFF; //清中断标志
    }
}

/** \brief USB 中断服务函数
 *
 * \return void
 *
 */
void UsbIsr()
{
    UsbTransfurEventHandler();
    UsbBusResetEventHandler();
    UsbBusSuspendEventHandler();
}
