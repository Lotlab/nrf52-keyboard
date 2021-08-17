#pragma once

enum fn_user_functions {
	TRICKY_KEY = 0x10, //0x0F以前为系统保留，自定义从0x10开始；
	RGB_LIGHT_CONTROL = 0x11,
	RGBLIGHT_CONTROL = 0x12,
};

enum fn_user_fn_key {
    AF_TRICKY_ESC,
    AF_TRICKY_SLSH,
    AF_TRICKY_ENTER,
    AF_TRICKY_L,
    AF_TRICKY_UP,
    AF_TRICKY_BSPC,
};

enum fn_rgb_light_control {
    RGB_LIGHT_STEP,
    RGB_LIGHT_TOGGLE,
    RGB_LIGHT_IHUE,
    RGB_LIGHT_DHUE,
    RGB_LIGHT_ISAT,
    RGB_LIGHT_DSAT,
    RGB_LIGHT_IVAL,
    RGB_LIGHT_DVAL,
    RGB_LIGHT_TML,
};

enum fn_rgblight_control {
    RGBLIGHT_STEP,
    RGBLIGHT_TOGGLE,
    RGBLIGHT_IHUE,
    RGBLIGHT_DHUE,
    RGBLIGHT_ISAT,
    RGBLIGHT_DSAT,
    RGBLIGHT_IVAL,
    RGBLIGHT_DVAL,
};