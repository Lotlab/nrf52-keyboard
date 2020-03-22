/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup bootloader_secure_ble main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file for secure DFU.
 *
 */

#include "app_error.h"
#include "app_error_weak.h"
#include "nrf.h"
#include "nrf_bootloader.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_dfu_timers.h"
#include "nrf_bootloader_info.h"
#include "nrf_delay.h"
#include "nrf_dfu.h"
#include "nrf_fstorage_sd.h"
#include "nrf_gpio.h"
#include "nrf_mbr.h"
#include "nrf_power.h"
#include <stdint.h>
#ifdef CONFIG_H_FILE
    #include CONFIG_H_FILE
#endif

static void on_error(void)
{
#ifdef NRF_DFU_DEBUG_VERSION
    NRF_BREAKPOINT_COND;
#endif
    NVIC_SystemReset();
}

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t* p_file_name)
{
    on_error();
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    on_error();
}

void app_error_handler_bare(uint32_t error_code)
{
    on_error();
}

/**
 * @brief Function notifies certain events in DFU process.
 */
static void dfu_observer(nrf_dfu_evt_type_t evt_type)
{
    switch (evt_type) {
    case NRF_DFU_EVT_DFU_FAILED:
    case NRF_DFU_EVT_DFU_ABORTED:
    case NRF_DFU_EVT_DFU_INITIALIZED:
        // todo: LED init.
        break;
    case NRF_DFU_EVT_TRANSPORT_ACTIVATED:
        // todo: set LED
        break;
    case NRF_DFU_EVT_DFU_STARTED:
        break;
    default:
        break;
    }
}

#ifdef NRF_BL_DFU_MULTI_ROLE_BTN

NRF_FSTORAGE_DEF(nrf_fstorage_t clr_fs);

/**
 * @brief Clear the internal stroage
 * 
 */
static void storage_clear()
{
    uint32_t const bootloader_addr = BOOTLOADER_ADDRESS;
    uint32_t const page_sz = NRF_FICR->CODEPAGESIZE;

#if defined(NRF52810_XXAA) || defined(NRF52811_XXAA)
    // Hardcode the number of flash pages, necessary for SoC emulation.
    // nRF52810 on nRF52832 and
    // nRF52811 on nRF52840
    uint32_t const code_sz = 48;
#else
    uint32_t const code_sz = NRF_FICR->CODESIZE;
#endif

    clr_fs.end_addr = (bootloader_addr != 0xFFFFFFFF) ? bootloader_addr : (code_sz * page_sz);
    clr_fs.start_addr = clr_fs.end_addr - NRF_DFU_APP_DATA_AREA_SIZE;

    nrf_fstorage_init(&clr_fs, &nrf_fstorage_sd, NULL);
    nrf_fstorage_erase(&clr_fs, clr_fs.start_addr, (NRF_DFU_APP_DATA_AREA_SIZE / 4096), NULL);
}

/**
 * @brief Force enter the DFU mode
 * 
 */
static void dfu_set_enter()
{
    nrf_power_gpregret_set(nrf_power_gpregret_get() | BOOTLOADER_DFU_START);
}

static void dfu_multi_role_btn()
{
    nrf_gpio_cfg_input(NRF_BL_DFU_MULTI_ROLE_BTN,
        NRF_GPIO_PIN_PULLUP);

    uint32_t press_count = 0;

    while (!nrf_gpio_pin_read(NRF_BL_DFU_MULTI_ROLE_BTN)) {
        nrf_delay_ms(10);
        press_count++;
    }

    if (press_count > 400) {
        if (press_count > 1000) {
            // 10秒以上，清除所有存储的数据
            storage_clear();
        } else {
            // 4秒以上，进入DFU
            dfu_set_enter();
        }
    }
}
#endif

/**@brief Function for application main entry. */
int main(void)
{
    uint32_t ret_val;

    // Protect MBR and bootloader code from being overwritten.
    ret_val = nrf_bootloader_flash_protect(0, MBR_SIZE, false);
    APP_ERROR_CHECK(ret_val);
    ret_val = nrf_bootloader_flash_protect(BOOTLOADER_START_ADDR, BOOTLOADER_SIZE, false);
    APP_ERROR_CHECK(ret_val);


#ifdef NRF_BL_DFU_MULTI_ROLE_BTN
    dfu_multi_role_btn();
#endif

    ret_val = nrf_bootloader_init(dfu_observer);
    APP_ERROR_CHECK(ret_val);

    // Either there was no DFU functionality enabled in this project or the DFU module detected
    // no ongoing DFU operation and found a valid main application.
    // Boot the main application.
    nrf_bootloader_app_start();
}

/**
 * @}
 */
