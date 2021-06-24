/*
Copyright (C) 2019 Geno Kolar <geno@live.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "user_fn.h"
#include "action_util.h"
#include "keyboard_fn.h"
#include "main.h"
#ifdef RGB_LIGHT_ENABLE
#include "rgb_light.h"
#endif

#define MODS_SHIFT_MASK (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT))
#define MODS_RSHIFT_MASK (MOD_BIT(KC_RSHIFT))
#define MODS_LSHIFT_MASK (MOD_BIT(KC_LSHIFT))

void custom_fn_handler(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    static uint8_t tricky_registered;
    switch (id) {
    case TRICKY_KEY:
        switch (opt) {
        case AF_TRICKY_ESC:
            if (record->event.pressed) {
                if (get_mods() & MODS_SHIFT_MASK) {
                    tricky_registered = KC_GRV;
                } else {
                    tricky_registered = KC_ESC;
                }
                register_code(tricky_registered);
            } else {
                unregister_code(tricky_registered);
            }
            break;
        case AF_TRICKY_SLSH:
            if (record->event.pressed) {
                if (get_mods() & MODS_RSHIFT_MASK) {
                    tricky_registered = KC_SLSH;
                } else {
                    tricky_registered = KC_DOT;
                }
                register_code(tricky_registered);
            } else {
                unregister_code(tricky_registered);
            }
            break;
        case AF_TRICKY_ENTER:
            if (record->event.pressed) {
                if (get_mods() & MODS_RSHIFT_MASK) {
                    tricky_registered = KC_QUOT;
                } else {
                    tricky_registered = KC_ENTER;
                }
                register_code(tricky_registered);
            } else {
                unregister_code(tricky_registered);
            }
            break;
        case AF_TRICKY_L:
            if (record->event.pressed) {
                if ((get_mods() & MODS_RSHIFT_MASK) && !(get_mods() & MODS_LSHIFT_MASK)) {
                    tricky_registered = KC_SCLN;
                } else {
                    tricky_registered = KC_L;
                }
                register_code(tricky_registered);
            } else {
                unregister_code(tricky_registered);
            }
            break;
        case AF_TRICKY_UP:
            if (record->event.pressed) {
                if (get_mods() & MODS_RSHIFT_MASK) {
                    tricky_registered = KC_SLSH;
                } else {
                    tricky_registered = KC_UP;
                }
                register_code(tricky_registered);
            } else {
                unregister_code(tricky_registered);
            }
            break;
        case AF_TRICKY_BSPC:
            if (record->event.pressed) {
                if (get_mods() & MODS_RSHIFT_MASK) {
                    tricky_registered = KC_BSLS;
                } else {
                    tricky_registered = KC_BSPC;
                }
                register_code(tricky_registered);
            } else {
                unregister_code(tricky_registered);
            }
            break;
        default:
            break;
        }
        break;
    case RGB_LIGHT_CONTROL:
        if (record->event.pressed) {
            switch (opt) {
#ifdef RGB_LIGHT_ENABLE // RGB灯（无控制芯片）控制
            case RGB_LIGHT_STEP:
                rgb_light_step();
                break;
            case RGB_LIGHT_TOGGLE:
                rgb_light_toggle();
                break;
            case RGB_LIGHT_IHUE:
                rgb_light_increase_hue();
                break;
            case RGB_LIGHT_DHUE:
                rgb_light_decrease_hue();
                break;
            case RGB_LIGHT_ISAT:
                rgb_light_increase_sat();
                break;
            case RGB_LIGHT_DSAT:
                rgb_light_decrease_sat();
                break;
            case RGB_LIGHT_IVAL:
                rgb_light_increase_val();
                break;
            case RGB_LIGHT_DVAL:
                rgb_light_decrease_val();
                break;
            case RGB_LIGHT_TML:
                rgb_indicator_toggle();
                break;
#endif
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}
FN_HANDLER(custom_fn_handler);
