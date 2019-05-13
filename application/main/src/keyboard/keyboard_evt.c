#include "../main.h"
#include "ble_keyboard.h"
#include "keyboard_led.h"
#include "sleep_reason.h"

void user_event_handler(enum user_ble_event arg)
{
    // 处理各项事件，启用对应的处理程序
    switch (arg) {
    case USER_EVT_SLEEP_AUTO:
        // 自动休眠时，设置休眠原因便于下次免按键启动
        sleep_reason_set(true);
        break;
    case USER_EVT_SLEEP_MANUAL:
        // 手动休眠时，设置下次必须按键启动
        sleep_reason_set(false);
        break;
    case USER_USB_CHARGE:
        // 接入USB后，切换至非省电模式防止自动休眠
        keyboard_led_powersave(false);
        ble_keyboard_powersave(false);
        break;
    case USER_USB_DISCONNECT:
        // 断开USB后，切换至省电模式节省电量
        keyboard_led_powersave(true);
        ble_keyboard_powersave(true);
        break;
    default:
        break;
    }

    // 这里可以放置用户自定义的处理程序，例如设置灯光等。
    switch (arg) {
    case USER_EVT_INITED:
        keyboard_led_rgb_set(0x66ccff); // 天依蓝
        break;
    case USER_USB_DISCONNECT:
        keyboard_led_rgb_set(0x66ccff); // 天依蓝
        break;
    case USER_USB_CHARGE:
        keyboard_led_rgb_set(0x0099ff); // 天蓝色
        break;
    case USER_USB_CONNECTED:
        keyboard_led_rgb_set(0x009900); // 翠绿色
        break;
    case USER_BLE_PASSKEY_REQUIRE:
        keyboard_led_rgb_set(0x993030); // 赭红色（理论）
        break;
    case USER_BLE_PASSKEY_SEND:
        keyboard_led_rgb_set(0x99C030); // 青绿色
        break;
    default:
        break;
    }
}