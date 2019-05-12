#ifndef __MAIN_H__
#define __MAIN_H__

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
    USER_BLE_PASSKEY, // 需要Passkey
    USER_EVT_SLEEP_AUTO, // 自动准备进入睡眠状态
    USER_EVT_SLEEP_MANUAL, // 手动准备进入睡眠状态
    USER_EVT_INITED, // 初始化完毕
};

void user_event_handler(enum user_ble_event arg);
void sleep(enum SLEEP_REASON reason);
extern bool erase_bonds;

#endif // __MAIN_H__
