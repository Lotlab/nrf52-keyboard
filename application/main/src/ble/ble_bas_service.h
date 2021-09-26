#pragma once
#include <stdint.h>

#define ADC_BUFFER_SIZE 6

void battery_service_init(void);

struct BatteryInfo {
    uint16_t voltage; // 电压 (mv)
    uint8_t percentage; // 百分比
};

/**
 * 电量信息
 */
extern struct BatteryInfo battery_info;

/**@brief 输出电池剩余电量.
 *
 */
void print_battery_percentage();
