#include "keyboard_fn.h"
#include "rgb.h"

static void rgblight_fn_handler(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (id == RGBLIGHT_CONTROL && record->event.pressed) {
        switch (opt) {
        case RGBLIGHT_TOGGLE:
            rgblight_toggle();
            break;
        case RGBLIGHT_MODE_INCREASE:
            rgblight_step();
            break;
        case RGBLIGHT_MODE_DECREASE:
            rgblight_step_reverse();
            break;
        case RGBLIGHT_HUE_INCREASE:
            rgblight_increase_hue();
            break;
        case RGBLIGHT_HUE_DECREASE:
            rgblight_decrease_hue();
            break;
        case RGBLIGHT_SAT_INCREASE:
            rgblight_increase_sat();
            break;
        case RGBLIGHT_SAT_DECREASE:
            rgblight_decrease_sat();
            break;
        case RGBLIGHT_VAL_INCREASE:
            rgblight_increase_val();
            break;
        case RGBLIGHT_VAL_DECREASE:
            rgblight_decrease_val();
            break;
        case RGBLIGHT_SPEED_INCREASE:
            rgblight_increase_speed();
            break;
        case RGBLIGHT_SPEED_DECREASE:
            rgblight_decrease_speed();
            break;
        default:
            break;
        }
    }
}

FN_HANDLER(rgblight_fn_handler);