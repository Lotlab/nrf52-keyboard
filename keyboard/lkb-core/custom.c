#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "ble_keyboard.h"
#include "bootmagic.h"
#include "keycode.h"
#include "config.h"

// 覆盖默认实现，取消软开机功能，即永远是按任意键开机
void hook_bootmagic() {
    if (bootmagic_scan_key(BOOTMAGIC_KEY_ERASE_BOND)) {
        erase_bonds = true;
    }
}
