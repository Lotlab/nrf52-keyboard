#include "../main.h"
#include "ble_keyboard.h"
#include "keyboard_led.h"
#include "sleep_reason.h"
#include "nrf_delay.h"

enum keyboard_status {
    kbd_ble,
    kbd_charge,
    kbd_usb
};

static enum keyboard_status status = 0;
static bool charging_full = false;
static bool ble_connected = false;

static void led_status_change() {
    switch (status)
    {
    case kbd_ble:
        keyboard_led_rgb_set(ble_connected ? 0x66ccff : 0xFFFFFF);
        break;
    case kbd_charge:
        keyboard_led_rgb_set(charging_full ? 0x00FF00: 0xFF8000);
        break;
    case kbd_usb:
        keyboard_led_rgb_set(0x0099FF);
    default:
        break;
    }
}

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
        led_status_change();
        break;
    case USER_USB_DISCONNECT:
        status = kbd_ble;
        led_status_change();
        break;
    case USER_USB_CHARGE:
        status = kbd_charge;
        led_status_change();
        break;
    case USER_USB_CONNECTED:
        status = kbd_usb;
        led_status_change();
        break;
    case USER_BAT_CHARGING:
        charging_full = false;
        led_status_change();
        break;
    case USER_BAT_FULL:
        charging_full = true;
        led_status_change();
        break;
    case USER_BLE_DISCONNECT:
        ble_connected = false;
        led_status_change();
        break;
    case USER_BLE_CONNECTED:
        ble_connected = true;
        keyboard_led_rgb_set(0x66ccff);
        break;
    case USER_BLE_PASSKEY_REQUIRE:
        keyboard_led_rgb_set(0xFFFF00);
        break;
    case USER_BLE_PASSKEY_SEND:
        keyboard_led_rgb_set(0xFF0080);
        break;
    case USER_EVT_SLEEP_AUTO:
    case USER_EVT_SLEEP_MANUAL:
        keyboard_led_rgb_direct(0b00000101);
        nrf_delay_ms(200);
    default:
        break;
    }
}