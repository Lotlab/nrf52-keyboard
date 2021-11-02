#pragma once
#include <stdint.h>

typedef struct {
    uint16_t voltage; // 电压 (mv)
    uint8_t percentage; // 百分比
}battery_info_t ;

/**
 * 电量信息
 */
extern battery_info_t battery_info;

/**@brief 输出电池剩余电量.
 *
 */
void print_battery_percentage();
