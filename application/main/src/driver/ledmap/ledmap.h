#pragma once

#include "events.h"

enum LED_Trigger {
    LED_TRIG_STATIC,
    LED_TRIG_BLINK,
    // LED_TRIG_BREATH, // 呼吸灯
    // LED_TRIG_PWM, // PWM 亮度
};

enum LED_Trigger_Static {
    LED_TRIG_OFF, // 关闭 LED
    LED_TRIG_ON, // 启用 LED
    LED_TRIG_REVERSE, // 反转 LED 状态
};

struct led_define {
    bool dir : 1; // 方向。0是拉低亮，1是拉高亮
    uint8_t pin : 7;
};

struct events_led {
    enum user_event event;
    uint8_t param;
    uint8_t led_mask;
    uint8_t action;
};


#define TRIG_LED(g, c) (((g & 0x03) << 6) + ((c) & 0x3F))

/**
 * @brief 开启 LED
 * 
 */
#define TRIG_LED_ON TRIG_LED(LED_TRIG_STATIC, LED_TRIG_ON)
/**
 * @brief 关闭 LED
 * 
 */
#define TRIG_LED_OFF TRIG_LED(LED_TRIG_STATIC, LED_TRIG_OFF)
/**
 * @brief 反转 LED
 * 
 */
#define TRIG_LED_REVERSE TRIG_LED(LED_TRIG_STATIC, LED_TRIG_REVERSE)

/**
 * @brief LED 以指定长度闪动指定次数
 * 
 */
#define TRIG_LED_BLINK(times, duration) TRIG_LED(LED_TRIG_BLINK, ((duration & 0x07) << 3) + (times & 0x07))
/**
 * @brief LED 开启一定时间后关闭
 * 
 */
#define TRIG_LED_ONESHOT(duration) TRIG_LED_BLINK(1, duration)
/**
 * @brief LED 一直闪动
 * 
 */
#define TRIG_LED_BLINK_FOREVER(duration) TRIG_LED_BLINK(0, duration)
