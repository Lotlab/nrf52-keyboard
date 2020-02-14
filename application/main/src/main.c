/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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

/** @file
 *
 * @defgroup ble_sdk_app_hids_keyboard_main main.c
 * @{
 * @ingroup ble_sdk_app_hids_keyboard
 * @brief HID Keyboard Sample Application main file.
 *
 * This file contains is the source code for a sample application using the HID, Battery and Device
 * Information Services for implementing a simple keyboard functionality.
 * Pressing Button 0 will send text 'hello' to the connected peer. On receiving output report,
 * it toggles the state of LED 2 on the mother board based on whether or not Caps Lock is on.
 * This application uses the @ref app_scheduler.
 *
 * Also it would accept pairing requests from any peer device.
 */

#include "app_error.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_soc.h"
#include <stdint.h>
#include <string.h>

#include "app_scheduler.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"

#include "ble/ble_bas_service.h"
#include "ble/ble_hid_service.h"
#include "ble/ble_services.h"

#include "main.h"

#include "keyboard/adc_convert.h"
#include "keyboard/ble_keyboard.h"
#include "keyboard/keyboard_bootcheck.h"
#include "keyboard/keyboard_command.h"
#include "keyboard/keyboard_evt.h"
#include "keyboard/keyboard_led.h"
#include "keyboard/keyboard_matrix.h"
#include "protocol/usb_comm.h"

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE 30 /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE 20 /**< Maximum number of events in the scheduler queue. */
#endif

static void set_stage(enum keyboard_state stage)
{
    trig_event_param(USER_EVT_STAGE, stage);
}

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t* p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief 准备RESET
 * 
 */
static void reset_prepare(void)
{
    // 禁用键盘LED
    keyboard_led_deinit();
    set_stage(KBD_STATE_SLEEP);

    ret_code_t err_code;
    err_code = app_timer_stop_all();
    APP_ERROR_CHECK(err_code);
}

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event) {
    case NRF_PWR_MGMT_EVT_PREPARE_DFU:;
        reset_prepare();
        break;

    default:
        // YOUR_JOB: Implement any of the other events available from the power management module:
        //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
        //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
        //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
        return true;
    }
    return true;
}

//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    ble_services_init();
    battery_service_init();
    hid_service_init(service_error_handler);
    adc_init();
}

/**@brief Function for starting timers.
 */
static void timers_start(void)
{
    ble_keyboard_timer_start();
#ifdef HAS_USB
    usb_comm_timer_start();
#endif
    adc_timer_start();
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    reset_prepare();
    matrix_sleep_prepare(); // 准备按键阵列用于唤醒
#ifdef HAS_USB
    usb_comm_sleep_prepare();
#endif

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    ret_code_t err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 发送键盘睡眠通知
 * 
 * @param reason 
 */
void notify_sleep(enum sleep_evt_type mode)
{
    trig_event_param(USER_EVT_SLEEP, mode);
}

/**
 * @brief 使键盘进入睡眠状态
 * 
 * @param reason 
 */
void sleep(enum SLEEP_REASON reason)
{
    switch (reason) {
    case SLEEP_NO_CONNECTION:
    case SLEEP_TIMEOUT:
        app_timer_stop_all();
        notify_sleep(SLEEP_EVT_AUTO);
        sleep_mode_enter();
        break;
    case SLEEP_MANUALLY:
        app_timer_stop_all();
        notify_sleep(SLEEP_EVT_MANUAL);
        sleep_mode_enter();
        break;
    case SLEEP_NOT_PWRON:
        sleep_mode_enter();
        break;
    default:
        break;
    }
}

/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    sd_power_dcdc_mode_set(1);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    app_sched_execute();
    execute_event();
    nrf_pwr_mgmt_run();
}

bool erase_bonds = false;

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    timers_init();
    power_management_init();

    set_stage(KBD_STATE_PRE_INIT);

    ble_stack_init();
    scheduler_init();
    services_init();
    ble_keyboard_init();

#ifndef BOOTMAGIC_ENABLE
    // use internal function to check if should boot.
    boot_check();
#endif

    // call custom init function
    set_stage(KBD_STATE_POST_INIT);

    // Start execution.
    timers_start();
    advertising_start(erase_bonds);

#ifdef COMMAND_ENABLE
    command_timer_init();
#endif
    set_stage(KBD_STATE_INITED);

    // Enter main loop.
    for (;;) {
        idle_state_handle();
    }
}

/**
 * @}
 */
