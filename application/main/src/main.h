#pragma once

#include <stdbool.h>
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
    // 非开机唤醒后休眠
    SLEEP_NOT_PWRON
};

void ble_user_event(enum user_ble_event arg);
void trig_event(enum user_event event, void* arg);
void trig_event_param(enum user_event event, uint8_t arg);
void sleep(enum SLEEP_REASON reason);
extern bool erase_bonds;
