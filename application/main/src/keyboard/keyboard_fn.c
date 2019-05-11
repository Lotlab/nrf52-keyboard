/**
 * @brief 键盘内置功能
 * 
 * @file keyboard_fn.c
 * @author Jim Jiang
 * @date 2019-05-11
 */
#include "keyboard_fn.h"
#include "../main.h"

void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) {
        switch (id) {
        case POWER_SLEEP:
            sleep_mode_enter();
            break;
        case SWITCH_DEVICE:
            break;
        default:
            break;
        }
    }
}
