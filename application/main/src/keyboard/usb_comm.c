/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

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

#include "usb_comm.h"
#include "../config/keyboard_config.h"
#include "app_error.h"
#include "app_uart.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrfx_uart.h"

#include <stdlib.h>
#include <string.h>

#include "../ble/ble_hid_service.h"
#include "../main.h"
#include "app_timer.h"
#include "host.h"
#include "keymap_storage.h"
#include "queue.h"

#ifdef HAS_USB

#define MAX_ITEM_SIZE 34 // id + data(32) + checksum
#define QUEUE_SIZE 10

uint8_t keyboard_led_val_usb;

static uint8_t recv_buf[62];
static uint8_t recv_index;

static bool has_host;
static bool is_full, is_connected, is_checked, is_disable;
static bool usb_protocol;

struct queue_item {
    uint8_t data[MAX_ITEM_SIZE];
    uint8_t len;
};

QUEUE(struct queue_item, queue, QUEUE_SIZE);

/**
 * @brief 计算校验值
 * 
 * @param data 
 * @param len 
 * @return uint8_t 
 */
static uint8_t checksum(uint8_t* data, uint8_t len)
{
    uint16_t checksum = 0;
    for (int i = 0; i < len; i++) {
        checksum = (checksum + data[i]) & 0xFF;
    }
    return (uint8_t)checksum;
}

enum uart_ack_state {
    UART_CHECK_FAIL,
    UART_SUCCESS, 
    UART_END
};

/**
 * @brief 回复请求
 * 
 * @param success 
 */
static void uart_ack(enum uart_ack_state state)
{
    app_uart_put(0x10 + state);
}

/**
 * @brief 发送事件
 * 
 * @param 事件类型
 */
static void send_event(enum user_ble_event arg)
{
#ifdef NKRO_ENABLE
    switch (arg) {
    case USER_USB_DISCONNECT:
    case USER_USB_CHARGE:
        // 设置为Boot protocol，防止键盘发送nkro包
        keyboard_protocol = 0;
        break;
    case USER_USB_CONNECTED:
    case USER_USB_PROTOCOL_BOOT:
    case USER_USB_PROTOCOL_REPORT:
        // 设置为实际的protocol
        keyboard_protocol = usb_protocol;
        break;
    default:
        break;
    }
#endif
    ble_user_event(arg);
}

/**
 * @brief 设置状态
 * 
 * @param host 是否连接到主机
 * @param charge_full 电量是否充满
 * @param protocol USB 当前协议类型
 * @param force 强制更新状态
 */
static void set_state(bool host, bool charge_full, bool protocol, bool force)
{
    if (host != has_host || force) {
        has_host = host;
        send_event(host ? USER_USB_CONNECTED : USER_USB_CHARGE);
    }
    if (charge_full != is_full || force) {
        is_full = charge_full;
        send_event(is_full ? USER_BAT_FULL : USER_BAT_CHARGING);
    }
    if (usb_protocol != protocol || force) {
        usb_protocol = protocol;
        send_event(protocol ? USER_USB_PROTOCOL_REPORT : USER_USB_PROTOCOL_BOOT);
    }
}

/**
 * @brief 直接向 UART 写入数据
 * 
 * @param data 
 * @param len 
 */
static void uart_send(uint8_t* data, uint8_t len)
{
    while (len--) {
        app_uart_put(*(data++));
    }
}

/**
 * @brief 接收消息
 * 
 */
static void uart_on_recv()
{
    // 通信协议：
    // CMD DAT ... DAT SUM
    uint8_t buff;
    while (app_uart_get(&buff) == NRF_SUCCESS) {
        recv_buf[recv_index] = buff;
        if (!recv_index) {
            if (buff >= 0x80) { // keymap sending
                recv_index++;
            } else if (buff >= 0x40) { // led
                keyboard_led_val_usb = buff & 0x1F; // 5bit
            } else if (buff >= 0x10) { // status
                bool success = buff & 0x01;
                bool charging_status = buff & 0x02;
                bool usb_status = buff & 0x04;
                bool protocol = buff & 0x08;

                // 设置当前状态
                set_state(usb_status, charging_status, protocol, false);

                // 成功接收，出队。
                if (success) {
                    queue_pop();
                }
                // 尝试发送下一个
                if (!queue_empty()) {
                    struct queue_item* next = queue_peek();
                    uart_send(next->data, next->len);
                }
                is_checked = true;
            }
        } else {
            recv_index++;
            if (recv_index >= 62) {
                recv_index = 0;
                uint8_t sum = checksum(recv_buf, 61);
                if (sum == recv_buf[61]) {
                    uint8_t id = recv_buf[0] & 0x7F;
                    if (keymap_set(id, 60, &recv_buf[1])) {
                        uart_ack(UART_END);
                    } else {
                        uart_ack(UART_SUCCESS);
                    }
                } else {
                    uart_ack(UART_CHECK_FAIL);
                }
            }
        }
    }
}

