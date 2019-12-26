#pragma once

#include <stdbool.h>

enum user_event {
    USER_EVT_INTERNAL, // 内部专用事件
    USER_EVT_BLE_STATE_CHANGE, // 蓝牙状态改变
    USER_EVT_BLE_PASSKEY_STATE, // 蓝牙Passkey状态改变
    USER_EVT_SLEEP, // 睡眠
    USER_EVT_STAGE, // 当前进度
    USER_EVT_CHARGE, // 充电状态
    USER_EVT_USB, // USB状态
    USER_EVT_PROTOCOL, // HID Protocol
    USER_EVT_POWERSAVE, // 省电状态（特指LED）
};

enum internal_event {
    INTERNAL_EVT_GATTS_TX_COMPLETE, // 蓝牙GATTS发送成功（启用加密情况下表示连接成功）
};

/**
 * 蓝牙状态
 */
enum ble_state {
    BLE_STATE_IDLE, // 蓝牙处于空闲状态（FAST_ADV->SLOW_ADV->IDLE)
    BLE_STATE_DISCONNECT, // 蓝牙连接断开
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
    SLEEP_EVT_MANUAL, // 手工休眠
    SLEEP_EVT_AUTO, // 自动睡眠
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
