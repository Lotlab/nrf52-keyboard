/**
 * @brief 键盘内置功能
 * 
 * @file keyboard_fn.c
 * @author Jim Jiang
 * @date 2019-05-11
 */
#include "keyboard_fn.h"
#include "../main.h"
#include "usb_comm.h"

void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) {
        switch (id) {
        case POWER_SLEEP:
            sleep(SLEEP_MANUALLY);
            break;
        case SWITCH_USB:
#ifdef HAS_USB
            usb_comm_switch();
#endif
            break;
        default:
            break;
        }
    }
}
