/**
 * @brief 键盘内置功能
 * 
 * @file keyboard_fn.c
 * @author Jim Jiang
 * @date 2018-05-13
 */
#include "keyboard_fn.h"
#include <stdint.h>

void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) {
        switch (id) {
        case POWER_SLEEP:
            break;
        case SWITCH_DEVICE:
            break;
        default:
            break;
        }
    }
}
