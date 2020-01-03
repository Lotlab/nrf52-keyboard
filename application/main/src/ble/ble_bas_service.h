#pragma once
#include <stdint.h>

void battery_service_init(void);

struct BatteryInfo {
    uint16_t voltage; // 电压 (mv)
    uint8_t percentage; // 百分比
};

/**
 * 电量信息
 */
extern struct BatteryInfo battery_info;