#pragma once
#include "action.h"

enum fn_functions {
    KEYBOARD_CONTROL,
    SWITCH_DEVICE,
};

enum fn_keyboard_control {
    CONTROL_SLEEP, // 睡眠键盘
    CONTROL_NKRO // 切换NKRO状态
};

enum fn_switch_device {
    SWITCH_DEVICE_USB, // 切换USB状态
};
