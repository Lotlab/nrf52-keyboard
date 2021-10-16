#pragma once
#include "action.h"

enum fn_functions {
    KEYBOARD_CONTROL,
    SWITCH_DEVICE,
    RGBLIGHT_CONTROL = 4, // RGB灯光相关
    RGBMATRIX_CONTROL, // RGB阵列灯光
};

enum fn_keyboard_control {
    CONTROL_SLEEP, // 睡眠键盘
    CONTROL_NKRO, // 切换NKRO状态
    CONTROL_BATTERY_PERCENTAGE, // 输出电池剩余电量
    CONTROL_BOOTLOADER = 0x0F // 进入bootloader
};

enum fn_switch_device {
    SWITCH_DEVICE_USB, // 切换USB状态
    SWITCH_DEVICE_BLE_0 = 0x08,
    SWITCH_DEVICE_BLE_1 = 0x09,
    SWITCH_DEVICE_BLE_2 = 0x0a,
    SWITCH_DEVICE_BLE_REBOND = 0x07,
};
