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

#include "usb_comm.h"
#include "hook.h"
#include "keycode.h"
#include "bootmagic.h"
#include "../main.h"
#include "sleep_reason.h"

#ifndef BOOTMAGIC_KEY_BOOT_TRAP
#define BOOTMAGIC_KEY_BOOT_TRAP KC_J
#endif
#ifndef BOOTMAGIC_KEY_BOOT_TRAP2
#define BOOTMAGIC_KEY_BOOT_TRAP2 KC_H
#endif

__attribute__((weak)) void hook_bootmagic() 
{
    bool sleep_flag = true;
    // 仅在按下开机按钮BOOT，且没按下BOOT_TRAP的情况下开机
    if (bootmagic_scan_key(BOOTMAGIC_KEY_BOOT)
        && !bootmagic_scan_key(BOOTMAGIC_KEY_BOOT_TRAP) 
        && !bootmagic_scan_key(BOOTMAGIC_KEY_BOOT_TRAP2)) 
        sleep_flag = false;

#ifdef DEBUG_SKIP_PWRON_CHECK
    // debug状态下自动开机
    sleep_flag = false;
#endif
#ifdef HAS_USB
    // 若连接至主机则自动开机
    if (usb_working())
        sleep_flag = false;
#endif
    // 自动休眠则不需要需要使用BOOT按键开机
    if (sleep_reason_get())
        sleep_flag = false;

    if (sleep_flag) {
        sleep(SLEEP_NOT_PWRON);
    }
    
    if (bootmagic_scan_key(BOOTMAGIC_KEY_ERASE_BOND)) {
        erase_bonds = true;
    }
}