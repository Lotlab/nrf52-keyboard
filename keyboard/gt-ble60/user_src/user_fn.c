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
#include "ble_services.h"
#include "main.h"
#include "status_led.h"
#include "usb_comm.h"
#include "user_func.h"

#define MODS_SHIFT_MASK (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT))

void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    static uint8_t tricky_esc_registered;
    switch (id) {
    case AF_POWER_SLEEP:
        if (!record->event.pressed) {
            sleep(SLEEP_MANUALLY);
        }
        break;
    case AF_POWER_OFF:
        if (!record->event.pressed) {
            systemoff();
        }
        break;
    case AF_SWITCH_MODE:
        if (!record->event.pressed) {
#ifdef HAS_USB
            usb_comm_switch();
#endif
        }
        break;
    case AF_DELETE_BOND:
        if (!record->event.pressed) {
            delete_bonds();
        }
        break;
    case AF_LED_DISPLAY:
        if (!record->event.pressed) {
            if (!usb_working()) {
                status_led_display();
            }
        }
        break;
    case AF_TRICKY_ESC:
        if (record->event.pressed) {
            if (get_mods() & MODS_SHIFT_MASK) {
                tricky_esc_registered = KC_GRV;
            } else {
                tricky_esc_registered = KC_ESC;
            }
            register_code(tricky_esc_registered);
            send_keyboard_report();
        } else {
            unregister_code(tricky_esc_registered);
            send_keyboard_report();
        }
        break;
    default:
        break;
    }
}
