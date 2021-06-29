/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

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

#include "config.h"

#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

static void custom_led_evt_handler(enum user_event event, void* arg)
{
    switch (event) {
    case USER_EVT_STAGE:
        switch ((uint32_t)arg) {
        case KBD_STATE_PRE_INIT: // 开机闪烁LED
#ifdef LED_CAPS
            nrf_gpio_cfg_output(LED_CAPS);
            LED_WRITE(LED_CAPS, 1);
            nrf_delay_ms(20);
#endif
            break;
        case KBD_STATE_SLEEP: // 关机闪烁LED
#ifdef LED_CAPS
            LED_WRITE(LED_CAPS, 1);
#endif
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(custom_led_evt_handler);