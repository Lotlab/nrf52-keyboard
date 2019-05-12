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
void led_set(uint8_t usb_led)
{
    keyboard_led_set(usb_led);
}