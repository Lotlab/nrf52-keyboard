#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "events.h"

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
    // 手动关机休眠，并禁用按键唤醒
    SLEEP_MANUALLY_NO_WAKEUP,
    // 非开机唤醒后休眠
    SLEEP_NOT_PWRON
};
/**
 * 触发一个事件
 * 
 * @param event: 事件类型
 * @param arg: 事件参数
 */
void trig_event(enum user_event event, void* arg);
/**
 * 触发一个事件
 * 请勿使用此方法传递内部参数以外的参数
 * 
 * @param event: 事件类型
 * @param arg: 事件参数
 */
void trig_event_param(enum user_event event, uint8_t arg);
/**
 * 使键盘进入睡眠状态
 * 
 * @param reason: 睡眠原因
 */
void sleep(enum SLEEP_REASON reason);
/**
 * 触发睡眠事件
 * 
 * @param mode: 睡眠模式
 */
void notify_sleep(enum sleep_evt_type mode);
extern bool erase_bonds;
