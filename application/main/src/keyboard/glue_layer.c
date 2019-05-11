#include <stdint.h>
#include "bootloader.h"

void bootloader_jump(void) {
    // todo: implement this
}

#include "led.h"
#include "keyboard_led.h"
void led_set(uint8_t usb_led) {
    keyboard_led_set(usb_led);
}