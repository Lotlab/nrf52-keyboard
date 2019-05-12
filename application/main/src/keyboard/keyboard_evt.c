#include "../main.h"
#include "sleep_reason.h"

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
    default:
        break;
    }
}