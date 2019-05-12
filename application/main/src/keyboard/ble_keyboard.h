#if !defined(_BLE_KEYBOARD_H_)
#define _BLE_KEYBOARD_H_

#include <stdbool.h>

void ble_keyboard_init(void);
void ble_keyboard_timer_start(void);
void ble_keyboard_powersave(bool save);

#endif // _BLE_KEYBOARD_H_

