#include "../main.h"
#include "sleep_reason.h"
#include "keyboard_led.h"

void user_event_handler(enum user_ble_event arg)
{
    switch (arg)
    {
    case USER_EVT_SLEEP_AUTO:
        sleep_reason_set(true);
        break;
    case USER_EVT_SLEEP_MANUAL:
        sleep_reason_set(false);
        break;
    case USER_EVT_INITED:
        keyboard_led_rgb_set(0x66ccff);
        break;
    default:
        break;
    }
}