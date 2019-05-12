#include "hook.h"
#include "keycode.h"
#include "bootmagic.h"
#include "../main.h"
#include "sleep_reason.h"
#include "usb_comm.h"

void hook_bootmagic() {
    if (!bootmagic_scan_key(BOOTMAGIC_KEY_BOOT)) {
        bool sleep_flag = true;
#ifdef DEBUG
        // debug状态下自动开机
        sleep_flag &= false;
#endif
#ifdef HAS_USB
        // 若连接至主机则自动开机
        sleep_flag &= !usb_working();
#endif
        // 自动休眠则不需要需要使用BOOT按键开机
        sleep_flag &= sleep_reason_get();

        if (sleep_flag) {
            sleep(SLEEP_NOT_PWRON);
        }
    }
    
    if (bootmagic_scan_key(BOOTMAGIC_KEY_ERASE_BOND)) {
        erase_bonds = true;
    }
}