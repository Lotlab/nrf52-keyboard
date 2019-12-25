#pragma once

#include <stdbool.h>

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

enum user_event {
    USER_EVT_BLE_STATE_CHANGE, // 蓝牙状态改变
    USER_EVT_BLE_PASSKEY_STATE, // 蓝牙Passkey状态改变
    USER_EVT_SLEEP, // 睡眠
    USER_EVT_STAGE, // 当前进度
    USER_EVT_CHARGE, // 充电状态
    USER_EVT_USB, // USB状态
    USER_EVT_PROTOCOL, // HID Protocol
    USER_EVT_POWERSAVE, // 省电状态（特指LED）
};

/**
 * 蓝牙状态
 */
enum ble_state {
    BLE_STATE_IDLE, // 蓝牙处于空闲状态（FAST_ADV->SLOW_ADV->IDLE)
    BLE_STATE_DISCONNECT, // 蓝牙连接断开
    BLE_STATE_GATTS_TX_COMPLETE, // 蓝牙GATTS发送成功（启用加密情况下表示连接成功）
    BLE_STATE_CONNECTED // 蓝牙已连接
};

/**
 * 配对码状态
 */
enum passkey_state {
    PASSKEY_STATE_REQUIRE, // 请求配对码
    PASSKEY_STATE_SEND, // 配对码已发送
};

/**
 * 睡眠状态
 */
enum sleep_evt_type {
    SLEEP_EVT_AUTO, // 自动睡眠
    SLEEP_EVT_MANUAL, // 手工休眠
};

/**
 * 键盘状态
 */
enum keyboard_state {
    KBD_STATE_PRE_INIT, // 各项服务初始化之前的事件
    KBD_STATE_POST_INIT, // 各项服务初始化之后的事件
    KBD_STATE_INITED, // 初始化完毕的事件
    KBD_STATE_SLEEP, // 准备休眠的事件
};

/**
 * 充电状态
 */
enum charging_state {
    BATT_NOT_CHARGING, // 未充电
    BATT_CHARGING, // 充电中
    BATT_CHARGED // 充电完毕
};

/**
 * USB 状态NO_HOST
 */
enum usb_state {
    USB_NOT_CONNECT, // USB未连接
    USB_NO_HOST, // USB已连接但没有连接到主机
    USB_WORKING, // USB当前正在工作
    USB_NOT_WORKING, // USB当前处于非活动状态（通过蓝牙等……）
};

/**
 * HID 的协议
 */
enum hid_protocol_type {
    HID_BOOT_PROTOCOL, // bool protocol
    HID_REPORT_PROTOCOL, // report protocol
};

/**
 * 省电事件状态
 */
enum power_save_mode {
    PWR_SAVE_ON, // 启用省电模式
    PWR_SAVE_OFF, // 禁用省电模式
    PWR_SAVE_ENTER, // 进入省电模式
    PWR_SAVE_EXIT, // 退出省电模式
};
