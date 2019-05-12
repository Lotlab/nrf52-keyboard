#include "hook.h"
#include "keycode.h"
#include "bootmagic.h"
#include "../main.h"
#include "sleep_reason.h"

void hook_bootmagic() {
    if (!bootmagic_scan_key(BOOTMAGIC_KEY_BOOT)) {
#ifndef DEBUG
        // 非自动休眠则需要使用BOOT按键开机
        if (!sleep_reason_get()) {
            sleep(SLEEP_NOT_PWRON);
        }
#endif
    }
    
    if (bootmagic_scan_key(BOOTMAGIC_KEY_ERASE_BOND)) {
        erase_bonds = true;
    }
}