#pragma once

#include <stdbool.h>

/**
 * @brief 睡眠原因
 * 
 */
enum SLEEP_REASON {
    // 无连接自动休眠
    SLEEP_NO_CONNECTION,
    // 按键超时自动休眠
    SLEEP_TIMEOUT,
    // 手动关机休眠
    SLEEP_MANUALLY,
    // 非开机唤醒后休眠
    SLEEP_NOT_PWRON
};

/**
 * @brief 蓝牙用户事件
 * 
 */
enum user_ble_event {
    USER_BLE_IDLE, // 蓝牙进入空闲状态
    USER_BLE_DISCONNECT, // 蓝牙被断开
    USER_BLE_GATTS_TX_COMPLETE, // GATTs发送完毕
    USER_BLE_CONNECTED, // 蓝牙已连接
    USER_BLE_PASSKEY_REQUIRE, // 需要 Passkey
    USER_BLE_PASSKEY_SEND, // Passkey 输入完毕
    USER_EVT_SLEEP_AUTO, // 自动准备进入睡眠状态
    USER_EVT_SLEEP_MANUAL, // 手动准备进入睡眠状态
    USER_EVT_INITED, // 初始化完毕
    USER_EVT_POST_INIT, // 自定义的初始化事件
    USER_BAT_CHARGING, // 正在充电
    USER_BAT_FULL, // 充电完成
    USER_USB_CONNECTED, // 连接至主机
    USER_USB_DISCONNECT, // 没有连接
    USER_USB_CHARGE, // 仅充电
    USER_USB_PROTOCOL_BOOT, // USB protocol 设置为0
    USER_USB_PROTOCOL_REPORT, // USB protocol 设置为1
    USER_LED_ON, // LED 状态切换至亮起
    USER_LED_OFF, // LED 状态切换至关闭
    USER_LED_DEINIT // 禁用 LED，准备睡眠
};

void ble_user_event(enum user_ble_event arg);
void sleep(enum SLEEP_REASON reason);
extern bool erase_bonds;