/**
 * @brief 关闭UART
 * 
 */
static void uart_to_idle()
{
    queue_clear();
    app_uart_close();
#ifndef UART_DET
    nrf_gpio_cfg_input(UART_RXD, NRF_GPIO_PIN_PULLDOWN);
#endif
    is_connected = false;
    send_event(USER_USB_DISCONNECT);
}

static void uart_evt_handler(app_uart_evt_t* p_app_uart_event)
{
    switch (p_app_uart_event->evt_type) {
    case APP_UART_DATA:
    case APP_UART_DATA_READY:
        uart_on_recv();
        break;

    case APP_UART_TX_EMPTY:
        break;

    case APP_UART_FIFO_ERROR:
        app_uart_flush();
        break;
    default:
        break;
    }
}

/**
 * @brief 启用UART
 * 
 */
static void uart_init_hardware()
{
    uint32_t err_code;
    const app_uart_comm_params_t config = {
        .baud_rate = UART_BAUDRATE,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .rx_pin_no = UART_RXD,
        .tx_pin_no = UART_TXD,
        .use_parity = false
    };

    APP_UART_FIFO_INIT(&config, 32, 32, uart_evt_handler, APP_IRQ_PRIORITY_LOW, err_code);
    APP_ERROR_CHECK(err_code);

    is_connected = true;
    set_state(false, false, false, true); // 重置为默认状态
}

static void uart_task(void* context)
{
    UNUSED_PARAMETER(context);
    if (is_connected) {
        // 已经连接状态，检查是否断开
        if (!is_checked) {
            uart_to_idle();
        } else {
            is_checked = false;
        }
    } else {
        // 未连接状态，检查是否连接
#ifdef UART_DET
        if (!nrf_gpio_pin_read(UART_DET)) {
#else
        if (nrf_gpio_pin_read(UART_RXD)) {
#endif
            uart_init_hardware();
        }
    }
}

/**
 * @brief 是否已经连接到主机
 * 
 * @return true 
 * @return false 
 */
bool usb_working(void)
{
    return has_host && is_connected && !is_disable;
}

/**
 * @brief 通过USB发送按键数据包
 * 
 * @param index 数据包类型
 * @param len 长度
 * @param pattern 
 */
void usb_send(uint8_t index, uint8_t len, uint8_t* pattern)
{
    if (len > 32)
        return;

    // 入队
    struct queue_item item;
    item.len = len + 2;
    item.data[0] = 0x80 + ((index) << 5) + len;
    memcpy(&item.data[1], pattern, len);
    item.data[len + 1] = checksum(item.data, len + 1);

    queue_push(item);
}

APP_TIMER_DEF(uart_check_timer);
#define UART_CHECK_INTERVAL APP_TIMER_TICKS(500)

/**
 * @brief 初始化USB通信
 * 
 */
void usb_comm_init()
{
    uint32_t err_code;

    err_code = app_timer_create(&uart_check_timer,
        APP_TIMER_MODE_REPEATED,
        uart_task);

    APP_ERROR_CHECK(err_code);
#ifdef UART_DET
    nrf_gpio_cfg_input(UART_DET, NRF_GPIO_PIN_PULLUP);
    if (!nrf_gpio_pin_read(UART_DET)) {
#else
    nrf_gpio_cfg_input(UART_RXD, NRF_GPIO_PIN_PULLDOWN);
    if (nrf_gpio_pin_read(UART_RXD)) {
#endif
    // 初始化时启用UART尝试接收事件，若没有主机则在超时处关闭
        uart_init_hardware();
    }
}

/**
 * @brief 启动USB通信计时器
 * 
 */
void usb_comm_timer_start()
{
    uint32_t err_code = app_timer_start(uart_check_timer, UART_CHECK_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief USB 准备睡眠
 * 
 */
void usb_comm_sleep_prepare()
{
    uart_to_idle();
#ifdef UART_DET
    nrf_gpio_cfg_sense_input(UART_DET, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
#else
    nrf_gpio_cfg_sense_input(UART_RXD, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
#endif
}

/**
 * @brief 切换USB和蓝牙设备
 * 
 */
void usb_comm_switch()
{
    if (is_connected && has_host) {
        is_disable = !is_disable;
        if (is_disable) {
            send_event(USER_USB_CHARGE);
        } else {
            send_event(USER_USB_CONNECTED);
        }
    }
}

#endif
