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

#include "bootloader.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_bootloader_info.h"
#include <stdint.h>

void bootloader_jump(void)
{
    // Set bootloader magic code
    sd_power_gpregret_set(0, BOOTLOADER_DFU_START);

    // Signal that DFU mode is to be enter to the power management module
    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_DFU);
}

#include "keyboard_led.h"
#include "led.h"
#include "keyboard_evt.h"
void led_set(uint8_t usb_led)
{
    keyboard_led_set(usb_led);
    trig_event_param(USER_EVT_LED, usb_led);
}