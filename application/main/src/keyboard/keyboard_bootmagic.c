#include "hook.h"
#include "keycode.h"
#include "bootmagic.h"
#include "../main.h"

void hook_bootmagic() {
    if (!bootmagic_scan_key(BOOTMAGIC_KEY_BOOT)) {
#ifndef DEBUG
        // todo: 根据上次睡眠原因判断是否应该直接开机
        sleep(SLEEP_NOT_PWRON);
#endif
    }
    
    if (bootmagic_scan_key(BOOTMAGIC_KEY_ERASE_BOND)) {
        erase_bonds = true;
    }
}