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

#include "CH554_SDCC.h"
#include "app_timer.h"
#include "compiler.h"
#include "endpoints.h"
#include "interrupt.h"
#include "system.h"
#include "uart.h"
#include "usb_comm.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool usb_sleep = false;

/**
 * @brief CH554 软复位
 *
 */
static void CH554SoftReset()
{
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bSW_RESET;
}

/** \brief CH554设备模式唤醒主机，发送K信号
 *
 */
static void CH554USBDevWakeup()
{
    UDEV_CTRL |= bUD_LOW_SPEED;
    DelayMs(2);
    UDEV_CTRL &= ~bUD_LOW_SPEED;
}

/** \brief CH559USB中断处理函数
 */
static INTERRUPT_USING(DeviceInterrupt, INT_NO_USB, 1) //USB中断服务程序,使用寄存器组1
{
    UsbIsr();
}

/**
 * @brief 按键发送事件
 *
 */
static void UsbOnKeySend()
{
    if (usb_sleep) {
        usb_sleep = false;
        CH554USBDevWakeup();
    }
}

/**
 * @brief 上传键盘通常按键数据包
 *
 * @param packet 数据包
 * @param len 长度。必须是8
 */
void KeyboardGenericUpload(uint8_t* packet, uint8_t len)
{
    if (len != 8)
        return;
    UsbOnKeySend();

    usb_busy = true;
    memcpy(&Ep1Buffer[64], packet, len);
    UEP1_T_LEN = len;
    UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

/**
 * @brief 上传键盘特殊按键数据包
 *
 * @param packet 数据包，第一个byte为ID
 * @param len 长度
 */
void KeyboardExtraUpload(uint8_t* packet, uint8_t len)
{
    UsbOnKeySend();

    usb_busy = true;
    memcpy(Ep2Buffer, packet, len);
    UEP2_T_LEN = len;
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

/**
 * @brief 响应KeyMap下载数据包
 *
 * @param packet 数据包
 * @param len 长度
 */
void ResponseConfigurePacket(uint8_t* packet, uint8_t len)
{
    if (len > 64)
        return;

    usb_busy = true;
    Ep3Buffer[64] = 0x3f; // packet id
    memcpy(&Ep3Buffer[65], packet, len);
    memset(&Ep3Buffer[65 + len], 0, 64 - len - 2);
    UEP3_T_LEN = 64;
    UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

/**
 * @brief 串口中断
 *
 */
static INTERRUPT(UARTInterrupt, INT_NO_UART1)
{
    if (U1RI) {
        uart_recv();
        // U1RI = 0;
    }
}

/**
 * @brief 端点3下传数据。下传的是Keymap数据包
 *
 */
void EP3_OUT()
{
    uint8_t len = Ep3Buffer[2] + 2;
    uart_send_keymap(&Ep3Buffer[1], len);
}

/**
 * @brief 端点1下传数据，里面的是键盘LED状态
 *
 */
void EP1_OUT()
{
    uint8_t datalen = USB_RX_LEN;
    uart_send_led(Ep1Buffer[0]);
}

/**
 * @brief 启用看门狗
 *
 */
static void EnableWatchDog()
{
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xaa; //进入安全模式
    GLOBAL_CFG |= bWDOG_EN; //启动看门狗复位
    SAFE_MOD = 0x00; //退出安全模式
    WDOG_COUNT = 0; //看门狗赋初值
}

/**
 * @brief 喂狗
 *
 */
static void FeedWatchDog()
{
    WDOG_COUNT = 0x00;
}

static INTERRUPT(TimerInterrupt, INT_NO_TKEY)
{
    TKEY_CTRL = 0;
    timer_tick();
}

/**
 * @brief USB睡眠事件
 *
 */
void UsbSuspendEvt(bool suspend)
{
    usb_sleep = suspend;
}

/**
 * @brief 初始化时钟
 *
 */
static void timer_init()
{
    timer_create(&FeedWatchDog, true, 500);
    timer_create(&uart_check, true, 1);
    IE_TKEY = 1;
}

static void main()
{
    CfgSysClock();
    DelayMs(5); //修改主频等待内部晶振稳定,必加
    // InitUART();  //串口0初始化
    uart_init();
    DelayMs(5);
    // printf_tiny("Build %s %s\n", __TIME__, __DATE__);
    timer_init();
    USBDeviceInit(); //USB设备模式初始化
    EnableWatchDog();
    EA = 1; //允许单片机中断
    UEP1_T_LEN = 0; //预使用发送长度一定要清空
    UEP2_T_LEN = 0; //预使用发送长度一定要清空
    UEP3_T_LEN = 0;

    // 拉低P1.5，通知主控使用UART接收
    P1_MOD_OC -= (P1_MOD_OC & bMOSI);
    MOSI = false;

    while (1) {
        timer_task_exec();
    }
}
