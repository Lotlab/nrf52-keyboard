#include "3led_status.h"

/** 
 * 使用3LED的显示作为默认的用户LED设置
 **/
void custom_event_handler(enum user_ble_event arg)
{
    status_led_evt_handler(arg);
}